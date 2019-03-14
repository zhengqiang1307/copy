#ifndef THREADLOCAL_H
#define THREADLOCAL_H

#include "muduo/base/noncopyable.h"
#include <pthread.h>

namespace muduo {

template <typename T> class ThreadLocal {

private:

  pthread_key_t key_;
  static void destory(void *x) {
    T *obj = static_cast<T *>(x);
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
    T_must_be_complete_type tt;
    (void)tt;
    delete obj;
  }

public:

  ThreadLocal() { pthread_key_create(&key_, ThreadLocal::destory); }
  ~ThreadLocal() { pthread_key_delete(&key_); }

  T &value() {
    T *obj = static_cast<T *>(pthread_getspecific(key_));
    if (!obj) {
      obj = new T();
      pthread_setspecific(key_, obj);
    }
    return *obj;
  }

};

} // namespace muduo

#endif // THREADLOCAL_H
