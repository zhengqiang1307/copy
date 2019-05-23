#include "muduo/base/FileUtil.h"
#include "muduo/base/Logging.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace muduo;

FileUtil::ReadSmallFile::ReadSmallFile(StringArg filename)
    : fd_(::open(filename.c_str(), O_RDONLY | O_CLOEXEC)), err_(0) {
  buf_[0] = '\0';
  if (fd_ < 0)
    err_ = errno;
}

FileUtil::ReadSmallFile::~ReadSmallFile() {
  if (fd_ >= 0)
    ::close(fd_);
}

int FileUtil::ReadSmallFile::readToBuffer(int *size) {
  int err = err_;
  if (fd_ > 0) {
    // pread() reads up to count bytes from file descriptor fd at offset offset
    // (from the start of the file) into the buffer starting at buf. The file
    // offset is not changed.
    ssize_t n = ::pread(fd_, buf_, sizeof(buf_) - 1, 0);
    if (n >= 0) {
      if (size)
        *size = static_cast<int>(n);
      buf_[n] = '\0';
    } else {
      err = errno;
    }
  }
  return err;
}

template <typename String>
int FileUtil::ReadSmallFile::readToString(int maxSize, String *content,
                                          int64_t *filesize,
                                          int64_t *modifyTime,
                                          int64_t *createTime) {
  static_assert(sizeof(off_t) == 8, "_FILE_OFFSET_BITS = 64");
  assert(content != NULL);

  int err = err_;

  // set filesize, modifyTime and createTime
  if (fd_ > 0) {
    content->clear();
    if (filesize) {
      struct stat statbuf;
      if (::fstat(fd_, &statbuf) == 0) {
        if (S_ISREG(statbuf.st_mode)) {
          *filesize = statbuf.st_size;
          content->reserve(static_cast<int>(
              std::min(implicit_cast<int64_t>(maxSize), *filesize)));
        } else if (S_ISDIR(statbuf.st_mode))
          err = EISDIR;

        if (modifyTime)
          *modifyTime = statbuf.st_mtime;
        if (createTime)
          *createTime = statbuf.st_ctime;
      } else
        err = errno;
    }

    while (content->size() < implicit_cast<int64_t>(maxSize)) {
      size_t toRead = std::min(
          implicit_cast<int64_t>(maxSize) - content->size(), sizeof(buf_));
      ssize_t n = ::read(fd_, buf_, toRead);
      if (n > 0)
        content->append(buf_, n);
      else {
        if (n < 0)
          err = errno;
        break;
      }
    }
  }
  return err;
}

FileUtil::AppendFile::AppendFile(StringArg filename)
    : fp_(::fopen(filename.c_str(), "ae")), writtenBytes_(0) {
  assert(fp_); // fp is a output stream
  ::setbuffer(fp_, buffer_, sizeof(buffer_)); // set file buffer
}

FileUtil::AppendFile::~AppendFile() { ::fclose(fp_); }

void FileUtil::AppendFile::append(const char *logline, size_t len) {
  size_t n = write(logline, len);
  size_t remain = len - n;
  while (remain > 0) {
    // read() does not distinguish between end-of-file and error
    // If an error occurs, or the end of the file is reached,
    // the return value is a short item count (or zero).
    size_t x = write(logline + n, remain);
    if (x == 0) {
      int err = ferror(fp_);
      if (err) {
        fprintf(stderr, "AppendFile::append() failed %s\n", strerror_tl(err));
      }
      break;
    }
    n += x;
    remain = len - n;
  }
  writtenBytes_ += len;
}

void FileUtil::AppendFile::flush() { ::fflush(fp_); }
//force a write of buffer_ for fp_ to physical disk

size_t FileUtil::AppendFile::write(const char *logline, size_t len) {
  return ::fwrite_unlocked(logline, 1, len, fp_);
}
