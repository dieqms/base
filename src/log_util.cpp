/*
 * log_util.cpp
 *
 *  Created on: 2017年10月24日
 *      Author: LJT
 */

#include "log_util.h"

namespace Base {
Logger::LogLevel Logger::Str2LogLevel(const char * str) {
	if (0 == strncasecmp(str, "emerg", 5)) 	return LOG_EMERG;
	if (0 == strncasecmp(str, "alert", 5)) 	return LOG_ALERT;
	if (0 == strncasecmp(str, "crit", 4)) 	return LOG_CRIT;
	if (0 == strncasecmp(str, "err", 3)) 	return LOG_ERR;
	if (0 == strncasecmp(str, "warn", 4)) 	return LOG_WARNING;
	if (0 == strncasecmp(str, "notice", 6)) return LOG_NOTICE;
	if (0 == strncasecmp(str, "info", 4)) 	return LOG_INFO;
	if (0 == strncasecmp(str, "debug", 5)) 	return LOG_DEBUG;

	return LOG_WARNING;
}

const char * Logger::LogLevel2Str(LogLevel level) {
	if (level == LOG_EMERG) 	return "emerg";
	if (level == LOG_ALERT) 	return "alert";
	if (level == LOG_CRIT) 		return "crit";
	if (level == LOG_ERR) 		return "err";
	if (level == LOG_WARNING) 	return "warn";
	if (level == LOG_NOTICE) 	return "notice";
	if (level == LOG_INFO) 		return "info";
	if (level == LOG_DEBUG) 	return "debug";
	return "warn";
}

Logger * Logger::Instance() {
	static Logger instance;
	return &instance;
}

void Logger::Open(const char* filename, LogLevel level, uint64_t max_size) {
	_Close();

	_file_name = filename;
	_level = level;
	_max_size = max_size;

	_Open();
}

void Logger::SetFile(const char* filename) {
	assert(filename != 0);
	_file_name = filename;

	_Close();
	_Open();
}

void Logger::SetLevel(LogLevel level) {
	_level = level;
}

void Logger::SetMaxSize(uint64_t max_size) {
	_max_size = max_size;
}

void Logger::Print(int level, const char* fmt, ...) {
	_mutex.lock();
	if (level <= _level) {
		va_list args;
		va_start(args, fmt);
		PrintV(level, fmt, args);
		va_end(args);
	}
	_mutex.unlock();
}

Logger::Logger() {
	_Init();
}

Logger::Logger(const char* filename, LogLevel level, uint64_t max_size) {
	_Init();

	_file_name = filename;
	_level = level;
	_max_size = max_size;

	_Open();
}

Logger::~Logger() {
	_Close();
}

void Logger::_Init() {
	_fp = 0;
	_file_name = "/dev/stderr";
	_level = LOG_DEBUG;
	_max_size = 1024 * 1024;
}

void Logger::PrintV(int level, const char* fmt, va_list args) {
	if (level <= _level) {
		_DoPrintV(level, fmt, args);
	}
}

void Logger::_Close() {
	if (_fp != 0) {
		if (_fp != stdout && _fp != stderr) {
			fclose(_fp);
		}
		_fp = 0;
	}
}

bool Logger::_Open() {
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
				Base::DirUtil::CreateDirectory(_file_name.substr(0, pos).c_str());
				_fp = fopen(_file_name.c_str(), "a");
			}
		}
	}
	return _fp != 0;
}

void Logger::_DoPrintV(int level, const char* fmt, va_list args) {
	if (_fp == NULL && (_file_name.empty() || !_Open())) {
		return;
	}

	//truncate if exceed
	struct stat f_stat;
	fstat(fileno(_fp), &f_stat);
	if ((size_t)f_stat.st_size > (_max_size * 2) / 3) {
		string cmd = Base::StringUtil::Format("cp %s %s.old", _file_name.c_str(), _file_name.c_str());
		ExecuteCmd(cmd.c_str());
		ftruncate(fileno(_fp), (off_t)0);
	}

	string time;
	_AppendTimestamp(time, "%04u-%02u-%02u %02u:%02u:%02u.%03u");
	fprintf(_fp, "%s [%s] ", time.c_str(), _LevelString(level));

	vfprintf(_fp, fmt, args);

	fflush(_fp);
	
	return;
}

void Logger::_AppendTimestamp(std::string& time, const char* fmt) {
	timeval curr_time = { 0 };
	gettimeofday(&curr_time, NULL);

	struct tm curr_tm;
	memset(&curr_tm, 0, sizeof(curr_tm));
	time_t sec = curr_time.tv_sec;
	localtime_r(&sec, &curr_tm);

	Base::StringUtil::AppendFormat(time, fmt, curr_tm.tm_year + 1900,
			curr_tm.tm_mon + 1, curr_tm.tm_mday, curr_tm.tm_hour,
			curr_tm.tm_min, curr_tm.tm_sec, curr_time.tv_usec / 1000);
}

const char* Logger::_LevelString(int level) const {
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

}


