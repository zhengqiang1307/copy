#ifndef eventloop_h
#define eventloop_h

#include <atomic>
#include <functional>
#include <vector>

#include <boost/any.hpp>

#include "muduo/base/Mutex.h"
#include "muduo/base/CurrentThread.h"
#include "muduo/base/Timestamp.h"
#include "muduo/net/Callbacks.h"
#include "muduo/net/TimerId.h"

namespace muduo
{

namespace net
{
  class Channel;
  class Poller;
  class TimerQueue;

  class EventLoop:noncopyable
  {
    public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();
    void loop();
    void quit();
    Timestamp pollReturnTime() const {return pollReturnTime_;}
    int64_t iteration() const {return iteration_;}
    void runInloop(Functor cb);
    void queueInLoop(Functor cb);
    size_t queueSize() const;

    TimerId runAt(Timestamp time, TimerCallback cb);
    TimerId runAfter(double delay, TimerCallback cb);
    TimerId runEvery(double interval, TimerCallback cb);
    void cancel(TimerId timerId);

    void wakeup();
    void updateChannel(Channel channel);
    void removeChannel(Channel channel);
    bool hasChannel(Channel* channel);

    void assertInLoopThread()
    {
      if(!inInLoopThread())
      {
        abortNotInLoopThread();
      }
    }

    bool isInLoopThread() const
    {
      return threadId_==CurrentThread::tid();
    }

    bool eventHandling() const {return eventHandling;}

    static EventLoop* getEventLoopOfCurrentThread();

    private:
    void abortNotInLoopThread();
    void handleRead();
    void doPendingFunctors();

    void printActiveChannels() const;
    typedef std::vector<Channel*> ChannelList;

    bool looping_;
    std::atomic<bool> quit_;
    bool eventHandling_;
    bool callingPendingFunctors_;
    int64_t iteration_;
    const pid_t threadId;
    Timestamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;
    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannel_;
    Channel* currentActiveChannel_;

    mutable MutexLock mutex_;
    std::vector<Functor> pendingFuntors_ GUARDED_BY(mutex);



    

  }
}
} // namespace muduo
#endif 