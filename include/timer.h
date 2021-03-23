#ifndef INFLUX_TIMER_H
#define INFLUX_TIMER_H

#include <memory>
#include <string>

#include <bthread/unstable.h>

class InfluxClient;
class InfluxTimer {
 public:
  InfluxTimer()=delete;
  //period to flush buffer
  InfluxTimer(int period, InfluxClient* client);
  ~InfluxTimer();
  void Close();
 private:
  InfluxClient* client;
  int period;
  void Flush();
  static void Task(void* arg);
  bthread_timer_t task_timer_;
};

#endif