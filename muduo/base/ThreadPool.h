#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "muduo/base/Condition.h"
#include "muduo/base/Mutex.h"
#include "muduo/base/Thread.h"
#include "muduo/base/Types.h"

#include <deque>
#include <vector>

namespace muduo {

class ThreadPool : noncopyable {
public:
  typedef std::function<void(void)> Task;
  explicit ThreadPool(const string& nameArg=string("ThreadPool"));
  ~ThreadPool();

  void setMaxQueueSize(int maxSize){maxQueueSize_=maxSize;}
  void setThreadInitCallback(const Task& cb){threadInitCallback_=cb;}

  void start(int numThreads);
  void stop();

  const string& name() const{return name_;}
  size_t queueSize() const;
  void run(Task f);//client call

private:
  bool isFull() const REQUIRES(mutex_);
  void runInThread();
  Task take();

  mutable MutexLock mutex_;
  Condition notEmpty_ GUARDED_BY(mutex_);
  Condition notFull_ GUARDED_BY(mutex_);
  string name_;
  Task threadInitCallback_;
  std::vector<std::unique_ptr<Thread>> threads_;
  std::deque<Task> queue_ GUARDED_BY(mutex_);
  size_t maxQueueSize_;
  bool running_;

};

} // namespace muduo

#endif // THREADPOOL_H
