#ifndef THREAD_H
#define THREAD_H

#include <muduo/base/Atomic.h>
#include <muduo/base/CountDownLatch.h>
#include <muduo/base/Types.h>

#include <functional>
#include <memory>
#include <pthread.h>

namespace muduo {

class Thread : public noncopyable {
public:
  using ThreadFunc = std::function<void(void)>;
  explicit Thread(ThreadFunc, const std::string &name = std::string());
  ~Thread();

  void start();
  int join();
  bool started() { return started_; }
  pid_t tid() const { return tid_; }
  const string &name() { return name_; }

  static int numCreated() { return numCreated_.get(); }

private:
  void setDefaultName();

  bool started_;
  bool joined_;
  pthread_t pthreadId_;
  pid_t tid_;
  ThreadFunc func_;
  std::string name_;
  CountDownLatch latch_;
  static AtomicInt32 numCreated_;
};

#endif // THREAD_H

} // namespace muduo
