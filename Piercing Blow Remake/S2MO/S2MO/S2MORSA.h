#pragma once

// RSA 대칭키 암호키 비트사이즈
#define		RSA_KEY_BIT_SIZE		1024
#define		RSA_KEY_BYTE_SIZE		(RSA_KEY_BIT_SIZE/8)

class S2MORSA
{
	void*					m_pKeyPrivate;			// 비밀키(Decrypt용)
	void*					m_pKeyPublic;			// 공개키(Encrypt용)
	INT32					m_i32MaxPackedSize;		// 한번의 Encrypt에 가능한 최대 압축가능사이즈(Byte)

public:
	S2MORSA(void);
	~S2MORSA(void);

	BOOL					Create();
	void					Destory();

	BOOL					CreatePairKey();														// 공개키, 비밀키 생성, 키를 생성한 횟수가 리턴됩니다.

	BOOL					Encrypt( void* pOut, INT32& i32OutSize, void* pIn, INT32 i32InSize );	// 생성해둔(혹은 받은) 공개키로 데이터 암호화
	BOOL					Decrypt( void* pOut, INT32& i32OutSize, void* pIn, INT32 i32InSize );	// 생성해둔 비밀키로 데이터 복호화
	
	BOOL					WritePublicKey( char* pBuffer, INT32& i32WriteSize );					// 공개키 전달을 위해 버퍼 Write
	BOOL					ReadPublicKey( char* pBuffer, INT32& i32ReadSize );						// 공개키 수신을 위해 버퍼 Read
};
