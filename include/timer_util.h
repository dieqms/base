/*
 * timer.h
 *
 *  Created on: 2017年8月22日
 *      Author: Administrator
 */

#ifndef COMMON_TIMER_H_
#define COMMON_TIMER_H_

#include <map>
#include <list>
#include "thread_util.h"

using std::map;
using std::list;

namespace Base {

#define MAX_TIMER 1024

class Timer;
typedef int (*timer_cb_t)(int timer_id, void* arg);

class TimerPool: public Pthread {
private:
	class Timer {
	public:
		Timer(); 
		Timer(int timer_id, uint64_t delay, timer_cb_t cb, void *arg, bool repeat);  
	    Timer(const Timer &timer);  
		~Timer();  
		int id();  

	private:
		int _timer_id;
		uint64_t _duration;
		timer_cb_t _cb;
		void *_arg;
		bool _repeat;
		uint64_t _time_passed;

		friend class TimerPool;
	};
	
public:
	static TimerPool * Instance(); 
	TimerPool();
	int CreateTimer(uint64_t delay, timer_cb_t cb, void *arg, bool rep); 
	void Reset(int timer_id, uint64_t delay); 
	void DestoryTimer(int timer_id); 

private:
	int _GenId(); 

	void _ReleaseId(int id); 
	int _AddTimer(Timer& timer);
	void _UpdateTimer(int id, uint64_t delay); 
	void _RemoveTimer(int id); 
	void runner(void * var); 

private:
	PthreadMutex 	_mutex;
	map<int, Timer> _timer_map;
	uint8_t _timer_id[MAX_TIMER];
};

}
#endif /* COMMON_TIMER_H_ */
