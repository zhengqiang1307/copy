#ifndef GZIPFILE_H
#define GZIPFILE_H

#include "muduo/base/StringPiece.h"
#include "muduo/base/noncopyable.h"

#include <zlib.h>

namespace muduo {

class GzipFile : noncopyable {
public:
  GzipFile(GzipFile &&rhs) noexcept : file_(rhs.file_) { rhs.file_ = nullptr; }
  ~GzipFile() {
    if (file_)
      gzclose(file_);
  }
  GzipFile &operator=(GzipFile &&rhs) noexcept {
    std::swap(file_, rhs.file_);
    return *this;
  }

  bool valid() const { return file_ != nullptr; }
  void swap(GzipFile &rhs) { std::swap(file_, rhs.file_); }
#if ZLIB_VERNUM >= 0x1240
  bool setBuffer(int size) { return ::gzbuffer(file_, size) == 0; }
#endif
  int read(void *buf, int len) { return ::gzread(file_, buf, len); }
  int write(StringPiece buf) {
    return ::gzwrite(file_, buf.data(), buf.size());
  }
  off64_t tell() const { return ::gztell64(file_); }
#if ZLIB_VERNUM >= 0x1240
  off64_t offset() const { return ::gzoffset64(file_); }
#endif
  static GzipFile openForRead(StringPiece filename) {
    return GzipFile(::gzopen(filename.data(), "rbe"));
  }
  static GzipFile openForAppend(StringPiece filename) {
    return GzipFile(::gzopen(filename.data(), "abe"));
  }
  static GzipFile openForWriteExclusive(StringPiece filename) {
    return GzipFile(::gzopen(filename.data(), "wbxe"));
  }
  static GzipFile openForWriteTruncate(StringPiece filename) {
    return GzipFile(::gzopen(filename.data(), "wbe"));
  }

private:
  explicit GzipFile(gzFile file) : file_(file) {}
  gzFile file_;
};

} // namespace muduo

#endif // GZIPFILE_H
