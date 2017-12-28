#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;

#include "string_util.h"

int main() {

	uint8_t bin_array[10] = {0x0a, 0x0b, 0x0c, 0x0d, 0x00, 0x12, 0x23, 0x34, 0x45, 0x56};
	char bin_str[21] = {0};

	std::string test;
	test.assign((const char*)bin_array, 10);
	
	printf("ori data: %s\n", Base::StringUtil::HexDump(bin_array, 10).c_str());
	printf("new data: %s\n", Base::StringUtil::HexDump(test.c_str(), test.size()).c_str());
	
	uint8_t bin_array2[10] = {0};
	memcpy(bin_array2, test.data(), 10);
	printf("new data2: %s\n", Base::StringUtil::HexDump(bin_array2, 10).c_str());
	
	return 0;
}

