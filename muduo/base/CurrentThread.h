#ifndef muduo_base_currentthread
#define muduo_base_currentthread

#include "muduo/base/Types.h"

#include <string>
#include <stdint.h>

using std::string;

namespace muduo
{

namespace CurrentThread
{
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char* t_threadName;

//    void cacheTid();

    inline int tid()
    {
        if(!__builtin_expect(t_cachedTid, 0))
        {
//            cacheTid();
        }
        return t_cachedTid;
    }

    inline const char* tidString()
    {
        return t_tidString;
    }

    inline int tidStringLength()
    {
        return t_tidStringLength;
    }

    inline const char* name()
    {
        return t_threadName;
    }

//    bool isMainThread();

//    void sleepUsec(int64_t usec);

    string stackTrace(bool demangle);

}

}

#endif
