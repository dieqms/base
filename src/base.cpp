/*
 * base.cpp
 *
 *  Created on: 2017年10月24日
 *      Author: LJT
 */

#include "base.h"

uint16_t __builtin_bswap16(uint16_t a) {
        return ((a >> 8) & 0xFF) | ((a << 8) & 0xFF00);
}

string ExecuteCmd(const char* cmdline, int* status) {
	std::string ret;
	int exit_status = -1;
	FILE* fp = ::popen(cmdline, "r");
	if (fp != NULL) {
		char buf[64 * 1024];
		size_t read = 0;
		while ((read = fread(buf, 1, sizeof(buf), fp)) != 0) {
			ret.append(buf, read);
		}

		exit_status = pclose(fp);
		fp = NULL;
	}

	if (status != NULL) {
		*status = exit_status;
	}
	return ret;
}


