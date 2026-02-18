#pragma once

#if defined( CRYPTO_BUILD)
#define	EXPORT_CRYPTO		__declspec( dllexport)
#else
#define	EXPORT_CRYPTO		__declspec( dllimport)
#endif


namespace Crypto
{
	typedef void*		MD5Handle;
}

namespace Crypto
{
	static const size_t MD5_Digest_Length = 16;

	struct MD5Result
	{
		unsigned char  data[MD5_Digest_Length];
	};
	
	EXPORT_CRYPTO MD5Handle MD5_CreateHandle();
	EXPORT_CRYPTO void		MD5_DeleteHandle(MD5Handle h);

	EXPORT_CRYPTO bool MD5_Init(MD5Handle h);
	EXPORT_CRYPTO void MD5_Update(MD5Handle h, const void* buff, size_t len);
	EXPORT_CRYPTO void MD5_Final(MD5Handle h, MD5Result& out);
	EXPORT_CRYPTO bool MD5(const unsigned char* buff, size_t len, MD5Result& out);
	
}
