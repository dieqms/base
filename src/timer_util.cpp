/*
 * timer_util.cpp
 *
 *  Created on: 2017年10月24日
 *      Author: LJT
 */

#include <string.h>
#include "timer_util.h"

namespace Base {
//#define MAXFDS 128
//#define EVENTS 100
//
//class Timer;
//typedef int(*timer_callback)(Timer &); //user callback
//
//class Timer
//{
//public:
//    Timer() :
//    	_internal(0.0),
//		_cb(0),
//		_timer_id(0),
//		_repeat(0),
//		_userdata(0)
//	{
//
//	}
//
//    Timer(uint64_t internal_value, int  (*callback)(Timer &ptimer), void *data, int rep) :
//    	_internal(internal_value), _cb(callback), _userdata(data), _repeat(rep)
//    {
//        _timer_id = timerfd_create(CLOCK_REALTIME, 0);
//        _set_nonblock(_timer_id);
//    }
//
//    Timer(const Timer &ptimer)
//    {
//    	_internal = ptimer._internal;
//		_cb = ptimer._cb;
//		_timer_id = ptimer._timer_id;
//		_repeat = ptimer._repeat;
//		_userdata = ptimer._userdata;
//    }
//
//    Timer & operator=(const Timer &ptimer)
//    {
//		if (this == &ptimer)
//		{
//			return *this;
//		}
//		_internal = ptimer._internal;
//		_cb = ptimer._cb;
//		_timer_id = ptimer._timer_id;
//		_repeat = ptimer._repeat;
//		_userdata = ptimer._userdata;
//		return *this;
//    }
//
//    int start()
//    {
//        struct itimerspec ptime_internal = {0};
//
//        ptime_internal.it_value.tv_sec = _internal / 1000;
//        ptime_internal.it_value.tv_nsec = (_internal % 1000) * 1000000;
//
//        if(_repeat)
//        {
//            ptime_internal.it_interval.tv_sec = ptime_internal.it_value.tv_sec;
//            ptime_internal.it_interval.tv_nsec = ptime_internal.it_value.tv_nsec;
//        }
//
//        timerfd_settime(_timer_id, 0, &ptime_internal, NULL);
//        return 0;
//    }
//
//    int stop()
//    {
//        close(_timer_id);
//        return 0;
//    }
//
//    int timer_modify_internal(uint64_t timer_internal)
//    {
//        this->_internal = timer_internal;
//        start();
//    }
//
//    int id()
//    {
//        return _timer_id;
//    }
//
//    void *userdata()
//    {
//        return _userdata;
//    }
//
//    timer_callback callback()
//    {
//        return _cb;
//    }
//
//    ~Timer()
//    {
//        stop();
//    }
//
//private:
//    bool _set_nonblock (int fd)
//    {
//        int flags = fcntl (fd, F_GETFL, 0);
//        flags |= O_NONBLOCK;
//        if (-1 == fcntl (fd, F_SETFL, flags))
//        {
//            return false;
//        }
//        return true;
//    }
//
//    int     _timer_id;
//    uint64_t _internal;
//    void    *_userdata;
//    bool    _repeat;//will the timer repeat or only once
//    timer_callback _cb;
//};
//
//class TimerPool : public Pthread
//{
//public:
//	static TimerPool * Instance() {
//		static TimerPool instance;
//		return &instance;
//	}
//
//	int add_timer(Timer &ptimer)
//	{
//		int timer_id = ptimer.id();
//		struct epoll_event ev;
//		ev.data.fd = timer_id;
//		ev.events = EPOLLIN | EPOLLET;
//		timers_map[timer_id] = ptimer; //add or modify
//		epoll_ctl (epfd, EPOLL_CTL_ADD, timer_id, &ev);
//		ptimer.start();
//
//		return 0;
//	}
//
//	int del_timer(Timer &ptimer)
//	{
//		int timer_id = ptimer.id();
//		struct epoll_event ev;
//		ev.data.fd = timer_id;
//		ev.events = EPOLLIN | EPOLLET;
//		epoll_ctl (epfd, EPOLL_CTL_DEL, timer_id, &ev);
//		timers_map.erase(timer_id);
//
//		return 0;
//	}
//
//private:
//    TimerPool(int max_num=128) : Pthread(NULL, NULL, "timer_poll")
//    {
//        active = 1;
//        epfd = epoll_create(max_num);
//    }
//
//	void runner(void * var)
//	{
//        char buf[128] ={0};
//        for (; active ; )
//        {
//            struct epoll_event events[MAXFDS] ={0};
//            int nfds = epoll_wait (epfd, events, MAXFDS, -1);
//            for (int i = 0; i < nfds; ++i)
//            {
//                std::map<int, Timer>::iterator itmp = timers_map.find(events[i].data.fd);
//                if (itmp != timers_map.end())
//                {
//                    //timer ptimer = itmp->second;
//                    while (read(events[i].data.fd, buf, 128) > 0);
//                    itmp->second.callback()(itmp->second);
//                }
//            }
//        }
//    }
//
//    int deactive()
//    {
//        active = 0;
//    }
//
//    ~TimerPool()
//    {
//
//    }
//private:
//    int epfd;
//    int active;
//    std::map<int, Timer> timers_map;
//    /* data */
//} ;

