/*
 * thread_util.h
 *
 *  Created on: 2017年8月16日
 *      Author: LJT
 */

#ifndef _THREAD_UTIL_H_
#define _THREAD_UTIL_H_
#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>  
#include <inttypes.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <stdio.h>
#include <limits.h>
#include <errno.h>

namespace Base {

class PthreadMutexAttr {
public:
	PthreadMutexAttr(int type = -1, int shared = -1, int robust = -1,
					int proto = -1, int prioceiling = -1);
	~PthreadMutexAttr();
	operator const pthread_mutexattr_t*() const;
	operator pthread_mutexattr_t*();
	int Type() const;
	void Type(int val); 
	int Shared() const; 
	void Shared(int val); 
	int Protocol() const; 
	void Protocol(int val); 
	int PriorityCeiling() const; 
	void PriorityCeiling(int val); 
#ifdef PTHREAD_MUTEX_ROBUST
	int Robust() const;
	void Robust(int val);
#endif

private:
	pthread_mutexattr_t _attr;
};

class PthreadMutex {
public:
	PthreadMutex(const pthread_mutexattr_t* attr = NULL); 
	~PthreadMutex();
	void lock();
	bool try_lock();
	void unlock();
	void wait();
	bool wait(int wait_ms);
	void signal();
	void broadcast(); 
private:
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;
};

typedef void (*runner_t)(void* args);

class Pthread {
public:
	Pthread(runner_t runner, void * args = NULL, std::string name = "Unknown");
	virtual ~Pthread();
	bool start(size_t stacksize = 1 * 1024 * 1024); 
	void join(); 
	void stop(bool with_block = false); 
	bool is_run(); 
	static unsigned int get_tid();

protected:
	virtual void runner(void * var); 

private:
	static void * loop(void * var); 

protected:
	pthread_t thread_handle;
	std::string thread_name;
	bool running;
	runner_t _runner;
	void * args;
};

}

#endif
