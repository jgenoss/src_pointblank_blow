
//////////////////////////////////////////////////////////////////////////

// 프로젝트에 따라 Header를 변경해야 합니다.

//////////////////////////////////////////////////////////////////////////

#include "pch.h"			// PB 프로젝트에 Link시
//#include "stdafx.h"		// S2 프로젝트에 Link시
#include "CountryIP.h"

INT16 g_WatchEncryption(char * pPacket, INT16 iPacketsize, INT16 iBuffersize )
{
	UDP_WATCH_HEADER	* pHeader	= (UDP_WATCH_HEADER *)pPacket; 
	UINT8				* pStartPos = (UINT8*)&pPacket[sizeof(UDP_WATCH_HEADER)]; 

	UINT32 key = pHeader->m_ui16Size % 6; 
	BitRotateEncript( pStartPos, (iPacketsize - sizeof(UDP_WATCH_HEADER)), key + 1);

	return iPacketsize; 
}

INT16 g_WatchDecryption(char * pPacket, INT16 iPacketsize, INT16 iBuffersize)
{
	UDP_WATCH_HEADER	* pHeader	= (UDP_WATCH_HEADER *)pPacket; 
	UINT8				* pStartPos = (UINT8*)&pPacket[sizeof(UDP_WATCH_HEADER)]; 

	UINT32 key = pHeader->m_ui16Size % 6; 
	BitRotateDecript( pStartPos, (iPacketsize - sizeof(UDP_WATCH_HEADER)), key + 1);

	return iPacketsize; 
}