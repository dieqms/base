/*
 * Number.cpp
 *
 *  Created on: 2017年10月24日
 *      Author: LJT
 */

#include "number_util.h"

namespace Base {
	Number::Unique::Unique(size_t range) {
		_range = range;
		flag = new uint8_t[range + 1];
	}
	
	Number::Unique::~Unique() {
		delete [] flag;
	}
	
	size_t Number::Unique::Get() {
		for (size_t i = 1; i <= _range; i++) {
			if (0 == flag[i]) {
				flag[i] = 1;
				return i;
			}
		}
		return 0;
	}
	
	void Number::Unique::Release(size_t num) {
		if (num <= _range)
			flag[num] = 0;
	}
	
	uint16_t Number::MakeU16(uint8_t h, uint8_t l)
	{
		return h << 8 | l;
	}

	uint32_t Number::MakeU32(uint16_t h, uint16_t l)
	{
		return h << 16 | l;
	}

	uint32_t Number::MakeU32(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
	{
		return a << 24 | b << 16 || c << 8 | d;
	}
	
	uint64_t MakeU64(uint32_t h, uint32_t l) {
		return (((uint64_t) h) << 16) | (uint64_t) l;
	}

	uint8_t LoByte(uint16_t v) {
		return (uint8_t) v;
	}

	uint8_t HiByte(uint16_t v) {
		return (uint8_t)(v >> 8);
	}

	uint16_t LoWord(uint32_t v) {
		return (uint16_t) v;
	}

	uint16_t HiWord(uint32_t v) {
		return (uint16_t)(v >> 16);
	}

	uint32_t LoDword(uint64_t v) {
		return (uint32_t) v;
	}

	uint32_t HiDword(uint64_t v) {
		return (uint32_t)(v >> 32);
	}
} /* namespace Base */
