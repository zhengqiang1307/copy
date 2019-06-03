#include "muduo/base/LogFile.h"
#include "muduo/base/Logging.h"
#include "muduo/base/ThreadPool.h"
#include "muduo/base/TimeZone.h"

#include <stdio.h>
#include <unistd.h>

int g_total = 0;
FILE* g_file = nullptr;
std::unique_ptr<muduo::LogFile> g_logFile;

void dummyOutput(const char* msg, int len) {
  g_total += len;
  if (g_file) {
    fwrite(msg, 1, static_cast<size_t>(len), g_file);
  } else if (g_logFile) {
    g_logFile->append(msg, len);
  }
}

void bench(const char* type) {
  muduo::Logger::setOutput(dummyOutput);
  muduo::Timestamp start(muduo::Timestamp::now());
  g_total = 0;

  int n = 1000 * 1000;
  const bool kLongLog = false;
  muduo::string empty = " ";
  muduo::string longStr(3000, 'x');
  longStr += " ";
  for (int i = 0; i < n; i++) {
    LOG_INFO << "Hello 0123456789"
             << " abcdefghijklmnopqrstuvwxyz" << (kLongLog ? longStr : empty)
             << i;
  }
  muduo::Timestamp end(muduo::Timestamp::now());
  double seconds = timeDifference(end, start);
  printf("%12s: %f seconds, %d bytes, %10.2f msg/s, %.2f MiB/s\n", type,
         seconds, g_total, n / seconds, g_total / seconds / 1024 / 1024);
}

void logInThread() {
  LOG_INFO << "logInThread";
  usleep(1000);
}

int main(int argc, char* argv[]) {
  getpid();
  muduo::ThreadPool pool("pool");
  pool.start(5);
  pool.run(logInThread);
  pool.run(logInThread);
  pool.run(logInThread);
  pool.run(logInThread);
  pool.run(logInThread);

  LOG_TRACE << "trace";
  LOG_DEBUG << "debug";
  LOG_INFO << "info";
  LOG_WARN << "warn";
  LOG_ERROR << "error";

  LOG_INFO << sizeof(muduo::Logger);
  LOG_INFO << sizeof(muduo::LogStream);
  LOG_INFO << sizeof(muduo::Fmt);
  LOG_INFO << sizeof(muduo::LogStream::Buffer);

  sleep(1);
  bench("top");

  char buffer[64 * 1024];

  g_file = fopen("/dev/null", "w");
  setbuffer(g_file, buffer, sizeof buffer);
  bench("/dev/null");
  fclose(g_file);

  g_file = fopen("/tmp/log", "w");
  setbuffer(g_file, buffer, sizeof buffer);
  bench("/tmp/log");
  fclose(g_file);

  g_file = nullptr;
  g_logFile.reset(new muduo::LogFile("test-log-st", 500 * 1000 * 1000, false));
  bench("test-log-st");

  g_logFile.reset(new muduo::LogFile("test-log-mt", 500 * 1000 * 1000, true));
  bench("test-log-mt");
  g_logFile.reset();

  {
      g_file=stdout;
      sleep(1);
      muduo::TimeZone beijing(8*3600,"CST");
      muduo::Logger::setTimeZone(beijing);

      LOG_TRACE << "trace CST";
      LOG_DEBUG << "debug CST";
      LOG_INFO << "info CST";
      LOG_WARN << "warn CST";
      LOG_ERROR << "error CST";

      sleep(1);
      muduo::TimeZone newyork("/usr/share/zoneinfo/America/New_York");
      muduo::Logger::setTimeZone(newyork);
      LOG_TRACE << "trace NYT";
      LOG_DEBUG << "debug NYT";
      LOG_INFO << "info NYT";
      LOG_WARN << "warn NYT";
      LOG_ERROR << "error NYT";
      g_file=nullptr;

  }
  bench("timezone nop");
  return 0;
}
