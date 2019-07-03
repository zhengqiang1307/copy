#ifndef BUFFER_H
#define BUFFER_H

#include "muduo/base/StringPiece.h"
#include "muduo/base/Types.h"
#include "muduo/base/copyable.h"

#include "muduo/net/Endian.h"

#include <algorithm>
#include <vector>

#include <assert.h>
#include <string.h>

namespace muduo {
namespace net {

class Buffer : public muduo::copyable {
private:
  std::vector<char> buffer_;
  size_t readIndex_;
  size_t writeIndex_;
  static const char kCRLF[];

  const char *begin() const { return buffer_.data(); }
  char *begin() { return buffer_.data(); }
  void makeSpace(size_t len) {
    if (prependableBytes() + writableBytes() < len + kCheapPrepend) {
      buffer_.resize(writeIndex_ + len);
    } else {
      assert(kCheapPrepend < readIndex_);
      size_t readable = readableBytes();
      std::copy(begin() + readIndex_, begin() + writeIndex_,
                begin() + kCheapPrepend);
      readIndex_ = kCheapPrepend;
      writeIndex_ = readIndex_ + readable;
      assert(readable == readableBytes());
    }
  }

public:
  static const size_t kCheapPrepend = 8;
  static const size_t kInitialSize = 1024;

  explicit Buffer(size_t initialSize = kInitialSize)
      : buffer_(initialSize + kCheapPrepend), readIndex_(kCheapPrepend),
        writeIndex_(kCheapPrepend) {}
  // implicit copy-ctor, copy assignment, move-ctor, move assignment and dtor
  // are fine
  void swap(Buffer &rhs) {
    buffer_.swap(rhs.buffer_);
    std::swap(readIndex_, rhs.readIndex_);
    std::swap(writeIndex_, rhs.writeIndex_);
  }
  size_t readableBytes() const { return writeIndex_ - readIndex_; }
  size_t writableBytes() const { return buffer_.size() - writeIndex_; }
  size_t prependableBytes() const { return readIndex_; }

  const char *peek() const { return begin() + readIndex_; }
  const char *beginWrite() const { return begin() + writeIndex_; }
  char *beginWrite() { return begin() + writeIndex_; }
  const char *findCRLF() const {
    const char *crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
    return crlf == beginWrite() ? nullptr : crlf;
  }
  const char *findCRLF(const char *start) const {
    assert(start >= peek());
    assert(start < beginWrite());
    const char *crlf = std::search(start, beginWrite(), kCRLF, kCRLF + 2);
    return crlf == beginWrite() ? nullptr : crlf;
  }
  const char *findEOL() const {
    const void *eol = memchr(peek(), '\n', readableBytes());
    return static_cast<const char *>(eol);
  }
  const char *findEOL(const char *start) const {
    assert(start >= peek());
    assert(start < beginWrite());
    const void *eol =
        memchr(start, '\n', static_cast<size_t>((beginWrite() - start)));
    return static_cast<const char *>(eol);
  }
  void retrieveAll() {
    readIndex_ = kCheapPrepend;
    writeIndex_ = kCheapPrepend;
  }
  void retrieve(size_t len) {
    assert(len <= readableBytes());
    if (len < readableBytes()) {
      readIndex_ += len;
    } else {
      retrieveAll();
    }
  }
  void retrieveUntil(const char *end) {
    assert(end >= peek());
    assert(end <= beginWrite());
    retrieve(static_cast<size_t>(end - peek()));
  }
  void retrieveInt64() { retrieve(sizeof(int64_t)); }
  void retrieveInt32() { retrieve(sizeof(int32_t)); }
  void retrieveInt16() { retrieve(sizeof(int16_t)); }
  void retrieveInt8() { retrieve(sizeof(int8_t)); }
  string retrieveAsString(size_t len) {
    assert(len <= readableBytes());
    string result(peek(), len);
    retrieve(len);
    return result;
  }
  string retrieveAllString() { return retrieveAsString(readableBytes()); }
  StringPiece toStringPiece() const {
    return StringPiece(peek(), static_cast<int>(readableBytes()));
  }
  void hasWriten(size_t len) {
    assert(len <= writableBytes());
    writeIndex_ += len;
  }
  void ensureWritableBytes(size_t len) {
    if (writableBytes() < len)
      makeSpace(len);
    assert(writableBytes() >= len);
  }
  void append(const char *data, size_t len) {
    ensureWritableBytes(len);
    std::copy(data, data + len, beginWrite());
    hasWriten(len);
  }
  void append(const StringPiece &str) {
    append(str.data(), static_cast<size_t>(str.size()));
  }
  void append(const void *data, size_t len) {
    append(static_cast<const char *>(data), len);
  }
  // read data directly into buffer
  ssize_t readFd(int fd, int *savedErrno);

