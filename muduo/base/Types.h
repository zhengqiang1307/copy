#ifndef muduo_base_types
#define muduo_base_types

#include <stdint.h>
#include <string.h>
#include <string>

namespace muduo {

using std::string;

inline void memZero(void *p, size_t n) { memset(p, 0, n); }
} // namespace muduo

#endif
