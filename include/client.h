#ifndef DEMO_CLIENT_H
#define DEMO_CLIENT_H

#include <brpc/channel.h>
#include "influxdb.h"

//INFLUX INSERT POINT EXAMPLE
void TSUpdate(const InfluxClient & client,  std::string measure, std::string author, int id , int status);

//INFLUX QUERY
std::string TSQuery(const InfluxClient & client,  std::string sql);


#endif