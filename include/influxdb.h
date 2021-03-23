#ifndef INFLUX_CLINET_H
#define INFLUX_CLIENT_H


#include <brpc/channel.h>
#include "point.h"

class InfluxTimer;
/*
*now support influxdb1 only
*TODO: change uri pattern to be compatible
*TODO: user and passwd support
*/
class InfluxClient
{
    private:
        //buffer size
        int bufferSize = 0;
        //std::string
        std::string buffer;
        //max buffer_size
        int maxBufferSize = 0;
        //period
        int period;
        //flush timer
        InfluxTimer * timer = nullptr;
        //brpc channel 
        brpc::Channel* channel = nullptr;
        //influx db uri, domain name or ip:port
        std::string uri;
        //influx db name
        std::string db;
        
        //influxdb write url
        std::string write_url;
        //influxdb query url;
        std::string query_url;
        //is buffering
        bool IsBuffer();
        //generate url to prevent concat cost in each call
        void Init();
    public:
        //init without infomation is forbidden
        InfluxClient()=delete;
        InfluxClient(brpc::Channel* in_channel, std::string in_uri, std::string in_db);

        //flush buffer
        void Flush();
        //size > 1 is batching
        //size = 0 is no batching
        void BatchSize(int bufferSize);
        //only >=0 meaningful
        void BatchTime(int period);
        //TODO: batch write
        void BatchWrite(const Point & point);

        //write a point to influxdb
        //TODO: async handle return value
        //TODO: add buffer param to handle response
        void AsyncWrite(const Point & point) const;

        //sync write a point to influxdb 
        void Write(const Point & point) const;

        //sync query
        //TODO: json parser
        //TODO: add open source json lib
        std::string Query(std::string sql) const;
};

#endif