/*
 * Number.h
 *
 *  Created on: 2017年10月24日
 *      Author: LJT
 */

#ifndef _UTIL_NUMBER_H_
#define _UTIL_NUMBER_H_
#include <stdint.h>
#include <stdlib.h>

namespace Base {

class Number {
public:
	class Unique {
	public:
		Unique(size_t range) {
			_range = range;
			flag = new uint8_t[range + 1];
		}
		
		~Unique() {
			delete [] flag;
		}
		
		size_t Get() {
			for (size_t i = 1; i <= _range; i++) {
				if (0 == flag[i]) {
					flag[i] = 1;
					return i;
				}
			}
			return 0;
		}
		
		void Release(size_t num) {
			if (num <= _range)
				flag[num] = 0;
		}
		
	private:
		size_t _range;
		uint8_t * flag;
	};
	
	static inline uint16_t MakeU16(uint8_t h, uint8_t l)
	{
		return h << 8 | l;
	}

	static inline uint32_t MakeU32(uint16_t h, uint16_t l)
	{
		return h << 16 | l;
	}

	static inline uint32_t MakeU32(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
	{
		return a << 24 | b << 16 || c << 8 | d;
	}
};

}

#endif /* 3RD_BASE_UTIL_NUMBER_H_ */
