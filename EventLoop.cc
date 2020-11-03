// excerpts from http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "EventLoop.h"

#include "Logging.h"

#include <assert.h>
#include <poll.h>

using namespace muduo;

__thread EventLoop* t_loopInThisThread = 0;

EventLoop::EventLoop()
  : looping_(false),
    threadId_(CurrentThread::tid())
{
  LOG << "EventLoop created " << this << " in thread " << threadId_;
  if (t_loopInThisThread)
  {
    LOG << "Another EventLoop " << t_loopInThisThread
              << " exists in this thread " << threadId_;
  }
  else
  {
    t_loopInThisThread = this;
  }
}

EventLoop::~EventLoop()
{
  assert(!looping_);
  t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
  assert(!looping_);
  assertInLoopThread();
  looping_ = true;

  ::poll(NULL, 0, 5*1000);

  LOG << "EventLoop " << this << " stop looping";
  looping_ = false;
}

void EventLoop::abortNotInLoopThread()
{
  LOG << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << threadId_
            << ", current thread id = " <<  CurrentThread::tid();
}

