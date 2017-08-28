/*
 * string_util.h
 *
 *  Created on: 2017年8月18日
 *      Author: LJT
 */
#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#include <string>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
using std::string;

class StringUtil {
public:
	static inline uint16_t MakeU16(uint8_t h, uint8_t l) {
		return h << 8 | l;
	}

	static inline uint32_t MakeU32(uint16_t h, uint16_t l) {
		return h << 16 | l;
	}

	static inline uint32_t MakeU32(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
		return a << 24 | b << 16 || c << 8 | d;
	}

	static inline uint64_t MakeU64(uint32_t h, uint32_t l) {
		return (((uint64_t) h) << 16) | (uint64_t) l;
	}

	static inline uint8_t LoByte(uint16_t v) {
		return (uint8_t) v;
	}

	static inline uint8_t HiByte(uint16_t v) {
		return (uint8_t)(v >> 8);
	}

	static inline uint16_t LoWord(uint32_t v) {
		return (uint16_t) v;
	}

	static inline uint16_t HiWord(uint32_t v) {
		return (uint16_t)(v >> 16);
	}

	static inline uint32_t LoDword(uint64_t v) {
		return (uint32_t) v;
	}

	static inline uint32_t HiDword(uint64_t v) {
		return (uint32_t)(v >> 32);
	}

	static bool IsNullOrSpace(const char* str, int len = -1) {
		if (str == NULL) {
			return true;
		}

		if (len == -1) {
			len = INT_MAX;
		}

		for (int i = 0; i < len && str[i] != 0; ++i) {
			if (!isspace(str[i])) {
				return false;
			}
		}
		return true;
	}

	static inline bool IsNullOrEmpty(const char* str) {
		return str == NULL || *str == 0;
	}

	static string& AppendFormat(string& str, const char* fmt, ...) {
		va_list ap;
		va_start(ap, fmt);
		_AppendFormatV(str, fmt, ap);
		va_end(ap);
		return str;
	}

	static string Format(const char* fmt, ...) {
		string str;

		va_list ap;
		va_start(ap, fmt);
		_AppendFormatV(str, fmt, ap);
		va_end(ap);

		return str;
	}

	static string& Format(string& str, const char* fmt, ...) {
		str.clear();

		va_list ap;
		va_start(ap, fmt);
		_AppendFormatV(str, fmt, ap);
		va_end(ap);

		return str;
	}

	static inline char ToHex(uint8_t num, bool upperCase = false) {
		const char* HexChars[] = { "0123456789abcdef", "0123456789ABCDEF" };
		return HexChars[(int) upperCase][num & 0x0F];
	}

	//convert binary to hex string
	static void ToHex(char* out, int outLen, const void* in, int inLen,
			bool upperCase = false) {
		const uint8_t* p = (const uint8_t*) in;
		inLen = std::min(inLen, outLen / 2);
		for (int i = inLen - 1; i >= 0; --i) {
			out[i * 2 + 1] = ToHex(p[i] & 0x0F, upperCase);
			out[i * 2] = ToHex((p[i] >> 4) & 0x0F, upperCase);
		}
	}

	static string ToHex(const void* in, int inLen,
			bool upperCase = false) {
		string hex;
		hex.resize(inLen * 2);

		const uint8_t* p = (const uint8_t*) in;
		for (int i = inLen - 1; i >= 0; --i) {
			hex.at(i * 2 + 1) = ToHex(p[i] & 0x0F, upperCase);
			hex.at(i * 2) = ToHex((p[i] >> 4) & 0x0F, upperCase);
		}

		return hex;
	}

	//convert hex string to binary
	static void FromHex(unsigned char* out, size_t outLen, const char *in,
			size_t inLen) {
		memset(out, 0, outLen);

		if (inLen == (size_t) - 1) {
			inLen = strlen(in);
		}

		inLen = std::min(inLen, outLen * 2);

		for (size_t i = 0; (i + 1) < inLen; i += 2) {
			out[i / 2] = (FromHex(in[i + 1]) & 0x0F)
					| ((FromHex(in[i]) << 4) & 0xF0);
		}
	}

	static uint8_t FromHex(char ch) {
		if (ch >= '0' && ch <= '9') {
			return ch - '0';
		} else if (ch >= 'a' && ch <= 'f') {
			return ch - 'a' + 10;
		} else if (ch >= 'A' && ch <= 'F') {
			return ch - 'A' + 10;
		} else {
			return 0;
		}
	}

