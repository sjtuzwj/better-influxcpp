#include <gflags/gflags.h>
#include <butil/logging.h>
#include <butil/time.h>
#include <brpc/channel.h>
#include "client.h"
#include <ctime>
#include "point.h"




//INFLUX INSERT POINT EXAMPLE
void TSBatchUpdate(InfluxClient & client, std::string measure, std::string author, int id , int status)
{
    Point point{measure};
    point.addField("id",id);
    point.addField("status",status);
    point.addTag("author",author);
    
    client.BatchWrite(point);
}

//INFLUX INSERT POINT EXAMPLE
void TSAsyncUpdate(const InfluxClient & client, std::string measure, std::string author, int id , int status)
{
    Point point{measure};
    point.addField("id",id);
    point.addField("status",status);
    point.addTag("author",author);
    
    client.AsyncWrite(point);
}


//INFLUX INSERT POINT EXAMPLE
void TSUpdate(const InfluxClient & client, std::string measure, std::string author, int id , int status)
{
    Point point{measure};
    point.addField("id",id);
    point.addField("status",status);
    point.addTag("author",author);
    
    client.Write(point);
}


//INFLUX QUERY
std::string TSQuery(const InfluxClient & client,  std::string sql)
{
    return client.Query(sql);
}
