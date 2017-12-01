/*
 * openssl_util.h
 *
 *  Created on: 2017年11月22日
 *      Author: LJT
 */

#ifndef INCLUDE_BASE_OPENSSL_UTIL_H_
#define INCLUDE_BASE_OPENSSL_UTIL_H_

#include <string>
#include <openssl/ssl.h>
#include <openssl/aes.h>

namespace Base {

class OpensslUtil {
public:
	OpensslUtil();
	virtual ~OpensslUtil();
	
	static std::string AesEncrypt(const std::string & info, const char* key)//, int olen)
	{
	    char key_adjust[16] = {0};
	    strcpy(key_adjust, key);

	    int len = info.size() % AES_BLOCK_SIZE ? (info.size()/AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE : info.size();
	    char * buffer_in = new char[len];
	    char * buffer_out = new char[len];
	    memset(buffer_in, 0, len);
	    memset(buffer_out, 0, len);

	    char * in = buffer_in;
	    char * out = buffer_out;
	    memcpy(in, info.c_str(), info.size());

	    AES_KEY aes;
	    if(AES_set_encrypt_key((unsigned char*)key_adjust, 128, &aes) < 0)
	    {
	        return "";
	    }

	    int en_len=0;
	    while(en_len < len)//输入输出字符串够长，并且是AES_BLOCK_SIZE的整数倍，需要严格限制
	    {
	        AES_encrypt((unsigned char*)in, (unsigned char*)out, &aes);
	        in += AES_BLOCK_SIZE;
	        out += AES_BLOCK_SIZE;
	        en_len += AES_BLOCK_SIZE;
	    }

	    std::string encrypted;
	    encrypted.assign(buffer_out, len);

//	    printf("<<<<aes_encrypt:\n");
//	    printf("pass: %s\n", key);
//	    printf("ori: %s\n", Base::StringUtil::HexDump(buffer_in, len).c_str());
//	    printf("encrypted: %s\n", Base::StringUtil::HexDump(encrypted.c_str(), encrypted.size()).c_str());

	    delete [] buffer_in;
	    delete [] buffer_out;

	    return encrypted;
	}

        static std::string AesDecrypt(uint8_t * data, int data_len, const char* key)
	{
	    char key_adjust[16] = {0};
	    strcpy(key_adjust, key);

	    int len = data_len % AES_BLOCK_SIZE ? (data_len/AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE : data_len;
	    char * buffer_in = new char[len];
	    char * buffer_out = new char[len];

	    memset(buffer_in, 0, len);
	    memset(buffer_out, 0, len);

	    char * in = buffer_in;
	    char * out = buffer_out;
	    memcpy(in, data, data_len);

	    AES_KEY aes;
	    if(AES_set_decrypt_key((unsigned char*)key_adjust, 128, &aes) < 0)
	    {
	        return "";
	    }
	    int en_len = 0;
	    while(en_len < len)
	    {
	        AES_decrypt((unsigned char*)in, (unsigned char*)out, &aes);
	        in += AES_BLOCK_SIZE;
	        out += AES_BLOCK_SIZE;
	        en_len += AES_BLOCK_SIZE;
	    }

	    std::string encrypted;
	    encrypted.assign(buffer_out, len);

//	    printf(">>>>aes_decrypt: \n");
//	    printf("pass: %s\n", key);
//	    printf("ori: %s\n", Base::StringUtil::HexDump(buffer_in, len).c_str());
//	    printf("decrypted: %s\n", Base::StringUtil::HexDump(encrypted.c_str(), encrypted.size()).c_str());

	    delete [] buffer_in;
	    delete [] buffer_out;

	    return encrypted;
	}
	
	static char * Base64Encode(const char * input, int length, bool with_new_line = false)
	{
		BIO * bmem = NULL;
		BIO * b64 = NULL;
		BUF_MEM * bptr = NULL;

		b64 = BIO_new(BIO_f_base64());
		if(!with_new_line) {
			BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
		}
		bmem = BIO_new(BIO_s_mem());
		b64 = BIO_push(b64, bmem);
		BIO_write(b64, input, length);
		BIO_flush(b64);
		BIO_get_mem_ptr(b64, &bptr);

		char * buff = (char *)malloc(bptr->length + 1);
		memcpy(buff, bptr->data, bptr->length);
		buff[bptr->length] = 0;

		BIO_free_all(b64);

		return buff;
	}

	static char * Base64Decode(char * input, int * length, bool with_new_line = false)
	{
	    BIO * b64 = NULL;
	    BIO * bmem = NULL;
	    char * buffer = (char *)malloc(*length);
	    memset(buffer, 0, *length);

	    b64 = BIO_new(BIO_f_base64());
	    if(!with_new_line) {
	        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	    }
	    bmem = BIO_new_mem_buf(input, *length);
	    bmem = BIO_push(b64, bmem);
	    int new_len = BIO_read(bmem, buffer, *length);

	    BIO_free_all(bmem);

	    *length = new_len;
	    return buffer;
	}
};

} /* namespace Base */

#endif /* INCLUDE_BASE_OPENSSL_UTIL_H_ */