	static void FromHex(const string& in, unsigned char *out,
			size_t outLen) {
		memset(out, 0, outLen);

		size_t inLen = in.size();
		inLen = std::min(inLen, outLen * 2);

		const char * in_char = in.data();
		for (size_t i = 0; (i + 1) < inLen; i += 2) {
			out[i / 2] = (FromHex(in_char[i + 1]) & 0x0F)
					| ((FromHex(in_char[i]) << 4) & 0xF0);
		}
	}

	// print binary in hex string way
	static string HexDump(const void* data, int len, int lineLength = 16,
			bool upperCase = true) {
		string result;
		if (data != NULL && len > 0) {
			static const int c_LineSize = lineLength = 16 * 4;
			static const int c_AsciiPos = lineLength = 16 * 3;

			int lines = len / lineLength + len % lineLength ? 1 : 0;
			char line[c_LineSize];
			result.reserve((c_LineSize + 1) * lines);

			int pos = 0;
			while (pos < len) {
				memset(line, ' ', sizeof(line));
				int step = std::min(lineLength = 16, len - pos);
				for (int i = 0; i < step; ++i) {
					line[i * 3] = ToHex(
							(((const uint8_t*) data)[pos + i] >> 4) & 0x0F,
							upperCase);
					line[i * 3 + 1] = ToHex(
							((const uint8_t*) data)[pos + i] & 0x0F, upperCase);
					line[i * 3 + 2] = ' ';
					line[c_AsciiPos + i] =
							isprint(((const uint8_t*) data)[pos + i]) ?
									((const uint8_t*) data)[pos + i] : '.';
				}

				if (!result.empty()) {
					result.push_back('\n');
				}
				result.append(line, sizeof(line));

				pos += step;
			}
		}
		return result;
	}

	static void HexPrint(unsigned char * buf, size_t len) {
		int i = 0;
		printf("[%lu]:", len);
		for (i = 0; i < len; i++) {
			printf("%02x ", buf[i]);
		}
		printf("\n");
	}

	static string& TrimLeft(string& str, const char* ch = "") {
		size_t i = 0;
		for (; i < str.size(); ++i) {
			if (strchr(ch, str[i]) == NULL && !isspace(str[i])) {
				break;
			}
		}

		str.erase(0, i);
		return str;
	}

	static char* TrimLeft(char* str, size_t len, const char* ch = "") {
		if (len == (size_t) - 1) {
			len = strlen(str);
		}

		size_t i = 0;
		for (; i < len; ++i) {
			if (strchr(ch, str[i]) == NULL && !isspace(str[i])) {
				break;
			}
		}

		memmove(str, str + i, len - i);
		return str;
	}

	static string& TrimRight(string& str, const char* ch = "") {
		size_t i = str.size();
		for (; i > 0; --i) {
			if (strchr(ch, str[i - 1]) == NULL && !isspace(str[i - 1])) {
				break;
			}
		}

		str.erase(i);
		return str;
	}

	static char* TrimRight(char* str, size_t len, const char* ch = "") {
		size_t i = len == (size_t) - 1 ? strlen(str) : len;
		for (; i > 0; --i) {
			if (strchr(ch, str[i - 1]) == NULL && !isspace(str[i - 1])) {
				break;
			}
		}

		str[i] = 0;
		return str;
	}

	static string& Trim(string& str, const char* ch = "") {
		TrimLeft(str, ch);
		TrimRight(str, ch);
		return str;
	}

	static char* Trim(char* str, size_t len, const char* ch = "") {
		TrimLeft(str, len, ch);
		TrimRight(str, len, ch);
		return str;
	}

	static char* MakeLower(char* str, int len = -1) {
		if (len == -1) {
			len = strlen(str);
		}

		for (int i = 0; i < len; ++i) {
			if (isupper(str[i]))
				str[i] = tolower(str[i]);
		}

		return str;
	}

	static string& MakeUpper(string& str) {
		for (auto p = str.begin(); p != str.end(); ++p) {
			if (islower(*p))
				*p = toupper(*p);
		}

		return str;
	}

	static string& MakeLower(string& str) {
		for (auto p = str.begin(); p != str.end(); ++p) {
			if (isupper(*p))
				*p = tolower(*p);
		}

		return str;
	}

