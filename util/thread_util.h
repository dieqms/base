/*
 * thread_util.h
 *
 *  Created on: 2017年8月16日
 *      Author: LJT
 */

#ifndef SERVERCOMM_THREAD_UTIL_H_
#define SERVERCOMM_THREAD_UTIL_H_

#include <sys/types.h>
#include <inttypes.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <stdio.h>
#include <errno.h>

/**
 * @brief pthread mutex封装
 * 为pthread mutex提供stl mutex相似的接口，可配合unique_lock使用
 **/
class PthreadMutex
{
public:
	PthreadMutex(const pthread_mutexattr_t* attr = NULL)
	{
		int err = pthread_mutex_init(&_mutex, attr);
		if(err != 0)
		{
			printf("failed initialize mutex\n");
		}
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
			printf("failed lock mutex\n");
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
			printf("failed unlock mutex\n");
		}
	}

private:
	pthread_mutex_t _mutex;
};

typedef void (*runner_t) (void* args);

class Pthread
{
public:
	Pthread(runner_t runner, void * args = NULL, std::string name = "Unknown")
        : _runner(runner), args(args)
	{
		thread_name = name;
		thread_handle = 0;
		running = false;
	}

	virtual ~Pthread()
	{
		stop();
		join();
	}

	bool start(size_t stacksize = 8 * 1024 * 1024)
	{
		stacksize = (stacksize < PTHREAD_STACK_MIN) ? PTHREAD_STACK_MIN : stacksize;

		pthread_attr_t attr;
		if (!running)
		{
			running = true;
			if (pthread_attr_init(&attr) || pthread_attr_setstacksize(&attr, stacksize)) {
				size_t get = 0;
				pthread_attr_getstacksize(&attr, &get);
				printf("[Pthread] error: set stacksize fail, set %lu get %lu\n", stacksize, get);
			}

			if (pthread_create(&thread_handle, &attr, loop, this) != 0)
			{
				printf("failed to create thread (%s)\n", thread_name.c_str());
				return false;
			}
		}
		return true;
	}

	void join()
	{
		if (thread_handle)
		{
			pthread_join(thread_handle, NULL);
			thread_handle = 0;
		}
	}

	void stop(bool with_block = false)
	{
		if (running)
		{
			running = false;
			if (with_block)
				join();
		}
	}

	bool is_run()
	{
		return running;
	}

//	unsigned int get_tid()
//	{
//		return gettid();
//	}

protected:
	virtual void runner(void * var)
	{
		if(_runner != NULL)
			_runner(var);
	}

private:
	static void * loop(void * var)
	{
		Pthread* p_thread = (Pthread*) var;
		p_thread->runner(p_thread->args);

		p_thread->running = false;
		p_thread->thread_handle = 0;

		return NULL;
	}

protected:
	pthread_t 		thread_handle;
	std::string 	thread_name;
	bool 			running;
	runner_t 		_runner;
	void * 			args;
};


#endif /* SERVERCOMM_THREAD_UTIL_H_ */
