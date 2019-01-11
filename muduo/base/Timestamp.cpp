#include <muduo/base/Timestamp.h>

using namespace muduo;
using namespace std;

string Timestamp::toString() const { return string(); }

string Timestamp::toFormattedString(bool showMicroseconds) const {}

int64_t Timestamp::microSecondsSinceEpoch() const {
  return microSecondsSinceEpoch_;
}

Timestamp Timestamp::now() { return Timestamp(); }
