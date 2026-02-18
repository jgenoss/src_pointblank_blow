// Crypto.cpp : Defines the exported functions for the DLL application.
//

#include "CryptoPCH.h"
#include "Crypto.h"
#include "openssl/rsa.h"
#include "openssl/rand.h"
#include "openssl/blowfish.h"

namespace Crypto
{
	namespace 
	{
		struct CryptoInitAndCleanup
		{
			CryptoInitAndCleanup()
			{
				ChangeRandomSeed();
			}

			~CryptoInitAndCleanup()
			{
//				ERR_remove_state(0);
				CRYPTO_set_id_callback(NULL);
				CRYPTO_set_locking_callback(NULL);

//				CONF_modules_unload(0);
//				EVP_cleanup();
				CRYPTO_cleanup_all_ex_data();

//				ERR_free_strings();
//				RAND_cleanup();
			}
		} _CryptoInitAndCleanup_;
		
		
	}

	void  ChangeRandomSeed()
	{
		srand((unsigned int)time(NULL));

		size_t num_buff = rand() % 1024 + 1;

		unsigned char* buff = (unsigned char*)::malloc( num_buff );

		for (size_t i = 0 ; i < num_buff ; ++i) 
			buff[i] =  rand() % 256;

		RAND_seed( buff, num_buff);

		::free(buff);
	}

	RSA*	GenerateRSAKey(int key_length)
	{
		return RSA_generate_key(key_length * 8, 3, NULL, NULL);
	}
		
	RSA*	CreateEmptyRSA()
	{
		return RSA_new();
	}

	void	DeleteRSA(RSA* r)
	{
		RSA_free(r);
	}

	bool	IsRSAKeyValid(const RSA* rsa)				// 생성 후 검사..
	{
		return RSA_check_key(rsa) == 1;
	}

	size_t	GetRSAOutputBuffSize(const RSA* rsa)	// 키버퍼와 데이터버퍼 모두 사이즈가 이 리턴값으로 고정된다...
	{
		return RSA_size(rsa);			// 넉넉하면 된다..
	}

	size_t	GetRSAInputBuffSize(const RSA* rsa)
	{
		return size_t(RSA_size(rsa) + RSA_INPUT_BUFFER_SIZE_OFFSET);		// 11보다 작아야 한다고 한다..
	}
	//

	//
	// 키 저장..
	//

	INT32	SizeOfConvertRSAPublicKeyToBuffer(RSA* rsa)	// 외부할당
	{
		return i2d_RSAPublicKey(rsa, NULL);	
	}

	INT32	SizeOfConvertRSAPrivateKeyToBuffer(RSA* rsa)	// 외부할당
	{
		return i2d_RSAPrivateKey(rsa, NULL);
	}

	void	ConvertRSAPublicKeyToBuffer( RSA* rsa, unsigned char* inoutBuff)	// 외부할당
	{
		i2d_RSAPublicKey(rsa, &inoutBuff);
	}

	void	ConvertRSAPrivateKeyToBuffer(RSA* rsa, unsigned char* inoutBuff)	// 외부할당
	{
		i2d_RSAPrivateKey(rsa, &inoutBuff);	
	}

	// 키 읽기..
	void	ConvertBufferToRSAPublicKey( RSA* out, const unsigned char* Buff, size_t Len)
	{
		d2i_RSAPublicKey(&out, &Buff, Len);
	}

	void	ConvertBufferToRSAPrivateKey( RSA* out, const unsigned char* Buff, size_t Len)
	{
		d2i_RSAPrivateKey(&out, &Buff, Len);
	}
	
	// 공개암호화(사용자) / 비공개해독(회사)
	bool	RSAPublicEncrypt(RSA* rsa, const unsigned char* Src, size_t SrcLen, unsigned char* Dest, size_t& DestLen)
	{
		DestLen = RSA_public_encrypt(SrcLen, Src, Dest, rsa, RSA_PKCS1_PADDING);
		return DestLen != -1;
	}

	bool	RSAPrivateDecrypt(RSA* rsa, const unsigned char* Src, size_t SrcLen, unsigned char* Dest, size_t& DestLen)
	{
		DestLen = RSA_private_decrypt(SrcLen, Src, Dest, rsa, RSA_PKCS1_PADDING);
		return DestLen != -1;
	}

	bool	RSAPrivateEncrypt(RSA* rsa, const unsigned char* Src, size_t SrcLen, unsigned char* Dest, size_t& DestLen)
	{
		DestLen = RSA_private_encrypt(SrcLen, Src, Dest, rsa, RSA_PKCS1_PADDING);
		return DestLen != -1;
	}

	bool	RSAPublicDecrypt(RSA* rsa, const unsigned char* Src, size_t SrcLen, unsigned char* Dest, size_t& DestLen)
	{
		DestLen = RSA_public_decrypt(SrcLen, Src, Dest, rsa, RSA_PKCS1_PADDING);
		return DestLen != -1;
	}

	//
	BF_KEY*				CreateEmptyBlowFishKey()
	{
		return new BF_KEY;
	}

	void				DeleteBlowFishKey(BF_KEY* k)
	{
		delete k;
	}

	void GetBlowFishKey(BF_KEY* out, const unsigned char* Buff, size_t BuffLen)
	{
		BF_set_key(out, BuffLen, Buff);
	}

	void BlowFishEncryt(const BF_KEY* key, unsigned char* EightBytesVec, int* VecOffset, const unsigned char* Src,
		unsigned char* Dest, long BuffLength)
	{
		BF_cfb64_encrypt(Src, Dest, BuffLength, key, EightBytesVec, VecOffset, BF_ENCRYPT);
	}

	void BlowFishDecrypt(const BF_KEY* key, unsigned char* EightBytesVec, int* VecOffset, const unsigned char* Src,
		unsigned char* Dest, long BuffLength)
	{
		BF_cfb64_encrypt(Src, Dest, BuffLength, key, EightBytesVec, VecOffset, BF_DECRYPT);
	}
	//

}
