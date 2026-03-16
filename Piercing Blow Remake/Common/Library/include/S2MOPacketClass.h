#ifndef __S2MO_PACKET_CLASS_H__
#define __S2MO_PACKET_CLASS_H__

#include <iostream>
using namespace std;

#include "S2MODataType.h"

typedef		INT16		T_PACKET_SIZE;

#define S2MO_PACKET_CONTENT_SIZE			8192
#define S2MO_PACKET_HEAD_SIZE				6

#pragma pack( push, 1)

struct S2MO_EXPORT_BASE S2MOPacketBase
{
protected:
	T_PACKET_SIZE			m_TPacketSize;
	T_PACKET_PROTOCOL		m_TProtocol;

	INT16					m_i16PacketRandSeed;	

public :
	S2MOPacketBase( T_PACKET_PROTOCOL TProtocol )	
	{	
		m_TPacketSize	= 0;
		m_TProtocol		= TProtocol;	
	}

	T_PACKET_SIZE			GetPacketSize()								{	return m_TPacketSize;				}
	T_PACKET_PROTOCOL		GetProtocol()								{	return m_TProtocol;					}

	void					SetPacketRandSeed( INT16 i16RandSeed )		{	m_i16PacketRandSeed = i16RandSeed;	}

	inline void				Reset();

	virtual BOOL			Packing_T( char* pBuffer );
	virtual BOOL			UnPacking_T( char* pBuffer );
	virtual BOOL			Packing_C( char* pBuffer );
	virtual BOOL			UnPacking_C( char* pBuffer );

	virtual S2MOPacketBase&	operator=( const S2MOPacketBase* pBase )
	{
		m_TPacketSize			= pBase->m_TPacketSize;
		m_TProtocol				= pBase->m_TProtocol;
		m_i16PacketRandSeed		= pBase->m_i16PacketRandSeed;	

		return *this;
	}
};

#pragma pack( pop )

template <T_PACKET_PROTOCOL P>
struct S2MOPacketBaseT : public S2MOPacketBase
{
	S2MOPackable			m_S2Packable;

protected:
	void					_SetValue(	S2MOPackable& pData )
	{
		S2MOPackable* pTemp = m_S2Packable.GetNext();
		m_S2Packable.SetNext( &pData );
		pData.SetNext( pTemp );
	}

public:	
	
	S2MOPacketBaseT()
		: S2MOPacketBase( P )
	{
	}
	
	BOOL					Packing_C( char* pBuffer )
	{
		S2MOPackable* pS2Packable = m_S2Packable.GetNext();
		int i32Size = 0;
		while( pS2Packable )
		{
			if( FALSE == pS2Packable->Packing( &pBuffer[ m_TPacketSize ], i32Size ) )				return FALSE;
			m_TPacketSize	+= (T_PACKET_SIZE)i32Size;

			pS2Packable = pS2Packable->GetNext();
		}
		
		return TRUE;
	}
	BOOL					UnPacking_C( char* pBuffer )
	{
		int i32Size = 0;
		int i32PacketSize = 0;
		S2MOPackable* pS2Packable = m_S2Packable.GetNext();
		while( pS2Packable )
		{
			if( FALSE == pS2Packable->UnPacking( &pBuffer[ i32PacketSize ], i32Size ) )				return FALSE;
			i32PacketSize += i32Size;

			pS2Packable = pS2Packable->GetNext();
		}
		return TRUE;
	}

	virtual S2MOPacketBase&	operator=( const S2MOPacketBase* pBase )
	{
		S2MOPacketBase::operator=( pBase );

		S2MOPackable* pS2Dis	= m_S2Packable.GetNext();
		S2MOPackable* pS2Src	= ((S2MOPacketBaseT*)pBase)->m_S2Packable.GetNext();
		while( pS2Src )
		{
			pS2Dis->Copy( pS2Src );

			pS2Src = pS2Src->GetNext();
			pS2Dis = pS2Dis->GetNext();
		}

		return *this;
	}
};

template <T_PACKET_PROTOCOL P>
struct S2MOPacketBaseResultT : public S2MOPacketBaseT<P>
{
public:

	T_RESULT				m_TResult;

	virtual BOOL			Packing_C( char* pBuffer )
	{
		memcpy( pBuffer, &m_TResult, sizeof(T_RESULT) );
		m_TPacketSize += sizeof(T_RESULT);

		if( S2MO_IS_SUCCESS( m_TResult ) )
		{
			if( FALSE == S2MOPacketBaseT::Packing_C( pBuffer ) )									return FALSE;
		}
		return TRUE;
	}
	virtual BOOL			UnPacking_C( char* pBuffer )
	{
		memcpy( &m_TResult, pBuffer, sizeof(T_RESULT) );

		if( S2MO_IS_SUCCESS( m_TResult ) )
		{
			if( FALSE == S2MOPacketBaseT::UnPacking_C( &pBuffer[sizeof(T_RESULT)] ) )				return FALSE;
		}
		return TRUE;
	}

	virtual S2MOPacketBase&	operator=( const S2MOPacketBase* pBase )
	{
		S2MOPacketBaseT::operator=( pBase );

		m_TResult = ((S2MOPacketBaseResultT*)pBase)->m_TResult;		

		return *this;
	}
};

template <T_PACKET_PROTOCOL P>
struct S2MOPacketBaseEncryptionT : public S2MOPacketBaseT<P>
{
public:

	virtual BOOL			Packing_C( char* pBuffer )
	{
		if( FALSE == S2MOPacketBaseT::Packing_C( pBuffer ) )										return FALSE;

		INT32 i32Size = 0;
		
		if( 0 < m_TPacketSize )
		{
			char pEncrypt[ S2MO_PACKET_CONTENT_SIZE ];
			S2MOPacketEncrypt( pEncrypt, i32Size, (void*)pBuffer, m_TPacketSize );
			memcpy( pBuffer, pEncrypt, i32Size );
			m_TPacketSize = (T_PACKET_SIZE)i32Size;
		}

		return TRUE;
	}
	virtual BOOL			UnPacking_C( char* pBuffer )
	{		
		if( 0 < m_TPacketSize )
		{
			INT32 i32Size = 0;
			char pEncrypt[ S2MO_PACKET_CONTENT_SIZE ];
			S2MOPacketDecrypt( pEncrypt, i32Size, (void*)pBuffer, m_TPacketSize );
			m_TPacketSize = (T_PACKET_SIZE)i32Size;
			memcpy( pBuffer, pEncrypt, i32Size );
		}

		if( FALSE == S2MOPacketBaseT::UnPacking_C( pBuffer ) )										return FALSE;

		return TRUE;
	}

};
#endif