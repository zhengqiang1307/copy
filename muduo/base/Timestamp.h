#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <boost/operators.hpp>
#include <muduo/base/copyable.h>
#include <stdint.h>
#include <string>

namespace muduo {

class Timestamp : public muduo::copyable,
                  public boost::equality_comparable<Timestamp>,
                  public boost::less_than_comparable<Timestamp> {
public:
  Timestamp() : microSecondsSinceEpoch_(0) {}
  Timestamp(int64_t microSecondsSinceEpochArg)
      : microSecondsSinceEpoch_(microSecondsSinceEpochArg) {}

  void swap(Timestamp &that) {
    std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
  }

  std::string toString() const;

  std::string toFormattedString(bool showMicroseconds = true) const;

  bool valid() const { return microSecondsSinceEpoch_ > 0; }

  int64_t microSecondsSinceEpoch() const;

  static const int64_t KMicroSecondsPerSecond = 1000 * 1000;

  time_t secondsSinceEpoch() const {
    return microSecondsSinceEpoch_ / KMicroSecondsPerSecond;
  }

  static Timestamp now();
  static Timestamp invalid() { return Timestamp(); }
  static Timestamp fromUnixTime(time_t t, int microSeconds) {
    return Timestamp(int64_t(t) * KMicroSecondsPerSecond + microSeconds);
  }
  static Timestamp fromUnixTime(time_t t) { return fromUnixTime(t, 0); }

private:
  int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs) {
  return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs) {
  return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline double timeDifference(Timestamp high, Timestamp low) {
  int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
  return static_cast<double>(diff / Timestamp::KMicroSecondsPerSecond);
}

inline Timestamp addTime(Timestamp timestamp, double seconds) {
  int64_t microSeconds = static_cast<int64_t>(seconds * Timestamp::KMicroSecondsPerSecond);
  return Timestamp(timestamp.microSecondsSinceEpoch() + microSeconds);
}

} // namespace muduo
#endif // TIMESTAMP_H
