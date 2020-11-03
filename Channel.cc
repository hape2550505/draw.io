// excerpts from http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "Channel.h"
#include "EventLoop.h"
#include "Logging.h"

#include <sstream>

#include <poll.h>

using namespace muduo;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fdArg)
  : loop_(loop),
    fd_(fdArg),
    events_(0),
    revents_(0),
    index_(-1)
{
}

void Channel::update()//调用eventloop::updatechannal()进行更新
{
  loop_->updateChannel(this);
}

void Channel::handleEvent()//由EventLoop::loop()调用 是一个中转站对到来的事件进行分支处理
{
  if (revents_ & POLLNVAL) {
    LOG << "Channel::handle_event() POLLNVAL";
  }

  if (revents_ & (POLLERR | POLLNVAL)) {
    if (errorCallback_) errorCallback_();
  }
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (readCallback_) readCallback_();
  }
  if (revents_ & POLLOUT) {
    if (writeCallback_) writeCallback_();
  }
}
