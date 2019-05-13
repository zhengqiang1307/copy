#include "muduo/base/LogStream.h"
#include <algorithm>
#include <assert.h>
#include <limits>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <type_traits>

using namespace muduo;
using namespace muduo::detail;

// TODO: better itoa.
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wtautological-compare"
#else
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

namespace muduo {
namespace detail {

const char digits[] = "9876543210123456789";
const char *zero = digits + 9;
static_assert(sizeof(digits) == 20, "wrong number of digits");

const char digitsHex[] = "0123456789ABCDEF";
static_assert(sizeof(digitsHex) == 17, "wrong number of digitsHex");

// efficient integer to string conversions, by matthew wilson
template <typename T> size_t convert(char buf[], T value) {
  T i = value;
  char *p = buf;
  do {
    int lsd = static_cast<int>(i % 10);
    i /= 10;
    *p++ = zero[lsd];
  } while (i != 0);

  if (value < 0) {
    *p++ = '-';
  }
  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

size_t convertHex(char buf[], uintptr_t value) {
  uintptr_t i = value;
  char *p = buf;
  do {
    int lsd = i % 16;
    i /= 16;
    *p++ = digitsHex[lsd];
  } while (i != 0);
  *p = '\0';
  std::reverse(buf, p);
  return p - buf;
}

template class FixedBuffer<KSmallBuffer>;
template class FixedBuffer<KLargeBuffer>;

} // namespace detail
} // namespace muduo

template <int SIZE> const char *FixedBuffer<SIZE>::debugString() {
  *cur_ = '\0';
    return data_;
}

template<int SIZE>
void FixedBuffer<SIZE>::cookieStart()
{

}

template<int SIZE>
void FixedBuffer<SIZE>::cookieEnd()
{

}

template <typename T> void LogStream::formatInteger(T v) {
  if (buffer_.avail() >= kMaxNumericSize) {
    size_t len = convert(buffer_.current(), v);
    buffer_.add(len);
  }
}

LogStream &LogStream::operator<<(short v) {
  *this << static_cast<int>(v);
  return *this;
}

LogStream::self &LogStream::operator<<(unsigned short v) {
  *this << static_cast<unsigned int>(v);
  return *this;
}

LogStream::self &LogStream::operator<<(int v) {
  formatInteger(v);
  return *this;
}

LogStream::self &LogStream::operator<<(unsigned int v) {
  formatInteger(v);
  return *this;
}

LogStream::self &LogStream::operator<<(long v) {
  formatInteger(v);
  return *this;
}

LogStream::self &LogStream::operator<<(unsigned long v) {
  formatInteger(v);
  return *this;
}

LogStream::self &LogStream::operator<<(long long v) {
  formatInteger(v);
  return *this;
}

LogStream::self &LogStream::operator<<(unsigned long long v) {
  formatInteger(v);
  return *this;
}

LogStream::self &LogStream::operator<<(const void *p) {
  uintptr_t i = reinterpret_cast<uintptr_t>(p);

  if (buffer_.avail() >= kMaxNumericSize) {
    char *cur = buffer_.current();
    *cur++ = '0';
    *cur++ = 'x';

    size_t len = convertHex(cur, i);
    buffer_.add(len + 2);
  }
  return *this;
}

LogStream::self &LogStream::operator<<(double v) {
  if (buffer_.avail() >= kMaxNumericSize) {
    int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
    buffer_.add(len);
  }
  return *this;
}

template <typename T> Fmt::Fmt(const char *fmt, T val) {
  static_assert(std::is_arithmetic<T>::value == true,
                "must be arithmetic type");
  length_ = snprintf(buf_, sizeof buf_, fmt, val);
  assert(static_cast<size_t>(length_) < sizeof buf_);
}

template Fmt::Fmt(const char *fmt, char val);
template Fmt::Fmt(const char *fmt, unsigned short val);
template Fmt::Fmt(const char *fmt, short val);
template Fmt::Fmt(const char *fmt, unsigned int val);
template Fmt::Fmt(const char *fmt, int val);
template Fmt::Fmt(const char *fmt, unsigned long val);
template Fmt::Fmt(const char *fmt, long val);
template Fmt::Fmt(const char *fmt, unsigned long long val);
template Fmt::Fmt(const char *fmt, long long val);
template Fmt::Fmt(const char *fmt, double val);
template Fmt::Fmt(const char *fmt, float val);

