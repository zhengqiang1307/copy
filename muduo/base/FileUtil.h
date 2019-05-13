#ifndef FILEUTIL_H
#define FILEUTIL_H

#include "muduo/base/StringPiece.h"
#include "muduo/base/noncopyable.h"
#include <sys/types.h> // for off_t

namespace muduo {

namespace FileUtil {

// read small file < 64KB
class ReadSmallFile : noncopyable {
public:
  ReadSmallFile(StringArg filename);
  ~ReadSmallFile();

  // return errno
  template <typename String>
  int readToString(int maxSize, String *content, int64_t *filesize,
                   int64_t *modifyTime, int64_t *createTime);
  int readToBuffer(int *size);
  const char *buffer() const { return buf_; }
  static const int kBufferSize = 64 * 1024;

private:
  int fd_;
  int err_;
  char buf_[kBufferSize];
};

template <typename String>
int readFile(StringArg filename, int maxSize, String *content,
             int64_t *filesize = nullptr, int64_t *modifyTime = nullptr,
             int64_t *createTime = nullptr) {
  ReadSmallFile file(filename);
  return file.readToString(maxSize, content, filesize, modifyTime, createTime);
}

class AppendFile : noncopyable {
public:
  explicit AppendFile(StringArg filename);
  ~AppendFile();
  void append(const char *logline, size_t len);
  void flush();
  off_t writtenBytes() const { return writtenBytes_; }

private:
  size_t write(const char *logline, size_t len);
  FILE *fp_;
  char buffer_[64 * 1024];
  off_t writtenBytes_;
};

} // namespace FileUtil

} // namespace muduo

#endif // FILEUTIL_H
