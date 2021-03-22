#ifndef INFLUX_POINT_H
#define INFLUX_POINT_H
/*
*function to format point 
*prototype from https://github.com/awegrzyn/influxdb-cxx
*use brpc to improve network performance and async support
*/

#include <string>
#include <chrono>

class Point
{
  public:
    /// Constructs point based on measurement name
    Point(const std::string& measurement);

    /// Default destructor
    ~Point() = default;

    /// Adds a tags
    Point&& addTag(const std::string& key, const std::string& value);

    /// Adds bool field
    Point&& addField(const std::string& name, bool value);

    /// Adds int field
    Point&& addField(const std::string& name, int value);

    /// Adds string field
    Point&& addField(const std::string& name, const std::string& value);

    /// Adds double field
    Point&& addField(const std::string& name, double value);

    /// Generetes current timestamp
    static auto getCurrentTimestamp() -> decltype(std::chrono::system_clock::now());

    /// Converts point to Influx Line Protocol
    std::string toLineProtocol() const;

    /// Sets custom timestamp
    Point&& setTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp);

    /// Name getter
    std::string getName() const;

    /// Timestamp getter
    std::chrono::time_point<std::chrono::system_clock> getTimestamp() const;

    /// Fields getter
    std::string getFields() const;

    /// Tags getter
    std::string getTags() const;

  protected:
    /// A name
    std::string mMeasurement;

    /// A timestamp
    std::chrono::time_point<std::chrono::system_clock> mTimestamp;

    /// Tags
    std::string mTags;

    /// Fields
    std::string mFields;
};

#endif
