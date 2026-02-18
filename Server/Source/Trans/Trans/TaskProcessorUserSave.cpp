#include "pch.h"
#include "TaskProcessor.h"
#include "UserFinder.h"
#include "ModuleSIA.h"

BOOL CTaskProcessor::_UpdateExitSave()
{
	// 1 프레임당 한개씩만 처리합니다.
	if( 0 == m_pDBUserSave->GetExitSaveCount() )	return false;

	RING_EXIT_SAVE_OUT* pRingOut = m_pDBUserSave->GetExitSave(); 

	_UserExit( pRingOut->pNode );

	m_pDBUserSave->GetExitSaveIdx(); 

	return TRUE;
}

void CTaskProcessor::_UserExit( USER_NODE * pNode )
{
	pNode->_ui32State = USER_STATE_EXIT;

	//로그아웃이 필요한 나라만 사용합니다. // 터키, 중남미, 북미
	if( TYPE_LOGOUT_ACK_SEND == pNode->m_i8LogOutSendType )
	{
		ST_PACKET_LOGOUT_REQ		 LogoutData;
		i3mem::Copy(LogoutData.m_strID, pNode->_strID, NET_ID_SIZE);
		LogoutData.m_ui32PCCafeBillingUID = pNode->m_PCCafeInfo.GetPCCafeBillingGUID();

		if( FALSE == g_pModuleSIA->SendLogout( &LogoutData ) )
		{
			g_pLog->WriteLog( L"[ERROR]SEND LOGOUT FAIL" );
		}
	}
	// 유저 정보를 삭제해 줍니다.
	g_pUserFinder->Delete( pNode->_i64UID );
	pNode->_ui32State = USER_STATE_INIT;
}

BOOL CTaskProcessor::_UpdateCreateNick()
{
	// 1 프레임당 한개씩만 처리합니다.
	if( 0 == m_pDBUserSave->GetCreateNickCount() )		return FALSE;

	RING_CREATE_NICK_OUT* pRingOut = m_pDBUserSave->GetCreateNick(); 

	USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( pRingOut->m_TUID );
	if( pNode )
	{
		if( EV_SUCCESSED( pRingOut->m_i32Result ) )
		{
			pNode->m_ui8DBLoadState = USER_DB_LOAD_STATE_LOAD;

			i3String::Copy( pNode->_sBasic.m_strNick, pRingOut->m_strNick, NET_NICK_NAME_SIZE );
			pNode->_ui32CreateDate		= pRingOut->m_ui32CreateDate;
			pNode->_sBasic.m_ui32Point	= pRingOut->m_ui32CreatePoint;

			g_pUserFinder->InsertNameHash( pNode );

			// 기본 캐릭터를 생성해 줍니다.
			_CreateDefaultChara( pNode );
		}

		// 유저의 정보를 보냅니다.
		i3NetworkPacket	SendPacket( PROTOCOL_BASE_CREATE_NICK_ACK  );
		SendPacket.WriteData( &pNode->_i32Sessionidx,			sizeof( INT32 ) ); 
		SendPacket.WriteData( &pNode->_i64UID,					sizeof( T_UID ) ); 
		SendPacket.WriteData( &pRingOut->m_i32Result,			sizeof( INT32 ) );
		if( EV_SUCCESSED( pRingOut->m_i32Result ) )
		{
			SendPacket.WriteData( &pNode->_sBasic.m_ui32Point,	sizeof( UINT32 ) ); 
			UINT8 ui8NickSize = (UINT8)i3String::Length( pNode->_sBasic.m_strNick )+1;
			SendPacket.WriteData( &ui8NickSize,					sizeof( UINT8 ) ); 
			SendPacket.WriteData( pNode->_sBasic.m_strNick,		sizeof( TTCHAR ) * ui8NickSize );

			// 닉네임 생성이 성공한 유저(신규 유저)에게 아이템 지급
			InsertEventItemNewUser( pNode );
			InsertEventItemOldUser( pNode );
		}

		SendGamePacket( pNode->_i32Serveridx, &SendPacket );
	}

	m_pDBUserSave->GetCreateNickIdx(); 

	return TRUE;
}

