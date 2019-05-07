#include "muduo/base/CurrentThread.h"
#include "muduo/base/Singleton.h"
#include "muduo/base/Thread.h"
#include <stdio.h>
#include <string>

class Test : muduo::noncopyable {

private:
  std::string name_;

public:
  Test() {
    printf("tid=%d, constructing %p\n", muduo::CurrentThread::tid(), this);
  }

  ~Test() {
    printf("tid=%d, destructing %p %s\n", muduo::CurrentThread::tid(), this,
           name_.c_str());
  }

  const std::string &name() { return name_; }
  void setName(const std::string &name) { name_ = name; }
};

class TestNoDestroy : muduo::noncopyable {

public:
  TestNoDestroy() {
    printf("tid=%d, constructing TestNoDestory %p\n",
           muduo::CurrentThread::tid(), this);
  }

  ~TestNoDestroy() {
    printf("tid=%d, destructing TestNoDestory %p\n",
           muduo::CurrentThread::tid(), this);
  }

  void no_destory();
};

void threadFunc() {
  printf("tid=%d, %p name=%s\n", muduo::CurrentThread::tid(),
         &muduo::Singleton<Test>::instance(),
         muduo::Singleton<Test>::instance().name().c_str());

  muduo::Singleton<Test>::instance().setName("only one, changed");
}

int main() {
  muduo::Singleton<Test>::instance().setName("only one");
  muduo::Thread t1(threadFunc);
  t1.start();
  t1.join();
  printf("tid=%d, %p name=%s\n", muduo::CurrentThread::tid(),
         &muduo::Singleton<Test>::instance(),
         muduo::Singleton<Test>::instance().name().c_str());

  muduo::Singleton<TestNoDestroy>::instance();
  printf("with valgrind, you should see %zd-byte memory leak.\n",
         sizeof(TestNoDestroy));
}
