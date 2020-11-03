#pragma once
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <functional>
#include <memory>
#include <string>
#include "CountDownLatch.h"
#include "noncopyable.h"

class Thread : noncopyable {
public:
	typedef std::function<void()> ThreadFunc;
	explicit Thread(const ThreadFunc&, const std::string& name = std::string());
	~Thread();
	void start();
	int join();//让主线程挂起 等待别的线程执行完再结束主线程
	bool started() const { return started_; }
	pid_t tid() const { return tid_; }
	const std::string& name() const { return name_; }

private:
	void setDefaultName();
	bool started_;
	bool joined_;
	pthread_t pthreadId_;
	pid_t tid_;
	ThreadFunc func_;
	std::string name_;
	CountDownLatch latch_;
};

