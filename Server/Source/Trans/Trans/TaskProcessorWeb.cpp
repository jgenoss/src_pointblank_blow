#include "pch.h"
#include "TaskProcessor.h"
#include "ModuleControl.h"
#include "UserFinder.h"

bool CTaskProcessor::_UpdateASCWeb()
{
	ASC_WEB_REQ_INFO *pReqInfo;
	INT32 i32Count = m_pRingASCWeb->GetBufferCount();
	if( 0 == i32Count )	return false;

	// 한프레임에 10개까지만 처리
	if( i32Count > TASK_WEB_REQ_MAX_PROC ) i32Count = TASK_WEB_REQ_MAX_PROC;

	for( INT32 i = 0; i < i32Count; i++ )
	{
		pReqInfo = (ASC_WEB_REQ_INFO *)m_pRingASCWeb->Pop();

		if( pReqInfo->i64UID < 0 ) return false;

		switch( pReqInfo->reqType )
		{
		case ASC_WEB_GM_KICK:
			_AccountKick( pReqInfo->i64UID, ACCOUNT_KICK_GM_TOOL );
			break;
		case ASC_WEB_USER_INFO_LOCATION:
			_ASCWeb_UserInfoLocationReq( pReqInfo );
			break;
		}

		m_pRingASCWeb->PopIdx();
	}

	return true;
}

void CTaskProcessor::_ASCWeb_UserInfoLocationReq( ASC_WEB_REQ_INFO *pReqInfo )
{
	INT32	i32Rv = EVENT_ERROR_FAIL;

	USER_NODE *pNode = g_pUserFinder->GetUserNodeByUID( pReqInfo->i64UID );
	if( pNode != NULL )
	{
		i3NetworkPacket	SendPacket( PROTOCOL_ASC_WEB_USER_INFO_LOCATION_REQ );
		SendPacket.WriteData( &pReqInfo->i32WebSessionIdx,		sizeof(INT32) );
		SendPacket.WriteData( &pReqInfo->i64UID,				sizeof(T_UID) );
		SendPacket.WriteData( &pNode->_i32Sessionidx,			sizeof(INT32) );
		SendGamePacket(pNode->_i32Serveridx, &SendPacket);
	}
	else
	{
		i3NetworkPacket SendPacket( PROTOCOL_ASC_WEB_USER_INFO_LOCATION_ACK );
		SendPacket.WriteData( &pReqInfo->i32WebSessionIdx,		sizeof(INT32) );
		SendPacket.WriteData( &i32Rv,							sizeof(INT32) );
		g_pModuleControl->SendPacketMessage( &SendPacket );
	}
}
