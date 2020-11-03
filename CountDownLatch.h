#pragma once
#include "Condition.h"
#include "MutexLock.h"
#include "noncopyable.h"
//调用count次countDown()后 广播所有正在wait的线程 注wait操作并不++count 只在初始化时赋值
// CountDownLatch的主要作用是确保Thread中传进去的func真的启动了以后
// 外层的start才返回
class CountDownLatch : noncopyable {
public:
	explicit CountDownLatch(int count);
	void wait();
	void countDown();

private:
	mutable MutexLock mutex_;
	Condition condition_;
	int count_;
};