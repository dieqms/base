/*
 * time_util.h
 *
 *  Created on: 2017年8月18日
 *      Author: LJT
 */

#ifndef TIME_UTIL_H_
#define TIME_UTIL_H_

#include <string>
#include <sys/time.h>
#include "thread_util.h"

using std::string;

namespace Base {

class TimeUtil {
public:
	static int SetSysTime(time_t time); 
	static time_t Str2Time(const char * szTime);  
	static time_t Str2Time(const char * szTime, const char * fmt);  
	static string Time2Str(time_t time);  
	static string Time2Str(time_t time, const char * fmt);  
};

class TimeMgr
{
public:
	static TimeMgr * Instance(); 
	
	void Sync(time_t newtime); 
	static time_t NowFromBoot(); 
	time_t BootTime(); 
	bool Synced(); 
	time_t Now(); 

private:
	TimeMgr();
	void Init(); 

	PthreadMutex _mutex;
	bool _synced;
	time_t _boot_time;
};

}

#endif /* SERVERCOMM_UTIL_H_ */
