#ifndef INFLUX_CLINET_H
#define INFLUX_CLIENT_H


#include <brpc/channel.h>
#include "point.h"

/*
*now support influxdb1 only
*TODO: change uri pattern to be compatible
*TODO: user and passwd support
*/
class InfluxClient
{
    private:
        //brpc channel 
        brpc::Channel* channel;
        //influx db uri, domain name or ip:port
        std::string uri;
        //influx db name
        std::string db;
        
        //influxdb write url
        std::string write_url;
        //influxdb query url;
        std::string query_url;
        //generate url to prevent concat cost in each call
        void Init();
    public:
        //init without infomation is forbidden
        InfluxClient()=delete;
        InfluxClient(brpc::Channel* in_channel, std::string in_uri, std::string in_db);

        //TODO: batch write
        //void BacthWrite();

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

        //setter useless, because construct overhead is little
        //you can use a new client instead of reuse it
        //TODO: check before set
        void SetChannel(brpc::Channel* new_channel);
        void SetURI(std::string new_uri);
        void SetDB(std::string new_db);
};

#endif