
#pragma once

#include "CurrentThread.h"
#include "Thread.h"

namespace muduo
{

class EventLoop : noncopyable
{
 public:

  EventLoop();
  ~EventLoop();

  void loop();
  //设计一个assertInLoopThread（）用来检测当前线程是否等于该eventloop对象的threadid
  void assertInLoopThread()
  {
    if (!isInLoopThread())
    {
      abortNotInLoopThread();
    }
  }

  bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

 private:

  void abortNotInLoopThread();

  bool looping_; /* atomic */
  const pid_t threadId_;
};

}

