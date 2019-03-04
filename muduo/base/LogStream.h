#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include "muduo/base/StringPiece.h"
#include "muduo/base/Types.h"
#include "muduo/base/noncopyable.h"

#include <assert.h>
#include <string>

namespace muduo {

namespace detail {

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template <int SIZE> class FixedBuffer : noncopyable {
private:
  char *test_;
  char *cur_;
  char data_[SIZE];
  void (*cookie_)();

  const char *end() const { return data_ + sizeof data_; }
  static void cookieStart();
  static void cookieEnd();

public:
  FixedBuffer() : cur_(data_) { setCookie(cookieStart); }
  ~FixedBuffer() { setCookie(cookieEnd); }

  int avail() const { return static_cast<int>(end() - cur_); }

  void append(const char *buf, size_t len) {
    if (static_cast<size_t>(avail()) > len) {
      memcpy(cur_, buf, len);
      cur_ += len;
    }
  }

  void setCookie(void (*cookie)()) { cookie_ = cookie; };

  const char *data() const { return data_; }
  int length() const { return static_cast<int>(cur_ - data_); }
  char *current() { return cur_; }
  void add(size_t len) { cur_ += len; }
  void reset() { cur_ = data_; }
  void bzero() { memZero(data_, sizeof data_); }

  //  for used by gdb
  const char *debugString();
  string toString() const { return string(data_, length()); }
  StringPiece toStringPiece() const { return StringPiece(data_, length()); }
};
} // namespace detail

class LogStream : noncopyable {
  typedef LogStream self;

public:
  typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;
  self &operator<<(bool v) {
    buffer_.append(v ? "1" : "0", 1);
    return *this;
  }

  self &operator<<(short);
  self &operator<<(unsigned short);
  self &operator<<(int);
  self &operator<<(unsigned int);
  self &operator<<(long);
  self &operator<<(unsigned long);
  self &operator<<(long long);
  self &operator<<(unsigned long long);
  self &operator<<(const void *);
  self &operator<<(double);
  self &operator<<(float v) {
    *this << static_cast<double>(v);
    return *this;
  }
  self &operator<<(char v) {
    buffer_.append(&v, 1);
    return *this;
  }
  self &operator<<(const char *str) {
    if (str) {
      buffer_.append(str, strlen(str));
    } else {
      buffer_.append("(null)", 6);
    }
    return *this;
  }
  self &operator<<(const string &v) {
    buffer_.append(v.c_str(), v.size());
    return *this;
  }
  self &operator<<(const StringPiece &v) {
    buffer_.append(v.data(), v.size());
    return *this;
  }
  self &operator<<(const Buffer &v) {
    *this << v.toString();
    return *this;
  }
  void append(const char *data, int len) { buffer_.append(data, len); }
  const Buffer &buffer() const { return buffer_; }
  void resetBuffer() { buffer_.reset(); }

private:
  void staticCheck();
  template <typename T> void formatInteger(T);
  Buffer buffer_;
  static const int kMaxNumericSize = 32;
};

class Fmt {
public:
  template <typename T> Fmt(const char *fmt, T val);

  const char *data() const { return buf_; }
  int length() const { return length_; }

private:
  char buf_[32];
  int length_;
};

inline LogStream &operator<<(LogStream &s, const Fmt &fmt) {
  s.append(fmt.data(), fmt.length());
  return s;
}
} // namespace muduo

#endif // LOGSTREAM_H
