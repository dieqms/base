/*
 * util.h
 *
 *  Created on: 2017年8月18日
 *      Author: LJT
 */

#ifndef FILE_UTIL_H_
#define FILE_UTIL_H_

#include <string>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include "string_util.h"

using std::string;

namespace Base {

class FileUtil {
public:
	FileUtil(); 
	explicit FileUtil(FILE* fp);  
	FileUtil(const char* name, const char* mode);  
	~FileUtil();  
	operator FILE*() const;  
	bool Open(const char* name, const char* mode);  
	void Close();  
	size_t Read(void* buf, size_t len) const;  
	size_t Write(const void* buf, size_t len);  
	bool SetPos(uint64_t offset, int whence);  
	uint64_t GetPos() const;  
	uint64_t Length() const;  
	static unsigned long Size(const char *path);  
	static int WriteStr(const char * file_path, string & str, bool append = true);  
	static int ReadLine(const char * file_path, string & str, int line_number = 1); 
	static string ReadAll(const char* filename);  
	static bool WriteAllBytes(const char* filename, const void* data,
			uint32_t len, bool append);  
	static bool Exist(const char* name);  
	static bool Delete(const char* name); 
private:
	FILE* _fp;
};

class DirUtil {
public:
	static bool CreateDirectory(const char* path, mode_t mode = 0777);
};

}

#endif /* SERVERCOMM_UTIL_H_ */