  void unwrite(size_t len) {
    assert(len <= readableBytes());
    writeIndex_ -= len;
  }

  ///
  /// Append int64_t using network endian
  ///
  void appendInt64(int64_t x) {
    int64_t be64 = sockets::hostToNetwork64(x);
    append(&be64, sizeof be64);
  }

  ///
  /// Append int32_t using network endian
  ///
  void appendInt32(int32_t x) {
    int32_t be32 = sockets::hostToNetwork32(x);
    append(&be32, sizeof be32);
  }

  void appendInt16(int16_t x) {
    int16_t be16 = sockets::hostToNetwork16(x);
    append(&be16, sizeof be16);
  }

  void appendInt8(int8_t x) { append(&x, sizeof x); }

  ///
  /// Read int64_t from network endian
  ///
  /// Require: buf->readableBytes() >= sizeof(int32_t)
  int64_t readInt64() {
    int64_t result = peekInt64();
    retrieveInt64();
    return result;
  }

  ///
  /// Read int32_t from network endian
  ///
  /// Require: buf->readableBytes() >= sizeof(int32_t)
  int32_t readInt32() {
    int32_t result = peekInt32();
    retrieveInt32();
    return result;
  }

  int16_t readInt16() {
    int16_t result = peekInt16();
    retrieveInt16();
    return result;
  }

  int8_t readInt8() {
    int8_t result = peekInt8();
    retrieveInt8();
    return result;
  }

  ///
  /// Peek int64_t from network endian
  ///
  /// Require: buf->readableBytes() >= sizeof(int64_t)
  int64_t peekInt64() const {
    assert(readableBytes() >= sizeof(int64_t));
    int64_t be64 = 0;
    ::memcpy(&be64, peek(), sizeof be64);
    return sockets::networkToHost64(be64);
  }

  ///
  /// Peek int32_t from network endian
  ///
  /// Require: buf->readableBytes() >= sizeof(int32_t)
  int32_t peekInt32() const {
    assert(readableBytes() >= sizeof(int32_t));
    int32_t be32 = 0;
    ::memcpy(&be32, peek(), sizeof be32);
    return sockets::networkToHost32(be32);
  }

  int16_t peekInt16() const {
    assert(readableBytes() >= sizeof(int16_t));
    int16_t be16 = 0;
    ::memcpy(&be16, peek(), sizeof be16);
    return sockets::networkToHost16(be16);
  }

  int8_t peekInt8() const {
    assert(readableBytes() >= sizeof(int8_t));
    int8_t x = *peek();
    return x;
  }

  ///
  /// Prepend int64_t using network endian
  ///
  void prependInt64(int64_t x) {
    int64_t be64 = sockets::hostToNetwork64(x);
    prepend(&be64, sizeof be64);
  }

  ///
  /// Prepend int32_t using network endian
  ///
  void prependInt32(int32_t x) {
    int32_t be32 = sockets::hostToNetwork32(x);
    prepend(&be32, sizeof be32);
  }

  void prependInt16(int16_t x) {
    int16_t be16 = sockets::hostToNetwork16(x);
    prepend(&be16, sizeof be16);
  }

  void prependInt8(int8_t x) { prepend(&x, sizeof x); }

  void prepend(const void * /*restrict*/ data, size_t len) {
    assert(len <= prependableBytes());
    readIndex_ -= len;
    const char *d = static_cast<const char *>(data);
    std::copy(d, d + len, begin() + readIndex_);
  }

  void shrink(size_t reserve) {
    // FIXME: use vector::shrink_to_fit() in C++ 11 if possible.
    Buffer other;
    other.ensureWritableBytes(readableBytes() + reserve);
    other.append(toStringPiece());
    swap(other);
  }

  size_t internalCapacity() const { return buffer_.capacity(); }

  string retrieveAllAsString()
    {
      return retrieveAsString(readableBytes());
    }
};

} // namespace net
} // namespace muduo

#endif // BUFFER_H
