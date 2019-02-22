#ifndef THREAD_H
#define THREAD_H

#include <muduo/base/Atomic.h>
#include <muduo/base/CountDownLatch.h>
#include <muduo/base/noncopyable.h>

#include <functional>
#include <string>

namespace muduo {

class Thread : public noncopyable {
public:
  using ThreadFunction = std::function<void(void)>;
  explicit Thread(ThreadFunction, const std::string name = std::string());
  ~Thread();

  void start();
  int join();
  bool started() { return started_; }
  pid_t tid() const { return tid_; }

private:
  bool started_;
  bool joined_;
  pthread_t pthreadId;
  pid_t tid_;
  ThreadFunction func_;
  std::string name_;
  CountDownLatch latch_;
  static AtomicInt32 numCreated_;
};

#endif // THREAD_H

} // namespace muduo
