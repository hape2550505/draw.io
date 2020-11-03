#pragma once
#include <pthread.h>
#include <cstdio>
#include "noncopyable.h"

class MutexLock : noncopyable {//需要自己上锁 解锁
public:
	MutexLock() { pthread_mutex_init(&mutex, NULL); }
	~MutexLock() {
		pthread_mutex_lock(&mutex);//这一步是保证现在没人持有锁
		pthread_mutex_destroy(&mutex);
	}
	void lock() { pthread_mutex_lock(&mutex); }
	void unlock() { pthread_mutex_unlock(&mutex); }
	pthread_mutex_t *get() { return &mutex; }

private:
	pthread_mutex_t mutex;//默认初始化

	// 友元类不受访问权限影响
private:
	friend class Condition;
};

class MutexLockGuard : noncopyable {//就是初始化锁 并加锁 而不是自己进行锁操作
public:
	explicit MutexLockGuard(MutexLock &_mutex) : mutex(_mutex) { mutex.lock(); }
	~MutexLockGuard() { mutex.unlock(); }//解锁并销毁 

private:
	MutexLock &mutex;
};
