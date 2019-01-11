#ifndef muduo_base_mutex
#define muduo_base_mutex

#include <assert.h>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/noncopyable.h>
#include <pthread.h>

// Enable thread safety attributes only with clang.
// The attributes can be safely erased when compiling with other compilers.
#if defined(__clang__) && (!defined(SWIG))
#define THREAD_ANNOTATION_ATTRIBUTE__(x) __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x) // no-op
#endif

#define CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

#define SCOPED_CAPABILITY THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

#define GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

#define PT_GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

#define ACQUIRED_BEFORE(...)                                                   \
  THREAD_ANNOTATION_ATTRIBUTE__(acquired_before(__VA_ARGS__))

#define ACQUIRED_AFTER(...)                                                    \
  THREAD_ANNOTATION_ATTRIBUTE__(acquired_after(__VA_ARGS__))

#define REQUIRES(...)                                                          \
  THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

#define REQUIRES_SHARED(...)                                                   \
  THREAD_ANNOTATION_ATTRIBUTE__(requires_shared_capability(__VA_ARGS__))

#define ACQUIRE(...)                                                           \
  THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

#define ACQUIRE_SHARED(...)                                                    \
  THREAD_ANNOTATION_ATTRIBUTE__(acquire_shared_capability(__VA_ARGS__))

#define RELEASE(...)                                                           \
  THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

#define RELEASE_SHARED(...)                                                    \
  THREAD_ANNOTATION_ATTRIBUTE__(release_shared_capability(__VA_ARGS__))

#define TRY_ACQUIRE(...)                                                       \
  THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

#define TRY_ACQUIRE_SHARED(...)                                                \
  THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_shared_capability(__VA_ARGS__))

#define EXCLUDES(...) THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define ASSERT_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(assert_capability(x))

#define ASSERT_SHARED_CAPABILITY(x)                                            \
  THREAD_ANNOTATION_ATTRIBUTE__(assert_shared_capability(x))

#define RETURN_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))

#define NO_THREAD_SAFETY_ANALYSIS                                              \
  THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)

#ifdef CHECK_PTHREAD_RETURN_VALUE

#ifdef NDEBUG
/* This __assert_perror_fail prints an "Assertion failed" message and aborts.
   In installed assert.h this is only declared in debug mode,
   so it has to be copyed here from assert.h file.  */
__BEGIN_DECLS
extern void __assert_perror_fail(int errnum, const char *file,
                                 unsigned int line,
                                 const char *function) noexcept
    __attribute__((__noreturn__));
__END_DECLS

#endif

#define MCHECK(ret)                                                            \
  ({                                                                           \
    __typeof__(ret) errnum = (ret);                                            \
    if (__builtin_expect(errnum != 0, 0))                                      \
      __assert_perror_fail(errnum, __FILE__, __LINE__, __func__);              \
  })

#else // CHECK_PTHREAD_RETURN_VALUE

#define MCHECK(ret)                                                            \
  ({                                                                           \
    __typeof__(ret) errnum = (ret);                                            \
    assert(errnum == 0);                                                       \
    (void)errnum;                                                              \
  })

#endif // CHECK_PTHREAD_RETURN_VALUE

namespace muduo {
/*
CAPABILITY is an attribute on classes, which specifies that objects of the class
can be used as a capability. The string argument specifies the kind of
capability in error messages
*/
class CAPABILITY("mutex") MutexLock : public noncopyable {
public:
  MutexLock() : holder_(0) { MCHECK(pthread_mutex_init(&mutex_, NULL)); }

  ~MutexLock() {
    assert(holder_ = 0);
    MCHECK(pthread_mutex_destroy(&mutex_));
  }

  bool isLockedByThisThread() const { return holder_ == CurrentThread::tid(); }

  // These are attributes on a function or method that does a run-time test to
  // see whether the calling thread holds the given capability.
  // The function is assumed to fail (no return) if the capability is not held
  void assertLocked() const ASSERT_CAPABILITY(this) {
    assert(isLockedByThisThread());
  }

private:
  pid_t holder_;
  pthread_mutex_t mutex_;

public:
  /*
  ACQUIRE is an attribute on functions or methods, which declares that the
  function acquires a capability, but does not release it. The caller must not
  hold the given capability on entry, and it will hold the capability on exit.
  ACQUIRE_SHARED is similar.

  RELEASE and RELEASE_SHARED declare that the function releases the given
  capability. The caller must hold the capability on entry, and will no longer
  hold it on exit. It does not matter whether the given capability is shared or
  exclusive.
  */
  void assignHolder() { holder_ = CurrentThread::tid(); }

  void unassignHolder() { holder_ = 0; }

  void lock() ACQUIRE() {
    MCHECK(pthread_mutex_lock(&mutex_));
    assignHolder();
  }

  void unlock() RELEASE() {
    unassignHolder();
    MCHECK(pthread_mutex_unlock(&mutex_));
  }

  pthread_mutex_t *getPthreadMutex() { return &mutex_; }

private:
  friend class Condition;

  class UnassignGuard : noncopyable {
  public:
    explicit UnassignGuard(MutexLock &owner) : owner_(owner) {
      owner.unassignHolder();
    }

    ~UnassignGuard() { owner_.assignHolder(); }

  private:
    MutexLock &owner_;
  };
};
// SCOPED_CAPABILITY is an attribute on classes that implement RAII-style
// locking, in which a capability is acquired in the constructor, and released
// in the destructor.  Such classes require special handling because the
// constructor and destructor refer to the capability via different names;

class SCOPED_CAPABILITY MutexLockGuard : noncopyable {
public:
  explicit MutexLockGuard(MutexLock &mutex) ACQUIRE(mutex) : mutex_(mutex) {
    mutex_.lock();
  }

  ~MutexLockGuard() RELEASE() { mutex_.unlock(); }

private:
  MutexLock &mutex_;
};

} // namespace muduo

// prevent misuse like:
// MutexLockGuard(mutex_)
// a tempory object does not hold the lock for long!
#define MutexLockGuard(x) error "Missing guard object name"

#endif // muduo_base_mutex
