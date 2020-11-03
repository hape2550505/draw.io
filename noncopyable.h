#pragma once
class noncopyable {//classÄ¬ÈÏprivate¼Ì³Ð 
protected:
	noncopyable() {};
		~noncopyable() {};

private:
	noncopyable(const noncopyable&) = delete;
	const noncopyable& operator=(const noncopyable&) = delete;
};