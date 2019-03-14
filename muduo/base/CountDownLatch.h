#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include<muduo/base/Mutex.h>
#include<muduo/base/Condition.h>

namespace muduo {

class CountDownLatch
{
public:
  explicit CountDownLatch(int count);
  void wait();
  void countDown();
  int getCount()const;
private:
  mutable MutexLock mutex_;
  Condition condition_ GUARDED_BY(mutex_);
  int count_ GUARDED_BY(mutex_);
};

}
#endif // COUNTDOWNLATCH_H

