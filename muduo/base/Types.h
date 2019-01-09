#ifndef muduo_base_types
#define muduo_base_types

#include <string.h>

namespace muduo
{
    inline void memZero(void* p, size_t n)
    {
        memset(p, 0, n);
    }
}

#endif