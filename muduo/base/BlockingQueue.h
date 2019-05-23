#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include "muduo/base/Condition.h"
#include "muduo/base/Mutex.h"

#include <assert.h>
#include <deque>

namespace muduo {

template <typename T> class BlockingQueue : noncopyable {
public:
  BlockingQueue() : mutex_(), notEmpty_(mutex_), queue_() {}
  void put(const T &x) {
    MutexLockGuard lock(mutex_);
    queue_.pop_back(x);
    notEmpty_.notify();
  }
  void put(const T &&x) {
    MutexLockGuard lock(mutex_);
    queue_.pop_back(std::move(x));
    notEmpty_.notify();
  }
  T take() {
    MutexLockGuard lock(mutex_);
    while (queue_.empty()) {
      notEmpty_.wait();
    }
    assert(!notEmpty_.empty());
    T front(std::move(queue_.front()));
    queue_.pop_front();
    return std::move(front);
  }
  size_t size() const {
    MutexLockGuard lock(mutex_);
    return queue_.size();
  }

private:
  mutable MutexLock mutex_;
  Condition notEmpty_ GUARDED_BY(mutex_);
  std::deque<T> queue_ GUARDED_BY(mutex_);
};

} // namespace muduo

#endif // BLOCKINGQUEUE_H
