/*
 * time_util.cpp
 *
 *  Created on: 2017年10月24日
 *      Author: LJT
 */
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include <string.h>
#include "time_util.h"

namespace Base {
	int TimeUtil::SetSysTime(time_t time) {
		struct timeval tv;
		tv.tv_sec = time;
		tv.tv_usec = 0;
		if (settimeofday(&tv, (struct timezone *) 0) < 0) {
			printf("[Time] error: set system datatime error!/n");
			return -1;
		}
		return 0;
	}

	time_t TimeUtil::Str2Time(const char * szTime) {
		tm tm_;
		if (NULL == strptime(szTime, "%Y-%m-%d %H:%M:%S", &tm_))
			return 0;

		tm_.tm_isdst = -1;
		return mktime(&tm_);
	}

	time_t TimeUtil::Str2Time(const char * szTime, const char * fmt) {
		tm tm_;
		if (NULL == strptime(szTime, fmt, &tm_))
			return 0;

		tm_.tm_isdst = -1;
		return mktime(&tm_);
	}

	string TimeUtil::Time2Str(time_t time) {
		struct tm tm_now;
		char datetime[200] = { "0" };

		localtime_r(&time, &tm_now);
		strftime(datetime, 200, "%Y-%m-%d %H:%M:%S", &tm_now);

		string time_str(datetime);
		return time_str;
	}

	string TimeUtil::Time2Str(time_t time, const char * fmt) {
		struct tm tm_now;
		char datetime[200] = { "0" };

		localtime_r(&time, &tm_now);
		strftime(datetime, 200, fmt, &tm_now);

		string time_str(datetime);
		return time_str;
	}

	TimeMgr * TimeMgr::Instance() {
		static TimeMgr instance;
		return &instance;
	}

	void TimeMgr::Sync(time_t newtime) {
		_mutex.lock();
		_boot_time += newtime - time(NULL);
		_synced = true;
		Base::TimeUtil::SetSysTime(newtime);
		_mutex.unlock();
	}

	time_t TimeMgr::NowFromBoot() {
		struct timespec time1 = {0, 0};
		clock_gettime(CLOCK_MONOTONIC, &time1);
		return time1.tv_sec;
	}

	time_t TimeMgr::BootTime() {
		return _boot_time;
	}

	bool TimeMgr::Synced() {
		return _synced;
	}

	time_t TimeMgr::Now() {
		return Synced() ? time(NULL) : 0;
	}

	TimeMgr::TimeMgr() : _synced(false), _boot_time(0) {
		Init();
	}

	void TimeMgr::Init() {
		struct sysinfo info;
		if (0 == sysinfo(&info)) {
			time_t cur_time = time(NULL);
			_boot_time = (cur_time > info.uptime) ? cur_time - info.uptime : info.uptime - cur_time;
		}
		else
			fprintf(stderr, "failed to get sysinfo, errno:%s\n", strerror(errno));
	}

}


