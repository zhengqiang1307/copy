#ifndef TIMERID_H
#define TIMERID_H

#include "muduo/base/copyable.h"

namespace muduo {
namespace net {

class Timer;

class TimerId : public muduo::copyable {
private:
  Timer *timer_;
  int64_t sequence_;

public:
  TimerId() : timer_(nullptr), sequence_(0) {}
  TimerId(Timer *timer, int64_t sequence)
      : timer_(timer), sequence_(sequence) {}
  friend class TimerQueue;
};

} // namespace net

} // namespace muduo

#endif // TIMERID_H
