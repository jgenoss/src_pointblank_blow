#ifndef __S2MO_BASE_H__
#define __S2MO_BASE_H__

#include "S2MORSA.h"

class S2MOBase
{
	INT16					m_i16PacketRandSeed;
	S2MORSA*				m_pS2MORSA;

public:
	S2MOBase();
	~S2MOBase();

	BOOL					Create();
	void					Destory();

	// Packet Rand Seed
	void					SetPacketRandSeed( INT16 i16PacketRandSeed )
	{
		m_i16PacketRandSeed = i16PacketRandSeed;
	}

	INT16					GetPacketRandSeed()
	{
		m_i16PacketRandSeed = (((m_i16PacketRandSeed * 214013L + 2531011L) >> 16) & 0x7fff);
		return m_i16PacketRandSeed;
	}

	// RSA Module
	BOOL					RSACreatePairKey()
	{
		return m_pS2MORSA->CreatePairKey();
	}
	
	BOOL					RSAWritePublicKey( char* pBuffer, INT32& i32WriteSize )
	{
		return m_pS2MORSA->WritePublicKey( pBuffer, i32WriteSize );
	}

	BOOL					RSAReadPublicKey( char* pBuffer, INT32& i32ReadSize )
	{
		return m_pS2MORSA->ReadPublicKey( pBuffer, i32ReadSize );
	}

	BOOL					RSAEncrypt( void* pOut, INT32& i32OutSize, void* pIn, INT32 i32InSize )
	{
		return m_pS2MORSA->Encrypt( pOut, i32OutSize, pIn, i32InSize );
	}

	BOOL					RSADecrypt( void* pOut, INT32& i32OutSize, void* pIn, INT32 i32InSize )
	{
		return m_pS2MORSA->Decrypt( pOut, i32OutSize, pIn, i32InSize );
	}
};
#endif