BOOL CTaskProcessor::_UpdateCreateChar()
{
	// 1 프레임당 한개씩만 처리합니다.
	if( 0 == m_pDBUserSave->GetCreateCharCount() )		return FALSE;

	RING_CREATE_CHAR* pRingOut = m_pDBUserSave->GetCreateChar(); 

	USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( pRingOut->m_TUID );
	if( pNode )
	{
		i3NetworkPacket	SendPacket;

		switch( pRingOut->m_eType )
		{
		case CHARA_RING_TYPE_CREATE:  // 기본 캐릭 생성시는 대표 캐릭터로 설정합니다.
			if (pRingOut->m_Inven._TItemWareDBIdx & DEFAULT_ITEM_DBIDX_MIN)
			{
				pNode->m_pDesign->SetTeamSlot(pNode->m_MultiSlot, pRingOut->m_ui8CharSlot, pRingOut->m_TGoodsID / 100);
			}
			// break;	// 주석이 맞음
		case CHARA_RING_TYPE_BUY_POINT:
		case CHARA_RING_TYPE_BUY_CASH:
			{
				S2MO_CHAR_SKILL_INFO CharSkillInfo;
				// 유저 정보 세팅
				if( EV_SUCCESSED( pRingOut->m_i32Result ) )
				{
					pNode->m_MultiSlot.InsertCharInfoAll( pRingOut->m_ui8CharSlot, (MULTI_SLOT_STATE)pRingOut->m_ui8SlotState, &pRingOut->m_CharBaseInfo );
					 
					CharSkillInfo.Reset();
					CharSkillInfo.m_ui8Class = pRingOut->m_ui8Class;
					pNode->m_MultiSlot.SetCharSkillInfo( pRingOut->m_ui8CharSlot, &CharSkillInfo );
				}

				_SendCreateChara( pNode, pRingOut, &CharSkillInfo);
			}
			break;

		case CHARA_RING_TYPE_DELETE:
			{
				if( EV_SUCCESSED( pRingOut->m_i32Result ) )
				{
					INT32 i32MainSlotIdx = -1;
					
					// 멀티 슬롯에서 삭제.
					if (pNode->m_pDesign->DeleteChara(pNode->m_MultiSlot, pRingOut->m_ui8CharSlot, i32MainSlotIdx) )
					{	
						// 인벤토리에서 삭제.
						UserDeleteItem(pNode, pRingOut->m_Inven._TItemWareDBIdx);

						_SendDeleteChara(pNode, pRingOut->m_i32Result, pRingOut->m_ui8CharSlot);

						if (i32MainSlotIdx != -1)
						{
							pNode->_ui32UpdateInfo |= SAVE_DATABASE_CHARA_COMMON;
						}
					}
					// else // 이미 멀티 슬롯에서 제거된 경우 입니다. 로그인시 만료.
				}				
			}
			break;

		case CHARA_RING_TYPE_USE:
			{
				if( EV_SUCCESSED( pRingOut->m_i32Result ) )
				{
					pNode->m_MultiSlot.SetActiveChar(  pRingOut->m_ui8CharSlot, (MULTI_SLOT_STATE)pRingOut->m_ui8SlotState );
				}

				SendPacket.SetProtocol( PROTOCOL_CHAR_CHANGE_STATE_ACK );
				SendPacket.WriteData( &pNode->_i32Sessionidx,		sizeof( INT32 ) );
				SendPacket.WriteData( &pNode->_i64UID,				sizeof( T_UID ) );
				SendPacket.WriteData( &pRingOut->m_i32Result,		sizeof( INT32 ) );
				SendPacket.WriteData( &pRingOut->m_ui8CharSlot,		sizeof( UINT8 ) );
				SendPacket.WriteData( &pRingOut->m_ui8SlotState,	sizeof( UINT8 ) );

				SendGamePacket( pNode->_i32Serveridx, &SendPacket );
			}
			break;
		}

		//SendGamePacket( pNode->_i32Serveridx, &SendPacket );
	}

	m_pDBUserSave->GetCreateCharIdx(); 

	return TRUE;
} 