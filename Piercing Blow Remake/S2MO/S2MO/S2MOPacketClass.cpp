#include "pch.h"
#include "S2MOPacketClass.h"

void S2MOPacketBase::Reset()
{
	m_TPacketSize = 0;
}

BOOL S2MOPacketBase::Packing_T( char* pBuffer )
{
	if( FALSE == Packing_C( pBuffer+S2MO_PACKET_HEAD_SIZE ) )					return FALSE;
	m_TPacketSize += sizeof(INT16);			// RandSeed 로 인해 sizeof(INT16) 를 추가 합니다.
	memcpy( pBuffer, &m_TPacketSize, S2MO_PACKET_HEAD_SIZE );
	return TRUE;
}

BOOL S2MOPacketBase::UnPacking_T( char* pBuffer )
{	
	memcpy( &m_TPacketSize, pBuffer, S2MO_PACKET_HEAD_SIZE );
	if( FALSE == UnPacking_C( pBuffer+S2MO_PACKET_HEAD_SIZE ) )					return FALSE;
	return TRUE;
}

BOOL S2MOPacketBase::Packing_C(	char* pBuffer )
{	pBuffer;
	return TRUE;
}

BOOL S2MOPacketBase::UnPacking_C( char* pBuffer )
{
	pBuffer;
	return TRUE;
}