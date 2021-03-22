/*
* code from https://github.com/awegrzyn/influxdb-cxx/blob/master/src/Point.cxx
* transfer to cpp11
* Point Builder and generate lineprotocol
*/

#include "point.h"
#include <chrono>

Point::Point(const std::string& measurement) :
  mMeasurement(measurement), mTimestamp(Point::getCurrentTimestamp())
{
  mTags = {};
  mFields = {};
}

Point&& Point::addField(const std::string& name, bool value)
{
  if (!mFields.empty()) mFields+=',';
  mFields += name;
  mFields += '=';
  mFields += value ? 't':'f';
  return std::move(*this);
}

Point&& Point::addField(const std::string& name, int value)
{
  if (!mFields.empty()) mFields+=',';
  mFields += name;
  mFields += '=';
  mFields += std::to_string(value);
  mFields += 'i'; 
  return std::move(*this);
}

Point&& Point::addField(const std::string& name, double value)
{
  if (!mFields.empty()) mFields+=',';
  mFields += name;
  mFields += '=';
  mFields += std::to_string(value);
  return std::move(*this);
}

Point&& Point::addField(const std::string& name, const std::string& value)
{
  if (!mFields.empty()) mFields+=',';
  mFields += name;
  mFields += "=\"";
  mFields += value;
  mFields += '\"';
  return std::move(*this);
}


Point&& Point::addTag(const std::string& key, const std::string& value)
{
  mTags += ',';
  mTags += key;
  mTags += '=';
  mTags += value;
  return std::move(*this);
}

Point&& Point::setTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp)
{
  mTimestamp = timestamp;
  return std::move(*this);
}

auto Point::getCurrentTimestamp() -> decltype(std::chrono::system_clock::now())
{
  return std::chrono::system_clock::now();
}

std::string Point::toLineProtocol() const
{
  return mMeasurement + mTags + " " + mFields + " " + std::to_string(
    std::chrono::duration_cast <std::chrono::nanoseconds>(mTimestamp.time_since_epoch()).count()
  );
}

std::string Point::getName() const
{
  return mMeasurement;
}

std::chrono::time_point<std::chrono::system_clock> Point::getTimestamp() const
{
  return mTimestamp;
}

std::string Point::getFields() const
{
  return mFields;
}

std::string Point::getTags() const
{
  return mTags.substr(1, mTags.size());
}
