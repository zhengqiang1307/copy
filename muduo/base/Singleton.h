#ifndef SINGLETON_H
#define SINGLETON_H

#include <assert.h>
#include <muduo/base/noncopyable.h>
#include <pthread.h>
#include <stdlib.h> // atexit

namespace muduo {

namespace detail {

template <typename T> struct has_no_destory {
  template <typename C> static char test(decltype(&C::no_destory));

  template <typename C> static int32_t test(...);

  const static bool value = sizeof(test(0)) == 1;
};

} // namespace detail

template <typename T> class Singleton : public noncopyable {
public:
  Singleton() = delete;
  ~Singleton() = delete;

  static T &instance() {
    pthread_once(&ponce_, init);
    assert(value_ != NULL);
    return *value_;
  }

  static void init() {
    value_ = new T();
    if (!detail::has_no_destory<T>::value) {
      ::atexit(destory);
    }
  }

  static void destory() {
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
    T_must_be_complete_type dummy;
    (void)dummy;
    delete value_;
    value_ = NULL;
  }

private:
  static pthread_once_t ponce_;
  static T *value_;
};

template <typename T> pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template <typename T> T *Singleton<T>::value_ = NULL;
} // namespace muduo

#endif // SINGLETON_H
