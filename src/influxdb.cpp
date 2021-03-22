#include <butil/logging.h>
#include "util.h"
#include "influxdb.h"

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
                   
//setter useless, because construct overhead is little
//you can use a new client instead of reuse it
void InfluxClient::SetChannel(brpc::Channel* new_channel)
{
    channel = new_channel;
}

void InfluxClient::SetURI(std::string new_uri)
{
    uri = new_uri;
    Init();
}

void InfluxClient::SetDB(std::string new_db)
{
    db = new_db;
    Init();
}

