#ifndef ATOMIC_H
#define ATOMIC_H

#include <muduo/base/noncopyable.h>
#include <stdint.h>

namespace muduo {

namespace detail {

template <typename T> class AtomicIntergerT : noncopyable {
public:
  AtomicIntergerT() : value_(0) {}

  T get() { return __sync_val_compare_and_swap(&value_, 0, 0); }

  T getAndAdd(T x) { return __sync_fetch_and_add(&value_, x); }

  T addAndGet(T x) { return getAndAdd(x) + x; }

  T incrementAndGet() { return addAndGet(1); }

  T decrementAndGet() { return addAndGet(-1); }

  void add(T x) { getAndAdd(x); }

  void increment() { incrementAndGet(); }

  void decrement() { decrementAndGet(); }

  T getAndSet(T newValue) {
    return __sync_lock_test_and_set(&value_, newValue);
  }

private:
  volatile T value_;
};

} // namespace detail

using AtomicInt32 = detail::AtomicIntergerT<int32_t>;
using AtomicInt64 = detail::AtomicIntergerT<int64_t>;

} // namespace muduo

#endif // ATOMIC_H
