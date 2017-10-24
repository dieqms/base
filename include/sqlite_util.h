/*
 * sqlite_util.h
 *
 *  Created on: 2017年9月1日
 *      Author: LJT
 */

#ifndef COMMON_SQLITE_UTIL_H_
#define COMMON_SQLITE_UTIL_H_

#include <sqlite3.h>
#include <string>
#include "thread_util.h"
using std::string;

namespace Base {


class SqliteData {
public:
	SqliteData() {
		_data = NULL;
		_row = 0;
		_col = 0;
	};
	~SqliteData() {
		Free();
	};
	void Free() {
		if (_data != NULL) {
			sqlite3_free_table(_data - _col);
			_data = NULL;
		}
	}

	string Item(int row, int col) {
		string data = "";
		if (row < _row && col < _col && _data[row * _col + col] != NULL)
			data = _data[row * _col + col];
		return data;
	}

private:
	char **	_data;
	int 	_row;
	int 	_col;

	friend class SqliteUtil;
};

class SqliteUtil {
public:
	SqliteUtil(string path) {
		_path = path;
		_conn = NULL;
		_opened = false;
	}

	~SqliteUtil() {
		Close();
	}

	bool Open() {
		if (!_opened) {
			int ret = sqlite3_open(_path.c_str(), &_conn);
			if(ret != SQLITE_OK) {
				printf("[SqliteUtil] open %s fail, error: %s\n", _path.c_str(), sqlite3_errmsg(_conn));
				return false;
			}
			_opened = true;
		}
		return _opened;
	}

	void Close() {
		if (_opened) {
			if(_conn != NULL)
				sqlite3_close(_conn);
			_opened = false;
		}
	}

	bool Execute(const string &sql) {
		if (!_opened)
			return false;

		int ret = 0;
		char * errmsg = NULL;
		ret = sqlite3_exec( _conn, sql.c_str(), 0, 0, &errmsg );
		if(ret != SQLITE_OK ) {
			printf("[SqliteUtil] sqlite3_exec error: %s, sql: %s\n", errmsg, sql.c_str());
			return false;
		}
		return true;
	}

	int64_t Query(const string &sql, int col_count, SqliteData& sqlite_data) {
		int 	result;
		char * 	errmsg = NULL;
		int 	nColumn = 0;
		int 	nRow = 0;
		char **	data;

		if (!_opened)
			return 0;

		result = sqlite3_get_table(_conn, sql.c_str(), &data, &nRow, &nColumn, &errmsg);
		if( SQLITE_OK == result ) {
			sqlite_data._data = data + nColumn;
			sqlite_data._row = nRow;
			sqlite_data._col = nColumn;
		}
		else {
			printf("[SqliteUtil] sqlite3_get_table error: %s\n", errmsg);
		}
		return nRow;
	}

	void Free(char **data) {
		sqlite3_free_table(data);
	}

private:
	string 		_path;
	sqlite3 *	_conn;
	bool		_opened;
};

}

#endif /* COMMON_SQLITE_UTIL_H_ */
