#ifndef THREADLOCALSINGLETON_H
#define THREADLOCALSINGLETON_H

#include <assert.h>
#include <pthread.h>

namespace muduo {

template <typename T> class ThreadLocalSingleton {
public:
  ThreadLocalSingleton() = delete;
  ~ThreadLocalSingleton() = delete;

  static T &instance() {
    if (t_value_ == nullptr) {
      t_value_ = new T();
      deleter_.setDeleter(t_value_);
    }
    return *t_value_;
  }

private:
  static void destroy(void *obj) {
    T *t = static_cast<T*>(obj);
    char T_MUST_BE_COMPELETED_TYPE[sizeof(obj) == 0 ? -1 : 1];
    (void)T_MUST_BE_COMPELETED_TYPE;
    delete t;
  }

  class Deleter {
  public:
    Deleter() { pthread_key_create(&key_, ThreadLocalSingleton::destroy); }

    ~Deleter() { pthread_key_delete(key_); }

    void setDeleter(T *obj) {
      assert(obj == t_value_);
      assert(pthread_getspecific(key_) == nullptr);
      pthread_setspecific(key_, obj);
    }

  private:
    pthread_key_t key_;
  };

private:
  static __thread T *t_value_;
  static Deleter deleter_;
};

template <typename T> __thread T *ThreadLocalSingleton<T>::t_value_ = nullptr;

template <typename T> typename ThreadLocalSingleton<T>::Deleter ThreadLocalSingleton<T>::deleter_;
} // namespace muduo

#endif // THREADLOCALSINGLETON_H
