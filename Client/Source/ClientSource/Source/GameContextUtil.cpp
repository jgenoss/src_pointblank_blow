#include "pch.h"
#include "GameContextUtil.h"

#include "UI/UIFramework.h"
#include "ui/UIMainFrame.h"
#include "UserLocationInfo.h"
#include "MyRoomInfoContext.h"
#include "Network/GameNetworkManager.h"


bool GameContextUtil::IsSameRoom(const CUserLocationInfo & UserLocInfo)
{
	if ( static_cast<UINT32>(g_pServerInfo->GetCurGameServerIndex()) != UserLocInfo.GetServerNumber() )
		return false;

	if ( static_cast<UINT32>(g_pServerInfo->GetCurChannelNumber()) != UserLocInfo.GetChannelNumber() )
		return false;

	i3Stage* pStage = g_pFramework->GetCurrentStage();
	if( i3::same_of<UIFramework*>(pStage)) 
	{
		if( ((UIFramework*)pStage)->GetUIMainframe()->GetCurrentPhaseType() != UIPHASE_READY_ROOM)
		{
			return false;
		}
	}

	if ( static_cast<UINT32>(MyRoomInfoContext::i()->getMyRoomIndex()) != UserLocInfo.GetRoomNumber() )
		return false;

	return true;
}

