/*
 * util.h
 *
 *  Created on: 2017年8月18日
 *      Author: Administrator
 */

#ifndef FILE_UTIL_H_
#define FILE_UTIL_H_

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

using std::string;

class FileUtil
{
public:
	static unsigned long Size(const char *path) {
		unsigned long filesize = 0;
		struct stat statbuff;
		if(stat(path, &statbuff) < 0){
			return filesize;
		}else{
			filesize = statbuff.st_size;
		}
		return filesize;
	}

	static int WriteNumber(char * file_path, uint64_t number, bool append = true) {
		int fd, ret;

		if (append)
			fd = open(file_path, O_CREAT | O_WRONLY | O_APPEND);
		else
			fd = open(file_path, O_CREAT | O_WRONLY | O_TRUNC);

		if(fd < 0)      {//打开失败
			printf("sys: open file \"%s\" failed, errno=%d.\n", file_path, errno);
			return -1;
		}

		char number_str[128] = {0};
		sprintf(number_str, "%lu\n", number);

		ret = write(fd, number_str, strlen(number_str));
		if (ret < 0) {
			printf("WriteNumber fail\n");
			return -2;
		}
		fsync(fd);
		close(fd);

		return 0;
	}

	static int WriteStr(const char * file_path, string & str, bool append = true) {
		int fd, ret;

		if (append)
			fd = open(file_path, O_CREAT | O_WRONLY | O_APPEND);
		else
			fd = open(file_path, O_CREAT | O_WRONLY | O_TRUNC);

		if(fd < 0)      {//打开失败
			printf("sys: open file \"%s\" failed, errno=%d.\n", file_path, errno);
			return -1;
		}

		ret = write(fd, str.c_str(), str.size());
		if (ret < 0) {
			printf("WriteStr fail\n");
			return -2;
		}
		fsync(fd);
		close(fd);

		return 0;
	}

	static int ReadLine(const char * file_path, string & str, int line_number = 1) {
		FILE *fp = fopen(file_path, "r");
		if(fp == NULL) {
			printf("util: fopen fail, %s\n", file_path);
			return -1;
		}

		int i = 0;
		do {
			char s[1024];
			if (fgets(s,1024,fp)!=NULL)
			{
				str = s;
			}
		} while (i++ < line_number - 1);

	    fclose(fp);
		return 0;
	}

	static inline bool Exist(const char* name)
	{
		struct stat statbuf;
		return stat(name, &statbuf) == 0 && !S_ISDIR(statbuf.st_mode);
	}

	static bool Delete(const char* name) {
		if(remove(name))
			return false;
		return true;
	}

	static bool CreateDirectory(const char* path, mode_t mode = 0777)
	{
		assert(path != NULL);

		char name[1024];
		int capacity = sizeof(name) - 1;
		int len = snprintf(name, capacity, "%s", path);
		if(len <= 0 || len >= capacity)
		{
			return false;
		}
		name[len] = '/';
		name[len + 1] = 0;

		char* p = strchr(name + 1, '/');
		if(*(p - 1) == ':')
		{
			p = strchr(p + 1, '/');
		}
		for (; p != NULL; p = strchr(p + 1, '/'))
		{
			*p = 0;
			int result = mkdir(name, mode);
			if (result == -1)
			{
				if (errno != EEXIST)
				{
					*p = '/';
					return false;
				}
			}
			*p = '/';
		}
		return true;
	}
};

#endif /* SERVERCOMM_UTIL_H_ */
