#include "Timer.h"

using namespace muduo;
using namespace muduo::net;

void Timer::restart(muduo::Timestamp now) {
  if (repeat_) {
    expiration_ = addTime(now, interval_);
  } else {
    expiration_ = Timestamp::invalid();
  }
}
