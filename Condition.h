#pragma once
#pragma once
#include <errno.h>
#include <pthread.h>
#include <pthread.h>
#include <time.h>
#include <cstdint>
#include "MutexLock.h"
#include "noncopyable.h"
/*
主要是用在多个消费者的情况

notify（）最多唤醒一个线程

主线程先用一个mutex初始化condition

然后在不同的线程wait

在主线程notify

封装后api更直观，而不用自己去控制pthread_cond_wait pthread_cond_signal这种难记的api*/

class Condition : noncopyable {
public:
	explicit Condition(MutexLock &_mutex) : mutex(_mutex) {
		pthread_cond_init(&cond, NULL);
	}
	~Condition() { pthread_cond_destroy(&cond); }
	void wait() { pthread_cond_wait(&cond, mutex.get()); }
	void notify() { pthread_cond_signal(&cond); }
	void notifyAll() { pthread_cond_broadcast(&cond); }
	bool waitForSeconds(int seconds) {//后加的
		struct timespec abstime;
		clock_gettime(CLOCK_REALTIME, &abstime);
		abstime.tv_sec += static_cast<time_t>(seconds);
		return ETIMEDOUT == pthread_cond_timedwait(&cond, mutex.get(), &abstime);
	}

private:
	MutexLock &mutex;
	pthread_cond_t cond;
};