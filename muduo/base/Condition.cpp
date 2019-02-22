#include <muduo/base/Condition.h>

bool muduo::Condition::waitForSeconds(double seconds) {
  struct timespec abstime;
  clock_gettime(CLOCK_REALTIME, &abstime);

  const int64_t KNanoSecondsPerSecond = 1000000000;
  int64_t nanoseconds = static_cast<int64_t>(seconds * KNanoSecondsPerSecond);

  abstime.tv_sec = (abstime.tv_nsec + nanoseconds) / KNanoSecondsPerSecond;
  abstime.tv_nsec = (abstime.tv_nsec + nanoseconds) % KNanoSecondsPerSecond;

  MutexLock::UnassignGuard ug(mutex_);
  return ETIMEDOUT ==
         pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);
}
