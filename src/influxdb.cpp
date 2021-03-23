#include <butil/logging.h>
#include "util.h"
#include "influxdb.h"
#include "timer.h"

/*
*now support influxdb1 only
*TODO: change uri pattern to be compatible
*/
InfluxClient::InfluxClient(brpc::Channel* in_channel, std::string in_uri, std::string in_db): 
channel(in_channel),
uri(in_uri),
db(in_db)
{
    Init();
}

//close client and flush buffer sync
InfluxClient::~InfluxClient()
{
    //release timer
    BatchTime(0);
    SyncFlush();
    BatchSize(0);
}

static void OnRPCDone(brpc::Controller* cntl,google::protobuf::Message* response) {
    // unique_ptr helps us to delete response/cntl automatically. unique_ptr in gcc 3.4 is an emulated version.
    std::unique_ptr<brpc::Controller> cntl_guard(cntl);
    if (cntl->Failed()) {
        LOG(INFO) << "write to influxdb failed";
        // RPC failed. fields in response are undefined, don't use.
    } else {
        LOG(INFO) << "write to influxdb success: " << cntl->response_attachment();
        // RPC succeeded, response has what we want. Continue the post-processing.
    }
    // Closure created by NewCallback deletes itself at the end of Run.
}

//write a point to influxdb
//TODO: async handle return value
//TODO: add buffer param
//maybe need a url reader lock 

void  InfluxClient::AsyncWrite(const Point & point) const
{
    brpc::Controller* cntl = new brpc::Controller;
    google::protobuf::Message* response = nullptr;
    cntl->http_request().uri() = write_url;  // Request URL
    cntl->http_request().set_method(brpc::HTTP_METHOD_POST);
    butil::IOBufBuilder os;
    os  << point.toLineProtocol();
    os.move_to(cntl->request_attachment());
    channel->CallMethod(NULL, cntl, NULL, NULL, brpc::NewCallback(OnRPCDone,cntl,response));
    LOG(INFO) << "async write to influxdb return: " << point.toLineProtocol();
}


//write a point to influxdb
//TODO: async handle return value
//TODO: add buffer param
//maybe need a url reader lock 

void  InfluxClient::Write(const Point & point) const
{
    brpc::Controller cntl;
    cntl.http_request().uri() = write_url;  // Request URL
    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    butil::IOBufBuilder os;
    os  << point.toLineProtocol();
    os.move_to(cntl.request_attachment());
    channel->CallMethod(NULL, &cntl, NULL, NULL, NULL/*done*/);
    LOG(INFO) << "write to influxdb: " << point.toLineProtocol();
}

//TODO: json parser
//TODO: add open source json lib
//maybe need a url reader lock 

std::string InfluxClient::Query(std::string sql) const
{
    brpc::Controller cntl;
    //URLENCODE
    std::string encoded_sql("&q=");
    url_encode(encoded_sql,sql);
    cntl.http_request().uri() = query_url + encoded_sql;  // Request URL
    cntl.http_request().set_method(brpc::HTTP_METHOD_GET);
    channel->CallMethod(NULL, &cntl, NULL, NULL, NULL/*done*/);
    LOG(INFO) << "query to influxdb: " << cntl.response_attachment();
    return cntl.response_attachment().to_string();
}

//maybe need a url writer lock 
void InfluxClient::Init()
{
    write_url = uri + "/write?db=" + db;
    query_url = uri + "/query?db=" + db;
}
                   

//is buffering
bool InfluxClient::IsBuffer()
{
    return maxBufferSize > 1 || period > 0;
    
}
//flush buffer sync because if exit main, async can't call back
void InfluxClient::SyncFlush()
{
    if(!IsBuffer() || buffer.size()==0)
        return;
    brpc::Controller cntl;
    cntl.http_request().uri() = write_url;  // Request URL
    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    butil::IOBufBuilder os;    
    LOG(INFO) << "flush to influxdb sync:\n" << buffer;
    os  << buffer;    
    bufferSize = 0;
    os.move_to(cntl.request_attachment());    
    //important because os use right value,  can't clear before move_to
    buffer.clear();
    channel->CallMethod(NULL, &cntl, NULL, NULL, NULL/*done*/);
}

//flush buffer
//need lock!
void InfluxClient::AsyncFlush()
{
    if(!IsBuffer() || buffer.size()==0)
        return;
    brpc::Controller* cntl = new brpc::Controller;
    google::protobuf::Message* response = nullptr;
    cntl->http_request().uri() = write_url;  // Request URL
    cntl->http_request().set_method(brpc::HTTP_METHOD_POST);
    butil::IOBufBuilder os;    
    LOG(INFO) << "flush to influxdb async:\n" << buffer;
    os  << buffer;    
    bufferSize = 0;
    os.move_to(cntl->request_attachment());    
    //important because os use right value,  can't clear before move_to
    buffer.clear();
    channel->CallMethod(NULL, cntl, NULL, NULL, 
    brpc::NewCallback(OnRPCDone,cntl,response)/*done*/);
}

//size > 1 is batching
//size = 0 is no batching
void InfluxClient::BatchSize(int in_bufferSize)
{
    if(in_bufferSize > 1 || in_bufferSize == 0)
        maxBufferSize = in_bufferSize;
}

//only >=0 meaningful
void InfluxClient::BatchTime(int in_period)
{
    if(period >= 0)
    {
    period = in_period;
    if(timer)
        delete timer;
    if(in_period == 0){
        timer = nullptr;
        return;
    }
    else 
    timer = new InfluxTimer(in_period, this);  
    }
}

//batch write
//if not batching call
void InfluxClient::BatchWrite(const Point & point)
{
    if(IsBuffer())
    {
    buffer += point.toLineProtocol();
    buffer += '\n';
    bufferSize++;
    //batch size and overflow
    if(maxBufferSize > 1 && bufferSize>=maxBufferSize)
        AsyncFlush();
    }
    else
    { 
        Write(point);
    }
}