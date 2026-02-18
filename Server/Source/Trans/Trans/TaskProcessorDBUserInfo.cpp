#include "pch.h"
#include "TaskProcessor.h"

BOOL CTaskProcessor::_UpdateDBUserInfo()
{
	i3RingBuffer* pRingOut = m_pDBUserInfo->GetRingOutBuffer();
	if( 0 == pRingOut->GetBufferCount() )	return FALSE;

	RING_USER_INFO_OUT* pUserInfo = (RING_USER_INFO_OUT*)pRingOut->Pop();

	i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK );
	SendPacket.WriteData( &pUserInfo->m_i32SessionIdx,			sizeof( INT32 ) ); 
	SendPacket.WriteData( &pUserInfo->m_TUID,					sizeof( T_UID ) );
	SendPacket.WriteData( &pUserInfo->m_TResult,				sizeof(T_RESULT) ); 
	SendPacket.WriteData( &pUserInfo->m_DetailInfo,				sizeof(S2_USER_DETAIL_INFO) ); 
	SendGamePacket( pUserInfo->m_i32ServerIdx, &SendPacket );

	pRingOut->PopIdx();

	return TRUE;
}