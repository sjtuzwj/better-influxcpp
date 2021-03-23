
#include <gflags/gflags.h>
#include <butil/logging.h>
#include <butil/time.h>
#include "client.h"
#include <ctime>

DEFINE_string(ts_ip, "127.0.0.1", "IP of ts server");
DEFINE_int32(ts_port, 8086, "TCP Port of ts server");
DEFINE_string(connection_type, "", "Connection type. Available values: single, pooled, short");
DEFINE_int32(timeout_ms, 100, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)"); 
DEFINE_int32(interval_ms, 1000, "Milliseconds between consecutive requests");

int main(int argc, char* argv[]) 
{
    // Parse gflags. We recommend you to use gflags as well.
    GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);
    
    // A Channel represents a communication line to a Server. Notice that 
    // Channel is thread-safe and can be shared by all threads in your program.
    brpc::Channel ts_channel;
  
    // Initialize the influxdb channel
    brpc::ChannelOptions ts_options;
    ts_options.protocol = brpc::PROTOCOL_HTTP;  // or brpc::PROTOCOL_H2
    ts_options.connection_type = FLAGS_connection_type;
    ts_options.timeout_ms = FLAGS_timeout_ms/*milliseconds*/;
    ts_options.max_retry = FLAGS_max_retry;
    if (ts_channel.Init(FLAGS_ts_ip.c_str() , FLAGS_ts_port /*any url*/, &ts_options) != 0) {
     LOG(ERROR) << "Fail to initialize influxdb channel";
     return -1;
    }

    InfluxClient client(&ts_channel,"127.0.0.1:8086","test");
    client.BatchTime(5);
    int i = 0;
    while (!brpc::IsAskedToQuit()) 
    {        
        
        TSBatchUpdate(client,"test", "VentureZhu", i, i & 1);
        TSQuery(client,"select * from test where status = 1");
        i++;
        usleep(FLAGS_interval_ms * 1000L);  
    }

    LOG(INFO) << "Client is going to quit";
    return 0;
}
