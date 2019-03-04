#include "muduo/base/Exception.h"
#include "muduo/base/CurrentThread.h"

namespace muduo {

Exception::Exception(string what)
    : message_(std::move(what)), stack_(CurrentThread::stackTrace(false)) {}

} // namespace muduo
