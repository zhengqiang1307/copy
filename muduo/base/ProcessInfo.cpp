#include "muduo/base/ProcessInfo.h"
#include "muduo/base/CurrentThread.h"
#include "muduo/base/FileUtil.h"

#include <algorithm>

#include <assert.h>
#include <dirent.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <unistd.h>

namespace muduo {
namespace detail {

__thread int t_numOpenedFiles = 0;

int fdDirFilter(const struct dirent *d) {
  if (::isdigit(d->d_name[0])) {
    ++t_numOpenedFiles;
  }
  return 0;
}

__thread std::vector<pid_t> *t_pids = nullptr;
int taskDirFilter(const struct dirent *d) {
  if (::isdigit(d->d_name[0])) {
    t_pids->push_back(atoi(d->d_name));
  }
  return 0;
}

int scanDir(const char *dirpath, int (*filter)(const struct dirent *)) {
  struct dirent **namelist = nullptr;
  int result = ::scandir(dirpath, &namelist, filter, alphasort);
  assert(namelist == NULL);
  return result;
}

Timestamp g_startTime = Timestamp::now();
int g_clockTicks = static_cast<int>(::sysconf(_SC_CLK_TCK));
int g_pageSize = static_cast<int>(::sysconf(_SC_PAGE_SIZE));

} // namespace detail

using namespace muduo;
using namespace muduo::detail;

pid_t ProcessInfo::pid() { return ::getpid(); }

std::string ProcessInfo::pidString() {
  char buf[32];
  snprintf(buf, sizeof buf, "%d", getpid());
  return buf;
}

uid_t ProcessInfo::uid() { return ::getuid(); }

std::string ProcessInfo::username() {
  struct passwd pwd;
  struct passwd *result = nullptr;
  char buf[8196];
  const char *name = "unknownuser";

  getpwuid_r(uid(), &pwd, buf, sizeof buf, &result);
  if (result)
    name = pwd.pw_name;
  return name;
}

uid_t ProcessInfo::euid() { return ::geteuid(); }

Timestamp ProcessInfo::startTime() { return g_startTime; }

int ProcessInfo::clockTicksPerSecond() { return g_clockTicks; }

int ProcessInfo::pageSize() { return g_pageSize; }

bool ProcessInfo::isDebugBuild() {
#ifdef NDBUG
  return false;
#else
  return true;
#endif
}

std::string ProcessInfo::hostname() {
  char buf[256];
  if (::gethostname(buf, sizeof buf) == 0) {
    buf[sizeof(buf) - 1] = '\0';
    return buf;
  } else
    return "unknownhost";
}

std::string ProcessInfo::procname() { return procname(procStat()).as_string(); }

StringPiece ProcessInfo::procname(const std::string &stat) {
  StringPiece name;
  size_t lp = stat.find('(');
  size_t rp = stat.find(')');
  if (lp != string::npos && rp != string::npos && lp < rp)
    name.set(stat.data() + lp + 1, static_cast<int>(rp - lp - 1));
  return name;
}

std::string ProcessInfo::procStat() {
  string result;
  FileUtil::readFile("/proc/self/stat", 65536, &result);
  return result;
}

std::string ProcessInfo::procStatus() {
  string result;
  FileUtil::readFile("/proc/self/status", 65536, &result);
  return result;
}

std::string ProcessInfo::threadStat() {
  char buf[64];
  snprintf(buf, sizeof buf, "/proc/self/task/%d/stat", CurrentThread::tid());
  string result;
  FileUtil::readFile(buf, 65536, &result);
  return result;
}

std::string ProcessInfo::exePath() {
  string result;
  char buf[1024];
  ssize_t n = readlink("/proc/self/exe", buf, sizeof buf);
  if (n > 0)
    result.assign(buf, n);
  return result;
}

int ProcessInfo::openedFiles() {
  t_numOpenedFiles = 0;
  scanDir("/proc/self/fd", fdDirFilter);
  return t_numOpenedFiles;
}

int ProcessInfo::maxOpenFiles() {
  struct rlimit rl;
  if (::getrlimit(RLIMIT_NOFILE, &rl))
    return openedFiles();
  else
    return static_cast<int>(rl.rlim_cur);
}

int ProcessInfo::numThreads() {
  int result = 0;
  string status = procStatus();
  size_t pos = status.find("Threads:");
  if (pos != string::npos)
    result = ::atoi(status.c_str() + pos + 8);
  return result;
}

std::vector<pid_t> ProcessInfo::threads() {
  std::vector<pid_t> result;
  t_pids = &result;
  scanDir("/proc/self/task", taskDirFilter);
  t_pids = nullptr;
  std::sort(result.begin(), result.end());
  return result;
}

} // namespace muduo
