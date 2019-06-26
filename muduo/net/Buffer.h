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

  const char* begin() const { return buffer_.data(); }
  char* begin() { return buffer_.data(); }
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
      : buffer_(initialSize + kCheapPrepend),
        readIndex_(kCheapPrepend),
        writeIndex_(kCheapPrepend) {}
  // implicit copy-ctor, copy assignment, move-ctor, move assignment and dtor
  // are fine
  void swap(Buffer& rhs) {
    buffer_.swap(rhs.buffer_);
    std::swap(readIndex_, rhs.readIndex_);
    std::swap(writeIndex_, rhs.writeIndex_);
  }
  size_t readableBytes() const { return writeIndex_ - readIndex_; }
  size_t writableBytes() const { return buffer_.size() - writeIndex_; }
  size_t prependableBytes() const { return readIndex_; }

  const char* peek() const { return begin() + readIndex_; }
  const char* beginWrite() const { return begin() + writeIndex_; }
  char* beginWrite()  { return begin() + writeIndex_; }
  const char* findCRLF() const {
    const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
    return crlf == beginWrite() ? nullptr : crlf;
  }
  const char* findCRLF(const char* start) const {
    assert(start >= peek());
    assert(start < beginWrite());
    const char* crlf = std::search(start, beginWrite(), kCRLF, kCRLF + 2);
    return crlf == beginWrite() ? nullptr : crlf;
  }
  const char* findEOL() const {
    const void* eol = memchr(peek(), '\n', readableBytes());
    return static_cast<const char*>(eol);
  }
  const char* findEOL(const char* start) const {
    assert(start >= peek());
    assert(start < beginWrite());
    const void* eol = memchr(start, '\n', static_cast<size_t>((beginWrite() - start)));
    return static_cast<const char*>(eol);
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
  void retrieveUntil(const char* end) {
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
    if (writableBytes() < len) makeSpace(len);
    assert(writableBytes() >= len);
  }
  void append(const char* data, size_t len) {
    ensureWritableBytes(len);
    std::copy(data, data + len, beginWrite());
    hasWriten(len);
  }
  void append(const StringPiece& str) { append(str.data(), static_cast<size_t>(str.size())); }
  void append(const void* data, size_t len) {
    append(static_cast<const char*>(data), len);
  }
};

}  // namespace net
}  // namespace muduo

#endif  // BUFFER_H
