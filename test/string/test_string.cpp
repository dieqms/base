#include<iostream>
using namespace std;

#include "string_util.h"

int main() {

	uint8_t bin_array[10] = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x12, 0x23, 0x34, 0x45, 0x56};
	char bin_str[21] = {0};

	StringUtil::ToHex(bin_str, 20, bin_array, 10, true);

	printf("ToHex: %s\n", bin_str);
	return 0;
}

