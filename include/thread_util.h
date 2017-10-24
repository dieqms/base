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
			int proto = -1, int prioceiling = -1) {
		int err = pthread_mutexattr_init(&_attr);
		if (err != 0) {
			printf("failed initialize mutexattr\n");
		}

		if (type != -1) {
			Type(type);
		}

		if (shared != -1) {
			Shared(shared);
		}
#ifdef PTHREAD_MUTEX_ROBUST
		if(robust != -1) {
			Robust(robust);
		}
#endif
		if (proto != -1) {
			Protocol(proto);
		}

		if (prioceiling != -1) {
			PriorityCeiling(prioceiling);
		}

	}

	~PthreadMutexAttr() {
		int err = pthread_mutexattr_destroy(&_attr);
		if (err != 0) {
			printf("failed initialize mutexattr\n");
		}
	}

public:
	operator const pthread_mutexattr_t*() const {
		return &_attr;
	}
	operator pthread_mutexattr_t*() {
		return &_attr;
	}

	int Type() const {
		int val = 0;
		int err = pthread_mutexattr_gettype(&_attr, &val);
		if (err != 0) {
			printf("failed get mutexattr property\n");
		}

		return val;
	}

	void Type(int val) {
		int err = pthread_mutexattr_settype(&_attr, val);
		if (err != 0) {
			printf("failed set mutexattr property\n");
		}
	}

	int Shared() const {
		int val = PTHREAD_PROCESS_PRIVATE;
		int err = pthread_mutexattr_getpshared(&_attr, &val);
		if (err != 0) {
			printf("failed get mutexattr property\n");
		}

		return val;
	}

	void Shared(int val) {
		int err = pthread_mutexattr_setpshared(&_attr, val);
		if (err != 0) {
			printf("failed set mutexattr property\n");
		}
	}

	int Protocol() const {
		int val = 0;
		int err = pthread_mutexattr_getprotocol(&_attr, &val);
		if (err != 0) {
			printf("failed get mutexattr property\n");
		}

		return val;
	}

	void Protocol(int val) {
		int err = pthread_mutexattr_setprotocol(&_attr, val);
		if (err != 0) {
			printf("failed set mutexattr property\n");
		}
	}

	int PriorityCeiling() const {
		int val = 0;
		int err = pthread_mutexattr_getprioceiling(&_attr, &val);
		if (err != 0) {
			printf("failed get mutexattr property\n");
		}

		return val;
	}

	void PriorityCeiling(int val) {
		int err = pthread_mutexattr_setprioceiling(&_attr, val);
		if (err != 0) {
			printf("failed set mutexattr property\n");
		}
	}
#ifdef PTHREAD_MUTEX_ROBUST
	int Robust() const {
		int val = PTHREAD_MUTEX_STALLED;
		int err = pthread_mutexattr_getrobust(&_attr, &val);
		if (err != 0) {
			printf("failed get mutexattr property\n");
		}

		return val;
	}

	void Robust(int val) {
		int err = pthread_mutexattr_setrobust(&_attr, val);
		if (err != 0) {
			printf("failed set mutexattr property\n");
		}
	}
#endif

private:
	pthread_mutexattr_t _attr;
};

class PthreadMutex {
public:
	PthreadMutex(const pthread_mutexattr_t* attr = NULL) {
		int err = pthread_mutex_init(&_mutex, attr);
		if (err != 0) {
			printf("failed initialize mutex\n");
		}

		_cond = PTHREAD_COND_INITIALIZER;
	}

	~PthreadMutex()
	{
		int err = pthread_mutex_destroy(&_mutex);
		if(err != 0)
		{
			printf("failed initialize mutex\n");
		}
	}

public:
	void lock()
	{
		int err = pthread_mutex_lock(&_mutex);
		if(err != 0)
		{
			perror("failed lock mutex\n");
		}
	}

	bool try_lock()
	{
		int err = pthread_mutex_trylock(&_mutex);
		if(err != 0 && err != EBUSY)
		{
			printf("failed try unlock mutex\n");
		}

		return (err == 0) ? true : false;
	}

	void unlock()
	{
		int err= pthread_mutex_unlock(&_mutex);
		if(err != 0)
		{
			perror("failed unlock mutex\n");
		}
	}

	void wait() {
		pthread_cond_wait(&_cond, &_mutex);
	}

	bool wait(int wait_ms) {
		struct timespec abstime;
		struct timeval now;
		gettimeofday(&now, 0);
		abstime.tv_sec = now.tv_sec + wait_ms/1000;
		abstime.tv_nsec = now.tv_usec*1000 + (wait_ms%1000)*1000000;

		int err = pthread_cond_timedwait(&_cond, &_mutex, &abstime);
		return (err == 0) ? true : false;
	}

	void signal() {
		pthread_cond_signal(&_cond);
	}

	void broadcast() {
		pthread_cond_broadcast(&_cond);
	}

private:
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;
};

typedef void (*runner_t)(void* args);

class Pthread {
public:
	Pthread(runner_t runner, void * args = NULL, std::string name = "Unknown") :
			_runner(runner), args(args) {
		thread_name = name;
		thread_handle = 0;
		running = false;
	}

	virtual ~Pthread() {
		stop();
		join();
	}

	bool start(size_t stacksize = 1 * 1024 * 1024) {
		stacksize =
				(stacksize < PTHREAD_STACK_MIN) ? PTHREAD_STACK_MIN : stacksize;

		pthread_attr_t attr;
		if (!running) {
			running = true;
			if (pthread_attr_init(&attr)
					|| pthread_attr_setstacksize(&attr, stacksize)) {
				size_t get = 0;
				pthread_attr_getstacksize(&attr, &get);
				printf("[Pthread] error: set stacksize fail, set %zu get %zu\n",
						stacksize, get);
			}
			pthread_attr_destroy(&attr);
			if (pthread_create(&thread_handle, &attr, loop, this) != 0) {
				printf("failed to create thread (%s)\n", thread_name.c_str());
				return false;
			}
		}
		return true;
	}

	void join() {
		if (thread_handle) {
			pthread_join(thread_handle, NULL);
			thread_handle = 0;
		}
	}

	void stop(bool with_block = false) {
		if (running) {
			running = false;
			if (with_block)
				join();
		}
	}

	bool is_run() {
		return running;
	}

	static unsigned int get_tid()
	{
//		syscall(SYS_gettid) 
		return syscall(__NR_gettid);
	}

protected:
	virtual void runner(void * var) {
		if (_runner != NULL)
			_runner(var);
	}

private:
	static void * loop(void * var) {
		Pthread* p_thread = (Pthread*) var;
		p_thread->runner(p_thread->args);

		p_thread->running = false;
		p_thread->thread_handle = 0;

		return NULL;
	}

protected:
	pthread_t thread_handle;
	std::string thread_name;
	bool running;
	runner_t _runner;
	void * args;
};

}

#endif