	TimerPool::Timer::Timer() {
		_timer_id = -1;
		_duration = 0;
		_time_passed = 0;
		_arg = NULL;
		_repeat = false;
		_cb = NULL;
	}

	TimerPool::Timer::Timer(int timer_id, uint64_t delay, timer_cb_t cb, void *arg, bool repeat) {
		_timer_id = timer_id;
		_duration = delay;
		_time_passed = 0;
		_arg = arg;
		_repeat = repeat;
		_cb = cb;
	}

	TimerPool::Timer::Timer(const Timer &timer) {
		_timer_id = timer._timer_id;
		_duration = timer._duration;
		_time_passed = timer._time_passed;
		_arg = timer._arg;
		_repeat = timer._repeat;
		_cb = timer._cb;
	}

	TimerPool::Timer::~Timer() {
	}

	int TimerPool::Timer::id() {
		return _timer_id;
	}

	TimerPool * TimerPool::Instance() {
		static TimerPool instance;
		return &instance;
	}
	
	TimerPool::TimerPool() : Pthread(NULL, NULL, "timer_poll"), _mutex(PthreadMutexAttr(PTHREAD_MUTEX_RECURSIVE_NP)) {
		_timer_map.clear();
		memset(_timer_id, 0, 1024);
	}
	
	int TimerPool::CreateTimer(uint64_t delay, timer_cb_t cb, void *arg, bool rep) {
		Timer timer(_GenId(), delay, cb, arg, rep);
		if (!is_run())
			start();
		return _AddTimer(timer);
	}

	void TimerPool::Reset(int timer_id, uint64_t delay) {
		_UpdateTimer(timer_id, delay);
	}

	void TimerPool::DestoryTimer(int timer_id) {
		_RemoveTimer(timer_id);
	}

	int TimerPool::_GenId() {
		for (int i = 0; i < MAX_TIMER; i++) {
			if (0 == _timer_id[i]) {
				_timer_id[i] = 1;
				return i;
			}
		}
		return -1;
	}

	void TimerPool::_ReleaseId(int id) {
		if (id < MAX_TIMER)
			_timer_id[id] = 0;
	}

	int TimerPool::_AddTimer(Timer& timer){
		_mutex.lock();
		_timer_map[timer.id()] = timer;
		_mutex.unlock();
		return timer.id();
	}

	void TimerPool::_UpdateTimer(int id, uint64_t delay) {
		_mutex.lock();
		auto iter = _timer_map.find(id);
		if (iter != _timer_map.end()) {
			iter->second._duration = delay;
		}
		_mutex.unlock();
	}

	void TimerPool::_RemoveTimer(int id) {
		_mutex.lock();
		auto iter = _timer_map.find(id);
		if (iter != _timer_map.end()) {
			_timer_map.erase(iter);
		}
		_ReleaseId(id);
		_mutex.unlock();
	}

	void TimerPool::runner(void * var) {
		while(is_run()) {
			_mutex.lock();
			if (_timer_map.empty()) {
				_mutex.unlock();
				sleep(1);
				continue;
			}

			for (auto iter = _timer_map.begin(); iter != _timer_map.end(); ++iter) 	{
				Timer& timer = iter->second;

//				PRINT_DEBUG("%s: timer %lu, time_repeat %d time_passed %lu duration %lu\n", 
//						__func__, iter->first,
//						timer._repeat ? 1 : 0, timer._time_passed, timer._duration);
				if(timer._duration != 0 && timer._time_passed++ >= timer._duration) {
					if (timer._cb)
						timer._cb(timer._timer_id, timer._arg);
					if (timer._repeat)
						timer._time_passed = 0;
					else
						timer._duration = 0;
				}
			}
			_mutex.unlock();
			sleep(1);
		}
	}

}

