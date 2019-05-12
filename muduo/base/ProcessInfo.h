#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <muduo/base/StringPiece.h>
#include <muduo/base/Timestamp.h>
#include <muduo/base/Types.h>

#include <sys/types.h>
#include <vector>

namespace muduo {
namespace ProcessInfo {

pid_t pid();
string pidString();
uid_t uid();
string username();
uid_t euid();
Timestamp startTime();
int clockTicksPerSecond();
int pageSize();
bool isDebugBuild();
int pageSize();
bool isDebugBuild(); // constexpr

string hostname();
string procname();
StringPiece procname(const string &stat);

// read /proc/self/status
string procStatus();

// read /proc/self/stat
string procStat();

// read /proc/self/task/tid/stat
string threadStat();

// readlink /proc/self/exe
string exePath();

int openedFiles();
int maxOpenFiles();

struct CpuTime {
  double userSeconds;
  double systemSeconds;
  CpuTime() : userSeconds(0.0), systemSeconds(0.0) {}
  double total() const { return userSeconds + systemSeconds; }
};

CpuTime cupTime();

int numThreads();
std::vector<pid_t> threads();

} // namespace ProcessInfo

} // namespace muduo

#endif // PROCESSINFO_H
