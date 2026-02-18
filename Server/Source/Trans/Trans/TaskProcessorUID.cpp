#include "pch.h"
#include "TaskProcessor.h"
#include "ModuleSIA.h"
#include "UserFinder.h"
#include "ModuleThaiPc.h"
#include "ModuleDBShop.h"
#include "Design/DesignBase.h"
#include "Design/DesignV1.h"
#include "Design/DesignV2.h"
// Login, Out UID 가 끝난 후 들어오게 됩니다.

bool CTaskProcessor::_UpdateUIDOut()
{
	S2RingBuffer* pRing = m_pDBUID->GetRingOut();

	if( 0 == pRing->GetBufferCount() )	return false;

	UID_OUT * pUIDOut = (UID_OUT*)pRing->Pop();
	if( NULL == pUIDOut )			return false;

	_DoTaskUIDOut( pUIDOut );
	pRing->PopIdx();

	return true;
}

void CTaskProcessor::_DoTaskUIDOut( UID_OUT * pUIDOut )
{
	switch( pUIDOut->_ui8Type )
	{
	case TYPE_UID_LOAD:
		{
			if( EV_FAILED( pUIDOut->_i32Rv ) )
			{	// 실패일경우 바로 실패 메시지 전달 해 줍니다.
				//_SendLogInPacket( pUIDOut->_i32Rv, pUIDOut->_ui32ServerIdx, pUIDOut->_ui32SessionIdx, pUIDOut->_i32AuthKey, pUIDOut->m_ui32BlockDate, NULL ); 
				_SendLoginBlock(pUIDOut);
				return;
			}

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( pUIDOut->_i64UID );
			if( pNode )
			{
				// 이미 존재하는 User 입니다.	// 엔씨 연동일때는 다른 방법으로 처리하는것을 생각해 봅니다. 엔씨가 알아서 주기는 합니 다. 
				if( pNode->_bLogOut || USER_STATE_INFO_SAVE <= pNode->_ui32State )
				{	// 이전 종료했을때의 저장이 아직 완료 되지 않았습니다. 잠시후 다시 로그인 해 주세요.
					_SendLogInPacket( EVENT_ERROR_EVENT_LOGOUTING, pUIDOut->_ui32ServerIdx, pUIDOut->_ui32SessionIdx, pUIDOut->_i32AuthKey, 0, NULL );
				}
				else
				{	// 중복 로그인 처리
					_AccountKick( pUIDOut->_i64UID, ACCOUNT_KICK_LOGIN );
					_SendLogInPacket( EVENT_ERROR_EVENT_LOG_IN_ALEADY_LOGIN, pUIDOut->_ui32ServerIdx, pUIDOut->_ui32SessionIdx, pUIDOut->_i32AuthKey, 0, NULL );
				}
				return;
			}

			bool bAASAppointedWay = g_pConfig->CheckTargetAASAppointed( pUIDOut->_birthday );

			if ( g_pConfig->m_AASOption.m_ui16AASProhibitionFlag & AAS_BLOCK_LOGIN )
			{
				// 1. 지정시간 금지방식 대상자일 경우(bAASAppointedWay)
				// 2. 지정시간 금지방식이 ON이고 현재 서버시간이 금지된 지정시간안에 포함되며(m_bAASMethodAppointed)
				// 1과2조건 모두 충족시 로그인 불가
				if ( bAASAppointedWay && g_pConfig->m_bAASMethodAppointed )
				{
					_SendLogInPacket( EVENT_ERROR_EVENT_LOG_IN_AAS, pUIDOut->_ui32ServerIdx, pUIDOut->_ui32SessionIdx, pUIDOut->_i32AuthKey, 0, NULL );
					return;
				}
			}

			UINT32 ui32LoginDelayTime = GetTickCount() - pUIDOut->m_ui32LoginTime;
			m_ui32LoginTimeArg += ui32LoginDelayTime;

			if( FALSE == m_bFirstLogin )
			{
				m_ui32LoginTimeArg /= 2;
			}

			// Insert User Node
			USER_NODE Node;
			i3mem::FillZero( &Node, sizeof( USER_NODE ));
			Node.Create();
			i3String::Copy( Node._strID, pUIDOut->_strID, NET_ID_SIZE );
			
			Node.m_ui8PBVer				= pUIDOut->m_ui8PBVer;
			Node._i64UID				= pUIDOut->_i64UID;
			Node._ui32AuthKey			= pUIDOut->_i32AuthKey; 
			Node._i32Serveridx			= pUIDOut->_ui32ServerIdx;
			Node._i32Sessionidx			= pUIDOut->_ui32SessionIdx;
			Node._ui32Cash				= pUIDOut->_ui32Cash;
			Node._ui8RoleFlag			= pUIDOut->_pcCafe;
			Node._ui64UserBillingIdx	= pUIDOut->_ui64UserBillingIdx;	// Use in Russia method.
			Node._nation				= pUIDOut->_nation;
			Node._ui32BirthDay			= pUIDOut->_birthday;
			Node._LastRecvTimeST		= i3ThreadTimer::GetServerTime();
			Node._sClan._ui32Mark		= CLAN_MARK_DEFAULT;
			Node._IpAddress				= pUIDOut->_IpAddress;
			Node.m_ui8LocaleCode		= pUIDOut->m_ui8LocaleCode;
			Node._ServerLeaveTimeST		= 0;
			Node.m_i8LogOutSendType		= pUIDOut->m_i8LogOutSendType;
			Node.m_bAASDailyUsageWay	= g_pConfig->CheckTargetAASDailyUsage();
			Node.m_bAASAppointedWay		= bAASAppointedWay;
			Node.m_ui8LinkMethod		= pUIDOut->m_ui8LinkMethod;

			switch( Node.m_ui8PBVer	)
			{
			case PB_VER_V10 :
				Node.m_pDesign		= Node.m_pDesignV1;
				break;
			default :
				Node.m_pDesign		= Node.m_pDesignV2;
				break;
			}

			if( NULL == Node.m_pDesign )
			{
				g_pLog->WriteLog( L"NEW USER DESIGN FAIL" );
				_SendLogInPacket( EVENT_ERROR_EVENT_LOG_IN_UNKNOWN, pUIDOut->_ui32ServerIdx, pUIDOut->_ui32SessionIdx, pUIDOut->_i32AuthKey, 0, NULL ); 
				return;
			}

			if ( PC_NOT_CAFE == Node._ui8RoleFlag && pUIDOut->_ui32PCCafeGUID > 0 )
			{
				Node.m_PCCafeInfo.SetState( PCCAFE_WORK_STATE_CHARGE_CHECK );
				Node.m_PCCafeInfo.SetChargeCheckTime();
			}
			else
				Node.m_PCCafeInfo.SetState( PCCAFE_WORK_STATE_NONE );
			Node.m_PCCafeInfo.SetPCCafeGUID( pUIDOut->_ui32PCCafeGUID );
			Node.m_PCCafeInfo.SetBillingUID( pUIDOut->_ui32PCCafeBillingUID );

			i3mem::Copy( Node.m_strBillingID, pUIDOut->m_strBillingID, BILLING_ID_SIZE );
			i3mem::Copy( Node.m_strExtUID, pUIDOut->m_strExtUID, EXT_UID_SIZE );
			i3mem::Copy( Node.m_strWebLoginKey, pUIDOut->m_strWebLoginKey, WEB_LOGINKEY_SIZE );

			//북미 필요
			Node._i64UniqueKey			= pUIDOut->_UniqueKey;
			Node.m_ui8Country			= pUIDOut->m_ui8Country;

			if( g_pConfig->m_bThaiIDPlusActive )
			{
				// PC방이 우선순위	
				if( PC_NOT_CAFE == Node._ui8RoleFlag )
				{
					if( EVENT_ITEM_LEVEL_ONE == g_pModulePcList->IsThaiIDPlus( Node._i64UID ) )
					{
						Node._ui8RoleFlag = PC_VACATION_EVENT_ITEM;
					}
				}
			}
						
			i3mem::FillZero( Node.m_aui32AASBanTime,	sizeof(UINT32)*DAY_COUNT_IN_WEEK );

			//배열로 넘어온 셧다운 타임 설정값을 일별로 묶어서 저장합니다.
			if ( g_pConfig->m_AASOption.m_ui8AASEnableFlag & AAS_METHOD_ACCOUNT_APPOINTED )
			{
				static INT32 ai32Table[DAY_COUNT_IN_WEEK] = { 1, 2, 3, 4,  5, 6, 0 };
				for( INT32 i = 0 ; i < SHUTDOWN_TIME_SIZE ; ++i )
				{
					Node.m_aui32AASBanTime[ ai32Table[i / HOUR_COUNT_IN_DAY] ]	|= ( pUIDOut->m_aui8ShutDownTime[i] << ( i % HOUR_COUNT_IN_DAY ) );
				}
			}

			//계정아이템 정보(위장계급, 위장닉네임, etc)
			i3mem::Copy( &Node._sInvenData, &pUIDOut->_sInvenData, sizeof(USER_INFO_INVITEM_DATA) );
			
			pNode = g_pUserFinder->Insert( &Node );
			if( NULL == pNode )
			{//요청
				g_pLog->WriteLog( L"USER INSERT BUFFER FULL" );
				_SendLogInPacket( EVENT_ERROR_EVENT_LOG_IN_MAXUSER, pUIDOut->_ui32ServerIdx, pUIDOut->_ui32SessionIdx, pUIDOut->_i32AuthKey, 0, NULL ); 
				return;
			}

			DB_INVENTORY_IN DBInvenIn;
			DBInvenIn._i64UID			= Node._i64UID;
			if(FALSE == g_pModuleDBShop->InInventory( &DBInvenIn ))
			{
				g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Get User Inventory / UID : %I64d", Node._i64UID );
			}

			LOGIN_INFO LoginInfo;
			LoginInfo.m_TUID						= Node._i64UID;
			LoginInfo.m_ui8LinkThreadIdx			= pUIDOut->m_ui8LinkThreadIdx;
			LoginInfo.m_ui32LoginDelayTime			= m_ui32LoginTimeArg;
			LoginInfo.m_ui8LocaleCode				= Node.m_ui8LocaleCode;
			i3mem::Copy( LoginInfo.m_strID,			Node._strID,		NET_ID_SIZE );
			i3mem::Copy( LoginInfo.m_strcustom,		pUIDOut->_Custom,	NORTH_CUSTOM_MSG_SIZE );
			i3mem::Copy( LoginInfo.m_strBillingID,	Node.m_strBillingID,	BILLING_ID_SIZE );
			i3mem::Copy( LoginInfo.m_strExtUID,		Node.m_strExtUID,		EXT_UID_SIZE);
			i3mem::Copy( LoginInfo.m_strWebLoginKey, Node.m_strWebLoginKey, WEB_LOGINKEY_SIZE);
			LoginInfo.m_ui32BlockDate				= 0;

			_SendLogInPacket( EVENT_ERROR_SUCCESS, Node._i32Serveridx, Node._i32Sessionidx, Node._ui32AuthKey, 0, &LoginInfo );

			// 캐쉬 잔액 조회를 요청합니다.
			PACKET_GET_CASH_REQ	CashReq;
			CashReq.m_TUID					= Node._i64UID;
			CashReq.m_ui64UserBillingIdx	= Node._ui64UserBillingIdx;
			CashReq.m_ui8LinkMethod			= Node.m_ui8LinkMethod;
			i3mem::Copy( CashReq.m_strBillingID, Node.m_strBillingID, BILLING_ID_SIZE );

			// #### SIA 서버로 보낸다.
			if( FALSE == g_pModuleSIA->GetCash_T( &CashReq ) )
			{	// Error
				g_pLog->WriteLog( L"SIA Send Fail - Get Cash Check TaskUID / UID : %I64d ", Node._i64UID );
			}
		}
		break;
	case TYPE_UID_BIRTHDAY_SET:
		{
			i3NetworkPacket	SendPacket( PROTOCOL_LOBBY_SET_BIRTHDAY_ACK  );
			SendPacket.WriteData( &pUIDOut->_ui32SessionIdx,	sizeof( UINT32 ) ); 
			SendPacket.WriteData( &pUIDOut->_i64UID,			sizeof( T_UID ) ); 
			SendPacket.WriteData( &pUIDOut->_i32Rv,				sizeof( INT32 ) );
			if( EV_SUCCESSED( pUIDOut->_i32Rv ) )
			{
				SendPacket.WriteData( &pUIDOut->_birthday,		sizeof( UINT32 ) ); 
			}
			SendGamePacket(pUIDOut->_ui32ServerIdx, &SendPacket);
		}
		break;
	case TYPE_UID_DIRECT_PENALTY:
		{
			i3NetworkPacket SendPacket( PROTOCOL_GMCHAT_APPLY_PENALTY_ACK );
			SendPacket.WriteData( &pUIDOut->_i32Rv,				sizeof( INT32 ) );
			SendPacket.WriteData( &pUIDOut->m_i32SessionIdx,	sizeof( INT32 ) ); 
			if( EV_SUCCESSED( pUIDOut->_i32Rv ) )
			{
				SendPacket.WriteData( &pUIDOut->_i64UID,			sizeof( T_UID ) ); 
				SendPacket.WriteData( &pUIDOut->m_i8PenaltyType,	sizeof( INT8 ) ); 
				SendPacket.WriteData( &pUIDOut->m_i32PenaltyAmount,	sizeof( INT32 ) ); 
			}

			SendGamePacket(pUIDOut->_ui32ServerIdx, &SendPacket);
		}
		break;
	default:
		{
			return;
		}
	}
}