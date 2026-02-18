#pragma once

// 현재는 RSA와 BlowFish만 작성될것이다..
// 기본래퍼 작성 후, 베이스쪽에 응용함수를 만듬...
//
// 로그 루틴이 i3Base에 존재하기 때문에, 응용은 i3Base에서 이루어질 듯 하다...(여기서부터 자료구조 활용 가능)
//
//
// namespace i3와 i3접두어 모두 쓰지 않는다...  
// 
// (외부 사용자는 굳이 여기 안봐도 됩니다..)

// RSA

#if defined( CRYPTO_BUILD)
#define	EXPORT_CRYPTO		__declspec( dllexport)
#else
#define	EXPORT_CRYPTO		__declspec( dllimport)
#endif

struct rsa_st;		// RSA의 전방선언이 잘 안된다...
struct bf_key_st;
//

namespace Crypto
{
	typedef rsa_st		RSA;	//
	typedef bf_key_st	BF_KEY; //
}
	//
namespace Crypto
{
	static const int	RSA_INPUT_BUFFER_SIZE_OFFSET = -12;
	static const int	BF_KEYLENGTH_OFFSET_FROM_RSA_KEYLENGTH = RSA_INPUT_BUFFER_SIZE_OFFSET;

	EXPORT_CRYPTO void  ChangeRandomSeed();

	EXPORT_CRYPTO RSA*	GenerateRSAKey(int key_length);
	//
	EXPORT_CRYPTO RSA*	CreateEmptyRSA();
	EXPORT_CRYPTO void	DeleteRSA(RSA* r);

	EXPORT_CRYPTO bool		IsRSAKeyValid(const RSA* rsa);				// 생성 후 검사..
	EXPORT_CRYPTO size_t	GetRSAOutputBuffSize(const RSA* rsa);		// 키버퍼와 데이터버퍼 모두 사이즈가 이 리턴값으로 고정된다...
	EXPORT_CRYPTO size_t	GetRSAInputBuffSize(const RSA* rsa);		//
	//

	// 키 저장..

	EXPORT_CRYPTO INT32	SizeOfConvertRSAPublicKeyToBuffer(RSA* rsa);	// 외부할당
	EXPORT_CRYPTO INT32	SizeOfConvertRSAPrivateKeyToBuffer(RSA* rsa);	// 외부할당

	EXPORT_CRYPTO void	ConvertRSAPublicKeyToBuffer( RSA* rsa, unsigned char* inoutBuff);	// 외부할당
	EXPORT_CRYPTO void	ConvertRSAPrivateKeyToBuffer(RSA* rsa, unsigned char* inoutBuff);	// 외부할당

	// 키 읽기..
	EXPORT_CRYPTO void	ConvertBufferToRSAPublicKey( RSA* out, const unsigned char* Buff, size_t Len);
	EXPORT_CRYPTO void	ConvertBufferToRSAPrivateKey( RSA* out, const unsigned char* Buff, size_t Len);   
	//

	// 공개암호화(사용자) / 비공개해독(회사)
	EXPORT_CRYPTO bool	RSAPublicEncrypt(RSA* rsa, const unsigned char* Src, size_t SrcLen, unsigned char* Dest, size_t& DestLen);
	EXPORT_CRYPTO bool	RSAPrivateDecrypt(RSA* rsa, const unsigned char* Src, size_t SrcLen, unsigned char* Dest, size_t& DestLen);
	//

	//
	EXPORT_CRYPTO bool	RSAPrivateEncrypt(RSA* rsa, const unsigned char* Src, size_t SrcLen, unsigned char* Dest, size_t& DestLen);
	EXPORT_CRYPTO bool	RSAPublicDecrypt(RSA* rsa, const unsigned char* Src, size_t SrcLen, unsigned char* Dest, size_t& DestLen);
	//

	// 블로우 피시 래퍼..
	EXPORT_CRYPTO BF_KEY*	CreateEmptyBlowFishKey();
	EXPORT_CRYPTO void		DeleteBlowFishKey(BF_KEY* k);

	EXPORT_CRYPTO void GetBlowFishKey(BF_KEY* out, const unsigned char* Buff, size_t BuffLen);
	EXPORT_CRYPTO void BlowFishEncryt(const BF_KEY* key, unsigned char* EightBytesVec, int* VecOffset, const unsigned char* Src,
		unsigned char* Dest, long BuffLength);
	EXPORT_CRYPTO void BlowFishDecrypt(const BF_KEY* key, unsigned char* EightBytesVec, int* VecOffset, const unsigned char* Src,
		unsigned char* Dest, long BuffLength);
	
}
