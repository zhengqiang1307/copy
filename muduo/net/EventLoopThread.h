#ifndef eventloopthread_h
#define eventloopthread_h

#include "muduo/base/Condition.h"
#include "muduo/base/Mutex.h"
#include "muduo/base/Thread.h"

namespace muduo
{
namespace net
{

class EventLoop;
class EventLoopThread: noncopyable
{
    public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;
    EventLoopThread(const ThreadInitCallback&cb=ThreadInitCallback(),
    const string& name=string());
    ~EventLoopThread();
    EventLoop* startLoop();

    private:
    void threadFunc();
    EventLoop* loop_ GUARDED_BY(mutex_);
    bool exiting_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_ GUARDED_BY(mutex_);
    ThreadInitCallback callback_;
}

}
}

#endif