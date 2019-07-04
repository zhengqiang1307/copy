#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "muduo/base/Timestamp.h"

#include <functional>
#include <memory>

namespace muduo {

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

template <typename T>
inline T* get_pointer(const std::shared_ptr<T>& ptr) {
  return ptr.get();
}

template <typename T>
inline T* get_pointer(const std::unique_ptr<T>& ptr) {
  return ptr.get();
}

template <typename To, typename From>
inline ::std::shared_ptr<To> down_pointer_cast(
    const ::std::shared_ptr<From>& f) {
  if (false) {
    implicit_cast<From*, To*>(0);
  }
#ifndef NDEBUG
  assert(f == nullptr || dynamic_cast<To*>(get_pointer(f)) != nullptr);
#endif
  return std::static_pointer_cast<To>(f);
}

namespace net {

class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void()> TimerCallback;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void(const TcpConnectionPtr&, size_t)>
    HighWaterMarkCallback;

typedef std::function<void(const TcpConnectionPtr&, Buffer*, Timestamp)>
    MessageCallback;

void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer,
                            Timestamp receiveTime);

}  // namespace net

}  // namespace muduo

#endif  // CALLBACKS_H
