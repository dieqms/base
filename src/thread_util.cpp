/*
 * thread_util.cpp
 *
 *  Created on: 2017年10月24日
 *      Author: LJT
 */
#include <stdio.h>
#include <sys/time.h>
#include <limits.h>
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include "thread_util.h"

namespace Base {
	PthreadMutexAttr::PthreadMutexAttr(int type, int shared, int robust,
			int proto, int prioceiling) {
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

	PthreadMutexAttr::~PthreadMutexAttr() {
		int err = pthread_mutexattr_destroy(&_attr);
		if (err != 0) {
			printf("failed initialize mutexattr\n");
		}
	}

	PthreadMutexAttr::operator const pthread_mutexattr_t*() const {
		return &_attr;
	}
	
	PthreadMutexAttr::operator pthread_mutexattr_t*() {
		return &_attr;
	}

	int PthreadMutexAttr::Type() const {
		int val = 0;
		int err = pthread_mutexattr_gettype(&_attr, &val);
		if (err != 0) {
			printf("failed get mutexattr property\n");
		}

		return val;
	}

	void PthreadMutexAttr::Type(int val) {
		int err = pthread_mutexattr_settype(&_attr, val);
		if (err != 0) {
			printf("failed set mutexattr property\n");
		}
	}

	int PthreadMutexAttr::Shared() const {
		int val = PTHREAD_PROCESS_PRIVATE;
		int err = pthread_mutexattr_getpshared(&_attr, &val);
		if (err != 0) {
			printf("failed get mutexattr property\n");
		}

		return val;
	}

	void PthreadMutexAttr::Shared(int val) {
		int err = pthread_mutexattr_setpshared(&_attr, val);
		if (err != 0) {
			printf("failed set mutexattr property\n");
		}
	}

	int PthreadMutexAttr::Protocol() const {
		int val = 0;
		int err = pthread_mutexattr_getprotocol(&_attr, &val);
		if (err != 0) {
			printf("failed get mutexattr property\n");
		}

		return val;
	}

	void PthreadMutexAttr::Protocol(int val) {
		int err = pthread_mutexattr_setprotocol(&_attr, val);
		if (err != 0) {
			printf("failed set mutexattr property\n");
		}
	}

	int PthreadMutexAttr::PriorityCeiling() const {
		int val = 0;
		int err = pthread_mutexattr_getprioceiling(&_attr, &val);
		if (err != 0) {
			printf("failed get mutexattr property\n");
		}

		return val;
	}

	void PthreadMutexAttr::PriorityCeiling(int val) {
		int err = pthread_mutexattr_setprioceiling(&_attr, val);
		if (err != 0) {
			printf("failed set mutexattr property\n");
		}
	}
#ifdef PTHREAD_MUTEX_ROBUST
	int PthreadMutexAttr::Robust() const {
		int val = PTHREAD_MUTEX_STALLED;
		int err = pthread_mutexattr_getrobust(&_attr, &val);
		if (err != 0) {
			printf("failed get mutexattr property\n");
		}

		return val;
	}

	void PthreadMutexAttr::Robust(int val) {
		int err = pthread_mutexattr_setrobust(&_attr, val);
		if (err != 0) {
			printf("failed set mutexattr property\n");
		}
	}
#endif

	PthreadMutex::PthreadMutex(const pthread_mutexattr_t* attr) {
		int err = pthread_mutex_init(&_mutex, attr);
		if (err != 0) {
			printf("failed initialize mutex\n");
		}

		_cond = PTHREAD_COND_INITIALIZER;
	}

	PthreadMutex::~PthreadMutex()
	{
		int err = pthread_mutex_destroy(&_mutex);
		if(err != 0)
		{
			printf("failed initialize mutex\n");
		}
	}

	void PthreadMutex::lock()
	{
		int err = pthread_mutex_lock(&_mutex);
		if(err != 0)
		{
			perror("failed lock mutex\n");
		}
	}

	bool PthreadMutex::try_lock()
	{
		int err = pthread_mutex_trylock(&_mutex);
		if(err != 0 && err != EBUSY)
		{
			printf("failed try unlock mutex\n");
		}

		return (err == 0) ? true : false;
	}

	void PthreadMutex::unlock()
	{
		int err= pthread_mutex_unlock(&_mutex);
		if(err != 0)
		{
			perror("failed unlock mutex\n");
		}
	}

	void PthreadMutex::wait() {
		pthread_cond_wait(&_cond, &_mutex);
	}

	bool PthreadMutex::wait(int wait_ms) {
		struct timespec abstime;
		struct timeval now;
		gettimeofday(&now, 0);
		abstime.tv_sec = now.tv_sec + wait_ms/1000;
		abstime.tv_nsec = now.tv_usec*1000 + (wait_ms%1000)*1000000;

		int err = pthread_cond_timedwait(&_cond, &_mutex, &abstime);
		return (err == 0) ? true : false;
	}

	void PthreadMutex::signal() {
		pthread_cond_signal(&_cond);
	}

	void PthreadMutex::broadcast() {
		pthread_cond_broadcast(&_cond);
	}
	
	PthreadMutex Pthread::mutex;
	std::list<PthreadInfo> Pthread::pthread_list;

	Pthread::Pthread(runner_t runner, void * args, std::string name) :
			_runner(runner), _args(args) {
		_thread_name = name;
		_thread_handle = 0;
		_running = false;
	}

	Pthread::~Pthread() {
		stop();
		join();
	}
	
	void Pthread::set_stacksize(size_t stacksize) {
		_stacksize = stacksize;
		_stacksize = (_stacksize < PTHREAD_STACK_MIN) ? PTHREAD_STACK_MIN : _stacksize;
	}

	bool Pthread::start(size_t stacksize) {
		set_stacksize(stacksize);

		pthread_attr_t attr;
		if (!_running) {
			_running = true;
			if (pthread_attr_init(&attr)
					|| pthread_attr_setstacksize(&attr, _stacksize)) {
				size_t get = 0;
				pthread_attr_getstacksize(&attr, &get);
				printf("[Pthread] error: set stacksize fail, set %zu get %zu\n",
						_stacksize, get);
			}
			pthread_attr_destroy(&attr);
			if (pthread_create(&_thread_handle, &attr, loop, this) != 0) {
				printf("failed to create thread (%s)\n", _thread_name.c_str());
				return false;
			}
		}
		return true;
	}

	void Pthread::join() {
		if (_thread_handle) {
			pthread_join(_thread_handle, NULL);
			_thread_handle = 0;
		}
	}

	void Pthread::stop(bool with_block) {
		if (_running) {
			_running = false;
			if (with_block)
				join();
		}
	}

	bool Pthread::is_run() {
		return _running;
	}

	unsigned int Pthread::get_tid() {
//		syscall(SYS_gettid) 
		return syscall(__NR_gettid);
	}
	
	void Pthread::dump() {
		mutex.lock();
		printf("#### Pthread Dump ####\n");
		int i = 0;
		for (auto iter = pthread_list.begin(); iter != pthread_list.end(); ++iter) {
			printf("  %d [%d] %s - stack %zd\n", ++i, iter->id, iter->name.c_str(), iter->stack_size);
		}
		printf("#### Pthread Dump End ####\n");
		mutex.unlock();
	}
	
	std::string Pthread::name() {
		return _thread_name;
	}
	
	pthread_t Pthread::handle() {
		return _thread_handle;
	}

	void Pthread::runner(void * var) {
		if (_runner != NULL)
			_runner(var);
	}

	void * Pthread::loop(void * var) {
		Pthread* p_thread = (Pthread*) var;
		unsigned int id = p_thread->get_tid();
		
		PthreadInfo info;
		mutex.lock();
		info.id = id;
		info.name = p_thread->name();
		info.stack_size = p_thread->_stacksize;
		pthread_list.push_back(info);
		mutex.unlock();
		
		p_thread->runner(p_thread->_args);

		p_thread->_running = false;
		p_thread->_thread_handle = 0;
		
		mutex.lock();
		for (auto iter = pthread_list.begin(); iter != pthread_list.end(); ++iter) {
			if (iter->id == id) {
				pthread_list.erase(iter);
				break;
			}
		}
		mutex.unlock();

		return NULL;
	}

}