	static int Compare(const void* s1, const void* s2, size_t len,
			bool ignoreCase = false) {
		if (ignoreCase) {
			for (size_t i = 0; i < len; ++i) {
				char ch1(tolower(((char*) s1)[i]));
				char ch2(tolower(((char*) s2)[i]));
				if (ch1 != ch2) {
					return ch1 - ch2;
				}
			}

			return 0;
		} else {
			return memcmp(s1, s2, len);
		}
	}

	static inline bool StartsWith(const char* str, const char* substr,
			bool ignoreCase = false) {
		return str != NULL && StartsWith(str, strlen(str), substr, ignoreCase);
	}

	static inline bool StartsWith(const char* str, size_t len,
			const char* substr, bool ignoreCase = false) {
		return str != NULL && substr != NULL
				&& StartsWith(str, len, substr, strlen(substr), ignoreCase);
	}

	static inline bool StartsWith(const char* str, size_t len,
			const char* substr, size_t subLen, bool ignoreCase = false) {
		if (str == 0 || substr == 0) {
			return false;
		}

		return len >= subLen && Compare(str, substr, subLen, ignoreCase) == 0;
	}

	static inline bool EndsWith(const char* str, const char* substr,
			bool ignoreCase = false) {
		return str != 0 && EndsWith(str, strlen(str), substr, ignoreCase);
	}

	static inline bool EndsWith(const char* str, size_t len, const char* substr,
			bool ignoreCase = false) {
		return str != 0 && substr != 0
				&& EndsWith(str, len, substr, strlen(substr), ignoreCase);
	}

	static inline bool EndsWith(const char* str, size_t len, const char* substr,
			size_t subLen, bool ignoreCase = false) {
		if (str == 0 || substr == 0) {
			return false;
		}

		return len >= subLen
				&& Compare(substr, str + len - subLen, subLen, ignoreCase) == 0;
	}

	static inline bool Contains(const char* str, const char* substr,
			bool ignoreCase = false) {
		return str != NULL && substr != NULL
				&& Contains(str, strlen(str), substr, strlen(substr),
						ignoreCase);
	}

	static inline bool Contains(const string& str,
			const string& substr, bool ignoreCase = false) {
		return Contains(str.c_str(), substr.c_str(), ignoreCase);
	}

	static inline bool Contains(const char* str, size_t len, const char* substr,
			bool ignoreCase = false) {
		return substr != NULL
				&& Contains(str, len, substr, strlen(substr), ignoreCase);
	}

	static inline bool Contains(const char* str, size_t len, const char* substr,
			size_t subLen, bool ignoreCase = false) {
		return Find(str, len, substr, subLen, ignoreCase) != NULL;
	}

	static inline char* Find(const char* str, const char* substr,
			bool ignoreCase = false) {
		if (str == NULL || substr == NULL) {
			return NULL;
		}

		return Find(str, strlen(str), substr, strlen(substr), ignoreCase);
	}

	static inline char* Find(const char* str, size_t len, const char* substr,
			bool ignoreCase = false) {
		if (substr == NULL) {
			return NULL;
		}

		return Find(str, len, substr, strlen(substr), ignoreCase);
	}

	static char* Find(const char* str, size_t len, const char* substr,
			size_t subLen, bool ignoreCase) {
		if (str == NULL || substr == NULL) {
			return NULL;
		}

		if (subLen == 0) {
			return (char*) str;
		}

		const char* end = str + len - subLen + 1;
		for (const char* cur = str; cur < end; ++cur) {
			if (Compare(cur, substr, subLen, ignoreCase) == 0) {
				return (char*) cur;
			}
		}

		return NULL;
	}

private:
	static string& _AppendFormatV(string& str, const char* fmt, va_list ap) {
		va_list cp;
		va_copy(cp, ap);
		int need_size = vsnprintf(0, 0, fmt, cp);
		va_end(cp);
		if (need_size < 0) {
			return str;
		}

		int curr_size = (int) str.size();
		str.resize(curr_size + need_size + 1);
		int final_size = vsnprintf(&str[curr_size], need_size + 1, fmt, ap);
		if (final_size > 0) {
			curr_size += std::min(final_size, need_size);
		}
		str.resize(curr_size);

		return str;
	}
};

#endif
