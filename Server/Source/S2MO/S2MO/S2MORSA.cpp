#include "pch.h"
#include "RSA/pch_rsa.h"
#include "../../../Library/include/S2MORSA.h"
#include "RSA/dll.h"
#include <time.h>

USING_NAMESPACE(CryptoPP)

S2MORSA::S2MORSA(void)
{
	m_pKeyPrivate	= NULL;
	m_pKeyPublic	= NULL;
}

S2MORSA::~S2MORSA(void)
{
	Destory();
}

static OFB_Mode<AES>::Encryption s_globalRNG;

RandomNumberGenerator & GlobalRNG()
{
	return s_globalRNG;
}

BOOL S2MORSA::Create()
{
	m_pKeyPrivate	= new RSAES_PKCS1v15_Decryptor;
	if( NULL == m_pKeyPrivate )		return FALSE;
	m_pKeyPublic	= new RSAES_PKCS1v15_Encryptor;
	if( NULL == m_pKeyPublic )		return FALSE;

	return TRUE;
}

void S2MORSA::Destory()
{
	RSAES_PKCS1v15_Decryptor* pPriv = (RSAES_PKCS1v15_Decryptor*)m_pKeyPrivate;
	if( pPriv )
	{
		delete pPriv;
		pPriv = NULL;

	}
	RSAES_PKCS1v15_Encryptor* pPub = (RSAES_PKCS1v15_Encryptor*)m_pKeyPublic;
	if( pPub )
	{
		delete pPub;
		pPub = NULL;
	}
}

BOOL S2MORSA::CreatePairKey()
{
	std::string seed = IntToString(time(NULL));
	seed.resize(16);
	s_globalRNG.SetKeyWithIV((byte *)seed.data(), 16, (byte *)seed.data());

	// 비밀키 생성 (복호화용)
	RSAES_PKCS1v15_Decryptor priv(GlobalRNG(), RSA_KEY_BIT_SIZE);
	RSAES_PKCS1v15_Decryptor* pPriv = (RSAES_PKCS1v15_Decryptor*)m_pKeyPrivate;
	pPriv->AccessKey().SetPrime1( priv.GetKey().GetPrime1() );
	pPriv->AccessKey().SetPrime2( priv.GetKey().GetPrime2() );
	pPriv->AccessKey().SetPrivateExponent( priv.GetKey().GetPrivateExponent() );
	pPriv->AccessKey().SetModPrime1PrivateExponent( priv.GetKey().GetModPrime1PrivateExponent() );
	pPriv->AccessKey().SetModPrime2PrivateExponent( priv.GetKey().GetModPrime2PrivateExponent() );
	pPriv->AccessKey().SetMultiplicativeInverseOfPrime2ModPrime1( priv.GetKey().GetMultiplicativeInverseOfPrime2ModPrime1() );
	pPriv->AccessKey().SetModulus( priv.GetKey().GetModulus() );
	pPriv->AccessKey().SetPublicExponent( priv.GetKey().GetPublicExponent() );

	// 공개키 생성 (암호화용)
	RSAES_PKCS1v15_Encryptor pub(priv);
	RSAES_PKCS1v15_Encryptor* pPub = (RSAES_PKCS1v15_Encryptor*)m_pKeyPublic;
	pPub->AccessKey().SetModulus( pub.GetKey().GetModulus() );
	pPub->AccessKey().SetPublicExponent( pub.GetKey().GetPublicExponent() );

	m_i32MaxPackedSize = pub.FixedMaxPlaintextLength();

	return TRUE;
}

BOOL S2MORSA::WritePublicKey( char* pBuffer, INT32& i32WriteSize )
{
	i32WriteSize = 0;
	Integer aKey[2];

	aKey[0] = ((RSAES_PKCS1v15_Encryptor*)m_pKeyPublic)->GetKey().GetModulus();
	aKey[1] = ((RSAES_PKCS1v15_Encryptor*)m_pKeyPublic)->GetKey().GetPublicExponent();

	for( INT32 k = 0; k < 2; k++ )
	{
		UINT16 ui16Size = aKey[k].ByteCount();
		memcpy( pBuffer + i32WriteSize, &ui16Size, sizeof(UINT16) );
		i32WriteSize += sizeof(UINT16);
		for( UINT16 i = 0; i < ui16Size; i++ )
		{
			UINT8 ui8Byte = (UINT8)aKey[k].GetByte( i );
			memcpy( pBuffer + i32WriteSize, &ui8Byte, sizeof(UINT8) );
			i32WriteSize++;
		}
	}

	return TRUE;
}

BOOL S2MORSA::ReadPublicKey( char* pBuffer, INT32& i32ReadSize )
{
	i32ReadSize = 0;
	Integer aKey[2];

	for( INT32 k = 0; k < 2; k++ )
	{
		UINT16 ui16Size;
		memcpy( &ui16Size, pBuffer + i32ReadSize, sizeof(UINT16) );
		i32ReadSize += sizeof(UINT16);
		for( UINT16 i = 0; i < ui16Size; i++ )
		{
			UINT8 ui8Byte;
			memcpy( &ui8Byte, pBuffer + i32ReadSize, sizeof(UINT8) );
			aKey[k].SetByte( i, ui8Byte );
			i32ReadSize++;
		}
	}

	((RSAES_PKCS1v15_Encryptor*)m_pKeyPublic)->AccessKey().SetModulus( aKey[0] );
	((RSAES_PKCS1v15_Encryptor*)m_pKeyPublic)->AccessKey().SetPublicExponent( aKey[1] );

	m_i32MaxPackedSize = ((RSAES_PKCS1v15_Encryptor*)m_pKeyPublic)->FixedMaxPlaintextLength();

	std::string seed = IntToString(time(NULL));
	seed.resize(16);
	s_globalRNG.SetKeyWithIV((byte *)seed.data(), 16, (byte *)seed.data());

	return TRUE;
}

BOOL S2MORSA::Encrypt( void* pOut, INT32& i32OutSize, void* pIn, INT32 i32InSize )
{
	INT32 i = 0;
	while( 0 < i32InSize )
	{
		UINT32 ui32Size = ( i32InSize > m_i32MaxPackedSize ) ? m_i32MaxPackedSize : i32InSize;

		((RSAES_PKCS1v15_Encryptor*)m_pKeyPublic)->Encrypt( GlobalRNG(), (const byte*)pIn + m_i32MaxPackedSize*i, ui32Size, (byte*)pOut + RSA_KEY_BYTE_SIZE*i );
		i32InSize -= ui32Size;
		i++;
	}

	i32OutSize = RSA_KEY_BYTE_SIZE*i;

	return TRUE;
}

BOOL S2MORSA::Decrypt( void* pOut, INT32& i32OutSize, void* pIn, INT32 i32InSize )
{
	i32OutSize = 0;
	for( INT32 i = 0; i < i32InSize /RSA_KEY_BYTE_SIZE; i++ )
	{
		DecodingResult dr = ((RSAES_PKCS1v15_Decryptor*)m_pKeyPrivate)->Decrypt( GlobalRNG(), (const byte*)pIn + i*RSA_KEY_BYTE_SIZE, RSA_KEY_BYTE_SIZE, (byte*)pOut + m_i32MaxPackedSize*i );

		if ( !dr.isValidCoding )					return FALSE;
		i32OutSize += (INT16)dr.messageLength;
	}
	
	return TRUE;
}
