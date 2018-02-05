/*
 * file_util.cpp
 *
 *  Created on: 2017年10月24日
 *      Author: LJT
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "file_util.h"

namespace Base {
//class FileUtil {
//public:
//	FileUtil() {
//		_fp = NULL;
//	}
//
//	explicit FileUtil(FILE* fp) {
//		_fp = fp;
//	}
//
//	FileUtil(const char* name, const char* mode, bool mkdir = false) {
//		Open(name, mode, mkdir);
//	}
//
//	~FileUtil() {
//		Close();
//	}
//
//public:
//	operator FILE*() const {
//		return _fp;
//	}
//
//	/**
//	 * Open or create file
//	 * @param name	the file name
//	 * @param mode	the mode to open the file, see fopen
//	 * @param mkdir	create intermediate folders if don't exist
//	 * @return return true if file opened successfully, or false
//	 */
//	bool Open(const char* name, const char* mode, bool mkdir = false) {
//		assert(name != NULL);
//		assert(mode != NULL);
//
//		_fp = fopen(name, mode);
//
//		if (_fp == NULL && errno == ENOENT && mkdir) {
//			std::string path = Path::GetDirName(name);
//			if (Path::CreateDirectory(path.c_str())) {
//				_fp = fopen(name, mode);
//			}
//		}
//
//		return _fp != NULL;
//	}
//
//	/**
//	 * Close file
//	 */
//	void Close() {
//		if (_fp != NULL) {
//			fclose(_fp);
//			_fp = NULL;
//		}
//	}
//
//	/**
//	 * Reads len bytes from the file, storing them at the  location given by buf
//	 * @param buf	the buffer to storing data at
//	 * @param len	the len to read
//	 * @return On success, return the number of items read. If an error occurs,
//	 * or the end of the file is reached, the return value is a short item count
//	 * or zero. Read does not distinguish between end-of-file and error,
//	 * and callers must use feof and ferror to determine which occurred.
//	 */
//	size_t Read(void* buf, size_t len) const {
//		return _fp != NULL ? fread(buf, 1, len, _fp) : 0;
//	}
//
//	/**
//	 * Writes len bytes to the file, obtaining them from the location given by buf
//	 * @param buf	the buffer to obtaining data from
//	 * @param len	the len to write
//	 * @return On success, return the number of items written. If an error occurs,
//	 * or the end of the file is reached, the return value is a short item count
//	 * or zero.
//	 */
//	size_t Write(const void* buf, size_t len) {
//		return _fp != NULL ? fwrite(buf, 1, len, _fp) : 0;
//	}
//
//	/**
//	 * Sets the file position indicator for the file.  The new position, measured
//	 * in bytes, is obtained by adding offset bytes to the position specified by
//	 * whence.  If whence is set to SEEK_SET, SEEK_CUR, or SEEK_END, the offset  is
//	 * relative to the start of the file, the current position indicator, or
//	 * end-of-file, respectively. A successful call to the Seek() function clears
//	 * the end-of-file indicator and undoes any effects of the ungetc function.
//	 * @param offset new position
//	 * @param whence SEEK_SET, SEEK_CUR, or SEEK_END
//	 * @return On success, return true, or return false
//	 */
//	bool SetPos(uint64_t offset, int whence) {
//		return _fp != NULL ? (fseeko64(_fp, offset, whence) == 0) : false;
//	}
//
//	uint64_t GetPos() const {
//		return _fp != NULL ? ftello64(_fp) : 0;
//	}
//
//	uint64_t Length() const {
//		if (_fp != NULL) {
//			struct stat64 si = { 0 };
//			if (::fstat64(::fileno(_fp), &si) == 0) {
//				return si.st_size;
//			}
//		}
//
//		return 0;
//	}
//
//	/**
//	 * read all data from file as text
//	 * @param filename the file name to read from
//	 * @return the contents
//	 **/
//	static std::string ReadAll(const char* filename) {
//		std::string ret;
//		FileUtil fp(filename, "r");
//		if (fp != NULL) {
//			uint64_t size = fp.Length();
//			if (size != 0) {
//				ret.resize(size);
//				size_t read = fp.Read(&ret[0], ret.size());
//				ret.resize(read);
//			}
//			fp.Close();
//		}
//
//		return ret;
//	}
//
//	/**
//	 * read all data from command's standard output
//	 * @param cmdline the file name to read from
//	 * @param status the child exit status, which is familiar with system
//	 * @return the contents
//	 **/
//	static std::string ReadAllOutput(const char* cmdline, int* status = NULL) {
//		std::string ret;
//		int exit_status = -1;
//		FILE* fp = ::popen(cmdline, "r");
//		if (fp != NULL) {
//			char buf[64 * 1024];
//			size_t read = 0;
//			while ((read = fread(buf, 1, sizeof(buf), fp)) != 0) {
//				ret.append(buf, read);
//			}
//
//			exit_status = pclose(fp);
//			fp = NULL;
//		}
//
//		if (status != NULL) {
//			*status = exit_status;
//		}
//		return ret;
//	}
//
//	/**
//	 * read all data from file as bytes
//	 * @param T a string, wstring or vector<POD>
//	 * @param out	the buffer to save contents loaded, out can be a string or vector
//	 * @param filename the file name to read from
//	 **/
//	template<typename T>
//	static bool ReadAllBytes(T& out, const char* filename) {
//		static_assert(
//				std::is_pod<typename T::value_type>::value && std::is_pointer<decltype(out.data())>::value,
//				"a string or vector<POD> required");
//
//		File fp(filename, "rb");
//		if (fp) {
//			struct stat si = { 0 };
//			if (::fstat(::fileno(fp), &si) == 0) {
//				out.resize(si.st_size / sizeof(out[0]));
//				size_t read = fp.Read(&out[0], out.size() * sizeof(out[0]));
//				out.resize(read / sizeof(out[0]));
//			}
//			fp.Close();
//			return true;
//		}
//
//		return false;
//	}
//
//	/**
//	 * append data into file
//	 * @param filename the file name to append into
//	 * @param data     the data to append
//	 * @param len      length of the data
//	 * @param append	true append, false overwrite
//	 **/
//	static bool WriteAllBytes(const char* filename, const void* data,
//			uint32_t len, bool append = true) {
//		FileUtil fp(filename, append ? "ab" : "wb");
//		if (fp) {
//			size_t written = fp.Write(data, len);
//			fp.Close();
//			return written == len;
//		}
//
//		return false;
//	}
//
//	/**
//	 * Check if file exists
//	 */
//	static inline bool Exist(const char* name) {
//		struct stat statbuf;
//		return stat(name, &statbuf) == 0 && !S_ISDIR(statbuf.st_mode);
//	}
//
//	static unsigned long Size(const char *path) {
//		unsigned long filesize = 0;
//		struct stat statbuff;
//		if (stat(path, &statbuff) < 0) {
//			return filesize;
//		} else {
//			filesize = statbuff.st_size;
//		}
//		return filesize;
//	}
//
//	static int WriteNumber(char * file_path, uint64_t number,
//			bool append = true) {
//		int fd, ret;
//
//		if (append)
//			fd = open(file_path, O_CREAT | O_WRONLY | O_APPEND);
//		else
//			fd = open(file_path, O_CREAT | O_WRONLY | O_TRUNC);
//
//		if (fd < 0) { //打开失败
//			printf("sys: open file \"%s\" failed, errno=%d.\n", file_path,
//					errno);
//			return -1;
//		}
//
//		char number_str[128] = { 0 };
//		sprintf(number_str, "%lu\n", number);
//
//		ret = write(fd, number_str, strlen(number_str));
//		if (ret < 0) {
//			printf("WriteNumber fail\n");
//			return -2;
//		}
//		fsync(fd);
//		close(fd);
//
//		return 0;
//	}
//
//	static int WriteStr(const char * file_path, string & str,
//			bool append = true) {
//		int fd, ret;
//
//		if (append)
//			fd = open(file_path, O_CREAT | O_WRONLY | O_APPEND);
//		else
//			fd = open(file_path, O_CREAT | O_WRONLY | O_TRUNC);
//
//		if (fd < 0) { //打开失败
//			printf("sys: open file \"%s\" failed, errno=%d.\n", file_path,
//					errno);
//			return -1;
//		}
//
//		ret = write(fd, str.c_str(), str.size());
//		if (ret < 0) {
//			printf("WriteStr fail\n");
//			return -2;
//		}
//		fsync(fd);
//		close(fd);
//
//		return 0;
//	}
//
//	static int ReadLine(const char * file_path, string & str, int line_number =
//			1) {
//		FILE *fp = fopen(file_path, "r");
//		if (fp == NULL) {
//			printf("util: fopen fail, %s\n", file_path);
//			return -1;
//		}
//
//		int i = 0;
//		do {
//			char s[1024];
//			if (fgets(s, 1024, fp) != NULL) {
//				str = s;
//			}
//		} while (i++ < line_number - 1);
//
//		fclose(fp);
//		return 0;
//	}
//
//	static bool Delete(const char* name) {
//		if (remove(name))
//			return false;
//		return true;
//	}
//
//private:
//	FILE* _fp;
//};

	FileUtil::FileUtil() {
		_fp = NULL;
	}

	FileUtil::FileUtil(FILE* fp) {
		_fp = fp;
	}

	FileUtil::FileUtil(const char* name, const char* mode) {
		Open(name, mode);
	}

	FileUtil::~FileUtil() {
		Close();
	}

	FileUtil::operator FILE*() const {
		return _fp;
	}

	bool FileUtil::Open(const char* name, const char* mode) {
		assert(name != NULL);
		assert(mode != NULL);

		_fp = fopen(name, mode);
		return _fp != NULL;
	}

	void FileUtil::Close() {
		if (_fp != NULL) {
			fclose(_fp);
			_fp = NULL;
		}
	}

	size_t FileUtil::Read(void* buf, size_t len) const {
		return _fp != NULL ? fread(buf, 1, len, _fp) : 0;
	}

	size_t FileUtil::Write(const void* buf, size_t len) {
		return _fp != NULL ? fwrite(buf, 1, len, _fp) : 0;
	}

	bool FileUtil::SetPos(uint64_t offset, int whence) {
		return _fp != NULL ? (fseeko64(_fp, offset, whence) == 0) : false;
	}

	uint64_t FileUtil::GetPos() const {
		return _fp != NULL ? ftello64(_fp) : 0;
	}

	uint64_t FileUtil::Length() const {
		if (_fp != NULL) {
			struct stat64 si = { 0 };
			if (::fstat64(::fileno(_fp), &si) == 0) {
				return si.st_size;
			}
		}

		return 0;
	}

	unsigned long FileUtil::Size(const char *path) {
		unsigned long filesize = 0;
		struct stat statbuff;
		if (stat(path, &statbuff) < 0) {
			return filesize;
		} else {
			filesize = statbuff.st_size;
		}
		return filesize;
	}

	int FileUtil::WriteStr(const char * file_path, string & str, bool append) {
		int fd, ret;

		if (append)
			fd = open(file_path, O_CREAT | O_WRONLY | O_APPEND, 0777);
		else
			fd = open(file_path, O_CREAT | O_WRONLY | O_TRUNC, 0777);

		if (fd < 0) { //打开失败
			printf("sys: open file \"%s\" failed, errno=%d.\n", file_path,
					errno);
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

	int FileUtil::ReadLine(const char * file_path, string & str, int line_number) {
		FILE *fp = fopen(file_path, "r");
		if (fp == NULL) {
			printf("util: fopen fail, %s\n", file_path);
			return -1;
		}

		int i = 0;
		do {
			char s[1024];
			if (fgets(s, 1024, fp) != NULL) {
				str = s;
			} else {
				str = "";
			}
		} while (i++ < line_number - 1);

		fclose(fp);
		return 0;
	}

	string FileUtil::ReadAll(const char* filename) {
		std::string ret;
		FileUtil fp(filename, "r");
		if (fp != NULL)
		{
			uint64_t size = fp.Length();
			if(size != 0)
			{
				ret.resize(size);
				size_t read = fp.Read(&ret[0], ret.size());
				ret.resize(read);
			}
			fp.Close();
		}

		return ret;
	}

	bool FileUtil::WriteAllBytes(const char* filename, const void* data,
			uint32_t len, bool append) {
		FileUtil fp(filename, append ? "ab" : "wb");
		if (fp) {
			size_t written = fp.Write(data, len);
			fp.Close();
			return written == len;
		}

		return false;
	}

	bool FileUtil::Exist(const char* name) {
		struct stat statbuf;
		return stat(name, &statbuf) == 0 && !S_ISDIR(statbuf.st_mode);
	}

	bool FileUtil::Delete(const char* name) {
		if (remove(name))
			return false;
		return true;
	}

	bool DirUtil::CreateDirectory(const char* path, mode_t mode) {
		assert(path != NULL);

		char name[1024];
		int capacity = sizeof(name) - 1;
		int len = snprintf(name, capacity, "%s", path);
		if (len <= 0 || len >= capacity) {
			return false;
		}
		name[len] = '/';
		name[len + 1] = 0;

		char* p = strchr(name + 1, '/');
		if (*(p - 1) == ':') {
			p = strchr(p + 1, '/');
		}
		for (; p != NULL; p = strchr(p + 1, '/')) {
			*p = 0;
			int result = mkdir(name, mode);
			if (result == -1) {
				if (errno != EEXIST) {
					*p = '/';
					return false;
				}
			}
			*p = '/';
		}
		return true;
	}

#define EVENT_SIZE  ( sizeof (struct inotify_event) )  
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )  
#define MAXLEN 1024  
	
	FileMonitor::FileMonitor(const string & dir, ListenerCb cb, void *cookie) : Pthread(NULL, NULL, "FileMonitor") {
		_fd = -1;
		_dir = dir;
		_cb = cb;
		_cookie = cookie;
	}
	
	FileMonitor::~FileMonitor() {
		if (_fd != -1) {
			for (auto iter = _watch_fds.begin(); iter != _watch_fds.end(); ++iter)
				inotify_rm_watch(_fd, *iter);
			close(_fd);
		}
	}
	
	void FileMonitor::Monitor(const string & file) {
		_files.insert(file);
	}
	
	bool FileMonitor::Start() {
		system(Base::StringUtil::Format("mkdir %s", _dir.c_str()).c_str());
			
		_fd = inotify_init();  
		if (_fd < 0) {
			perror("inotify_init");
			return false;
		}
		
		int wd = inotify_add_watch(_fd, _dir.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE);
		_watch_fds.push_back(wd);
		
		for (auto iter = _files.begin(); iter != _files.end(); ++iter) {
			wd = inotify_add_watch(_fd, iter->c_str(), IN_MODIFY | IN_CREATE | IN_DELETE);
			_watch_fds.push_back(wd);
		}
		
		start();
		
		return true;
	}
	
	void FileMonitor::runner(void * var) {
		fd_set set;  
		fd_set rset;  
		int check_set[MAXLEN];  
		int max_fd = 0;  
		int arrlen = 0;  
		
		
		//初始化  
		FD_ZERO(&rset);
		int i = 0;
		for (i = 0; i < MAXLEN; ++i)
			check_set[i] = -1;
		
		//添加监控fd  
		FD_SET(_fd, &rset);
		check_set[arrlen++] = _fd;

		if (max_fd < _fd)
			max_fd = _fd;
		
		while (is_run()) {
			set = rset;
			int nready = select(max_fd + 1, &set, NULL, NULL, NULL);
			if (nready == -1) {
				perror("error select !");
				exit(-1);
			}
			else if (nready == 0) {
				printf("timeout!");
				continue;
			}
			
			for (int i = 0; i < arrlen; ++i) //轮询数据连接  
			{
				int set_fd = check_set[i];
				if (FD_ISSET(set_fd, &set)) {
					_Process(set_fd);
				}
			}
		}
	}
	
	void FileMonitor::_Process(int fd) {
		char buffer[BUF_LEN];
		int length = read(fd, buffer, BUF_LEN);
		if (length < 0) {
			perror("read");
		}
		
		int i = 0;
		while (i < length) {
			struct inotify_event *event = (struct inotify_event *) &buffer[i];
			if (event->len) {
				auto iter = _files.find(event->name);
				if (iter != _files.end()) {
					if (_cb)
						_cb(_dir, *iter, event->mask, _cookie);
				}
			}
			else {
				auto iter = _files.find(event->name);
				if (iter != _files.end()) {
					if (_cb)
						_cb(_dir, *iter, event->mask, _cookie);
				}
			}
			i += EVENT_SIZE + event->len;
		}
		
		memset(buffer, 0, BUF_LEN);
	}

}

