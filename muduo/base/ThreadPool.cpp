#include "ThreadPool.h"
#include "muduo/base/Exception.h"
#include "string.h"

//  mutable MutexLock mutex_;
//  Condition notEmpty_ GUARDED_BY(mutex_);
//  Condition notFull_ GUARDED_BY(mutex_);
//  string name_;
//  Task threadInitCallback_;
//  std::vector<std::unique_ptr<Thread>> threads_;
//  std::deque<Task> queue_ GUARDED_BY(mutex_);
//  size_t maxQueueSize_;
//  bool running_;

namespace muduo {
ThreadPool::ThreadPool(const std::string &nameArg)
    : mutex_(), notEmpty_(mutex_), notFull_(mutex_), name_(nameArg),
      maxQueueSize_(0), running_(false) {}

ThreadPool::~ThreadPool() {
  if (running_)
    stop();
}

void ThreadPool::start(int numThreads) {
  assert(threads_.empty());
  threads_.reserve(numThreads);
  running_ = true;
  for (int i = 0; i < numThreads; i++) {
    char id[32];
    snprintf(id, sizeof(id), "%d", i + 1);
    threads_.emplace_back(new Thread(std::bind(&ThreadPool::runInThread, this),
                          name_ + id));
    threads_[i]->start();
  }
  if (numThreads == 0 && threadInitCallback_) {
    threadInitCallback_();
  }
}

void ThreadPool::stop() {
  {
    MutexLockGuard guard(mutex_);
    running_ = false;
    notEmpty_.notifyAll();
  }
  for (auto &thr : threads_) {
    thr->join();
  }
}

size_t ThreadPool::queueSize() const {
  MutexLockGuard guard(mutex_);
  return queue_.size();
}

void ThreadPool::run(ThreadPool::Task f) {
  if (threads_.empty())
    f();
  else {
    MutexLockGuard guard(mutex_);
    while (isFull()) {
      notFull_.wait();
    }
    assert(!isFull());
    queue_.push_back(f);
    notEmpty_.notify();
  }
}

void ThreadPool::runInThread() {
  try {
    if (threadInitCallback_)
      threadInitCallback_();

    while (running_) {
      Task task = take();
      if (task)
        task();
    }
  } catch (const Exception &ex) {
    fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
    abort();
  } catch (const std::exception &ex) {
    fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    abort();
  } catch (...) {
    fprintf(stderr, "unknown exception caught in ThreadPool %s\n",
            name_.c_str());
    throw; // rethrow
  }
}

ThreadPool::Task ThreadPool::take() {
  MutexLockGuard guard(mutex_);
  while (queue_.empty() && running_) {
    notEmpty_.wait();
  }
  Task task;
  if (!queue_.empty()) {
    task = queue_.front();
    queue_.pop_front();
    if (maxQueueSize_ > 0)
      notFull_.notify();
  }
  return task;
}

bool ThreadPool::isFull() const {
  mutex_.assertLocked();
  return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}
} // namespace muduo
