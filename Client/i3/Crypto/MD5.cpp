#include "CryptoPCH.h"
#include "MD5.h"
#include "openssl/md5.h"
#include <malloc.h>

namespace Crypto
{
	MD5Handle MD5_CreateHandle()
	{
		return ::malloc( sizeof(MD5_CTX) );
	}

	void	  MD5_DeleteHandle(MD5Handle h)
	{
		::free(h);
	}

	bool MD5_Init(MD5Handle h)
	{
		return ::MD5_Init(reinterpret_cast<MD5_CTX*>(h)) != NULL;
	}

	void MD5_Update(MD5Handle h, const void* buff, size_t len)
	{
		::MD5_Update(reinterpret_cast<MD5_CTX*>(h), buff, len);
	}

	void MD5_Final(MD5Handle h, MD5Result& out)
	{
		::MD5_Final(out.data, reinterpret_cast<MD5_CTX*>(h));
	}

	bool MD5(const unsigned char* buff, size_t len, MD5Result& out)
	{
		return ::MD5(buff, len, out.data) != NULL;
	}

}
