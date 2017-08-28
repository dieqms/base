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
#include "string_util.h"
#include "string_util.h"

using std::string;

class TimeUtil {
public:
	static int SetSysTime(time_t time)
	{
		struct timeval tv;
		tv.tv_sec = time;
		tv.tv_usec = 0;
		if(settimeofday (&tv, (struct timezone *) 0) < 0) {
			printf("[Time] error: set system datatime error!/n");
			return -1;
		}
		return 0;
	}

	static time_t Str2Time(const char * szTime) {
		tm tm_;
		if (NULL == strptime(szTime, "%Y-%m-%d %H:%M:%S", &tm_))
			return 0;

		tm_.tm_isdst = -1;
		return mktime(&tm_);
	}

	static time_t Str2Time(const char * szTime, const char * fmt) {
		tm tm_;
		if (NULL == strptime(szTime, fmt, &tm_))
			return 0;

		tm_.tm_isdst = -1;
		return mktime(&tm_);
	}

	static string Time2Str(time_t time)
	{
		struct tm tm_now;
		char    datetime[200] = {"0"};

		localtime_r(&time, &tm_now);
		strftime(datetime, 200, "%Y-%m-%d %H:%M:%S", &tm_now);

		string time_str(datetime);
		return time_str;
	}

	static string Time2Str(time_t time, const char * fmt)
	{
		struct tm tm_now;
		char    datetime[200] = {"0"};

		localtime_r(&time, &tm_now);
		strftime(datetime, 200, fmt, &tm_now);

		string time_str(datetime);
		return time_str;
	}
};

#endif /* SERVERCOMM_UTIL_H_ */
