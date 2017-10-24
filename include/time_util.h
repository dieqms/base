/*
 * time_util.h
 *
 *  Created on: 2017年8月18日
 *      Author: LJT
 */

#ifndef TIME_UTIL_H_
#define TIME_UTIL_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
#include <sys/sysinfo.h>
#include "string_util.h"
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
