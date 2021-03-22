#ifndef INFLUX_UTIL_H
#define INFLUX_UTIL_H

/* 
*function to format sql in url
*code from https://github.com/orca-zhang/influxdb-cpp/blob/master/influxdb.hpp
*/
#include <string>

unsigned char to_hex(unsigned char x);
void url_encode(std::string& out, const std::string& src);

#endif
