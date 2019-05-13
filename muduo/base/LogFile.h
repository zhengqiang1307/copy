#ifndef LOGFILE_H
#define LOGFILE_H

#include "muduo/base/Mutex.h"
#include "muduo/base/Types.h"

#include <memory>

namespace muduo {

namespace FileUtil {
class AppendFile;
}

class LogFile : noncopyable {
public:
  LogFile(const string &basename, off_t rollSize, bool threadSafe = true,
          int flushInterval = 3, int checkEveryN = 1024);

  ~LogFile() = default;
  void append(const char *logline, int len);
  void flush();
  bool rollFile();

private:
  void append_unlocked(const char *logline, int len);
  static string getLogFileName(const string &basename, time_t *now);
  const string basename_;
  const off_t rollSize_;
  const int flushInverval_;
  const int checkEveryN_;

  int count_;

  std::unique_ptr<MutexLock> mutex_;
  time_t startOfPeriod_;
  time_t lastRoll_;
  time_t lastFlush_;
  std::unique_ptr<FileUtil::AppendFile> file_;
  const static int kRollPerSeconds_ = 60 * 60 * 24;
};

} // namespace muduo

#endif // LOGFILE_H