// getMyRoomIndex는 함수명이 좋은 함수가 아니다...
namespace GameContextUtil
{

i3::rc_wstring GetRadioMessage( RADIOCHAT_TYPE nType, INT32 nMsgNum )
{
	char szTemp[256];

	strncpy(szTemp, "STBL_IDX_RADIOMSG_CMD_KR1", 256);

	if( g_pEnvSet->m_nRadioMessageType == 1 )
	{
		switch( nType )
		{
		case RADIOCHAT_TYPE_COMMAND :	_snprintf(szTemp, 256, "STBL_IDX_RADIOMSG_CMD_KR%d", nMsgNum); break;
		case RADIOCHAT_TYPE_REQUEST :	_snprintf(szTemp, 256, "STBL_IDX_RADIOMSG_REQ_KR%d", nMsgNum); break;
		case RADIOCHAT_TYPE_STATUS :	_snprintf(szTemp, 256, "STBL_IDX_RADIOMSG_STT_KR%d", nMsgNum); break;
		case RADIOCHAT_TYPE_AUTO :		_snprintf(szTemp, 256, "STBL_IDX_RADIOMSG_AUTO_KR%d", nMsgNum); break;
		default : I3PRINTLOG(I3LOG_FATAL,  "Invalid radio message." ); return i3::rc_wstring();
		}
	}
	else
	{
		switch( nType )
		{
		case RADIOCHAT_TYPE_COMMAND :	_snprintf(szTemp, 256, "STBL_IDX_RADIOMSG_CMD_US%d", nMsgNum); break;
		case RADIOCHAT_TYPE_REQUEST :	_snprintf(szTemp, 256, "STBL_IDX_RADIOMSG_REQ_US%d", nMsgNum); break;
		case RADIOCHAT_TYPE_STATUS :	_snprintf(szTemp, 256, "STBL_IDX_RADIOMSG_STT_US%d", nMsgNum); break;
		case RADIOCHAT_TYPE_AUTO :		_snprintf(szTemp, 256, "STBL_IDX_RADIOMSG_AUTO_US%d", nMsgNum); break;
		default : I3PRINTLOG(I3LOG_FATAL,  "Invalid radio message." ); return i3::rc_wstring();
		}
	}

	return GAME_RCSTRING(szTemp);
}

i3SoundPlayState * PlayRadioMessageSound( RADIOCHAT_TYPE nType, INT32 nMsgNum, CGameCharaBase * pPlayer)
{
	I3ASSERT( pPlayer != nullptr);
	INT32 idx = 0;

	if( g_pEnvSet->m_nRadioMessageType == 1 )
	{
		switch( nType )
		{
		case RADIOCHAT_TYPE_COMMAND :	idx = STBL_IDX_RADIOMSG_CMD_KR; break;
		case RADIOCHAT_TYPE_REQUEST :	idx = STBL_IDX_RADIOMSG_REQ_KR; break;
		case RADIOCHAT_TYPE_STATUS :	idx = STBL_IDX_RADIOMSG_STT_KR; break;
		case RADIOCHAT_TYPE_AUTO :		idx = STBL_IDX_RADIOMSG_AUTO_KR; break;
		default:
			return nullptr;
		}
	}
	else
	{
		switch( nType )
		{
		case RADIOCHAT_TYPE_COMMAND :	idx = STBL_IDX_RADIOMSG_CMD_US; break;
		case RADIOCHAT_TYPE_REQUEST :	idx = STBL_IDX_RADIOMSG_REQ_US; break;
		case RADIOCHAT_TYPE_STATUS :	idx = STBL_IDX_RADIOMSG_STT_US; break;
		case RADIOCHAT_TYPE_AUTO :		idx = STBL_IDX_RADIOMSG_AUTO_US; break;
		default:
			return nullptr;
		}
	}

	idx += nMsgNum - STBL_IDX_RADIOMSG_CMD_KR;

	INT32 nSex = pPlayer->getDBInfo()->GetCharaSexTypeDISPLAY();
	
	if(pPlayer->IsDino())
	{
		nSex = 2;
	}

	return g_pSndMng->StartRadioMessage( idx, nSex, pPlayer);
}


void QA_SetRank(UINT32 rank)
{
/*
	UINT32 Temp;
	Temp = 1;
	i3NetworkPacket SendPacket(PROTOCOL_TEST_NC_QA);
	SendPacket.WriteData(&Temp, sizeof(UINT32));
	SendPacket.WriteData(&rank, sizeof(UINT32));

	CGameNetworkManager::i()->SendTCPMessage( &SendPacket);
	//m_pTCPClientSocket->SendMessage(&SendPacket);
*/
}

void QA_SetPino(UINT32 Pino)
{
/*
	UINT32 Temp;
	Temp = 2;
	i3NetworkPacket SendPacket(PROTOCOL_TEST_NC_QA);
	SendPacket.WriteData(&Temp, sizeof(UINT32));
	SendPacket.WriteData(&Pino, sizeof(UINT32));

	CGameNetworkManager::i()->SendTCPMessage( &SendPacket);
	//m_pTCPClientSocket->SendMessage(&SendPacket);
*/
}

void QA_SetChallenge(void)
{
	if( g_pFramework->IsBattleStage() == false)
		return;
}

void QA_SetExp(UINT32 Exp)
{
/*
	UINT32 Temp;
	Temp = 3;
	i3NetworkPacket SendPacket(PROTOCOL_TEST_NC_QA);
	SendPacket.WriteData(&Temp, sizeof(UINT32));
	SendPacket.WriteData(&Exp, sizeof(UINT32));

	CGameNetworkManager::i()->SendTCPMessage( &SendPacket);
*/
}

void QA_SetCash(UINT32 Exp)
{
/*
	UINT32 Temp;
	Temp = 4;
	i3NetworkPacket SendPacket(PROTOCOL_TEST_NC_QA);
	SendPacket.WriteData(&Temp, sizeof(UINT32));
	SendPacket.WriteData(&Exp, sizeof(UINT32));

	CGameNetworkManager::i()->SendTCPMessage( &SendPacket);
*/
}



void QA_SetRound(UINT8 Red, UINT8 Blue)
{
/*
	UINT32 Temp;
	Temp = 10;
	i3NetworkPacket SendPacket(PROTOCOL_TEST_NC_QA);
	SendPacket.WriteData(&Temp, sizeof(UINT32));
	SendPacket.WriteData(&Red,	sizeof(UINT8));
	SendPacket.WriteData(&Blue, sizeof(UINT8));

	CGameNetworkManager::i()->SendTCPMessage( &SendPacket);
*/
}

void QA_SetKillCount(UINT16 Red, UINT16 Blue)
{
/*
	UINT32 Temp;
	Temp = 11;
	i3NetworkPacket SendPacket(PROTOCOL_TEST_NC_QA);
	SendPacket.WriteData(&Temp, sizeof(UINT32));
	SendPacket.WriteData(&Red,	sizeof(UINT16));
	SendPacket.WriteData(&Blue, sizeof(UINT16));

	CGameNetworkManager::i()->SendTCPMessage( &SendPacket);
*/
}

void QA_SetEndlessBattle(void)
{
/*
	UINT32 Temp;
	Temp = 12;
	i3NetworkPacket SendPacket(PROTOCOL_TEST_NC_QA);
	SendPacket.WriteData(&Temp, sizeof(UINT32));

	CGameNetworkManager::i()->SendTCPMessage( &SendPacket);
*/
}

void QA_SetClanRank(UINT32 Pino)
{
/*
	UINT32 Temp;
	Temp = 13;
	i3NetworkPacket SendPacket(PROTOCOL_TEST_NC_QA);
	SendPacket.WriteData(&Temp, sizeof(UINT32));
	SendPacket.WriteData(&Pino, sizeof(UINT32));
	SendPacket.WriteData(&Pino, sizeof(UINT32));
	SendPacket.WriteData(&Pino, sizeof(UINT32));

	CGameNetworkManager::i()->SendTCPMessage( &SendPacket);
*/
}

void QA_SetClanExp(UINT32 Pino)
{
/*
	UINT32 Temp;
	Temp = 14;
	i3NetworkPacket SendPacket(PROTOCOL_TEST_NC_QA);
	SendPacket.WriteData(&Temp, sizeof(UINT32));
	SendPacket.WriteData(&Pino, sizeof(UINT32));
	SendPacket.WriteData(&Pino, sizeof(UINT32));
	SendPacket.WriteData(&Pino, sizeof(UINT32));

	CGameNetworkManager::i()->SendTCPMessage( &SendPacket);
*/
}

void QA_SetClanMatchPoint( UINT32 Pino)
{
/*
	UINT32 Temp;
	Temp = 15;
	i3NetworkPacket SendPacket(PROTOCOL_TEST_NC_QA);
	SendPacket.WriteData(&Temp, sizeof(UINT32));
	SendPacket.WriteData(&Pino, sizeof(UINT32));
	SendPacket.WriteData(&Pino, sizeof(UINT32));
	SendPacket.WriteData(&Pino, sizeof(UINT32));

	CGameNetworkManager::i()->SendTCPMessage( &SendPacket);
*/
}

void QA_SetClanMatchWinLose( UINT32 wMatch, UINT32 wWin, UINT32 wLose )
{
/*
	UINT32 Temp;
	Temp = 16;
	i3NetworkPacket SendPacket(PROTOCOL_TEST_NC_QA);
	SendPacket.WriteData(&Temp,		sizeof(UINT32));
	SendPacket.WriteData(&wMatch,	sizeof(UINT32));
	SendPacket.WriteData(&wWin,		sizeof(UINT32));
	SendPacket.WriteData(&wLose,	sizeof(UINT32));

	CGameNetworkManager::i()->SendTCPMessage( &SendPacket);
*/
}

void QA_SetClanMatchRoomInsert( UINT32 wCount )
{
/*
	UINT32 Temp;
	Temp = 17;
	i3NetworkPacket SendPacket(PROTOCOL_TEST_NC_QA);
	SendPacket.WriteData(&Temp,		sizeof(UINT32));
	SendPacket.WriteData(&wCount,	sizeof(UINT32));
	SendPacket.WriteData(&wCount,	sizeof(UINT32));
	SendPacket.WriteData(&wCount,	sizeof(UINT32));

	CGameNetworkManager::i()->SendTCPMessage( &SendPacket);
*/
}

void QA_SetMedalCount(UINT8 Type, UINT8 Number, UINT8 Count)
{
/*
	UINT32 Temp;
	Temp = 18;
	i3NetworkPacket SendPacket(PROTOCOL_TEST_NC_QA);
	SendPacket.WriteData(&Temp,		sizeof(UINT32));
	SendPacket.WriteData(&Type,		sizeof(UINT8));
	SendPacket.WriteData(&Number,	sizeof(UINT8));
	SendPacket.WriteData(&Count,	sizeof(UINT8));

	CGameNetworkManager::i()->SendTCPMessage( &SendPacket);
*/
}


#ifdef UITEST_MINIMAP_MOVE
	namespace
	{
		bool g_bMinimapMoveTestEnable = false;
	}

