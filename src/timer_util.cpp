/*
 * timer_util.cpp
 *
 *  Created on: 2017年10月24日
 *      Author: LJT
 */

#include <string.h>
#include <unistd.h>
#include "timer_util.h"
#include "string_util.h"

namespace Base {
	Timer::Timer() {
		_timer_id = -1;
		_duration = 0;
		_time_passed = 0;
		_arg = NULL;
		_repeat = false;
		_cb = NULL;
		_thread = NULL;
	}

	Timer::Timer(int timer_id, uint64_t delay, timer_cb_t cb, void *arg, bool repeat, Pthread * thread) {
		_timer_id = timer_id;
		_duration = delay;
		_time_passed = 0;
		_arg = arg;
		_repeat = repeat;
		_cb = cb;
		_thread = thread;
	}

	Timer::Timer(const Timer &timer) {
		_timer_id = timer._timer_id;
		_duration = timer._duration;
		_time_passed = timer._time_passed;
		_arg = timer._arg;
		_repeat = timer._repeat;
		_cb = timer._cb;
		_thread = timer._thread;
	}

	Timer::~Timer() {
	}

	int Timer::id() {
		return _timer_id;
	}

	TimerPool * TimerPool::Instance() {
		static TimerPool instance;
		return &instance;
	}
	
	TimerPool::TimerPool() : _mutex(PthreadMutexAttr(PTHREAD_MUTEX_RECURSIVE_NP)) {
		_timer_map.clear();
		memset(_timer_id, 0, 1024);
	}
	
	void TimerPool::_timer_body(void* _args) {
		int id = (int)_args;
		Timer timer = TimerPool::Instance()->GetTimer(id);
		
		printf("TimerPool new timer %d started\n", id);
		
		while (timer._thread && timer._thread->is_run()) {
			sleep(1);
			
			if(timer._duration != 0 && timer._time_passed++ >= timer._duration) {
				if (timer._cb)
					timer._cb(timer._timer_id, timer._arg);
				if (timer._repeat)
					timer._time_passed = 0;
				else
					timer._duration = 0;
			}
		}
		
		printf("TimerPool new timer %d exited\n", id);
	}
	
	int TimerPool::CreateTimer(uint64_t delay, timer_cb_t cb, void *arg, bool rep) {
		int id = _GenId();
		if (id < 0) {
			printf("[TimerPool] CreateTimer fail, exceed MAX timers %d\n", MAX_TIMER);
			return id;
		}
		Pthread * thread = new Pthread(_timer_body, (void*)id, Base::StringUtil::Format("Timer %d", id));
		Timer timer(id, delay, cb, arg, rep, thread);
		_AddTimer(timer);
		thread->start();
		return id;
	}

	void TimerPool::Reset(int timer_id, uint64_t delay) {
		_UpdateTimer(timer_id, delay);
	}

	void TimerPool::DestoryTimer(int timer_id) {
		_RemoveTimer(timer_id);
	}
	
	Timer TimerPool::GetTimer(int id) {
		Timer timer;
		
		_mutex.lock();
		auto iter = _timer_map.find(id);
		if (iter != _timer_map.end()) {
			timer = iter->second;
		}
		_mutex.unlock();
		
		return timer;
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
			
			Timer timer = iter->second;
			if (timer._thread) {
				timer._thread->stop(true);
				timer._thread->start();
			}
		}
		_mutex.unlock();
	}

	void TimerPool::_RemoveTimer(int id) {
		_mutex.lock();
		auto iter = _timer_map.find(id);
		if (iter != _timer_map.end()) {
			Timer timer = iter->second;
			if (timer._thread)
				delete timer._thread;
			_timer_map.erase(iter);
		}
		_ReleaseId(id);
		_mutex.unlock();
	}
}

