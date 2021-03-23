
#include "timer.h"
#include "influxdb.h"

InfluxTimer::InfluxTimer(int in_period , InfluxClient* in_client) {
      period = in_period;
      client = in_client;
      bthread_timer_add(
      &task_timer_,
      butil::seconds_from_now(period),
      Task,
      this);
}
 
InfluxTimer::~InfluxTimer() {
    Close();
}
 
void InfluxTimer::Close() {
  bthread_timer_del(task_timer_);
}
 
void InfluxTimer::Task(void* arg) {
  InfluxTimer* brpc_timer = reinterpret_cast<InfluxTimer*>(arg);
  brpc_timer->Flush();
  bthread_timer_add(
      &brpc_timer->task_timer_,
      butil::seconds_from_now(brpc_timer->period),
      Task,
      brpc_timer);
}
 
void InfluxTimer::Flush() {
  client->Flush();
}