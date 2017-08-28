/*
 * log_utih.h
 *
 *  Created on: 2017年8月16日
 *      Author: LJT
 */

#ifndef LOG_UTIL_H_
#define LOG_UTIL_H_

#include <assert.h>
#include <stdarg.h>
#include <string>
#include "string_util.h"
#include "thread_util.h"

using std::string;

#define PRINT_ERROR(fmt, args...) \
	Logger::Instance()->Print(Logger::LOG_ERR, fmt, ##args)
#define PRINT_DEBUG(fmt, args...) \
	Logger::Instance()->Print(Logger::LOG_DEBUG, fmt, ##args)
#define PRINT_WARN(fmt, args...) \
	Logger::Instance()->Print(Logger::LOG_WARNING, fmt, ##args)
#define PRINT_INFO(fmt, args...) \
	Logger::Instance()->Print(Logger::LOG_INFO, fmt, ##args)

class Logger {
public:
	typedef enum _LogLevel {
		LOG_EMERG = 0,
		LOG_ALERT = 1,
		LOG_CRIT = 2,
		LOG_ERR = 3,
		LOG_WARNING = 4,
		LOG_NOTICE = 5,
		LOG_INFO = 6,
		LOG_DEBUG = 7
	} LogLevel;

	static Logger * Instance() {
		static Logger instance;
		return &instance;
	}

	void Open(const char* filename, LogLevel level, uint64_t max_size) {
		_Close();

		_file_name = filename;
		_level = level;
		_max_size = max_size;

		_Open();
	}

	void SetFile(const char* filename) {
		assert(filename != 0);
		_file_name = filename;
	}

	void SetLevel(LogLevel level) {
		_level = level;
	}

	void SetMaxSize(uint64_t max_size) {
		_max_size = max_size;
	}

	void Print(int level, const char* fmt, ...) {
		_mutex.lock();
		if (level <= _level) {
			va_list args;
			va_start(args, fmt);
			PrintV(level, fmt, args);
			va_end(args);
		}
		_mutex.unlock();
	}

private:
	Logger() {
		_Init();
	}

	Logger(const char* filename, LogLevel level, uint64_t max_size) {
		_Init();

		_file_name = filename;
		_level = level;
		_max_size = max_size;

		_Open();
	}

	~Logger() {
		_Close();
	}

	void _Init() {
		_fp = 0;
		_file_name = "/dev/stderr";
		_level = LOG_DEBUG;
		_max_size = 1024 * 1024;
	}

	void PrintV(int level, const char* fmt, va_list args) {
		if (level <= _level) {
			_DoPrintV(level, fmt, args);
		}
	}

	void _Close() {
		if (_fp != 0) {
			if (_fp != stdout && _fp != stderr) {
				fclose(_fp);
			}
			_fp = 0;
		}
	}

	bool _Open() {
		if (_file_name == "/dev/stdout") {
			_fp = stdout;
		}
		else if (_file_name == "/dev/stderr") {
			_fp = stderr;
		}
		else {
			_fp = fopen(_file_name.c_str(), "a");
			if (_fp == 0 && errno == ENOENT) {
				size_t pos = _file_name.rfind('/');
				if (pos != string::npos) {
					FileUtil::CreateDirectory(_file_name.substr(0, pos).c_str());
					_fp = fopen(_file_name.c_str(), "a");
				}
			}
		}
		return _fp != 0;
	}

	void _DoPrintV(int level, const char* fmt, va_list args) {
		if (_fp == NULL && (_file_name.empty() || !_Open())) {
			return;
		}

		//truncate if exceed
		struct stat f_stat;
		fstat(fileno(_fp), &f_stat);
		if (f_stat.st_size > _max_size) {
			ftruncate(fileno(_fp), 0);
		}

		string time;
		_AppendTimestamp(time, "%04u-%02u-%02u %02u:%02u:%02u.%03u");
		fprintf(_fp, "%s [%s] ", time.c_str(), _LevelString(level));

		vfprintf(_fp, fmt, args);

		fflush(_fp);
	}

	void _AppendTimestamp(std::string& time, const char* fmt) {
		timeval curr_time = { 0 };
		gettimeofday(&curr_time, NULL);

		struct tm curr_tm;
		memset(&curr_tm, 0, sizeof(curr_tm));
		time_t sec = curr_time.tv_sec;
		localtime_r(&sec, &curr_tm);

		StringUtil::AppendFormat(time, fmt, curr_tm.tm_year + 1900,
				curr_tm.tm_mon + 1, curr_tm.tm_mday, curr_tm.tm_hour,
				curr_tm.tm_min, curr_tm.tm_sec, curr_time.tv_usec / 1000);
	}

	const char* _LevelString(int level) const {
		const char* levels[] = {
			"EMERG",    //LOG_EMERG
			"ALERT",    //LOG_ALERT
			"CRIT",     //LOG_CRIT
			"ERROR",    //LOG_ERR
			"WARN",     //LOG_WARNING
			"NOTICE",   //LOG_NOTICE
			"INFO",     //LOG_INFO
			"DEBUG"     //LOG_DEBUG
		};

		const int levelcount = sizeof(levels) / sizeof(levels[0]);
		return level < 0 ? "MSG" : (level >= levelcount ? 0 : levels[level]);
	}

private:
	FILE* 		_fp;
	string 		_file_name;
	LogLevel 	_level;
	uint64_t 	_max_size;
	PthreadMutex _mutex;
};
#endif