	void UITest_SetMinimapMove(bool enable) 
	{ 
		g_bMinimapMoveTestEnable = enable; 
	}
	
	bool UITest_IsMinimapMoveEnable()
	{ 
		return g_bMinimapMoveTestEnable;   
	}

#endif


	void CheckWeaponList( const ITEM_INFO * pList)
	{
		for( UINT32 i = 0; i < MAX_WEAPON_CHECK_SLOT_COUNT; i++)
		{
			if( pList[ i ].m_TItemID != 0)	// <- 0으로 설정되어 있을경우에는, 무기가 설정되지 않았음을 뜻합니다.
			{
				UINT32 iUsage = GetWeaponSlot_WeaponItemIDFunc( pList[ i ].m_TItemID);

				if( iUsage != i )
				{	// Error 잘못된 슬롯 번호
					I3PRINTLOG(I3LOG_FATAL,  "invalid USAGE of weapon index %d weapon %x / %d / %p", i,  pList[ i ].m_TItemID,  pList[ i ].m_TItemID,  pList[ i ].m_TItemID);
				}
			}
		}
	}

	INT32 getKillRate(UINT32 iKill, UINT32 iDeath)
	{
		INT32 killRate = 0;
		REAL32 total = (REAL32)(iKill + iDeath);
		if (0 < total)
		{
			killRate = (INT32)(((iKill / total) * 100.0f) + 0.5f);
		}
		
		return killRate;  
	}


}
