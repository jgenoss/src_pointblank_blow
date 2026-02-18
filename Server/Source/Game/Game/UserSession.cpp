#include "pch.h"
#include "UserSession.h"
#include "DllInjection_NProtect.h"
#include "DllInjection_AhnHS.h"
#include "RoomManager.h"
#include "UserManager.h"
#include "DataBase.h"
#include "ModuleTrans.h"
#include "ModuleClan.h"
#include "ModuleMessenger.h"
#include "ModuleContextNc.h"
#include "Room.h"
#include "FieldShop.h"


I3_CLASS_INSTANCE(CUserSession, i3ElementBase); 

CUserSession::CUserSession()
{	
	m_bSendPacket				= FALSE;
	m_ui32LoginKey				= 0;
	m_WebKey[0]					= '\0';		//초기화 꼬옥 필요	웹연동이면 필요없음  	

	m_ui32FirstObserverTime		= 0;					// 난입 후 첫 관전시간
	m_bFirstObserver			= FALSE;	

	m_ui8Country				= 0;
	m_ui32ChangedRank			= 0;

	m_bRequestUserList			= FALSE;
	m_bHSCheck					= TRUE;

	m_pDesign					= NULL;
	m_pDesignV1					= NULL;
	m_pDesignV2					= NULL;



	m_pDesign	= NULL;
	m_pDesignV1 = NULL;
	m_pDesignV2 = NULL;
}

CUserSession::~CUserSession()
{
	OnDestroy();

}

void CUserSession::OnDestroy()
{
	I3_SAFE_DELETE( m_pDesignV1 );
	I3_SAFE_DELETE( m_pDesignV2 );
}


BOOL CUserSession::Create()
{
	if( FALSE == m_MultiSlot.Create() )	return FALSE;

	m_pDesignV1 = new	CDesignV1;
	if( NULL == m_pDesignV1 )				return FALSE;
	if( FALSE == m_pDesignV1->OnCreate() )	return FALSE;

	m_pDesignV2 = new	CDesignV2;
	if( NULL == m_pDesignV2 )				return FALSE;
	if( FALSE == m_pDesignV2->OnCreate() )	return FALSE;

	return TRUE;
}

BOOL CUserSession::OnConnect(SOCKET Socket, struct sockaddr_in * pAddr)
{
	BOOL i32Rv = i3NetworkSession::OnConnect(Socket, pAddr); 

	
	m_ui32RoomBattleCount		= 0;
	m_ui32PlayTime				= 0;
	//Set Idx & Number
	m_i64UID					= SERVER_VALUE_UNKNOWN;
	m_i32RoomIdx				= SERVER_VALUE_UNKNOWN;
	m_i32SlotIdx				= SERVER_VALUE_UNKNOWN;
	m_i32SelectChannelNum		= SERVER_VALUE_UNKNOWN;
	m_i32ChannelNum				= SERVER_VALUE_UNKNOWN;
	m_i32LogChannelNum			= SERVER_VALUE_UNKNOWN;

	//Position	
	m_i32MainTask				= GAME_TASK_CONNECT;
	m_i32SubTask				= GAME_TASK_SUB_NONE;

	m_ui32EncriptKey			= (m_SessionIdx % 7) + 1;					// 비밀 키값 설정

	//Set Time 	
	m_dt32LoginDate.Reset();
	m_dt32LastLoginTime.Reset();
	m_ui32ATLogin				= 0;
	m_ui32ATLastRecv			= i3ThreadTimer::GetServerTime();
	m_ui32ATIsKill				= 0;
	m_ui32ATKillDelay			= 0;
	m_wstrHackNotice[0]			= '\0';
	m_ui32KillReason			= KILL_REASON_NO;
	m_ui32ErrorCode				= 0;
	m_bHackKill					= FALSE;
	m_strId[0]					= '\0';

	m_i32SendErrorCount			= 0;

	m_ui32ATPacketSecond		= i3ThreadTimer::GetServerTime();
	m_iPacketSecondCount		= 0; 

	//setGameGuard타임 
	m_ui32ATGameGuardSend		= i3ThreadTimer::GetServerTime();
	m_ui32ATGameGuardRecv		= m_ui32ATGameGuardSend;

	//Set Inventory
	m_Inventory.Reset();
	
	//메달정보 입니다.
	m_ui32Miniature				= 0;
	m_ui32Insignia				= 0;
	m_ui32Order					= 0;
	m_ui32Master				= 0;
	m_ui64GainUserTitle			= 0;

	m_i32RespawnCount			= 0;

	m_ui8LoadingStart			= LOAD_START_NONE;

	//m_bGetFirstInfo			= FALSE; 

	m_ui32QuestEventDate		= 0; 
	m_TTeamID					= MATCH_TEAM_INVALID_IDX;
	m_TMerID					= MERCENARY_INVALID_IDX;
	m_bMerPenalty				= false;
	m_dt32MerPenaltyTime.Reset();
	m_ui8MerClass				= INIT_UINT8;
	m_ui8MerRank				= INIT_UINT8;
	m_bHaveInfo					= FALSE; 
	m_i32ChangeDBFlage			= 0x00;
	m_ui64ChangeCharaDBFlage	= 0;

	m_ui8LocaleCode				= g_pContextMain->m_ui8LocalCode;

	m_ui8Ping					= 0;

	m_ui16BackProtocolID		= 0;

	m_bIsBattleComplete			= FALSE;

	m_i16PacketRandSeed			= (INT16)rand();

	m_ui8LoginLinkThreadIdx		= 0;
	m_ui32LoginWaitCount		= 0;
	m_ui32ATLoginWaitSend		= 0xFFFFFFFF;
	
	m_UserInfoBasic.Reset();
	m_UserInfoRecord.Reset();
	m_UserInfoInvItemData.Reset();

	// v1.5 TC	
	m_bVisitClanPageEnter	= false;	
	m_ui32ClanRankingDate	= 0;
	
	m_MultiSlot.Reset();

	i3mem::FillZero( m_abGetInfoToServer, sizeof(m_abGetInfoToServer) );
	i3mem::FillZero( &m_UserInfoServer, sizeof(USER_INFO_SERVER) );

	// 로그인 시간 체크
	m_ui32LoginST	= 0;
	m_ui32ATInfo	= 0;

	// 어뷰징 체크
	m_ui32HoldAccExp		= 0;	
	m_ui32HoldAccPoint		= 0;
	m_ui8AbusingLevel		= ABUSING_LEVEL_NONE;
	m_ui32AbusingPopupTime	= 0;
	m_ui32RTBattleStart		= 0;
	m_ui32BattleStartInvenTime = 0;
	
	m_ui32RTBattleDeath		= 0;

	m_ui16PrevExpRank		= 1000;		// 최초 기록이 발생하기 전까지 최하위 순위를 가진다

	m_bCheckLogout			= TRUE;

	// v1.5 Medal
	i3mem::FillZero( &m_MedalAllInfo,			sizeof( MEDAL_ALL_INFO)) ;	
	
	// 메달 고유 인덱스 초기화.

	for( INT32 Type = MEDAL_MC_CHECK_POSITION_START ; Type < MEDAL_MC_CHECK_POSITION_MAX ; Type++)
	{
		for( INT32 MedalCount = 0 ; MedalCount < MAX_MEDAL_COUNT; MedalCount++)
		{
			m_MedalAllInfo.m_aMedal[Type][MedalCount].m_Medal.m_ui16Idx		= INIT_MEDAL_NUMBER;
		}
	}

	// 진행 중인 메달 셋트 고유 인덱스 초기화
	for( UINT16 i = 0 ; i < MAX_CUR_MEDAL_SET_COUNT; i++)
	{
		m_MedalAllInfo.m_aCurMedalSet[i].m_ui16Idx = INIT_MEDAL_NUMBER;
	}

	// 중복채팅 방지 시스템.
	m_i32ChatCount			= 0;
	m_ui32ChatLimitTime		= 0;
	m_ui32ChatWarningTime	= 0;
	m_eChatLimitState		= CHAT_LIMIT_STATE_NORMAL;

	i3mem::FillZero( m_aQuickJoinInfo, sizeof( QUICKJOIN_INFO ) * MAX_QUICK_JOIN_INFO_COUNT );

	// QA 자동화
	m_ui8QAAutoFlag			= 0;

	m_bWriteHackLog = false;
	m_strEndReason[0] = '\0';

	// 실시간 PC방 혜택변경
	m_ui8OldPCCafe			= PC_NOT_CAFE;
	m_ui8NewPCCafe			= PC_NOT_CAFE;

	m_ui32MatchCountCheckTime = 0;

	m_ui64PingTime			= 0;
	m_ui32PingCount			= 0;

	m_bWriteHackLog			= false;
	m_ui8PBVer				= g_pContextMain->RegistPBVersion((UINT8)g_pContextMain->m_eServerVersion);

	i3mem::FillZero(&m_stPCSpec, sizeof(USER_INFO_PCSPECS));
	
	m_ui8UserScopeType = 0;
	
	return i32Rv;
}

BOOL CUserSession::OnConnectInit()
{
	if( FALSE == i3NetworkSession::OnConnectInit() )	return FALSE;

	#ifdef USE_GPK
	const unsigned char *pCode = NULL;
	INT32 i32Len = 0;
#endif

	switch( g_pContextMain->m_i32GameGuard )
	{
	case GAMEGUARD_NPROTECT:
		{
#ifdef USE_NPROTECT
			m_pCsa.Init(true); 
#endif
		}
		break;
	case GAMEGUARD_HACKSHIELD:
		{
			m_hClient = ANTICPX_INVALID_HANDLE_VALUE;
			// 해당 서버의 핸들을 이용해 접속한 유저의 핸들을 생성
			m_hClient = g_pAhnHS_CreateClientObject( g_hAhnhsServer );

			if ( NULL == m_hClient )
			{
				return FALSE;
			}
		}
		break;
	case GAMEGUARD_XTRAP:
		{	// Xtrap
			UINT32 ui32Res = XTrap_S_SessionInit( XTRAP_TIMEOUT, CSFILE_NUM, g_pXTrapModule->m_ui8XTrapMap,  m_pXtrapSessionBuf ) ;
			if ( XTRAP_API_RETURN_OK != ui32Res ) 
			{
				I3TRACE( "XTrap_S_SessionInit()	Failed Error:%d\n", ui32Res ); 
				return FALSE;
			}
		}
		break;
	case GAMEGUARD_GPK:
		{
		#ifdef USE_GPK		
			DWORD dwOldTime = GetTickCount();
			m_i32CodeIdx = g_pSvrDynCode->GetRandIdx();
			if( -1 == m_i32CodeIdx)						return FALSE;
			i32Len = (INT32)g_pSvrDynCode-> GetCltDynCode(m_i32CodeIdx,&pCode);
			if( 14000 <= i32Len ||  (0 >= i32Len) || (NULL == pCode) )	
			{
				return FALSE;
			}
			DWORD dwNewTime = GetTickCount() - dwOldTime;
			if( dwNewTime > SESSION_CHECK_TIMEOUT_NOT_LOGIN )//
			{	// User Connect TIme Out
				g_pLog->WriteLog( L"[USER_CONNECTIONSUCCESS] GPK Loading TimeOut : %d, m_i32CodeIdx : %d ", dwNewTime, m_i32CodeIdx );
			}
		#endif
		}
		break;
	}	

	m_bSendPacket				= TRUE;
	// HeartBit2 초기화
	m_bIsRecvHeartBit2			= 0;
	// 패킷 처리 테스트코드
	m_r64ParseTimeHigh			= 0.f;
	m_ui16ParseTimeHighProtocol = 0;
	m_ui32TestLogLoginStartTime = 0;

	PACKET_BASE_CONNECT_ACK Packet;
	Packet.m_i32SessionIdx			= m_SessionIdx;
	Packet.m_i16PacketRandSeed		= m_i16PacketRandSeed;
	Packet.m_ui8ChannelType.SetValue( g_pContextMain->m_pChannelType, 0, GAME_CHANNEL_COUNT );
	Packet.m_pRSAKey.SetValue( g_pContextMain->m_pRSAKey, 0, g_pContextMain->m_i16RSAKeySize );

	SC_VERSION ServerVersion;	
	Packet.m_stServerVersion = ServerVersion;
	
	S2MOSendEvent( &Packet );

	return TRUE;
}

//
//
//워크, 메인에서 호출 가능 Auth는 사용하지 않음 
BOOL CUserSession::OnDisconnect(BOOL bForceMainThread)
{	
	m_bSendPacket = FALSE; 

	INT32	i32UserState = GetPosition();
	INT32	i32ChannelNum = m_i32LogChannelNum;
	INT32	i32RoomIdx = m_i32RoomIdx;

	INT32 iWorkingThread = m_WorkThreadIdx; 
	if(bForceMainThread)iWorkingThread = MAIN_THREAD_IDX; 

	// 죽일려고 하려는 시도가 있으면. 메인이 죽이도록 만든다.이 부분 아주 중요합니다. 만약 이부분이 없다면. 크래쉬 할 수 있습니다. 
	if( FALSE == bForceMainThread )
	{
		// 쓰레드에서 죽일려는 시도가 있으면 메인에서 바로 죽을수 있게 m_iIsKillST 을 변경합니다.

		if( 0 == m_ui32ATIsKill )
		{
			// 로그아웃 패킷에 의해 정상적인 절차로 종료되지 않았다면 로그 남깁니다.
			if( FALSE == m_bCheckLogout )
			{
				_SendErrorMessage( iWorkingThread, EVENT_ERROR_EVENT , 0, KILL_REASON_WRONG_LOGOUT, L"Wrong Logout" );
			}
			else
			{
				m_ui32ATIsKill	= 1;
				m_ui32KillReason	= KILL_REASON_LOGOUT;
			}
		}
		return FALSE;
	}

	// Check Room 
	g_pRoomManager->OnLeaveRoom( this, m_i32ChannelNum, iWorkingThread, ROOM_ENDTYPE_DISCONNECT );

	//꼭해야함 하지않으면 크래쉬
	SetPosition(GAME_TASK_NONE);

	// Check Channel
	if((m_i32ChannelNum > SERVER_VALUE_UNKNOWN) && ( m_i32ChannelNum < (INT32)(GAME_CHANNEL_COUNT) ))//슈퍼 유저땜시 뒤에것은 검사해야함.
	{
		g_pUserManager->OnLeaveChannel( this, m_i32ChannelNum );
		m_i32ChannelNum		= SERVER_VALUE_UNKNOWN;
		m_i32LogChannelNum	= SERVER_VALUE_UNKNOWN;
	}

	T_UID TUID = m_i64UID;

	// 로그인 된 유저에 한해서 로그아웃 을 전송 합니다.
	if( GAME_TASK_LOGIN_ING <= i32UserState )
	{
		//Zepetto 다른 서버에 로그아웃을 알립니다.
		if( !g_pModuleTrans->AuthSendLogOut(iWorkingThread, this ) )
		{
			g_pLog->WriteLogSession( this, L"#### OnDestroy ChangeDBFlage :%d / UID : %I64d", m_i32ChangeDBFlage, TUID ) ; 
		}
		if( GAME_TASK_LOGIN <= i32UserState )
		{
			g_pModuleClan->ClanSendLogOut(iWorkingThread, this );			
			g_pModuleMessenger->MessengerLogout(iWorkingThread, m_i64UID);
		
			if(0 != m_dt32LoginDate.GetDate())
			{
				g_pModuleContextNC->LogSendMessage(iWorkingThread, LOG_MESSAGE_U_LOGOUT, this, &i32ChannelNum, &i32RoomIdx);
			}

			m_i64UID = SERVER_VALUE_UNKNOWN;
		}
	}
	
	switch( g_pContextMain->m_i32GameGuard )
	{
	case GAMEGUARD_NPROTECT:
		{
		#ifdef USE_NPROTECT
			m_pCsa.Close(); 
		#endif
		}
		break;
	case GAMEGUARD_HACKSHIELD:
		{
			if( ANTICPX_INVALID_HANDLE_VALUE == m_hClient )
			{
				g_pLog->WriteLogSession( this, L"g_pAhnHS_CloseClientHandle INVAILD HANDLE / UID : %I64d", TUID ) ; 
			}
			else g_pAhnHS_CloseClientHandle( m_hClient );
		}
		break;
	case GAMEGUARD_XIGNCODE:
		{
			g_pXignCode->DisConnect( TUID );
		}
		break;
	}
	return i3NetworkSession::OnDisconnect(bForceMainThread);
}

void CUserSession::_SendErrorMessage(INT32 i32WorkIdx, UINT32 ui32ErrorCode, PROTOCOL ProtocolID, INT32 i32KillReason, wchar_t* wstrMessage )
{	// 함수 사용하시기 전에 쓰레드 다시한번 확인 바랍니다.
	if( 0 < m_ui32ATIsKill ) return;

	//Set Time
	UINT32	ui32CutTime		= i3ThreadTimer::LocalTime().GetDate();
	BOOL	bLog			= TRUE;

	i3NetworkPacket Packet;
	switch( i32KillReason )
	{
	case KILL_REASON_GAME_GUARD:
		{   
			// 게임가드 체크 Pass 계정은 Kick하지 않습니다.
			if( CheckSpecialAbility( SA_GAME_GUARD ) ) return;
		}
		break;
	case KILL_REASON_SERVER_MOVE:
	case KILL_REASON_WRONG_LOGOUT:
		bLog = FALSE;
		break;
	}
	
	m_ui32KillReason	= i32KillReason;

	if( bLog )
	{
		Packet.SetProtocol(PROTOCOL_SERVER_MESSAGE_ERROR);
		Packet.WriteData( &ui32CutTime,		sizeof(UINT32) );
		Packet.WriteData( &ui32ErrorCode,	sizeof(UINT32) );
		SendPacketMessage( &Packet );
		
		wchar_t wstrMsg[ MAX_PATH ];
		wstrMsg[ 0 ] = '\0';
		if( wstrMessage )
		{
			i3String::Copy( wstrMsg, wstrMessage, MAX_PATH );
		}

		g_pLog->WriteLogSession( this, L"[ERROR]SendErrorMessage(UID:%I64d, Protocol:%d, MainTask : %d, BackProtocol:%d,timestamp(%d), Error:%x, KillReason:%d, Message:%s)", 
			m_i64UID, ProtocolID, m_i32MainTask, m_ui16BackProtocolID, (i3ThreadTimer::GetServerTime() - m_ui32ATLastRecv), ui32ErrorCode, i32KillReason, wstrMsg );
	}

	m_ui32ATIsKill = i3ThreadTimer::GetServerTime();

	return; 
}

void CUserSession::DisconnectUserDelay( UINT32 ui32ErrorCode, INT32 i32KillReason, wchar_t* wstrMessage )
{
	if( 0 < m_ui32ATKillDelay )		return;

	// Dis 모드에서만 작동 합니다.
	m_ui32ATKillDelay = 0;

	switch( i32KillReason )
	{
	case KILL_REASON_SERVER_MOVE:
		m_ui32ATKillDelay		= 5;						// 5초 후에 끊깁니다.
		break;
	case KILL_REASON_USERKICK:
#if defined( BUILD_DIST_VERSION ) 
		m_ui32ATKillDelay		= rand() % 50;				// 0 ~ 50초 안에 랜덤하게 끊깁니다.
#endif
		break;
	case KILL_REASON_HACK_CLIENT:
#if defined( BUILD_DIST_VERSION ) 
		m_ui32ATKillDelay		= (rand() % 8) + 2;			// 2 ~ 10초 안에 랜덤하게 끊깁니다.
#endif
		break;
	case KILL_REASON_HACK_SERVER_G:
#if defined( BUILD_DIST_VERSION ) 
		m_ui32ATKillDelay		= (rand() % 30) + 30;		// 30초 ~ 1분 안에 짤립니다.
#endif
		break;
	default:
		m_ui32ATKillDelay		= (rand() % 45) + 5;		// 5 ~ 50초 안에 랜덤하게 끊깁니다.
		break;
	}

	m_ui32ATKillDelay		+= i3ThreadTimer::GetServerTime();
	m_ui32ErrorCode			= ui32ErrorCode;
	m_ui32KillReason		= i32KillReason;
	if( wstrMessage )
	{
		i3String::Copy( m_wstrHackNotice, wstrMessage, DEF_SIZE_HACK_NOTICE );
	}
}

void CUserSession::_SendFieldShopInfo( UINT32 ui32FieldShopVersion )
{
	if( ui32FieldShopVersion == g_pContextMain->GetFieldShopVersion() )	return;

	// Send All FieldShop information
	i3NetworkPacket SendPacket(PROTOCOL_FIELDSHOP_GOODSLIST_ACK);
	SendPacket.WriteData( g_pContextMain->GetFieldShop(), sizeof( FieldShopInfo ) );
	SendPacketMessage( &SendPacket );
}

BOOL CUserSession::_SendShopInfo(char* strMD5Key )
{
	// 현재 서버의 Shop Version과 ShopFile MD5Key와 일치하면 상점 데이터를 줄필요가 없으니 리턴.
	CShop* pShop = g_pContextMain->GetShop();
	if( 0 == i3String::Compare(strMD5Key, pShop->GetShopMD5Key() ) 	)	return FALSE;

	UINT32 ui32Version				= pShop->GetVersion();
	UINT32 ui32GoodsCount			= pShop->GetGoodsCount();
	UINT32 ui32ItemCount			= pShop->GetItemCount();
	UINT32 ui32MatchingCount		= pShop->GetMatchingCount();
	UINT32 ui32RepairCount			= pShop->GetRepairCount();


	SHOP_GOODS_EXPANSION*	pGoodsData	= pShop->GetGoodsAllData();
	SHOP_ITEM_BASE*			pItemData	= pShop->GetItemAllData();
	SHOP_MATCHING_DATA*		pMatching	= pShop->GetMatchingAllData();
	SHOP_ITEM_REPAIR_DATA*	pRepairData	= pShop->GetRepairAllData();

	// Send Ready
	INT32	i32PacketSize		= PACKETBUFFERSIZE - PACKETHEADERSIZE - sizeof( UINT32 ) - sizeof( UINT32 ) - sizeof( UINT32 ) - sizeof( UINT32 );
	UINT32	ui32MaxCount		= 0;
	UINT32	ui32RemainCount		= 0;
	UINT32	ui32CurItemCount	= 0;
	UINT32	ui32StartIdx		= 0;
	bool	bSendContinue		= false;


	// ItemList
	ui32RemainCount		= ui32ItemCount;
	ui32CurItemCount	= 0;
	ui32StartIdx		= 0;

	bSendContinue		= true;			// 데이터가 0개라도 데이터가 0개임을 보내서 알린다.
	ui32MaxCount		= (i32PacketSize / sizeof( SHOP_ITEM_BASE ));
	while (bSendContinue)
	{
		ui32StartIdx += ui32CurItemCount;
		if (ui32RemainCount > ui32MaxCount) { ui32CurItemCount = ui32MaxCount; }
		else { ui32CurItemCount = ui32RemainCount; bSendContinue = false; }
		ui32RemainCount -= ui32CurItemCount;

		i3NetworkPacket	ItemPacket(PROTOCOL_AUTH_SHOP_ITEMLIST_ACK);
		ItemPacket.WriteData(&ui32ItemCount, sizeof(UINT32));
		ItemPacket.WriteData(&ui32CurItemCount, sizeof(UINT32));
		ItemPacket.WriteData(&ui32StartIdx, sizeof(UINT32));
		ItemPacket.WriteData(&pItemData[ui32StartIdx], (UINT16)(sizeof(SHOP_ITEM_BASE)*ui32CurItemCount));
		ItemPacket.WriteData(&ui32Version, sizeof(UINT32));
		SendPacketMessage(&ItemPacket);
	}

	// GoodsList
	ui32RemainCount		= ui32GoodsCount;
	ui32CurItemCount	= 0;
	ui32StartIdx		= 0;

	bSendContinue		= true;			// 데이터가 0개라도 데이터가 0개임을 보내서 알린다.
	ui32MaxCount		= (i32PacketSize / sizeof( SHOP_GOODS_EXPANSION ));
	while( bSendContinue )
	{
		ui32StartIdx			+= ui32CurItemCount;
		if( ui32RemainCount > ui32MaxCount )	{	ui32CurItemCount	= ui32MaxCount;								}
		else									{	ui32CurItemCount	= ui32RemainCount; bSendContinue = false;	}
		ui32RemainCount -= ui32CurItemCount;

		i3NetworkPacket	GoodsPacket( PROTOCOL_AUTH_SHOP_GOODSLIST_ACK );
		GoodsPacket.WriteData( &ui32GoodsCount,				sizeof( UINT32 ) );
		GoodsPacket.WriteData( &ui32CurItemCount,			sizeof( UINT32 ) );
		GoodsPacket.WriteData( &ui32StartIdx,				sizeof( UINT32 ) );
		GoodsPacket.WriteData( &pGoodsData[ui32StartIdx],	(UINT16)(sizeof( SHOP_GOODS_EXPANSION )*ui32CurItemCount) );
		GoodsPacket.WriteData( &ui32Version,				sizeof( UINT32 ) );
		SendPacketMessage( &GoodsPacket );
		
	}

	// RepairList
	ui32RemainCount		= ui32RepairCount;
	ui32CurItemCount	= 0;
	ui32StartIdx		= 0;

	bSendContinue		= true;			// 데이터가 0개라도 데이터가 0개임을 보내서 알린다.
	ui32MaxCount		= (i32PacketSize / sizeof( SHOP_ITEM_REPAIR_DATA ));
	while( bSendContinue )
	{
		ui32StartIdx			+= ui32CurItemCount;
		if( ui32RemainCount > ui32MaxCount )	{	ui32CurItemCount	= ui32MaxCount;								}
		else									{	ui32CurItemCount	= ui32RemainCount; bSendContinue = false;	}
		ui32RemainCount -= ui32CurItemCount;

		i3NetworkPacket	RepairPacket( PROTOCOL_AUTH_SHOP_REPAIRLIST_ACK );
		RepairPacket.WriteData( &ui32RepairCount,			sizeof( UINT32 ) );
		RepairPacket.WriteData( &ui32CurItemCount,			sizeof( UINT32 ) );
		RepairPacket.WriteData( &ui32StartIdx,				sizeof( UINT32 ) );
		RepairPacket.WriteData( &pRepairData[ui32StartIdx],	(UINT16)(sizeof( SHOP_ITEM_REPAIR_DATA )*ui32CurItemCount) );
		RepairPacket.WriteData( &ui32Version,					sizeof( UINT32 ) );

		SendPacketMessage( &RepairPacket );
	}

	// MatchingList
	ui32RemainCount		= ui32MatchingCount;
	ui32CurItemCount	= 0;
	ui32StartIdx		= 0;

	bSendContinue		= true;			// 데이터가 0개라도 데이터가 0개임을 보내서 알린다.
	ui32MaxCount		= (i32PacketSize / sizeof( SHOP_MATCHING_DATA ));
	while( bSendContinue )
	{
		ui32StartIdx			+= ui32CurItemCount;
		if( ui32RemainCount > ui32MaxCount )	{	ui32CurItemCount	= ui32MaxCount;								}
		else									{	ui32CurItemCount	= ui32RemainCount; bSendContinue = false;	}
		ui32RemainCount -= ui32CurItemCount;

		i3NetworkPacket	MatchingPacket( PROTOCOL_AUTH_SHOP_MATCHINGLIST_ACK );
		MatchingPacket.WriteData( &ui32MatchingCount,		sizeof( UINT32 ) );
		MatchingPacket.WriteData( &ui32CurItemCount,		sizeof( UINT32 ) );
		MatchingPacket.WriteData( &ui32StartIdx,			sizeof( UINT32 ) );
		MatchingPacket.WriteData( &pMatching[ui32StartIdx],	(UINT16)(sizeof( SHOP_MATCHING_DATA )*ui32CurItemCount) );
		MatchingPacket.WriteData( &ui32Version,				sizeof( UINT32 ) );
		SendPacketMessage( &MatchingPacket );
	}

	return TRUE; 
}

// -----------------------------------------------------------
// Name : _GetMyInfoPacket
// Desc : Trans 서버로 부터 받는 나의 정보에 대한 패킷을 파싱합니다.
// -----------------------------------------------------------
void CUserSession::_GetMyInfoPacket( INT32 i32ThreadIdx, i3NetworkPacket* pPacket, BOOL bClan )
{
	pPacket->ReadData( &m_ui8PBVer,									sizeof(UINT8) );

	m_ui8PBVer = g_pContextMain->RegistPBVersion(m_ui8PBVer);

	switch( m_ui8PBVer )
	{
	case PB_VER_V10 : 
		m_pDesign = m_pDesignV1;
		break;
	default: 
		m_pDesign = m_pDesignV2;
		break; 
	}

	pPacket->ReadData( &m_ui32BirthDay,								sizeof(UINT32) );
	pPacket->ReadData( &m_ui8Gender,								sizeof(UINT8) );
	pPacket->ReadData( &m_UserInfoBasic.m_ui32Cash,					sizeof(UINT32) );
	pPacket->ReadData( &m_UserInfoBasic.m_ui8ESportID,				sizeof(UINT8) );

	pPacket->ReadData( &m_UserInfoBasic,							sizeof(USER_INFO_BASIC_BASE) );	
	pPacket->ReadData( &m_UserInfoServer,							sizeof(USER_INFO_SERVER) );	
	m_UserInfoServer.m_dt32HackChatBlockDate.CheckDATE32();
	m_dt32HackChatBlockDate = m_UserInfoServer.m_dt32HackChatBlockDate;
	if( bClan )
	{
		pPacket->ReadData( &m_UserInfoBasic.m_ui32Clanidx,			sizeof(UINT32) );
		pPacket->ReadData( &m_UserInfoBasic.m_ui32ClanState,		sizeof(UINT32) );
		pPacket->ReadData( &m_UserInfoBasic.m_ui8ClanExpRank,		sizeof(UINT8) );
		pPacket->ReadData( &m_UserInfoBasic.m_ui8ClanPerRank,		sizeof(UINT8) );
		pPacket->ReadData( &m_UserInfoBasic.m_ui32ClanMark,			sizeof(UINT32) );
		pPacket->ReadData( &m_UserInfoBasic.m_strClanName,		 	sizeof(TTCHAR) * NET_CLAN_NAME_SIZE );
		pPacket->ReadData( &m_ui32ClanRankingDate,					sizeof(UINT32) );
	}	

	ITEM_INFO				aCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];								// 장착 정보
	pPacket->ReadData( aCommonEquips,								sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COMMON_COUNT );
	m_MultiSlot.SetCommonEquipmentAll( aCommonEquips );
	
	UINT8 ui8RedSlot;
	UINT8 ui8Bluelot;
	pPacket->ReadData( &ui8RedSlot,							sizeof(UINT8) );
	pPacket->ReadData( &ui8Bluelot,							sizeof(UINT8) );
	m_MultiSlot.SetTeamSlot( (INT32)ui8RedSlot, TEAM_RED  );
	m_MultiSlot.SetTeamSlot( (INT32)ui8Bluelot, TEAM_BLUE );

	pPacket->ReadData( &m_UserInfoRecord,							sizeof(USER_INFO_RECORD) );
	pPacket->ReadData( &m_UserInfoInvItemData,						sizeof(USER_INFO_INVITEM_DATA) );

	pPacket->ReadData( &m_dt32LoginDate,							sizeof(DATE32) );
	pPacket->ReadData( &m_ui32AccConnectTime,						sizeof(UINT32) );
	pPacket->ReadData( &m_ui32AccConnectCount,						sizeof(UINT32) );
	pPacket->ReadData( &m_dt32LastLoginTime,						sizeof(DATE32) );
	pPacket->ReadData( &m_UserInfoDaily,							sizeof(USER_INFO_DAILY) );
	pPacket->ReadData( &m_ui8Country,								sizeof(UINT8));
	pPacket->ReadData( &m_stAttendance,								sizeof(ATTENDANCE_USER_SVR)*ATTENDANCE_TYPE_MAX );
	pPacket->ReadData( &m_stAttendanceInfo,							sizeof(ATTENDANCE_INFO)*ATTENDANCE_TYPE_MAX );
	pPacket->ReadData( m_aQuickJoinInfo,							sizeof( QUICKJOIN_INFO )*MAX_QUICK_JOIN_INFO_COUNT );
	RecvMedalAllInfoFromTrans( pPacket );

	pPacket->ReadData( &m_bAASDailyUsageWay,						sizeof(bool));
	pPacket->ReadData( &m_bAASAppointedWay,							sizeof(bool));
	pPacket->ReadData( &m_ui16AASAccMin,							sizeof(UINT16));

	pPacket->ReadData( m_aui32AASBanTime,							sizeof(UINT32)*DAY_COUNT_IN_WEEK);
	
	m_ui16AASConnectTime = g_pContextMain->m_ui16AASCurTime;

	m_dt32MegaPhoneUseTime	= m_dt32LoginDate;	//최초접속시 로그인 시간으로 초기 값을 설정해줍니다.

	pPacket->ReadData( &m_bGiftAutoAuth,							sizeof(bool));

	INT32 i32GachaPurchaseRecordCnt = 0;
	pPacket->ReadData( &i32GachaPurchaseRecordCnt,					sizeof(INT32));

	for( INT32 i = 0; i < i32GachaPurchaseRecordCnt ; ++i )
	{
		UINT16 ui16ItemKey = 0;		// 패킷 사이즈 축소를 위해 줄입니다.
		UINT16 ui16PurchaseCnt = 0; // 패킷 사이즈 축소를 위해 줄입니다.

		pPacket->ReadData( &ui16ItemKey,							sizeof(UINT16));
		pPacket->ReadData( &ui16PurchaseCnt,						sizeof(UINT16));

		m_GachaPurchase.m_arPurchaseRecord[i].InsertPurchaseRecord(ui16ItemKey,ui16PurchaseCnt);
	}

	m_GachaPurchase.SetRecordCount(i32GachaPurchaseRecordCnt);
	
	// 게임서버에서 Trans서버로부터 받은 값들을 가지고 기본적으로 설정하는 값입니다.
	_SetUserRecord();						// 유저 전적 세팅
	_SetCalcLimitTitleCount();				// 호칭슬롯 해제
	_SetAge();								// 나이설정
	_SetRankByAuthLevel();					// GM 확인	

	//_ExceptionEquip(   );		// 공짜아이템 추가
	//_ExceptionSpecial(  );		// 스페샬 아이템 추가	

	//_SyncEquipIdx();						// 장비 ItemID 설정

	_CheckEquipment();						// 장착된 아이템 확인
	_SetQuestEvent();						// 새로운 이벤트 임무카드를 확인합니다.

	CheckInvenItemByUserInfo(i32ThreadIdx);

	m_bHSCheck		= TRUE;
	m_ui8OldPCCafe	= m_UserInfoBasic.m_ui8PCCafe;
	m_ui8NewPCCafe	= m_UserInfoBasic.m_ui8PCCafe;

	// 이전 시즌 클랜 랭킹 정보 ( 1~3위)
	if( bClan ) 
	{
		SendSeasonRankingClan();
	}

	return;
}

void CUserSession::_SetUserRecord()
{
	// Trans 에 받을 당시에 전체 전적에는 초기화된 전적이 들어있습니다.
	// 전체 전적 = 초기화 전적 + 시즌 전적
	m_UserInfoRecord._match			+= m_UserInfoRecord._smatch;
	m_UserInfoRecord._win			+= m_UserInfoRecord._swin;
	m_UserInfoRecord._lose			+= m_UserInfoRecord._slose;
	m_UserInfoRecord._draw			+= m_UserInfoRecord._sdraw;
	m_UserInfoRecord._killcount		+= m_UserInfoRecord._skillcount;
	m_UserInfoRecord._headshot		+= m_UserInfoRecord._sheadshot;
	m_UserInfoRecord._death			+= m_UserInfoRecord._sdeath;
	m_UserInfoRecord._dmatch		+= m_UserInfoRecord._sdmatch;
	m_UserInfoRecord._dkillcount	+= m_UserInfoRecord._sdkillcount;
	m_UserInfoRecord._dis			+= m_UserInfoRecord._sdis;	
	m_UserInfoRecord._AssistCount	+= m_UserInfoRecord._sAssistCount;
}

void CUserSession::_SetCalcLimitTitleCount( )
{
	m_i32LimitUserTitleCount = 1;

	// 돌격 부대원, 침투 부대원, 특수 부대원 중 하나만 획득했을 경우 2번째 슬롯이 풀린다.
	if( (1 == GET_USERTITLE( m_ui64GainUserTitle, 5 )) ||
	(1 == GET_USERTITLE( m_ui64GainUserTitle, 6 )) ||
	(1 == GET_USERTITLE( m_ui64GainUserTitle, 7 )))
	{
		m_i32LimitUserTitleCount++;
	}
	if( (1 == GET_USERTITLE( m_ui64GainUserTitle, 11 )) ||
	(1 == GET_USERTITLE( m_ui64GainUserTitle, 17 )) ||
	(1 == GET_USERTITLE( m_ui64GainUserTitle, 23 )) ||
	(1 == GET_USERTITLE( m_ui64GainUserTitle, 43 )))
	{
		m_i32LimitUserTitleCount++;
	}
}

void CUserSession::_SetAge()
{
	
	// 트랜스 서버에서 나이를 사용하지 않는나라의 경우 생년월일을 1980년으로 임의 설정 하고 있음
	if( m_ui32BirthDay == 0 )
	{
		m_ui8Age = 0;
	}
	else
	{		
		m_ui8Age	= (UINT8)((i3ThreadTimer::LocalTime().GetDateTimeYYYYMMDD() - m_ui32BirthDay) / 10000);
		
		switch( m_ui8LocaleCode )
		{
		case LOCALE_CODE_INDONESIA		:
			{
				if( 20 > m_ui8Age ) m_ui8Age = 20;
			}
			break;
		}
	}
}

void CUserSession::_ExceptionInsert( T_ItemID TItemID, UINT8 ui8ItemType, INVEN_BUFFER_SET* pInvenSet )
{
	if (ITEM_TYPE_CHARA == GET_ITEM_TYPE(TItemID))
	{
		_ExceptionChara(TItemID, ui8ItemType);
		return;
	}

	INVEN_BUFFER InvenItem;
	g_MakeBasicItemDBIdx( InvenItem._TItemWareDBIdx, TItemID );
	InvenItem._ui32ItemID			= TItemID;
	InvenItem._ui8ItemType			= ui8ItemType;
	InvenItem._ui32ItemArg			= 1;

	OnInsertItem( &InvenItem, pInvenSet );
}

INT32 CUserSession::CheckEnterChannel( UINT32 ui32Channel )
{	
	if( GAME_CHANNEL_COUNT <= ui32Channel )
		return EVENT_ERROR_SERVER;

	if( CheckAuthLevel() )
		return EVENT_ERROR_SUCCESS;

	REAL32	r32Killcount	= (REAL32)m_UserInfoRecord._killcount;
	REAL32	r32Deathcount	= (REAL32)m_UserInfoRecord._death;

#ifdef USE_NEW_CHANNEL_SYS
	ENTRANCE_CHANNEL_TYPE* pChannelType;

	for( INT32 i = 0 ; i < ENTRANCE_CHANNEL_SET_COUNT ; i++ )
	{		
		pChannelType = g_pContextMain->GetChannelType( ui32Channel, i );
		if( NULL == pChannelType ) continue;

		switch( pChannelType->m_ui8Type )
		{
		case CHANNEL_TYPE_NORMAL:
			break;
		case CHANNEL_TYPE_RANK:
			if( pChannelType->m_ui8Min > m_UserInfoBasic.m_ui32Rank )									return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
			if( pChannelType->m_ui8Max < m_UserInfoBasic.m_ui32Rank )									return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
			break;
		case CHANNEL_TYPE_KILL_RATE:

			if( 0 == (m_UserInfoRecord._killcount + m_UserInfoRecord._death) )							return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					
			ui82Rate = (UINT8)((r32Killcount / (r32Killcount + r32Deathcount)) * 100.f);

			if( pChannelType->m_ui8Min > ui82Rate )														return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
			if( pChannelType->m_ui8Max < ui82Rate )														return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
			break;
		case CHANNEL_TYPE_CLAN:
			// 클랜원이 아닐 경우
			if( 0 == m_UserInfoBasic.m_ui32Clanidx ||
				CLAN_MEMBER_LEVEL_UNKNOWN == m_UserInfoBasic.m_ui32ClanState )
			{
				// 클랜 매치 시스템 비활성화시 입장 실패.
				if( FALSE == g_pContextMain->IsContentsEnabled(CONTENTS_CLAN_MATCH_NEW) )				return EVENT_ERROR_CLAN_NOMEMBER;
			
				//// 이미 용병이면 성공처리.
				//if( m_TMerID != MERCENARY_INVALID_IDX )  return EVENT_ERROR_SUCCESS;

				T_RESULT TResult = CheckMercenaryRegist();
				if( EV_FAILED(TResult) )	return TResult;

				// 용병 등록 요청 실패하면 입장 실패.
				if( FALSE == g_pModuleClan->SendClanMatchRegistMercenary( m_WorkThreadIdx, this  ) )	return EVENT_ERROR_NETWORK;
			}
			break;
		case CHANNEL_TYPE_NATION:
			if( pChannelType->m_ui8Min != m_ui8Country )												return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
			break;
		case CHANNEL_TYPE_PCCAFE:
			if( pChannelType->m_ui8Min > m_UserInfoBasic.m_ui8PCCafe )									return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
			if( pChannelType->m_ui8Max < m_UserInfoBasic.m_ui8PCCafe )									return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
			break;
		case CHANNEL_TYPE_CHAMPIONSHIP:
			break;
		}
	}
#else
	
	switch(g_pContextMain->m_pChannelType[ ui32Channel ])
	{
	case CHANNEL_TYPE_BEGIN1	:
		{	//KillDeath 확인 
			if ((m_UserInfoRecord._killcount + m_UserInfoRecord._death) != 0)
			{
				if ((r32Killcount / (r32Killcount + r32Deathcount)) > 0.40f)
				{
					return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
				}
			} 
		}
		break; 
	case CHANNEL_TYPE_BEGIN2	: 
		{
			switch (m_ui8LocaleCode)
			{
			case LOCALE_CODE_BRAZIL:
				{
					if (RANK_11 < m_UserInfoBasic.m_ui32Rank)
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			case LOCALE_CODE_RUSSIA:
				{
					if( RANK_04 < m_UserInfoBasic.m_ui32Rank )
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			case LOCALE_CODE_LATIN_AMERICA:
				{
					if (RANK_03 < m_UserInfoBasic.m_ui32Rank)
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			case LOCALE_CODE_THAILAND:
				{
					if (RANK_07 < m_UserInfoBasic.m_ui32Rank)
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			default:
				{
					if (RANK_06 < m_UserInfoBasic.m_ui32Rank)
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			}
			if( m_ui8LocaleCode == LOCALE_CODE_RUSSIA )
			{
				if (RANK_04 < m_UserInfoBasic.m_ui32Rank)
					return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
			}
		}
		break; 

	case CHANNEL_TYPE_PCCAFE_CLAN:
		if( m_ui8LocaleCode == LOCALE_CODE_INDONESIA )
		{
			if( m_UserInfoBasic.m_ui8PCCafe != PC_INDO_SILVER && m_UserInfoBasic.m_ui8PCCafe != PC_INDO_GOLD )
				return EVENT_ERROR_CHANNEL_ENTER_CONDITION_GWARNET;
		}
		// break 없음
	case CHANNEL_TYPE_CLAN		: 
		{
			if( LOCALE_CODE_INDONESIA == m_ui8LocaleCode )
			{
				if( m_UserInfoServer.m_ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME )
					return EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME;
			}

			// 클랜원이면 성공 - 가입신청만해도 m_ui32Clanidx에 값이 들어가므로 m_ui32ClanState와 묶어서 같이 검사합니다.
			if (m_UserInfoBasic.m_ui32Clanidx > 0 &&
				m_UserInfoBasic.m_ui32ClanState > CLAN_MEMBER_LEVEL_UNKNOWN) return EVENT_ERROR_SUCCESS;

			// 클랜 매치 시스템 비활성화시 입장 실패.
			if( !g_pContextMain->IsContentsEnabled(CONTENTS_CLAN_MATCH_NEW) )	return EVENT_ERROR_CLAN_NOMEMBER;
			
			//// 이미 용병이면 성공처리.
			//if( m_TMerID != MERCENARY_INVALID_IDX )  return EVENT_ERROR_SUCCESS;

			T_RESULT TResult = CheckMercenaryRegist();
			if( EV_FAILED(TResult) )	return TResult;

			// 용병 등록 요청 실패하면 입장 실패.
			if( FALSE == g_pModuleClan->SendClanMatchRegistMercenary( m_WorkThreadIdx, this  ) )	return EVENT_ERROR_NETWORK;

			return EVENT_ERROR_CLAN_WAR_MERCENAY; // 클랜 미가입자가 클랜채널을 선택한 경우, PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK 에서 입장 처리 합니다.
			
		}
		break; 

	case CHANNEL_TYPE_EXPERT	: 
		{
			if( LOCALE_CODE_INDONESIA == m_ui8LocaleCode )
			{
				if (m_UserInfoServer.m_ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME) return EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME;
			}
			
			switch( m_ui8LocaleCode )
			{
			case LOCALE_CODE_INDONESIA		:
				{
					if( RANK_34 > m_UserInfoBasic.m_ui32Rank )			// 소령 1호봉 이상
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			case LOCALE_CODE_RUSSIA			:
				{
					if( RANK_30 > m_UserInfoBasic.m_ui32Rank )			// 대위 1호봉 이상
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			case LOCALE_CODE_BRAZIL			:
			case LOCALE_CODE_TURKEY			:
				{
					if( RANK_26 > m_UserInfoBasic.m_ui32Rank )			// 중위 1호봉 이상
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			case LOCALE_CODE_PHILIPPINES			:
				{
					if( RANK_16 > m_UserInfoBasic.m_ui32Rank )			// 중사 1호봉 이상
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			case LOCALE_CODE_LATIN_AMERICA:
				{
					if (RANK_04 > m_UserInfoBasic.m_ui32Rank)
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			default:
				{
					if( RANK_10 > m_UserInfoBasic.m_ui32Rank )			// 병장이상
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			}
		}
		break; 
	case CHANNEL_TYPE_EXPERT2	: 
		{
			if( LOCALE_CODE_INDONESIA == m_ui8LocaleCode )
			{
				if (m_UserInfoServer.m_ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME) return EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME;
			}

			switch (m_ui8LocaleCode)
			{
			case LOCALE_CODE_LATIN_AMERICA:
				{
					if (RANK_12 > m_UserInfoBasic.m_ui32Rank)	// 상사 1호봉이상
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			default:
				{
					if (RANK_19 > m_UserInfoBasic.m_ui32Rank)	// 상사 1호봉이상
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			}			
		}
		break;
	case CHANNEL_TYPE_MIDDLE	:
		{
			if( LOCALE_CODE_INDONESIA == m_ui8LocaleCode )
			{
				if (m_UserInfoServer.m_ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME) return EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME;
			}
			
			switch( m_ui8LocaleCode )
			{
			case LOCALE_CODE_LATIN_AMERICA		:
				{
					// 하사 1호봉 ~ 상사 3호봉 까지
					if( RANK_04 > m_UserInfoBasic.m_ui32Rank )
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
					if( RANK_11 < m_UserInfoBasic.m_ui32Rank )
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			case LOCALE_CODE_TURKEY				:
				{
					// 하사 2호봉 ~ 원수
					if( RANK_14 > m_UserInfoBasic.m_ui32Rank )
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
					if( RANK_50 < m_UserInfoBasic.m_ui32Rank )
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			case LOCALE_CODE_BRAZIL:
				{
					if (RANK_12 > m_UserInfoBasic.m_ui32Rank)			// 12랭크 이상 
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
					if (RANK_25 < m_UserInfoBasic.m_ui32Rank)			// 25랭크 이하 
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			default:
				{
					// 하사 1호봉 ~ 상사3호봉 까지
					if( RANK_13 > m_UserInfoBasic.m_ui32Rank )
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
					if( RANK_21 < m_UserInfoBasic.m_ui32Rank )
					{
						return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
					}
				}
				break;
			}
		}
		break;
	case CHANNEL_TYPE_PCCAFE_SILVER	: 
		if( m_ui8LocaleCode == LOCALE_CODE_INDONESIA )
		{
			if (m_UserInfoServer.m_ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME) return EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME;
			if (m_UserInfoBasic.m_ui8PCCafe != PC_INDO_SILVER && m_UserInfoBasic.m_ui8PCCafe != PC_INDO_GOLD)	return EVENT_ERROR_CHANNEL_ENTER_CONDITION_GWARNET;
		}
		break; 
	case CHANNEL_TYPE_PCCAFE_GOLD	: 
		if( m_ui8LocaleCode == LOCALE_CODE_INDONESIA )
		{
			if (m_UserInfoServer.m_ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME) return EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME;
			if (m_UserInfoBasic.m_ui8PCCafe != PC_INDO_GOLD)	return EVENT_ERROR_CHANNEL_ENTER_CONDITION_GWARNET;
		}
		break; 
	case CHANNEL_TYPE_AZERBAIJAN:
		if( 0 != strcmp(g_astrGeoIP2_country_code[m_ui8Country], "AZ"))	return EVENT_ERROR_CHANNEL_ENTER_CONDITION_AZERBAIJAN;
		break;
	case CHANNEL_TYPE_NORMAL:
	{	 
		switch (m_ui8LocaleCode)
		{
			case LOCALE_CODE_THAILAND:
			{
				if ( m_UserInfoBasic.m_ui32Rank < RANK_08)
				{
					return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
				}
			}
			break;
		}
	}
	break;
	default:
		if( LOCALE_CODE_INDONESIA == m_ui8LocaleCode )
		{
			if (m_UserInfoServer.m_ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME) return EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME;
		}
		break; 
	}
#endif
	return EVENT_ERROR_SUCCESS;
}

INT32 CUserSession::CreateRoom( INT32 i32ThreadIdx, ROOM_INFO_CREATE* pInfoCreate )
{
	pInfoCreate->nChannelNum			= m_i32ChannelNum;

	m_i32RoomIdx = SERVER_VALUE_UNKNOWN;
	m_i32SlotIdx = SERVER_VALUE_UNKNOWN;

	INT32 i32RoomIdx;
	INT32 i32Check = _ChaeckCreateRoom(); 
	
	if( EV_SUCCESSED( i32Check ))
	{
		i32RoomIdx = g_pRoomManager->OnCreateRoom( this, pInfoCreate ); 
		if( SERVER_VALUE_UNKNOWN < i32RoomIdx )
		{
			SetPosition(GAME_TASK_READY_ROOM); 
			m_pRoom				= g_pRoomManager->GetRoom( m_i32ChannelNum, i32RoomIdx ); 
			if( NULL == m_pRoom ) return EVENT_ERROR_USER_WRONG_CHANNEL_IDX;
			m_i32RoomIdx		= i32RoomIdx; 
			m_i32SlotIdx		= 0; 	
			m_bForceRoomLeave	= FALSE; 

			g_pModuleMessenger->MessengerSendFriendRoomEnter( i32ThreadIdx, this, (UINT8)m_i32ChannelNum, (UINT16)m_i32RoomIdx ); 
		}
	}
	else i32RoomIdx = i32Check;
	 
	i3NetworkPacket SendPacket( PROTOCOL_ROOM_CREATE_ACK );
	SendPacket.WriteData( &i32RoomIdx,				sizeof(INT32) );
	if( SERVER_VALUE_UNKNOWN < i32RoomIdx )
	{
		SendPacket.WriteData(&m_pRoom->m_Info._InfoBasic,	sizeof(ROOM_INFO_BASIC) );
		SendPacket.WriteData(&m_pRoom->m_Info._InfoAdd,		sizeof(ROOM_INFO_ADD) );

		g_pModuleClan->ClanSendRoomEnter( i32ThreadIdx, this, m_i32RoomIdx, m_pRoom->IsMatchMaking() );
		g_pModuleTrans->AuthSendRoomEnter( i32ThreadIdx, this );
	}
	SendPacketMessage( &SendPacket );

	return i32RoomIdx;
}


INT32 CUserSession::_BuyGoods( UINT8 ui8GoodsCount, BUY_BASKET *stBasket, SPEND_TYPE eBuyType, TTCHAR* pNick, TTCHAR* pMsg )
{
	// 구매 갯수 오류.
	if( 0 == ui8GoodsCount || ui8GoodsCount > MAX_SHOP_GOODS_BUY_COUNT ) return  EVENT_ERROR_USER_HACK_PACKET;

	INT32 i32Result = CheckExistItemInven(stBasket, ui8GoodsCount);
	if (!EV_SUCCESSED(i32Result)) return EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL_SHOP;

	PRICE_TYPE	ePriceType = PRICE_TYPE_SHOP_REAL;

	// 선물 구매시 닉네임 정보 없으면 실패.
	if( SPEND_TYPE_BUY_GIFT == eBuyType ) 
	{
		if( NULL == pNick) return  EVENT_ERROR_USER_HACK_PACKET;
		ePriceType = PRICE_TYPE_GIFT_REAL; 
	}
	
	BUY_PRICE	stBuyPrice;
	i3mem::FillZero( &stBuyPrice, sizeof(BUY_PRICE));	// 없으면 안됨.
	stBuyPrice.m_ePriceType	 = ePriceType;

	for( UINT8 i = 0; i < ui8GoodsCount; ++i )
	{
		// 장바구니 구매 또는 선물하기시 캐릭터는 무조건 실패.
		if (ui8GoodsCount > 1 || SPEND_TYPE_BUY_GIFT == eBuyType)
		{
			if (ITEM_TYPE_CHARA == GET_GOODS_FLAG_TYPE(stBasket[i].m_ui32GoodsID))
			{
				return EVENT_ERROR_SHOP_BUY_FAIL_BASCKET_CHARA_BUY;
			}
		}
		SHOP_GOODS_EXPANSION* pGoods = g_pContextMain->GetShop()->GetGoodsData( stBasket[i].m_ui32GoodsID );

		stBuyPrice.m_eBuyType = (SIA_GOODS_BUY)stBasket[i].m_ui8BuyType;
		INT32 i32Rv = _CheckBuyGoods( pGoods, &stBuyPrice );
		if( EV_FAILED( i32Rv ) )
		{
			return i32Rv;
		}

		i32Rv =  _CheckBuyRestriction( stBasket[i].m_ui32GoodsID );	// 구입 제한
		if( EV_FAILED( i32Rv ) )
		{
			return i32Rv;
		}
	} 

	BOOL bSendPacket = FALSE;

	switch( eBuyType ) 
	{
	case SPEND_TYPE_BUY_GOODS:
		bSendPacket =  g_pModuleTrans->AuthSendBuyGoodsBasket( m_WorkThreadIdx, this, stBasket, ui8GoodsCount );
		break;
	case SPEND_TYPE_BUY_GIFT:
		bSendPacket =  g_pModuleTrans->AuthSendGiftGoods( m_WorkThreadIdx, this, ui8GoodsCount, stBasket, pNick, pMsg );
		break;
	}

	if( !bSendPacket ) 
	{
		return EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED;
	}

	// 포인트 복사 방지를 위해 먼저 차감 합니다. 구매 실패시 복구함.
	if( m_UserInfoBasic.m_ui32Point > stBuyPrice.m_ui32Price[GOODS_BUY_POINT] )
	{ 
		m_UserInfoBasic.m_ui32Point	-= stBuyPrice.m_ui32Price[GOODS_BUY_POINT]; 
	}
	else m_UserInfoBasic.m_ui32Point = 0;

	ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );

	return EVENT_ERROR_SUCCESS;
}

INT32 CUserSession::_CheckBuyRestriction( UINT32 ui32GoodsID )
{
	if( ITEM_TYPE_SET == GET_GOODS_FLAG_TYPE(ui32GoodsID) ) 
	{	// 셋트 상품은 체크하지 않습니다.
		return EVENT_ERROR_SUCCESS;
	}

	CShop* pShop = g_pContextMain->GetShop();
	UINT32 ui32ItemCount			= pShop->GetItemCount();
	UINT32 ui32MatchingCount		= pShop->GetMatchingCount(); 
	SHOP_ITEM_BASE*			pItemData	= pShop->GetItemAllData();
	SHOP_MATCHING_DATA*		pMatching	= pShop->GetMatchingAllData();

	// ui32GoodsID에 매칭되는 ItemID를 검색한다. 
	UINT32 ui32ItemID = 0;
	for ( UINT32 i = 0 ; i < ui32MatchingCount ; i ++ )
	{
		if( pMatching[i]._ui32GoodsID == ui32GoodsID )
		{
			ui32ItemID	= pMatching[i]._ui32ItemID ;
			break;
		}
	}

	if( 0 == ui32ItemID ) return EVENT_ERROR_SUCCESS; // 매칭되는 ItemID가 없다면 강제 성공.

	UINT8 ui8Restriction = 0;
	UINT16 ui16Condition = 0;
	TYPE_ITEM_RESTRICTION eRestrictionType = TYPE_ITEM_RESTRICTION_NONE;
	for ( UINT32 i = 0 ; i < ui32ItemCount ; i ++ )
	{
		if( pItemData[i]._ui32ItemID == ui32ItemID )
		{
			eRestrictionType	= (TYPE_ITEM_RESTRICTION)pItemData[i]._ui8RestrictionType ;
			ui16Condition		= pItemData[i]._ui16Condition;
			ui8Restriction		= pItemData[i]._ui8Restriction;
			break;
		}
	}
	if( 0 == eRestrictionType ) return EVENT_ERROR_SUCCESS; //제한 없는 무기의 경우 바로 성공 리턴

	switch ( eRestrictionType )
	{
	case TYPE_ITEM_RESTRICTION_MEDAL :
		{	
			if( !m_pDesign->CheckBuyRestrictionMedal() ) return EVENT_ERROR_SUCCESS;

			// ui16Condition은 노말 메달 인덱스, ui8Restriction은 달성 레벨을 의미합니다. 
			if( !_CheckCompleteMedal( MEDAL_TYPE_NORMAL, ui16Condition, ui8Restriction  ) ) return EVENT_ERROR_SHOP_BUY_FAIL_RESTRICTION_MEDAL;
		}
		break;
	case TYPE_ITEM_RESTRICTION_TITLE :
		{
			if( !m_pDesign->CheckBuyRestrictionTitle() ) return EVENT_ERROR_SUCCESS;

			//호칭에 따른 무기 착용 제한 체크
			if( !GET_USERTITLE( m_ui64GainUserTitle, ui8Restriction ) ) return EVENT_ERROR_SHOP_BUY_FAIL_RESTRICTION_TITLE;			
		}
		break;
	default :
		break;
	}

	return EVENT_ERROR_SUCCESS;
}

void CUserSession::_EquipmentInBattle( ITEM_INFO* pItemInfo )
{
	if( NULL == pItemInfo ) return;
	if( 0 == pItemInfo->m_TItemDBIdx || 0 == pItemInfo->m_TItemID  ) return;

	UINT16	ui16ChangeFlag = 0;
	INT32 i32Check = EVENT_ERROR_SUCCESS;
	INVEN_BUFFER	aWeaponInven[CHAR_EQUIPMENT_WEAPON_COUNT];
	INVEN_BUFFER	aCharInven[CHAR_EQUIPMENT_PARTS_COUNT];
	INT32 i32CheckSkipWeaponFlag	= 0xFFFFFFFF;
	INT32 i32CheckSkipCharFlag		= 0xFFFFFFFF;
	// vv3 - check
	UINT16  ui16ItemType = (UINT16)GET_ITEM_TYPE( pItemInfo->m_TItemID );
	switch( ui16ItemType )
	{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			{
				i3mem::Copy( &m_aTBattleEquipWeapon[ ui16ItemType - ITEM_TYPE_PRIMARY ], pItemInfo, sizeof( ITEM_INFO ) );
				i32CheckSkipWeaponFlag &= ~( 1 << ( ui16ItemType - 1 ) );
				ui16ChangeFlag |= 1 << ( ui16ItemType - 1 );
				i32Check = _CheckWeaponEquip( TRUE, m_pRoom->GetInfo()->_WeaponFlag, m_aTBattleEquipWeapon, aWeaponInven, i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI(), i32CheckSkipWeaponFlag );
			_SetWeaponEquip( aWeaponInven, i32CheckSkipWeaponFlag );
			}
			break;
		
		//case ITEM_TYPE_CHARA:	// 추후제거
		//case ITEM_TYPE_BERET:	// 추후제거
		case ITEM_TYPE_HEADGEAR:
		case ITEM_TYPE_FACEGEAR:
		case ITEM_TYPE_UPPER:
		case ITEM_TYPE_LOWER:
		case ITEM_TYPE_GLOVE:
		case ITEM_TYPE_BELT:
		case ITEM_TYPE_HOLSTER:
		case ITEM_TYPE_SKIN:
		case ITEM_TYPE_DINO: 
			{
				i3mem::Copy( &m_aTBattleEquipParts[ ui16ItemType - ITEM_TYPE_HEADGEAR ], pItemInfo, sizeof( ITEM_INFO ) );
				i32CheckSkipCharFlag &= ~( 1 << ( ui16ItemType - ITEM_TYPE_HEADGEAR ) );
				ui16ChangeFlag |= 1 << ( ui16ItemType - 1 );

				INT32 i32AtkDefChange = m_pRoom->m_pRoomMode->GetAtkDefChange() ? 1 : 0;

				i32Check = _CheckCharEquip( TRUE, ((m_i32SlotIdx + i32AtkDefChange) % TEAM_COUNT), m_aTBattleEquipParts, aCharInven, i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI(), i32CheckSkipCharFlag );
			_SetCharEquip( aCharInven, i32CheckSkipCharFlag );
			}
			break;
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_WRAP_PERIOD:
		case ITEM_TYPE_WRAP_COUNT:
			{
				m_pRoom->GetSlotInfo( m_i32SlotIdx )->_Item = GetUseItemInRoom();					// 아이템 정보 담기
			ui16ChangeFlag |= 1 << ( ITEM_TYPE_DINO );	// 캐쉬 아이템들은 마지막 플래그를 사용합니다.
			}
			break;
	}

	if( EV_FAILED(i32Check) )
	{	// 이상한 아이템으로 장착을 시도 했습니다.
		_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, PROTOCOL_BATTLE_SLOT_EQUIPMENT_REQ, KILL_REASON_HACK_PACKET, L"_EquipmentInBattle - Not Use Item" );
		return;
	}
	// 무기일 경우 내구도 처리
	m_pRoom->SendSlotEquipmentInfo( m_WorkThreadIdx, m_i32SlotIdx, ui16ChangeFlag );
}

INT32 CUserSession::_CheckWeaponEquip(	BOOL bUsedItem, UINT8 ui8WeaponFlag, ITEM_INFO* aWeaponList, INVEN_BUFFER* aWeaponInven, UINT32 ui32InvenTime, INT32 i32CheckSkipFlag )
{	
	i3mem::FillZero( aWeaponInven, sizeof( INVEN_BUFFER ) * CHAR_EQUIPMENT_WEAPON_COUNT );
	wchar_t wstrErrorLog[MAX_PATH];

	// 무기별 게임 모드(돌격전 모드, 저격전 모드, 샷건전 모드, RPG전 모드) 실행시 주무기 장착이 안된 경우, 클라이언트에서 강제로 입력해주는 무기가 올바른지 검사.
	CRoom * pRoom = g_pRoomManager->GetRoom( m_i32ChannelNum, m_i32RoomIdx );
	if( NULL == pRoom )
	{
		i3String::Format( wstrErrorLog, MAX_PATH, L"Invalid Channel - Channel = %d Room = %d Position = %d", m_i32ChannelNum, m_i32RoomIdx, GetPosition() ) ;
		_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, PROTOCOL_BATTLE_READYBATTLE_REQ, KILL_REASON_HACK_PACKET, wstrErrorLog );
		return -1;
	}

	INT32 i32WeaponUseStop = 0;

	// 예외 처리
	if( 0 == (i32CheckSkipFlag & (0x01<<CHAR_EQUIPMENT_WEAPON_PRIMARY)) )
	{
		BOOL bFakePrimary = FALSE;
		switch( aWeaponList[ CHAR_EQUIPMENT_WEAPON_PRIMARY ].m_TItemID )
		{
		case 0:
			{
				if( false == g_pContextMain->IsContentsEnabled( CONTENTS_DEFAULT_WEAPON ) )
				{
					bFakePrimary = TRUE;
				}
			}
			break;
		case 103047:				//MAKE_ITEM_ID(ITEM_TYPE_PRIMARY,WEAPON_CLASS_ASSAULT,WEAPON_NUM_ASSAULT_K2_IN_MODE):	
		case 105025:				//MAKE_ITEM_ID(ITEM_TYPE_PRIMARY,WEAPON_CLASS_SNIPER,WEAPON_NUM_SNIPER_SSG69_IN_MODE):		// GET_ROOM_INFO_WEAPON_SNIPER( pRoom->m_Info._InfoBasic._WeaponFlag )
		case 106007:				//MAKE_ITEM_ID(ITEM_TYPE_PRIMARY,WEAPON_CLASS_SHOTGUN,WEAPON_NUM_SHOTGUN_870MCS_IN_MODE):	// GET_ROOM_INFO_WEAPON_SHOTGUN( pRoom->m_Info._InfoBasic._WeaponFlag )
			{
				bFakePrimary = TRUE;
			}
			break;
		case 116002:				//MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_ROCKET_LAUNCHER, WEAPON_NUM_ROCKET_LAUNCHER_RPG7):
			if( 0 != GET_ROOM_INFO_WEAPON_RPG7( pRoom->m_Info._InfoBasic._WeaponFlag ))
			{
				bFakePrimary = TRUE;
			}
			break;
		case 103045:				// TUTORIAL (M4 SR-16 F.C)
		case 103058:				// TUTORIAL (AK-47 F.C)
			if( pRoom->IsTutorialMode() )
			{
				bFakePrimary = TRUE;
			}
			break;
		default:
			break;
		}
		if( bFakePrimary )
		{
			i32CheckSkipFlag |= (0x01 << CHAR_EQUIPMENT_WEAPON_PRIMARY);
			i32WeaponUseStop |= (0x01 << CHAR_EQUIPMENT_WEAPON_PRIMARY);
		}
	}
 
	// 무기 제한 옵션 체크.
	pRoom->CheckWeaponFlag( aWeaponList, i32CheckSkipFlag, i32WeaponUseStop );

	// 모드별 Default 무기 체크.
	pRoom->m_pRoomMode->CheckDefaultWeapon( aWeaponList, i32CheckSkipFlag, i32WeaponUseStop, m_i32SlotIdx );

	// 무기 내구도 차감 중지.
	for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT ; ++i )
	{
		if( ! ( i32WeaponUseStop & ( 0x01 << i ) ) ) continue;
		
		m_aEnduranceUsedWeapon[ i ].UseStop();
		m_aUseWeaponEndurance[ i ] = 100; 
	}
	

	CShop* pShop = g_pContextMain->GetShop();
	for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT ; i++ )
	{
		if( ( i32CheckSkipFlag & (0x01<<i) )  ) 
		{	// 체크 스킵 플래그가 켜져 있다면 ItemID, DBIdx만 셋팅한다. 
			aWeaponInven[i]._ui32ItemID			= aWeaponList[i].m_TItemID;
			aWeaponInven[i]._TItemWareDBIdx		= aWeaponList[i].m_TItemDBIdx;
			//m_aUseWeaponEndurance[ i ] = 100; 
			continue;
		}

		if( 0 == aWeaponList[i].m_TItemDBIdx || 0 == aWeaponList[i].m_TItemID ) continue;

		INVEN_BUFFER* pInvenBuffer = m_Inventory.GetItemByWareDBIdx( aWeaponList[i].m_TItemDBIdx );
		if( NULL == pInvenBuffer )
		{
			g_pLog->WriteLogSession( this, L"[CheckWeapon] DBidx is not exist. UID : %I64d / ItemDBIdx : %d / EquipIdx : %d ", m_i64UID, aWeaponList[i].m_TItemDBIdx, i );
			return -2;
		}

		if( pInvenBuffer->_ui32ItemID != aWeaponList[i].m_TItemID ) 
		{
			g_pLog->WriteLogSession( this, L"[CheckWeapon] ItemID is not same. UID : %I64d / ItemDBIdx : %d / ItemID(Inv) : %d / ItemID(Eqp) : %d / EquipIdx : %d ", 
				m_i64UID, aWeaponList[i].m_TItemDBIdx, pInvenBuffer->_ui32ItemID, aWeaponList[i].m_TItemID, i );
			return -3;
		}

		i3mem::Copy( &aWeaponInven[i], pInvenBuffer, sizeof( INVEN_BUFFER ) );

		// 슬롯에 맞지 않는 무기를 장착하고 있습니다.
		if( FALSE == m_pDesign->CheckItemPosition( aWeaponInven[i]._ui32ItemID, (CHAR_EQUIPMENT)i ) )		
		{
			g_pLog->WriteLogSession( this, L"[CheckWeapon] EquipSlot is not match. UID : %I64d / ItemID : %d / EquipIdx : %d", m_i64UID, aWeaponInven[i]._ui32ItemID, i );
			return -4;
		}
		
		m_aEnduranceUsedWeapon[i].UseStop();

		if( ITEM_ATTR_TYPE_AUTH < aWeaponInven[i]._ui8ItemType )
		{	// 밀어넣기 아이템일 더이상 계산하지 않으며 내구도는 100% 입니다. 	
			m_aUseWeaponEndurance[ i ]	= 100;
			continue;
		}
		
		// 상점에 등록된 아이템인지 체크
		SHOP_ITEM_BASE*	pItem = pShop->GetItemData( aWeaponInven[i]._ui32ItemID );
		if( NULL == pItem )																	
		{
			g_pLog->WriteLogSession( this, L"[CheckWeapon] ItemID is not exist. UID : %I64d / ItemID : %d / EquipIdx : %d", m_i64UID, aWeaponInven[i]._ui32ItemID, i );
			return -5;
		}


		switch( pItem->_ui8AuthType )
		{
		case ITEM_TYPE_COUNT:
			{
				// 내구도 무기일 경우
				if( ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType )
				{
					switch( g_pContextMain->GetContentsVersion( CONTENTS_REPARE_DURABILITY ) )
					{
					case REPARE_DURABILITY_VER_1:
						{
							if( 0 == aWeaponInven[i]._ui32ItemArg )		m_aUseWeaponEndurance[ i ]		= 0;
							else										m_aUseWeaponEndurance[ i ]		= 100;
						}
						break;
					case REPARE_DURABILITY_VER_2:
						{	// 내구도 시스템 이면서 새로운 수리 시스템이면 내구도 % 를 포함 해 줍니다.
							SHOP_ITEM_DATA*	pItemData = pShop->GetItemDataEx( aWeaponInven[i]._ui32ItemID );
							if( 0 < pItemData->_ui32Durability )
							{
								m_aUseWeaponEndurance[ i ] = (UINT8)((aWeaponInven[i]._ui32ItemArg*100) / pItemData->_ui32Durability);
								if( 0 == m_aUseWeaponEndurance[ i ] )	m_aUseWeaponEndurance[ i ] = 1;
							}
							else
							{
								m_aUseWeaponEndurance[ i ]	= 0;
							}		
						}
						break;
					}
				}
				else
				{
					// 개수제는 0일 경우 게임이 안됩니다.
					if( 0 == aWeaponInven[i]._ui32ItemArg )												return FALSE;

					m_aUseWeaponEndurance[ i ]		= 100;
				}

				// 내구도를 소비하지 않고 체크만 합니다.
				if( FALSE == bUsedItem )			continue;

				// 공룡이 사용되는 모드일경우 공룡이면 무기를 소모하지 않음
				if( (m_pRoom->m_Info._i32StageType == STAGE_MODE_ESCAPE) ||	(m_pRoom->m_Info._i32StageType == STAGE_MODE_CROSSCOUNT) )
				{
					if( m_pRoom->m_Info._MiDinoInfo.IsDinoIdx( (UINT8)m_i32SlotIdx ) )
					{
						continue;
					}
				}
				
				UINT8 ui8ItemFlag = 0;
				switch( i )
				{
				case CHAR_EQUIPMENT_WEAPON_THROWING2:								// 주, 보조, 칼, 투척 순이기때문에 칼 다음에 수류탄이 없습니다. 예외처리입니다.
					ui8ItemFlag = CHAR_EQUIPMENT_WEAPON_THROWING2;
					break;
				default:
					ui8ItemFlag = CHAR_EQUIPMENT_WEAPON_THROWING1;
					break;
				}
				// ui8WeaponFlag 는 방에서 활성화 된 무기입니다. ( 0x01111111, 0, 샷건, 저격, RPG7, 주, 보조, 칼, 투척 )
				if( ui8WeaponFlag & (0x1 << (ui8ItemFlag-i)) )
				{
					UINT8 ui8Type = USED_ITEM_TYPE_NONE;
					switch( pItem->_ui8AuthExpireType )
					{
					case ITEM_EXPIRE_LEAVE:
						{
							switch( g_pContextMain->GetContentsVersion( CONTENTS_REPARE_DURABILITY ) )
							{
							case REPARE_DURABILITY_VER_1:
								{
									ui8Type		= USED_ITEM_TYPE_ONE;
								}
								break;
							case REPARE_DURABILITY_VER_2:
								{
									ui8Type		= USED_ITEM_TYPE_ENDURANCE;
								}
								break;
							}
						}
						break;
					case ITEM_EXPIRE_DELETE:
						{
							ui8Type			= USED_ITEM_TYPE_ONE;
						}
						break;
					}
					m_aEnduranceUsedWeapon[i].UseStart( ui8Type, aWeaponInven[i]._TItemWareDBIdx, i3ThreadTimer::GetServerTime() );
				}
			}
			break;
		case ITEM_TYPE_TIME:
			{
				// 인증 되지 않은 아이템을 들고 있음
				if( ITEM_ATTR_TYPE_AUTH != aWeaponInven[i]._ui8ItemType )
				{
					g_pLog->WriteLogSession( this, L"[CheckWeapon] Item is not authorized. UID : %I64d / ItemID : %d / EquipIdx : %d", m_i64UID, aWeaponInven[i]._ui32ItemID, i );
					return -6;
				}

				// 기간이 지난 무기를 들고 있는지 체크
				if( ui32InvenTime > aWeaponInven[i]._ui32ItemArg )
				{	// 레디룸 입장후에 아이템 기간이 만료되었으면 나올 수 있습니다. 로그 남기지 않음.
					return -7;
				}
				
				m_aUseWeaponEndurance[ i ] = 100;
			}
			break;
		case ITEM_TYPE_ETERNITY:
			{
				m_aUseWeaponEndurance[ i ] = 100;
			}
			break;
		}
	}

	return TRUE;
}

void CUserSession::_SetWeaponEquip(	INVEN_BUFFER* aWeaponInven, INT32 i32CheckSkipFlag )
{
	for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT ; i++ )
	{
		if( i32CheckSkipFlag & (0x01<<i) )		continue;
		m_aTBattleEquipWeapon[i].m_TItemDBIdx	= aWeaponInven[i]._TItemWareDBIdx;
		m_aTBattleEquipWeapon[i].m_TItemID		= aWeaponInven[i]._ui32ItemID;
	}
}

INT32 CUserSession::_CheckCharEquip(	BOOL bUsedItem, UINT32 ui32Team, ITEM_INFO* aCharaList, INVEN_BUFFER* aCharaInven, UINT32 ui32InvenTime, INT32 i32CheckSkipFlag )		
{
	// vv3 - check
	i3mem::FillZero( aCharaInven, sizeof( INVEN_BUFFER )*CHAR_EQUIPMENT_PARTS_COUNT ); 

	CShop* pShop = g_pContextMain->GetShop();

	CHAR_EQUIPMENT	eEquipmentSlot = CHAR_EQUIPMENT_PRIMARY;

	for( INT32 i = 0 ; i < CHAR_EQUIPMENT_PARTS_COUNT ; i++ )
	{
		if( (i32CheckSkipFlag & (0x01<<i) ) )
		{	// 체크 스킵. 변경 없음.
			aCharaInven[i]._TItemWareDBIdx		= aCharaList[i].m_TItemDBIdx;
			aCharaInven[i]._ui32ItemID			= aCharaList[i].m_TItemID;
			continue;
		}

		if( i > CHAR_EQUIPMENT_PARTS_CHARA && aCharaList[i].m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN )
		{	// 기본 파츠
			aCharaInven[i]._TItemWareDBIdx		= aCharaList[i].m_TItemDBIdx;
			aCharaInven[i]._ui32ItemID			= aCharaList[i].m_TItemID;
			continue;
		}

		if( 0 == aCharaList[i].m_TItemDBIdx || 0 == aCharaList[i].m_TItemID )
		{
			continue;
		}

		INVEN_BUFFER* pInvenBuffer = m_Inventory.GetItemByWareDBIdx( aCharaList[i].m_TItemDBIdx );
		if( NULL == pInvenBuffer )
		{
			g_pLog->WriteLogSession( this, L"[CheckEquip] DBidx is not exist. UID : %I64d / ItemDBIdx : %d / EquipIdx : %d", m_i64UID, aCharaList[i].m_TItemDBIdx, i );
			return -1;
		}

		if( pInvenBuffer->_ui32ItemID != aCharaList[i].m_TItemID ) 
		{
			g_pLog->WriteLogSession( this, L"[CheckEquip] ItemID is not same. UID : %I64d / ItemDBIdx : %d / ItemID(Inv) : %d / ItemID(Eqp) : %d / EquipIdx : %d", m_i64UID, aCharaList[i].m_TItemDBIdx, pInvenBuffer->_ui32ItemID, aCharaList[i].m_TItemID, i );
			return -2;
		}

		i3mem::Copy( &aCharaInven[i], pInvenBuffer, sizeof( INVEN_BUFFER ) );

		m_aEnduranceUsedChara[ i ].UseStop();
		eEquipmentSlot	= (CHAR_EQUIPMENT)(CHAR_EQUIPMENT_WEAPON_COUNT+i);

		// 슬롯에 맞지 않는 무기를 장착하고 있습니다.
		if( FALSE == m_pDesign->CheckItemPosition( aCharaInven[i]._ui32ItemID, eEquipmentSlot ))
		{
			g_pLog->WriteLogSession( this, L"[CheckEquip] EquipSlot is not match. UID : %I64d / ItemID : %d / EquipIdx : %d", m_i64UID, aCharaInven[i]._ui32ItemID, i );
			return -3;
		}

		if( ITEM_ATTR_TYPE_AUTH < aCharaInven[i]._ui8ItemType )
		{	// 밀어넣기 아이템일 더이상 계산하지 않으며 
			continue;
		}

		SHOP_ITEM_BASE*	pItem = pShop->GetItemData( aCharaInven[i]._ui32ItemID ); 
		if( pItem == NULL )
		{	// 오류 체크를 위해 추가
			g_pLog->WriteLogSession( this, L"[CheckEquip] ItemID is not exist. UID : %I64d / ItemID : %d / EquipIdx : %d", m_i64UID, aCharaInven[i]._ui32ItemID, i );
			return -4;
		}

		switch( pItem->_ui8AuthType )
		{
		case ITEM_TYPE_COUNT:
			{
				if( FALSE == bUsedItem )			break;

				UINT8 ui8Type = USED_ITEM_TYPE_NONE;
				INT8 i8UsedCharaSlot = (INT8)(eEquipmentSlot - CHAR_EQUIPMENT_WEAPON_COUNT);
				switch( pItem->_ui8AuthExpireType )
				{
				case ITEM_EXPIRE_LEAVE:
					{
						switch( g_pContextMain->GetContentsVersion( CONTENTS_REPARE_DURABILITY ) )
						{
						case REPARE_DURABILITY_VER_1:
							{
								ui8Type		= USED_ITEM_TYPE_ONE;
							}
							break;
						case REPARE_DURABILITY_VER_2:
							{
								ui8Type		= USED_ITEM_TYPE_ENDURANCE;
							}
							break;
						}
					}
					break;
				case ITEM_EXPIRE_DELETE:
					{
						ui8Type			= USED_ITEM_TYPE_ONE;
					}
					break;
				}
				m_aEnduranceUsedChara[i8UsedCharaSlot].UseStart(ui8Type, aCharaInven[i]._TItemWareDBIdx, i3ThreadTimer::GetServerTime());
				//_SetUsedItem( pInvenItem->_i64ItemWareDBIdx, pInvenItem->_ui32ItemID, USED_ITEM_TYPE_ONE, 1 );
			}
			break;
		case ITEM_TYPE_TIME:
			{
				if( ITEM_ATTR_TYPE_AUTH != aCharaInven[i]._ui8ItemType )
				{
					g_pLog->WriteLogSession( this, L"[CheckEquip] Item is not authorized. UID : %I64d / ItemID : %d / EquipIdx : %d", m_i64UID, aCharaInven[i]._ui32ItemID, i );
					return -5;
				}

				if( ui32InvenTime > aCharaInven[i]._ui32ItemArg )
				{
					// 레디룸 입장후에 아이템 기간이 만료되었으면 나올 수 있습니다. 로그 남기지 않음.
					//i3String::Format( strErrorLog, MAX_PATH, "[CheckEquip] Item is expired. UID : %I64d / ItemID : %d", m_i64UID, aCharaInven[i]._ui32ItemID );
					//g_pLog->WriteLog(g_i32TotalLogIdx, this, strErrorLog );
					return -6;
				}
			}
			break;
		}
	}

	return TRUE;
}

void CUserSession::_SetCharEquip(	INVEN_BUFFER* aCharaInven, INT32 i32CheckSkipFlag )
{
	for( INT32 i = 0 ; i < CHAR_EQUIPMENT_PARTS_COUNT ; i++ )
	{
		if( i32CheckSkipFlag & (0x01<<i) )		continue;

		m_aTBattleEquipParts[i].m_TItemDBIdx	= aCharaInven[i]._TItemWareDBIdx;
		m_aTBattleEquipParts[i].m_TItemID		= aCharaInven[i]._ui32ItemID;
	}
}

void CUserSession::_SetUsedItem( T_ItemDBIdx TItemWareDBIdx, UINT32 ui32ItemID, UINT8 ui8Type, UINT32 ui32UsedDurablity, UINT8 ui8CheckSlotInfo )
{
	UINT16 ui16Idx = m_ui16UsedItemCount;
	for( UINT16 i = 0 ; i < m_ui16UsedItemCount ; i++ ) 
	{
		if( m_aUsedItemList[ i ].m_TUsedItemDBIdx == TItemWareDBIdx )
		{
			ui16Idx = i;
			break;
		}
	}
	// 새로 추가해야 하고, 크래쉬 검사합니다.
	if( ui16Idx == m_ui16UsedItemCount )
	{
		if( MAX_INVEN_COUNT > m_ui16UsedItemCount )
		{
			m_ui16UsedItemCount++;

			switch( GET_USED_CHECK_TYPE(ui8CheckSlotInfo) )
			{
			case USED_CHECK_WEAPON:
				if( GET_USED_CHECK_SLOT(ui8CheckSlotInfo) >= CHAR_EQUIPMENT_WEAPON_COUNT ) break;
				m_aui8WeaponUseCount[ GET_USED_CHECK_SLOT(ui8CheckSlotInfo) ]++;
				break;
			case USED_CHECK_CHARA:
				// vv3 - check
				if( GET_USED_CHECK_SLOT(ui8CheckSlotInfo) >= CHAR_EQUIPMENT_PARTS_COUNT ) break;
				m_aui8CharaUseCount[ GET_USED_CHECK_SLOT(ui8CheckSlotInfo) ]++;
				break;
			}
		}
		else
		{
			// 더 이상 추가 할 수 없습니다. 로그 박고 리턴
			return;
		}
	}
	
	m_aUsedItemList[ ui16Idx ].m_TUsedItemDBIdx			= TItemWareDBIdx;
	m_aUsedItemList[ ui16Idx ].m_ui32ItemID				= ui32ItemID;
	m_aUsedItemList[ ui16Idx ].m_ui8Type				= ui8Type;
	m_aUsedItemList[ ui16Idx ].m_ui32UsedDurablity		+= ui32UsedDurablity;
	m_aUsedItemList[ ui16Idx ].m_ui8CheckSlotInfo		= ui8CheckSlotInfo;
}

INT32 CUserSession::_CharChangeEquipment( PACKET_CHAR_CHANGE_EQUIP_REQ* pPacket )
{
	UINT32					ui32Check = 0;
	INT32					i32ChangeCharFlag = 0;		
	S2MO_CHAR_EQUIP_INFO	CharEquipInfo[ S2MO_MULTI_SLOT_PACKET_COUNT ];

	// 케릭터 변경 유무
	if( S2MO_MULTI_SLOT_PACKET_COUNT < pPacket->m_ui8ChangeSlotCount )
	{
		// 잘못 된 패킷이므로 실패 처리 합니다.
		return EVENT_ERROR_EQUIPMENT_CHARA_COUNT;
	}

	// 팀슬롯 변경
	INT32 i32CheckTeamCount = m_pDesign->GetActiveTeamCount();
	for( INT32 i = 0 ; i < i32CheckTeamCount ; ++i )
	{
		INT32 i32ChangeSlot = pPacket->m_ui8TeamSlot[i];
		if( FALSE == m_MultiSlot.SetTeamSlot( i32ChangeSlot, (TEAM_TYPE)i ) )
		{
			// 사용가능한 캐릭터가 아닙니다.
			return EVENT_ERROR_INVENTORY_LEAVE_MAIN_SLOT;
		}

		ChangeDataBaseInfo( SAVE_DATABASE_CHARA_COMMON );
	}

	// 공통장비 해킹 체크
	if( pPacket->m_bCommonEquipFlag )
	{
		if( FALSE == _CheckCommonEquipment( &pPacket->m_aCommonEquips, ui32Check ) )
		{
			// Common
			return EVENT_ERROR_EQUIPMENT_COMMON;
		}
	}

	// 케릭터 장비 정보 해킹체크
	for( INT32 i = 0 ; i < pPacket->m_ui8ChangeSlotCount ; i++ )
	{
		INT32 i32CharaSlotIdx = (INT32)pPacket->m_ChangeEquipInfo[i].m_ui8SlotIdx;
		if(S2MO_MULTI_SLOT_COUNT <= i32CharaSlotIdx )
		{
			continue;
		}

		if( FALSE == m_MultiSlot.IsActiveChar( i32CharaSlotIdx, MULTI_SLOT_CHECK_USING ) )
		{
			continue;
		}
		
		CharEquipInfo[ i ].SetCharEquipmentAll( pPacket->m_ChangeEquipInfo[i].m_aCharEquipment );

		i32ChangeCharFlag |= 1 << i;
	}

	if( i32ChangeCharFlag > 0 )
	{
		ITEM_INFO pOrgItemInfo;
		ITEM_INFO pCompItemInfo;

		for( INT32 CharIdx = 0 ; CharIdx < pPacket->m_ui8ChangeSlotCount; CharIdx++ )
		{
			// 1. 변경요청이 없다면 pass.
			if( ! ( i32ChangeCharFlag & (1 << CharIdx) ) )	continue;

			// 2. 파츠 슬롯간 착용 가능 여부 체크
			if( FALSE == m_pDesign->CheckItemSlot( CharEquipInfo[CharIdx].m_aCharEquipment, CharIdx ) ) 
			{
				return EVENT_ERROR_EQUIPMENT_SLOT_TO_SLOT;	// 무기,캐릭터는 불가능.
			}

			// 3. 장비 DB Idx, ItemID 유효성 체크.
			for( INT32 EquipIdx = 0 ; EquipIdx < CHAR_EQUIPMENT_COUNT ; ++EquipIdx )
			{
				// 캐릭터는 생성/구매/삭제/만료시 장착 정보가 변경됩니다.
				if( CHAR_EQUIPMENT_CHARA == EquipIdx )  continue;

				T_ItemDBIdx	TItemDBIdx	= CharEquipInfo[CharIdx].m_aCharEquipment[EquipIdx].m_TItemDBIdx;
				T_ItemID	TItemID		= CharEquipInfo[CharIdx].m_aCharEquipment[EquipIdx].m_TItemID;

				// 1. 해당슬롯에 맞는 ItemID인지 체크.
				if( FALSE == m_pDesign->CheckItemPosition( TItemID, (CHAR_EQUIPMENT)EquipIdx ) )
				{ 
					return EVENT_ERROR_EQUIPMENT_WRONG_ITEM_ID;
				}

				if (ITEM_TYPE_BERET == GET_ITEM_TYPE(TItemID)) //호칭 베레모 장착 확인
				{
					if (FALSE == _CheckBeretTitle(TItemID))
					{
						return EVENT_ERROR_EQUIPMENT_SLOT_TO_SLOT;
					}
				}
				
				// 2. 미장착 가능 여부 체크.
				if( 0 == TItemDBIdx )
				{
					if( FALSE == m_pDesign->CheckReleaseEquip( (CHAR_EQUIPMENT)EquipIdx ) )
					{
						return EVENT_ERROR_EQUIPMENT_WRONG_DB_IDX;
					}
					continue;
				}

				// 3. 인벤토리에 존재하는지 체크.
				INVEN_BUFFER* pInvenBuffer = m_Inventory.GetItemByWareDBIdx( TItemDBIdx );
				if( NULL == pInvenBuffer )
				{ 
					return EVENT_ERROR_EQUIPMENT_NOT_EXIST_INVENTORY;

					//if( EquipIdx <= CHAR_EQUIPMENT_CHARA)	
					//{
					//	return EVENT_ERROR_EQUIPMENT_WRONG_DB_IDX; 
					//}
					//if( !( TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN ) )
					//{
					//	return EVENT_ERROR_EQUIPMENT_WRONG_DB_IDX; 
					//}
					//if( TItemID < DEFAULT_PARTS_ITEM_ID_MIN )	
					//{
					//	return EVENT_ERROR_EQUIPMENT_WRONG_DB_IDX; 
					//}

					//// 기본 지급 파츠는 통과.
					//continue;	
				}

				// d
				if( pInvenBuffer->_ui32ItemID != TItemID )
				{ 
					return EVENT_ERROR_EQUIPMENT_WRONG_ITEM_ID;
				}
			}

			if( TRUE == m_pDesign->CheckDuplicationParts() )
			{
				// 3. 파츠 중복 장착 체크. 소지중인 전 캐릭에 대해서 검사가 필요하므로 S2MO_MULTI_SLOT_COUNT 가 맞습니다.
				for( INT32 CompCharIdx = 0 ; CompCharIdx < S2MO_MULTI_SLOT_COUNT ; ++CompCharIdx )
				{
					// 동일 슬롯 제외.
					if( CharIdx == CompCharIdx ) continue;

					// 비 사용 슬롯 제외
					if( FALSE == m_MultiSlot.IsActiveChar( CompCharIdx, MULTI_SLOT_CHECK_USING ) ) continue; 

					for( INT32 i32Parts = 0 ; i32Parts < CHAR_EQUIPMENT_PARTS_COUNT ; ++i32Parts )
					{
						CharEquipInfo[CharIdx].GetCharEquipmentInfo( i32Parts + CHAR_EQUIPMENT_WEAPON_COUNT, &pOrgItemInfo );
						CharEquipInfo[CompCharIdx].GetCharEquipmentInfo( i32Parts + CHAR_EQUIPMENT_WEAPON_COUNT, &pCompItemInfo );

						// 장착 하지 않는게 가능한 파츠면 패스.
						if( 0 == pOrgItemInfo.m_TItemDBIdx ) continue;
						if( 0 == pCompItemInfo.m_TItemDBIdx ) continue;

						// 기본 파츠면 패스.
						if( pOrgItemInfo.m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN ) continue;
						if( pCompItemInfo.m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN ) continue;

						if( pOrgItemInfo.m_TItemDBIdx == pCompItemInfo.m_TItemDBIdx ) 
						{ 
							return EVENT_ERROR_EQUIPMENT_DUPLICATE_PARTS; 
						}
					}
				}
			}
		}
	}

	// 아이템 세팅
	if( pPacket->m_bCommonEquipFlag )
	{
		m_MultiSlot.SetCommonEquipmentAll( &pPacket->m_aCommonEquips );
		ChangeDataBaseInfo( SAVE_DATABASE_CHARA_COMMON );
	}

	if( i32ChangeCharFlag > 0 )
	{ 
		ITEM_INFO ItemInfo[ CHAR_EQUIPMENT_COUNT ];
		for( INT32 CharIdx = 0 ; CharIdx < pPacket->m_ui8ChangeSlotCount; CharIdx++ )
		{
			if( ! ( i32ChangeCharFlag & (1 << CharIdx) ) )	continue;

			INT32 i32SlotIdx = (INT32)pPacket->m_ChangeEquipInfo[CharIdx].m_ui8SlotIdx;

			CharEquipInfo[ CharIdx ].GetCharEquipmentAll( ItemInfo );

			for( INT32 k = 0 ; k < CHAR_EQUIPMENT_COUNT; ++k )
			{	
				// 캐릭터는 생성/구매/삭제/만료시 장착 정보가 변경됩니다.
				if( CHAR_EQUIPMENT_CHARA == k )  continue;

				m_MultiSlot.SetCharEquipment( i32SlotIdx, k, ItemInfo[k].m_TItemDBIdx, ItemInfo[k].m_TItemID );
			}

			ChangeCharaDataBaseInfo( ( (UINT64)0x01 << i32SlotIdx) );
		}
	}

	return EVENT_ERROR_SUCCESS;
}

//장착된 총기, 캐릭터가 유효한지를 검사합니다. 
//0x00000010	//Equip Character
//0x00000020	//Equip Weapon
INT32 CUserSession::_CheckEquipment(void)
{
	INT32	i32Rv = 0; 
	UINT32	ui32CheckList; 
	T_ItemDBIdx TItemDBIdx;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//장착된 캐릭터를 검사합니다.
	S2MO_CHAR_BASE_INFO		CharBaseInfo;
	S2MO_CHAR_EQUIP_INFO	CharEquipInfo;
	T_ItemID*				pDefaultWeapon;
	//ITEM_INFO*				pDefaultEquip;

	for( INT32 i32SlotIdx = 0 ; i32SlotIdx < S2MO_MULTI_SLOT_COUNT ; i32SlotIdx++ )
	{
		if( FALSE == m_MultiSlot.IsActiveChar( i32SlotIdx, MULTI_SLOT_CHECK_USING ) )	continue;

		m_MultiSlot.GetCharBaseInfo( i32SlotIdx, &CharBaseInfo );
		m_MultiSlot.GetCharEquipInfo( i32SlotIdx, &CharEquipInfo );

		ui32CheckList = 0;
		if( FALSE == _CheckCharEquipment( i32SlotIdx, &CharEquipInfo, ui32CheckList ) )
		{	// 바뀐 아이템이 있다면 Default 아이템으로 변경해 줍니다.

			// 1. 무기
			pDefaultWeapon = g_pContextMain->GetDefaultCharWeaponSetting( m_ui8LocaleCode );
			if( NULL == pDefaultWeapon )	return INVENTORY_FLAG_WEAPON;
			for( INT32 i32Equip = 0 ; i32Equip < CHAR_EQUIPMENT_WEAPON_COUNT ; i32Equip++ )
			{
				if( 0 == (ui32CheckList & (0x1 << i32Equip) ) )
				{
					g_MakeBasicItemDBIdx( TItemDBIdx, pDefaultWeapon[ i32Equip ] );
					m_MultiSlot.SetCharEquipment( i32SlotIdx, i32Equip,	TItemDBIdx, pDefaultWeapon[ i32Equip ] );
				}
			}
			 
			if (0 == (ui32CheckList & (0x1 << CHAR_EQUIPMENT_CHARA)))
			{	
				// 기본 캐릭으로 임시 변경합니다.
				T_ItemDBIdx TCharaDBIdx = 0;
				T_ItemID TCharaItemID = g_TCharEquipBasic[i32SlotIdx % TEAM_COUNT];
				g_MakeBasicItemDBIdx(TCharaDBIdx, TCharaItemID);
				m_MultiSlot.SetCharEquipment(i32SlotIdx, CHAR_EQUIPMENT_CHARA, TCharaDBIdx, TCharaItemID);

				/*ChangeDataBaseInfo(SAVE_DATABASE_CHARA_COMMON);
				ChangeCharaDataBaseInfo(((UINT64)0x01 << i32SlotIdx));*/
				i32Rv |= INVENTORY_FLAG_CHARACTER;
				continue;
			}

			// 3. 파츠
			m_pDesign->EquipDefaultParts( this, i32SlotIdx, ui32CheckList );

			ChangeCharaDataBaseInfo( ( (UINT64)0x01 << i32SlotIdx) );
			i32Rv |= INVENTORY_FLAG_CHARACTER;
		}
	}

	ITEM_INFO aCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];
	m_MultiSlot.GetCommonEquipmentAll( aCommonEquips );
	if( FALSE == _CheckCommonEquipment( aCommonEquips, ui32CheckList ) )
	{
		for( INT32 i32Equip = 0 ; i32Equip < CHAR_EQUIPMENT_COMMON_COUNT ; i32Equip++ )
		{
			if( 0 == (ui32CheckList & (0x1 << i32Equip) ) )
			{
				g_MakeBasicItemDBIdx( TItemDBIdx, g_pContextMain->m_TBasicCommonEquip[ m_ui8LocaleCode ][ i32Equip ] );
				m_MultiSlot.SetCommonEquipment( (CHAR_EQUIPMENT_COMMON)i32Equip, TItemDBIdx, g_pContextMain->m_TBasicCommonEquip[ m_ui8LocaleCode ][ i32Equip ] );
			}
		}

		ChangeDataBaseInfo( SAVE_DATABASE_CHARA_COMMON );
		i32Rv |= INVENTORY_FLAG_CHARACTER;
	}
		
	return i32Rv; 
}

//INT32 CUserSession::_CheckEquipment(void)
//{
//	// 케릭터의 모든 장비 얻어오기
//
//	// 슬롯에 맞는 장비인지 체크
//	// 인벤에 있는지 체크
//	// 기간 만료 템인지 체크
//	// 케릭터의 장비 중복 체크
//
//	// 만약.. 문제가 있다면 default 아이템으로 세팅
//	// 문제 되는 템들은 Send
//}

BOOL CUserSession::_CheckCharEquipment( INT32 i32SlotIdx, S2MO_CHAR_EQUIP_INFO* pCharEquipInfo, UINT32& ui32Check )
{
	UINT32 ui32OrgCheckFlag = 0;
	ITEM_INFO	ItemInfo;
	INVEN_BUFFER*	pInven;
	for( INT32 i = 0 ; i < CHAR_EQUIPMENT_COUNT ; i++ )
	{
		ui32OrgCheckFlag |= (0x1 << i);

		pCharEquipInfo->GetCharEquipmentInfo( i, &ItemInfo );	

		switch( i )
		{
		case CHAR_EQUIPMENT_PRIMARY:
			if( false == g_pContextMain->IsContentsEnabled( CONTENTS_DEFAULT_WEAPON ) )
			{	// 주무기 없이 가는 버전, 주무기가 0일때 주무기 세팅 없어도 True 입니다.
				ui32Check |= (0x1 << i);
				continue;
			}
			break;
		case CHAR_EQUIPMENT_HEAD:
		case CHAR_EQUIPMENT_FACE:
		//case CHAR_EQUIPMENT_SKIN:
		case CHAR_EQUIPMENT_BERET:
			if( 0 == ItemInfo.m_TItemDBIdx )
			{	// 착용 안할 수도 있습니다.
				ui32Check |= (0x1 << i);
				continue;
			}
			break;
		}

		// 기본 캐릭터는 인벤토리에 넣지 않으므로 Pass합니다.
		if( ItemInfo.m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN && i == CHAR_EQUIPMENT_CHARA )
		{
			ui32Check |= (0x1 << i);
			continue;
		}

		// DB Idx 검사.
		pInven = m_Inventory.GetItemByWareDBIdx( ItemInfo.m_TItemDBIdx );
		if( NULL == pInven )																
		{
			continue;
		}
		if( pInven->_ui32ItemID != ItemInfo.m_TItemID )
		{
			continue; 
		}
		if( FALSE == _CheckItemAuth( pInven ) )									
		{
			continue;
		}
		if( FALSE == m_pDesign->CheckItemPosition( pInven->_ui32ItemID, (CHAR_EQUIPMENT)i ) )
		{
			continue;
		}

		ui32Check |= (0x1 << i);
	}

	// 파츠 슬롯간 장착 가능 여부 체크
	ITEM_INFO aParts[ CHAR_EQUIPMENT_COUNT ];
	m_MultiSlot.GetCharEquipmentAll( i32SlotIdx, aParts );
	if( FALSE == m_pDesign->CheckItemSlot( aParts, i32SlotIdx ) )
	{
		return FALSE;	
	}

	if( ui32OrgCheckFlag == ui32Check )	return TRUE;

	return FALSE;
}

BOOL CUserSession::_CheckCommonEquipment( ITEM_INFO* pEquipment, UINT32& ui32Check )
{
	UINT32 ui32OrgCheckFlag = 0;
	ui32Check = 0;
	INVEN_BUFFER* pInven;
	for( INT32 i = 0 ; i < CHAR_EQUIPMENT_COMMON_COUNT ; i++ )
	{
		ui32OrgCheckFlag |= (0x1 << i);

		pInven = m_Inventory.GetItemByWareDBIdx( pEquipment[i].m_TItemDBIdx );
		if( NULL == pInven )																
		{
			continue;
		}
		
		if(  pInven->_ui32ItemID != pEquipment[ i ].m_TItemID )
		{
			// 기본 공룡은 인벤토리에 넣지 않으므로 Pass합니다.
			if (pEquipment[i].m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN )
			{
				pEquipment[i].m_TItemID = pInven->_ui32ItemID; 
			}
			else
			{
				continue;
			}
		}
		
		if (FALSE == _CheckItemAuth(pInven))
		{
			continue;
		}
		if (FALSE == _CheckItemPositionCommon(pInven->_ui32ItemID, (CHAR_EQUIPMENT_COMMON)i))
		{
			continue;
		}
	 
		ui32Check |= (0x1 << i);
	}

	if( ui32OrgCheckFlag == ui32Check )	return TRUE;

	return FALSE;
}

BOOL CUserSession::_CheckItemAuth( INVEN_BUFFER* pInvenItem )
{
	if( ITEM_ATTR_TYPE_BASIC <= pInvenItem->_ui8ItemType )					return TRUE;

	CShop* pShop = g_pContextMain->GetShop();

	SHOP_ITEM_BASE*	pItem = pShop->GetItemData( pInvenItem->_ui32ItemID );  
	if( NULL == pItem )														return FALSE;

	switch( pItem->_ui8AuthType )
	{
	case ITEM_TYPE_COUNT:
		{
			if( ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType )			return TRUE;		// 내구도 무기는 개수를 체크하지 않는다.
			else if( 0 == pInvenItem->_ui32ItemArg )						return FALSE;		// 개수제일 경우 0이면 에러
		}
		break;
	case ITEM_TYPE_TIME:
		{
			if( ITEM_ATTR_TYPE_AUTH != pInvenItem->_ui8ItemType )			return FALSE;		// 미인증 아이템
			if( i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() > pInvenItem->_ui32ItemArg ) return FALSE;		// 기간 체크
		}
		break;
	}

	return TRUE;
}

//BOOL CUserSession  ::_CheckItemPosition( UINT32 ui32ItemID, CHAR_EQUIPMENT eSlot )
//{
//	if( 0 == ui32ItemID )	return TRUE;
//	if( eSlot < CHAR_EQUIPMENT_PRIMARY || eSlot >= CHAR_EQUIPMENT_COUNT )	return TRUE;
//
//	ITEM_TYPE eItemType = ITEM_TYPE(GET_ITEM_TYPE( ui32ItemID ));	// ui32ItemID의 ITEM_TYPE
//	ITEM_TYPE eEquipType;	// eSlot에 해당하는 ITEM_TYPE
//	INT32 i32Gap = 1;		// ITEM_TYPE 과 CHAR_EQUIPMENT 간의 차이.
//	
//	if( eSlot > CHAR_EQUIPMENT_THROWING2 )	i32Gap = 1; 
//	eEquipType = (ITEM_TYPE)(eSlot+i32Gap);
//
//	if( CHAR_EQUIPMENT_BERET == eSlot )
//	{
//		eEquipType = 
//	}
//	
//	if( eEquipType != eItemType )
//	{
//		return FALSE;
//	}
//
//	return TRUE;
//}
//

BOOL CUserSession::_CheckItemPositionCommon( UINT32 ui32Item, CHAR_EQUIPMENT_COMMON eSlot )
{
	switch( eSlot )
	{
	case CHAR_EQUIPMENT_COMMON_DINO:		if( ITEM_TYPE_DINO != GET_ITEM_TYPE( ui32Item ) )		return FALSE;		break;
	default: return FALSE;
	}

	return TRUE; 
}


T_RESULT CUserSession::_CheckCreateChar( PACKET_CHAR_CREATE_CHARA_REQ* pCreateChar )
{
	if (ITEM_TYPE_CHARA != GET_GOODS_TYPE(pCreateChar->m_TGoodsID) ) return EVENT_ERROR_EVENT_BUY_GOODS_INVALID_DATA;

	T_RESULT TResult		= EVENT_ERROR_SUCCESS; 

	TResult = m_pDesign->CheckBuyChara( pCreateChar->m_ui8CreateType, this, pCreateChar->m_TGoodsID ) ;
	if( EV_FAILED( TResult ) )	return TResult;

	BUY_PRICE	stBuyPrice;
	i3mem::FillZero( &stBuyPrice, sizeof(BUY_PRICE));	// 없으면 안됨.
	stBuyPrice.m_ePriceType = PRICE_TYPE_SHOP_REAL;
	UINT8 ui8CreateType = pCreateChar->m_ui8CreateType;
	stBuyPrice.m_eBuyType = (SIA_GOODS_BUY)ui8CreateType;

	CShop* pShop = g_pContextMain->GetShop();
	SHOP_GOODS_EXPANSION* pGoods	= pShop->GetGoodsData( pCreateChar->m_TGoodsID );

	TResult = _CheckBuyGoods( pGoods, &stBuyPrice ); 
	if( EV_FAILED( TResult ) )
	{
		return TResult;
	}		

	T_ItemDBIdx TCharaDBIdx;

	g_MakeBasicItemDBIdx( TCharaDBIdx, pCreateChar->m_TGoodsID / 100 );

	if( FALSE == g_pModuleTrans->AuthSendCreateChar( m_WorkThreadIdx, this, pCreateChar, TCharaDBIdx ) )
	{
		return EVENT_ERROR_EVENT_TRANS;
	}

	// 포인트 복사 방지를 위해 먼저 차감 합니다. 구매 실패시 복구함.
	if( m_UserInfoBasic.m_ui32Point > stBuyPrice.m_ui32Price[GOODS_BUY_POINT] )
	{
		m_UserInfoBasic.m_ui32Point	-= stBuyPrice.m_ui32Price[GOODS_BUY_POINT];
	}
	else m_UserInfoBasic.m_ui32Point = 0;

	ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );

	return EVENT_ERROR_SUCCESS;
}

BOOL CUserSession::_ChangeUserTitle( UINT8 ui8UserTitleId, LOG_GET_TITLE* pstLogGetTitle )
{
	// 교환 조건 테이블
	const USER_TITLE_REQUIREMENT* pRequirement = &g_UserTitleRequirement[ui8UserTitleId];

	// 계급 낮으면 불가	
	if( m_UserInfoBasic.m_ui32Rank < pRequirement->_rank )
	{
		return FALSE;
	}

	// 이전 호칭 없으면 불가
	for( INT32 i = 0 ; i < USER_TITLE_PRIORITY_COUNT ; i++ )
	{
		UINT64 ui64TitleId = pRequirement->_userTitleId[i];

		if( ( 0 < ui64TitleId ) && (0 == GET_USERTITLE( m_ui64GainUserTitle, ui64TitleId )))
		{
			return FALSE;
		}
	}
	
	// 교환
	pstLogGetTitle->Reset();
	for(INT32 i = 0; i < USER_TITLE_REQUIREMENT_COUNT; i++)
	{
		switch( pRequirement->_ribbon[i]._type )
		{
		case UTR_RIBBON:
			pstLogGetTitle->_i32Miniature	+= pRequirement->_ribbon[i]._count;
			break;
		case UTR_ENSIGN:
			pstLogGetTitle->_i32Insignia	+= pRequirement->_ribbon[i]._count;
			break;
		case UTR_MEDAL:
			pstLogGetTitle->_i32Order		+= pRequirement->_ribbon[i]._count;
			break;
		case UTR_MASTER:
			pstLogGetTitle->_i32Master		+= pRequirement->_ribbon[i]._count;
			break;
		}
	}

	if( pstLogGetTitle->_i32Miniature	> (INT32)m_ui32Miniature )	return FALSE;
	if( pstLogGetTitle->_i32Insignia	> (INT32)m_ui32Insignia )	return FALSE;
	if( pstLogGetTitle->_i32Order		> (INT32)m_ui32Order )		return FALSE;
	if( pstLogGetTitle->_i32Master		> (INT32)m_ui32Master )		return FALSE;		

	// 표장을 소모한다.
	m_ui32Miniature	-= pstLogGetTitle->_i32Miniature;
	m_ui32Insignia	-= pstLogGetTitle->_i32Insignia;
	m_ui32Order		-= pstLogGetTitle->_i32Order;
	m_ui32Master	-= pstLogGetTitle->_i32Master;

	// 호칭 획득
	m_ui64GainUserTitle = SET_USERTITLE( m_ui64GainUserTitle, ui8UserTitleId );
	return TRUE;
}

void CUserSession::CheckEndurance()
{
	for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT ; i++ )
	{
		if( false == m_aEnduranceUsedWeapon[i].m_bUse )	continue;

		_SetUsedItem( m_aEnduranceUsedWeapon[i].m_TWareDBIdx, m_aTBattleEquipWeapon[i].m_TItemID, m_aEnduranceUsedWeapon[i].m_ui8Type, i3ThreadTimer::GetServerTime() - m_aEnduranceUsedWeapon[i].m_ui32ATUsed, (UINT8)MAKE_USED_CHECK_SLOTINFO(USED_CHECK_WEAPON, i) );
		m_aEnduranceUsedWeapon[i].m_ui32ATUsed = i3ThreadTimer::GetServerTime();
	}

	for( INT32 i = 0; i < CHAR_EQUIPMENT_PARTS_COUNT; i++ )
	{
		if( false == m_aEnduranceUsedChara[i].m_bUse )	continue;

		// vv3 - check
		_SetUsedItem( m_aEnduranceUsedChara[i].m_TWareDBIdx, m_aTBattleEquipParts[i].m_TItemID, m_aEnduranceUsedChara[i].m_ui8Type, i3ThreadTimer::GetServerTime() - m_aEnduranceUsedChara[i].m_ui32ATUsed, (UINT8)MAKE_USED_CHECK_SLOTINFO(USED_CHECK_CHARA, i) );
		m_aEnduranceUsedChara[i].m_ui32ATUsed = i3ThreadTimer::GetServerTime();
	}
}

void CUserSession::CharUseAllStop()
{
	CheckEndurance();
	// vv3 - check
	for( INT32 i = 0; i < CHAR_EQUIPMENT_PARTS_COUNT ; i++ )
	{
		if( false == m_aEnduranceUsedChara[i].m_bUse )	continue;

		m_aEnduranceUsedChara[i].UseStop();
	}
}

REAL32	CUserSession::_GetRepairPremium( UINT32 ui32Endurance )
{
	if( 90 < ui32Endurance )		return 0.f;
	else if( 10 < ui32Endurance )	return 0.1f;
	else							return 0.25f;
}

bool CUserSession::_PacketDecript( i3NetworkPacket* pPacket )
{
	INT16 i16Value;
	pPacket->ReadData( &i16Value, sizeof(INT16) );

	m_i16PacketRandSeed = (((m_i16PacketRandSeed * 214013L + 2531011L) >> 16) & 0x7fff);
	if( m_i16PacketRandSeed != i16Value )		return false;

	return true;
}
void CUserSession::CheckQuestItem2()
{
	if( FALSE == g_pContextMain->IsContentsEnabled(CONTENTS_MISSION_CARD)) return;
	// 2011. 04. 07 - 수정 서동권
	UINT8	ui8Use			= m_QuestIngInfo.m_ActiveIndexUse;							// 진행중인 카드셋
	UINT8	ui8CardType		= m_QuestIngInfo.m_pCardSetType[ui8Use];					// 카드 타입
	UINT8	ui8CardSet		= m_QuestIngInfo.m_pActiveIndexCardSet[ui8Use];				// 카드셋에서 진행중인 카드	

	if( ui8CardType == 0 )				return;	// 임무카드가 없다.

	if( ui8CardType >= QUEST_CARDSET_TYPE_COUNT )
	{
		I3TRACE("CardType is wrong. UID:%d, ActiveIng:%d, %d/%d/%d/%d, %d/%d/%d/%d\n", m_i64UID, m_QuestIngInfo.m_ActiveIndexUse, m_QuestIngInfo.m_pCardSetType[0], m_QuestIngInfo.m_pCardSetType[1], m_QuestIngInfo.m_pCardSetType[2], m_QuestIngInfo.m_pCardSetType[3], m_QuestIngInfo.m_pActiveIndexCardSet[0], m_QuestIngInfo.m_pActiveIndexCardSet[1], m_QuestIngInfo.m_pActiveIndexCardSet[2], m_QuestIngInfo.m_pActiveIndexCardSet[3] );
		return;
	}

	// 이미 완료된 임무카드라면 return.
	if( 0xFFFF == m_QuestIngInfo.m_pActiveIndexCard[ui8Use][ui8CardSet] )	return;

	QUEST_CTX*				pQuest			= NULL;
	QUEST_ITEM_INFO_NEW*	pItemInfo		= NULL;
	QUEST_ITEM_INFO_NEW*	pLastItemInfo		= NULL; // 최종 보상 아이템 정보 2011.4.11 서동권

	switch( ui8CardType )
	{	// 현재 사용중인 임무카드를 얻어옵니다.
	case QUEST_CARDSET_TYPE_EVENT:
		pQuest		= &g_pContextMain->m_pQuestEvent[ m_ui8LocaleCode ][ ui8CardSet*MAX_QUEST_PER_CARD_NEW ];
		pItemInfo	= &g_pContextMain->m_pQuestEvent_ItemInfo[ m_ui8LocaleCode ].m_CardReward[ui8CardSet];
		pLastItemInfo = &g_pContextMain->m_pQuestEvent_ItemInfo[ m_ui8LocaleCode ].m_CompleteReward; // 최종 보상 정보 추가
		break;
	default:
		pQuest		= &g_pContextMain->m_pQuest[ ui8CardType ][ ui8CardSet*MAX_QUEST_PER_CARD_NEW ];
		pItemInfo	= &g_pContextMain->m_pQuest_ItemInfo[ ui8CardType ].m_CardReward[ui8CardSet];
		pLastItemInfo = &g_pContextMain->m_pQuest_ItemInfo[ ui8CardType ].m_CompleteReward;
		break;
	}
	// 현재 몇개까지 완료했는지 숫자 얻어오기
	UINT8* pQuestCtxActive	= &m_aaui8QuestCtxActive[ ui8Use ][ ui8CardSet*MAX_QUEST_PER_CARD_NEW ];

	if( NULL == pQuest )				return;
	if( NULL == pItemInfo )				return;
	if( NULL == pQuestCtxActive )		return;

	// 해당 줄을 완료 하였는지에 대한 체크
	UINT16	ui16NewQuestCtx = 0;
	for( INT32 i = 0 ; i < MAX_QUEST_PER_CARD_NEW ; i++ )
	{
		switch( pQuest[i].m_ID )
		{
		case 1:
		case 3:
			{	// 현재 완료한 카운트
				if( pQuest[i].m_Arg[2] > (pQuestCtxActive[i] + m_aui8QuestCtxActiveNow[i]) )
				{
					if( m_aui8QuestCtxActiveNow[i] > 0 )
					{
						// 누적 되지 않는 퀘스트이므로 
						// 이번 배틀에서 임무 수행했던 것은 배틀 시작 전 수행 회수를 클라이언트에 다시 보내 초기화 한다.
						UINT8 ui8QuestCtxActiveIdx	= (UINT8)i;
						UINT8 ui8QuestCtxActive = pQuestCtxActive[i];

						i3NetworkPacket SendPacket( PROTOCOL_BASE_QUEST_CHANGE_ACK );
						SendPacket.WriteData( &ui8QuestCtxActiveIdx, sizeof(UINT8) );		// Finish : 0xF0, Index : 0x0F
						SendPacket.WriteData( &ui8QuestCtxActive, sizeof(UINT8) );			// Count
						SendPacketMessage( &SendPacket );
					}

					continue;
				}
				pQuestCtxActive[i] = pQuest[i].m_Arg[2];
				ChangeDataBaseInfo(SAVE_DATABASE_QUEST);
			}
			break;
		case 2:	
			{	// 누적 카운트 계산
				if( m_aui8QuestCtxActiveNow[i] )
				{
					pQuestCtxActive[i]		+= m_aui8QuestCtxActiveNow[i];
					ChangeDataBaseInfo(SAVE_DATABASE_QUEST);
				}

				if( pQuest[i].m_Arg[2] > pQuestCtxActive[i] )								continue;
				pQuestCtxActive[i] = pQuest[i].m_Arg[2];
			}
			break;
		default:	// 임무가 지정되지 않은 경우. 완료 처리 합니다.
			pQuestCtxActive[i] = pQuest[i].m_Arg[2];
			ChangeDataBaseInfo(SAVE_DATABASE_QUEST);

			UINT8 ui8QuestCtxActiveIdx = UINT8(i | 0xF0);
			UINT8 ui8QuestCtxActive	=	 0xFF;

			i3NetworkPacket SendPacket( PROTOCOL_BASE_QUEST_CHANGE_ACK );
			SendPacket.WriteData( &ui8QuestCtxActiveIdx, sizeof(UINT8) );			// Finish : 0xF0, Index : 0x0F
			SendPacket.WriteData( &ui8QuestCtxActive, sizeof(UINT8) );			// Count
			SendPacketMessage( &SendPacket );

			break;
		}

		ui16NewQuestCtx |= (0xF << (i*4));
	}

	if( m_QuestIngInfo.m_pActiveIndexCard[ui8Use][ui8CardSet] != ui16NewQuestCtx )
	{
		m_QuestIngInfo.m_pActiveIndexCard[ui8Use][ui8CardSet] = m_QuestIngInfo.m_pActiveIndexCard[ui8Use][ui8CardSet] | ui16NewQuestCtx;
		ChangeDataBaseInfo(SAVE_DATABASE_QUEST);
	}

	//if( 0xFFFF != ui16NewQuestCtx ) return;
	if( 0xFFFF != m_QuestIngInfo.m_pActiveIndexCard[ui8Use][ui8CardSet] ) return;


	// 임무카드를 모두 끝냈는지에 대한 체크
	BOOL bLastGet = TRUE;
	for(INT8 i = 0; i < MAX_CARD_PER_CARDSET; i++)
	{
		if( i == ui8CardSet ) continue;
		if( 0xFFFF != m_QuestIngInfo.m_pActiveIndexCard[ui8Use][i] )
		{
			bLastGet = FALSE;
			break;
		}
	}

	INT32 i32Rv = _LastQuestItem( pItemInfo, pLastItemInfo, bLastGet );

	//결과를 확인한다. 
	//마지막 보상으로 총기를 받으면 리스트를 삭제합니다 - 이벤트 카드셋  제외
	if( QUEST_CARDSET_TYPE_EVENT != ui8CardType && bLastGet )
	{	//마지막 카드까지 종료 했습니다. 삭제 합니다. 
		_DeleteQuestCardSet( ui8Use );
	}

	i3NetworkPacket SendPacket(PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK);
	SendPacket.WriteData(&i32Rv,		sizeof(INT32));
	SendPacket.WriteData(&ui8CardSet, sizeof(UINT8));
	if(i32Rv & GET_QUEST_EXP_POINT)
	{
		//완료된 임무카드가 있습니다. 변경된 값들을 Clinet 에게 보냅니다. 
		SendPacket.WriteData( &m_UserInfoBasic.m_ui32Exp,		sizeof(UINT32)); 
		SendPacket.WriteData( &m_UserInfoBasic.m_ui32Point,		sizeof(UINT32)); 
		SendPacket.WriteData( &m_ui32Miniature,				sizeof(UINT32));
		SendPacket.WriteData( &m_ui32Insignia,				sizeof(UINT32));
		SendPacket.WriteData( &m_ui32Order,					sizeof(UINT32));
		SendPacket.WriteData( &m_ui32Master,				sizeof(UINT32));
		SendPacket.WriteData( &m_UserInfoBasic.m_ui32Rank,		sizeof(UINT32));					//뒤에 추가함 .. 0410
	}
	SendPacketMessage( &SendPacket );

	CheckRankUp( MAIN_THREAD_IDX );

	ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO | SAVE_DATABASE_QUEST); 
}
 
INT32 CUserSession::_LastQuestItem( QUEST_ITEM_INFO_NEW* pItemInfo, QUEST_ITEM_INFO_NEW* pLastItemInfo,  BOOL bLastGet )
{
	// 2011.4.11 아이템 보상 부분 대부분 수정 하였습니다. - 서동권
	// 최종 보상에 관련하여 직접 코드로 입력하여 지급하는것이 아니라 퀘스트 파일에서 로드하여 지급하는 방식으로 바뀌었습니다.
	// 따라서 국가별로 지급하는 보상이 달라야할 경우에는 퀘스트 파일을 국가별로 만들어야 합니다.

	INT32 i32Rv = 0; 

	// 완료 된 카드인지 확인
	UINT8	ui8Use				= m_QuestIngInfo.m_ActiveIndexUse;					// 진행중인 카드셋
	INT32	i32Miniature		= 0;
	INT32	i32Insignia			= 0;
	INT32	i32Order			= 0;
	INT32	i32Master			= 0;
	INT32	i32LastMiniature	= 0;
	INT32	i32LastInsignia		= 0;
	INT32	i32LastOrder		= 0;
	INT32	i32LastMaster		= 0;

	// 카드 보상 //
	// Exp,Point
	UINT32 ui32Exp		= ( pItemInfo->m_ui32Exp );
	UINT32 ui32Point	= ( pItemInfo->m_ui32Point );
	// 메달 Get.
	 
	if( 51 > pItemInfo->m_ui32Prize && pItemInfo->m_ui32Prize > 0 )		i32Miniature++;			//약장
	else if( 101 > pItemInfo->m_ui32Prize && pItemInfo->m_ui32Prize > 50 )		i32Insignia++;			//휘장
	else if ( 119 > pItemInfo->m_ui32Prize && pItemInfo->m_ui32Prize > 100 )	i32Order++;				//훈장
	else if (  pItemInfo->m_ui32Prize > 118 )								i32Master++;			//마스터 훈장(러시아 크리스마스 임무카드, Prize = 119)

	// 카드별 임무 완료시에도 아이템을 지급하도록 하였습니다.
	UINT8 ui8ItemCount = 0;
	ST_ITEM aItem[MAX_SHOP_DB_ITEM_COUNT];
	for( int i = 0; i < MAX_QUEST_ITEM_CARD; ++i )
	{
		if( pItemInfo->m_Item[i].m_ui32ItemID != 0 )
		{
			aItem[ui8ItemCount].m_TItemID	= pItemInfo->m_Item[i].m_ui32ItemID;
			aItem[ui8ItemCount].m_TItemArg	= pItemInfo->m_Item[i].m_ui32Arg;
			ui8ItemCount++;
		}
	}
	g_pModuleTrans->AuthSendInsertItem( MAIN_THREAD_IDX, this, ITEM_INSERT_REQUEST_MISSION_CARD, ui8ItemCount,aItem );


	// 카드 10개 전부 완료시 최종 보상 //
	if( bLastGet )
	{//마지막 보상을 준다. 		
		i32Rv = i32Rv | GET_QUEST_FINISH_LAST_CARD;

		UINT8 ui8LastItemCount = 0;
		ST_ITEM aLastItem[MAX_SHOP_DB_ITEM_COUNT];

		for( int i = 0; i < MAX_QUEST_ITEM_CARD; ++i ) // 최종 보상으로 주어지는 아이템 //
		{
			if( pLastItemInfo->m_Item[i].m_ui32ItemID != 0 )
			{
				aLastItem[ui8LastItemCount].m_TItemID	= pLastItemInfo->m_Item[i].m_ui32ItemID;
				aLastItem[ui8LastItemCount].m_TItemArg	= pLastItemInfo->m_Item[i].m_ui32Arg;
				ui8LastItemCount++;

				i32Rv = i32Rv | GET_QUEST_WEAPON;
			}
		}
		g_pModuleTrans->AuthSendInsertItem( MAIN_THREAD_IDX, this, ITEM_INSERT_REQUEST_MISSION_CARD, ui8LastItemCount,aLastItem );
		
		ui32Point += pLastItemInfo->m_ui32Point;
		ui32Exp += pLastItemInfo->m_ui32Exp;

		if( 51 > pLastItemInfo->m_ui32Prize && pLastItemInfo->m_ui32Prize > 0 )	
		{
			++i32LastMiniature;		//약장
		}
		else if( 101 > pLastItemInfo->m_ui32Prize && pLastItemInfo->m_ui32Prize > 50 )
		{
			++i32LastInsignia;		//휘장
		}
		else if ( 119 > pLastItemInfo->m_ui32Prize && pLastItemInfo->m_ui32Prize > 100 )
		{
			++i32LastOrder;			//훈장
		}
		else if (  pLastItemInfo->m_ui32Prize > 118 )
		{
			++i32LastMaster;		//마스터 훈장(러시아 크리스마스 임무카드, Prize = 119)
		}
	}

	if( (0 < ui32Exp) || (0 < ui32Point) )
	{
		m_UserInfoBasic.m_ui32Exp		+= ui32Exp;
		m_UserInfoBasic.m_ui32Point		+= ui32Point;

		m_UserInfoDaily._exp		+= ui32Exp;
		m_UserInfoDaily._point		+= ui32Point;

		ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );
		i32Rv = i32Rv | GET_QUEST_EXP_POINT;

		if( 0 < ui32Exp )
		{	// 경험치를 받았기 때문에 계급 변화 체크합니다.
			CheckRankUp( MAIN_THREAD_IDX );
		}
	}

	if( 0 < i32Miniature || 0 < i32LastMiniature )
	{
		m_ui32Miniature	+= i32Miniature + i32LastMiniature;
		if( m_ui32Miniature > MEDAL_MAX_COUNT )	m_ui32Miniature = MEDAL_MAX_COUNT;
	}
	if( 0 < i32Insignia || 0 < i32LastInsignia )
	{
		m_ui32Insignia	+= i32Insignia + i32LastInsignia;
		if( m_ui32Insignia > MEDAL_MAX_COUNT )	m_ui32Insignia = MEDAL_MAX_COUNT;
	}
	if( 0 < i32Order || 0 < i32LastOrder )
	{
		m_ui32Order		+= i32Order + i32LastOrder;
		if( m_ui32Order > MEDAL_MAX_COUNT )		m_ui32Order = MEDAL_MAX_COUNT;
	}
	if( 0 < i32Master || 0 < i32LastMaster )
	{
		m_ui32Master	+= i32Master + i32LastMaster;
		if( m_ui32Master > MEDAL_MAX_COUNT )	m_ui32Master = MEDAL_MAX_COUNT;
	}
 
	LOG_MISSION stMission;
	stMission._ui8CardType		= m_QuestIngInfo.m_pCardSetType[ui8Use];
	stMission._ui8CardSet		= m_QuestIngInfo.m_pActiveIndexCardSet[ui8Use];
	stMission._ui32Exp			= ui32Exp;
	stMission._ui32Point		= ui32Point;
	stMission._i32Miniature		= i32Miniature;
	stMission._i32Insignia		= i32Insignia;
	stMission._i32Order			= i32Order;
	stMission._i32Master		= i32Master;
	stMission._ui32LastExp		= pLastItemInfo->m_ui32Exp;
	stMission._ui32LastPoint	= pLastItemInfo->m_ui32Point;
	stMission._i32LastMiniature	= i32LastMiniature;
	stMission._i32LastInsignia	= i32LastInsignia;
	stMission._i32LastOrder		= i32LastOrder;
	stMission._i32LastMaster	= i32LastMaster;
	stMission._bFinish			= bLastGet;

	for( INT32 i = 0; i < MAX_QUEST_ITEM_CARD; ++i ) // 보상 아이템 정보
	{
		stMission._ui32GetItem[i]		= pItemInfo->m_Item[i].m_ui32ItemID;
		stMission._ui32LastGetItem[i]	= pLastItemInfo->m_Item[i].m_ui32ItemID;
	}

	g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_U_QUEST_FINISH, this, &stMission );

	if( stMission._ui32Point > 0 || stMission._ui32LastPoint > 0 )
	{
		UINT32 ui32SendPoint	= stMission._ui32Point + stMission._ui32LastPoint;
		INT32 i32GetRoute		= GET_POINT_ROUTE_QUEST;
		g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_U_GET_POINT, this, &ui32SendPoint, &i32GetRoute );
	}

	return i32Rv; 
}

// -----------------------------------------------------------
// Desc : 카드를 삭제합니다. 
// -----------------------------------------------------------
void CUserSession::_DeleteQuestCardSet(UINT8 ui8CardSet)
{
	m_QuestIngInfo.m_ActiveIndexUse						= 0;
	m_QuestIngInfo.m_pActiveIndexCardSet[ui8CardSet]	= 0;
	m_QuestIngInfo.m_pCardSetType[ui8CardSet]			= QUEST_CARDSET_TYPE_NONE;

	for(INT32 i = 0; i < MAX_CARD_PER_CARDSET; i++)
	{
		m_QuestIngInfo.m_pActiveIndexCard[ui8CardSet][i]	= 0;
	}
	// 등록되어있는 퀘스트를 삭제합니다.
	i3mem::FillZero( m_aaui8QuestCtxActive[ui8CardSet],	sizeof( UINT8 ) * MAX_QUEST_PER_CARDSET_NEW );

	ChangeDataBaseInfo( SAVE_DATABASE_QUEST ); 
}

void CUserSession::_SetQuestEvent()
{
	//이벤트를 확인합니다.
	if( 0 == g_pContextMain->m_ui32QuestEventDate )
	{	//진행중인 이벤트 없음
		if( 0 == m_ui32QuestEventDate )	return;

		// 이벤트 카드를 삭제합니다.
		_DeleteQuestCardSet( EVENT_CARDSET_SLOT );

		m_ui32QuestEventDate = 0;
	}
	else
	{	//진행중인 이벤트 있음
		if( m_ui32QuestEventDate == g_pContextMain->m_ui32QuestEventDate )	return;

		g_pContextMain->GetQuest( QUEST_CARDSET_TYPE_EVENT, EVENT_CARDSET_SLOT, &m_QuestIngInfo, m_aaui8QuestCtxActive[ EVENT_CARDSET_SLOT ], m_ui8LocaleCode );

		m_ui32QuestEventDate = g_pContextMain->m_ui32QuestEventDate;

		// 이벤트 카드 보급 통보
		//SendNewEventCardSet();

		ChangeDataBaseInfo( SAVE_DATABASE_QUEST );
	}
}

void CUserSession::_SendQuestInfoPacket( INT8 i8CardSolt )
{
	if( FALSE == g_pContextMain->IsContentsEnabled(CONTENTS_MISSION_CARD)) return;
	//쿼스트 정보 보내기
	i3NetworkPacket SendPacket( PROTOCOL_BASE_QUEST_GET_INFO_ACK );
	SendPacket.WriteData( &i8CardSolt,			sizeof( INT8 ) );
	SendPacket.WriteData( &m_QuestIngInfo,		sizeof(QUESTING_INFO) );
	SendPacketMessage( &SendPacket );
	ChangeDataBaseInfo(SAVE_DATABASE_QUEST);
	return;
}

INT32 CUserSession::_SetItemAbilityAuth( INVEN_BUFFER* pBuffer )
{
	if( ITEM_TYPE_WRAP_COUNT != GET_ITEM_TYPE( pBuffer->_ui32ItemID ) )	return EVENT_ERROR_SUCCESS;

	switch( pBuffer->_ui32ItemID )
	{
	case CASHITEM_ITEMID_CLR_HISTORY:
		{	// 전적 초기화
			_ResetRecord_MAU();
			//GIP
			// 일단 초기화 정보 업데이트
			i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_MYINFO_RECORD_ACK );
			SendPacket.WriteData( &m_UserInfoRecord, sizeof(USER_INFO_RECORD) );
			SendPacketMessage( &SendPacket );
		}
		break;
	case CASHITEM_ITEMID_CLR_ESCAPE:
		{	// 이탈률 초기화
			_ResetDisconnect_MAU();

			i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_MYINFO_RECORD_ACK );
			SendPacket.WriteData( &m_UserInfoRecord, sizeof(USER_INFO_RECORD) );
			SendPacketMessage( &SendPacket );
		}
		break;
	case CASHITEM_ITEMID_CLR_SCORE:
		{	// 킬/데스 초기화
			_ResetKillDeath_MAU();
			//GIP
			// 일단 초기화 정보 업데이트
			i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_MYINFO_RECORD_ACK );
			SendPacket.WriteData( &m_UserInfoRecord, sizeof(USER_INFO_RECORD) );
			SendPacketMessage( &SendPacket );
		}
		break;
	case CASHITEM_ITEMID_CLAN_PERSON_PLUS_50:
		{	// 클랜원 증가
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)										return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if( m_UserInfoBasic.m_ui32ClanState != CLAN_MEMBER_LEVEL_MASTER )		return EVENT_ERROR_CLAN_LEVEL_MASTER;
			
			g_pModuleClan->ClanSendPersonMaxRequest(m_WorkThreadIdx, this, 50 );
		}
		break;
	case CASHITEM_ITEMID_CLAN_PERSON_PLUS_10:
		{	// 클랜원 증가
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)										return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if( m_UserInfoBasic.m_ui32ClanState != CLAN_MEMBER_LEVEL_MASTER )		return EVENT_ERROR_CLAN_LEVEL_MASTER;
			
			g_pModuleClan->ClanSendPersonMaxRequest(m_WorkThreadIdx, this, 10 );
		}
		break;
	case CASHITEM_ITEMID_CLR_CLAN_HISTORY:
		{	// 클랜전적 초기화
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)										return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if( m_UserInfoBasic.m_ui32ClanState != CLAN_MEMBER_LEVEL_MASTER )		return EVENT_ERROR_CLAN_LEVEL_NOMASTER;

			g_pModuleClan->ClanSendRecordResetRequest(m_WorkThreadIdx, this );
		}
		break;
	case CASHITEM_ITEMID_CLAN_POINT_RESET:
		{	// 클랜 포인트 초기화
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)										return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if( m_UserInfoBasic.m_ui32ClanState != CLAN_MEMBER_LEVEL_MASTER )		return EVENT_ERROR_CLAN_LEVEL_MASTER;
				
            g_pModuleClan->ClanSendPointResetRequest(m_WorkThreadIdx, this );
		}
		break;
	}
	return EVENT_ERROR_SUCCESS;
}

INT32 CUserSession::_SetItemAbilityAuth( INVEN_BUFFER* pBuffer, void* pData )
{
	
	switch( GET_ITEM_TYPE( pBuffer->_ui32ItemID ) )
	{	
	case ITEM_TYPE_WRAP_PERIOD:
		{
			// 기간제를 구매 하는 것은 기간제로 변환한다.
			UINT32 ui32MaintenenceID = MAKE_ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, GET_ITEM_NUMBER(pBuffer->_ui32ItemID));
			switch(ui32MaintenenceID)
			{	
				// 컬러 닉네임 갯수제 -> 기간제로 기획 변경. 2014.10.23 노준영
			case CASHITEM_ITEMID_COLOR_NICK:
				{	// 컬러 닉네임
					UINT8 ui8Color;
					i3mem::Copy( &ui8Color, pData, sizeof(UINT8) );
					m_UserInfoInvItemData._ui8NickColor = ui8Color;
				}
				break;
			case CASHITEM_ITEMID_DISGUISE_RANK:
				{	// 위장 계급
					UINT8 ui8Rank;
					i3mem::Copy( &ui8Rank, pData, sizeof(UINT8) );

					//현재계급 ±10을 넘어가는 경우 실패처리합니다.
					if( ui8Rank > RANK_GAMEMASTER || 
						ui8Rank < RANK_00 ||
						abs( (INT32)m_UserInfoBasic.m_ui32Rank - (INT32)ui8Rank ) > 10) return EVENT_ERROR_DISGUISE_RANK_RANGE_OUT;

					// 임시로 저장을 해놓는다.
					m_UserInfoInvItemData._wRank = (UINT32)ui8Rank;
				}
				break;
			case CASHITEM_ITEMID_DISGUISE_RANK_APRIL_FOOL:
				{	//만우절 위장 계급
					UINT8 ui8Rank;
					i3mem::Copy( &ui8Rank, pData, sizeof(UINT8) );

					//만우절이 아닌경우 사용실패 처리한다.
					//if( 4 != i3ThreadTimer::LocalTime().GetMonth() || 1 != i3ThreadTimer::LocalTime().GetDay() ) return EVENT_ERROR_DISGUISE_RANK_NOT_APRIL_FOOL;

					if( ui8Rank > RANK_GAMEMASTER || ui8Rank < RANK_00 ) return EVENT_ERROR_DISGUISE_RANK_RANGE_OUT;

					// 임시로 저장을 해놓는다.
					m_UserInfoInvItemData._wAprilFoolRank = (UINT32)ui8Rank;
				}
				break;
			case CASHITEM_ITEMID_COLOR_MUZZLE_FLASH:
				{	//총구 화염 컬러 변경권
					UINT16 ui16Color;
					i3mem::Copy( &ui16Color, pData, sizeof(UINT16) );
					m_UserInfoInvItemData._ui16MuzzleFlashColor = ui16Color;
				}
				break;
			case CASHITEM_ITEMID_DISGUISE_NICK:
				{	// 위장 닉네임
					TTCHAR strNickname[NET_NICK_NAME_SIZE];
					i3mem::Copy(strNickname, (TTCHAR*)pData, NET_NICK_NAME_SIZE);
					strNickname[NET_NICK_NAME_SIZE - 1] = '\0';

					// 아이디 중복 체크
					INT32 i32Result = 0;
					if (EV_FAILED(i32Result))												return i32Result;

					// 임시로 저장을 해놓는다.
					i3String::Copy(m_UserInfoInvItemData._cNick, strNickname, NET_NICK_NAME_SIZE);

					m_UserInfoInvItemData._cNick[NET_NICK_NAME_SIZE - 1] = '\0';
				}
				break;
			case CASHITEM_ITEMID_COLOR_CROSSHAIR:
				{	// 컬러 크로스헤어
					UINT8 ui8Color;
					i3mem::Copy(&ui8Color, pData, sizeof(UINT8));

					// 임시로 저장을 해놓는다.
					m_UserInfoInvItemData._ui8CrossHairColor = ui8Color;
				}
				break;
			}
		}
		break;

	case ITEM_TYPE_WRAP_COUNT :
		{
			switch( pBuffer->_ui32ItemID ) 
			{
			case CASHITEM_ITEMID_CHANGE_NICK:
				{	// 닉네임 변경권
					// 위장 닉네임 사용시 닉네임 변경 실패			
					if( NULL == GetItemByItemID(CASHITEM_ITEMID_CHANGE_NICK))	return EVENT_ERROR_FAIL;
					// 자신의 닉네임을 다시 변경 요청시 변경 실패처리
					if( i3String::Compare((TTCHAR*)pData, m_UserInfoBasic.m_strNick) == 0)		return EVENT_ERROR_FAIL;

					T_RESULT TResult = _CheckUseNickName( (TTCHAR*)pData );
					if( EV_FAILED( TResult ) )												return TResult;
				}
				break;
			case CASHITEM_ITEMID_CHANGE_CLAN_NAME:
				{	// 클랜 이름 변경권			
					if(m_UserInfoBasic.m_ui32Clanidx	  == 0)										return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
					else if( m_UserInfoBasic.m_ui32ClanState != CLAN_MEMBER_LEVEL_MASTER )		return EVENT_ERROR_CLAN_LEVEL_MASTER;

					TTCHAR strClanName[NET_CLAN_NAME_SIZE]; 
					i3mem::Copy( strClanName, pData, sizeof(TTCHAR) * NET_CLAN_NAME_SIZE );
					strClanName[NET_CLAN_NAME_SIZE - 1] = 0; 

					// 클랜 이름 변경
					if(FALSE == g_pModuleClan->ClanSendNameRequest(m_WorkThreadIdx, this, strClanName ))
					{
						return EVENT_ERROR_FAIL;
					}
				}
				break;
			case CASHITEM_ITEMID_CHANGE_CLAN_MARK:
				{	// 클랜 마크 변경권
					if(m_UserInfoBasic.m_ui32Clanidx	  == 0)										return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
					else if( m_UserInfoBasic.m_ui32ClanState != CLAN_MEMBER_LEVEL_MASTER )		return EVENT_ERROR_CLAN_LEVEL_MASTER;

					UINT32 ui32Mark;
					i3mem::Copy( &ui32Mark, pData, sizeof(UINT32) );

					// 클랜 마크 변경
					if( FALSE == g_pModuleClan->ClanSendMarkRequest(m_WorkThreadIdx, this, ui32Mark ))
					{
						return EVENT_ERROR_FAIL;
					}
				}
				break;
				// 클랜 컬러 네임 기간제로 전환되어 안쓰인다.
			case CASHITEM_ITEMID_COLOR_CLAN:
				{	// 클랜 컬러 네임
					if(m_UserInfoBasic.m_ui32Clanidx	  == 0)										return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
					else if( m_UserInfoBasic.m_ui32ClanState != CLAN_MEMBER_LEVEL_MASTER )		return EVENT_ERROR_CLAN_LEVEL_MASTER;

					UINT8 ui8Color;
					i3mem::Copy( &ui8Color, pData, sizeof(UINT8) );

					// 클랜 서버에 알려준다.
					if( FALSE == g_pModuleClan->ClanSendClanColorRequest(m_WorkThreadIdx, this, CASHITEM_ITEMID_COLOR_CLAN, 0, ui8Color ) )
					{
						return EVENT_ERROR_FAIL;
					}
				}
				break; 
				// 기간제 안쓰이는 코드
			case CASHITEM_ITEMID_COLOR_CHATTING:
				{	// 컬러 체팅
					UINT8 ui8Color;
					i3mem::Copy( &ui8Color, pData, sizeof(UINT8) );

					// 임시로 저장을 해놓는다.
					m_UserInfoInvItemData._ui8ChattingColor = ui8Color;
				}
				break;

			case CASHITEM_ITEMID_VIEW_OTHER_USER_ITEM_INFO:
				{	// 상대방 정보보기

					// 방에 있는지 확인
					if(m_i32RoomIdx == SERVER_VALUE_UNKNOWN || m_i32SlotIdx == SERVER_VALUE_UNKNOWN)
					{
						_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_POSITION, PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_REQ);
						return EVENT_ERROR_FAIL;
					}

					INT8 i8Slot;
					i3mem::Copy( &i8Slot, pData, sizeof(INT8) );

					if( NULL == m_pRoom )
					{
						return EVENT_ERROR_FAIL;
					}

					if( FALSE == m_pRoom->GetUserItemInfo( this, i8Slot ) )
					{
						return EVENT_ERROR_FAIL;
					}
				}
				break;
			case CASHITEM_ITEMID_MAIN_SKILL_ALLOPEN:
				{	// 캐릭터 메인스킬 전레벨 개방

					UINT8 ui8CharSlot;
					S2MO_CHAR_BASE_INFO stBaseInfo;

					i3mem::Copy( &ui8CharSlot, pData, sizeof(INT8) );

					m_MultiSlot.GetCharBaseInfo( ui8CharSlot, &stBaseInfo );
					stBaseInfo.m_ui32SkillRelTime = 1800000;
					m_MultiSlot.SetCharBaseInfo( ui8CharSlot, &stBaseInfo );
				}
				break;
			case CASHITEM_ITEMID_MEGAPHONE:
				{	//확성기
					//1초에 1번만 사용가능합니다.
					if (i3ThreadTimer::LocalTime().DiffTime(m_dt32MegaPhoneUseTime) > 0)
					{
						m_dt32MegaPhoneUseTime = i3ThreadTimer::LocalTime();
					}
					else
					{
						return EVENT_ERROR_MEGAPHONE_DELAY_TIME;
					}
				}
				break;
			}
		}
		break;
	}

	return EVENT_ERROR_SUCCESS;
}

INT32 CUserSession::_SetItemAbilityChange( INVEN_BUFFER* pBuffer, void* pData )
{
	INT32 i32Result = EVENT_ERROR_SUCCESS;

	switch( GET_ITEM_TYPE( pBuffer->_ui32ItemID ) )
	{
	case ITEM_TYPE_MAINTENANCE:			
		{
			switch( pBuffer->_ui32ItemID )
			{
			case CASHITEM_ITEMID_COLOR_MUZZLE_FLASH:
				{	//총구 화염 컬러 변경권(사용중 변경)
					UINT16 ui16Color;
					i3mem::Copy( &ui16Color, pData, sizeof(UINT16) );
					m_UserInfoInvItemData._ui16MuzzleFlashColor = ui16Color;
					SetChangeColorMuzzleFlash();
				}
				break;

			default:
				{
					i32Result = EVENT_ERROR_FAIL;
				}
				break;
			}
		}
		break;

	default:
		{
			i32Result = EVENT_ERROR_FAIL;
		}
		break;
	}

	return i32Result;
}

void CUserSession::_ResetRecord_MAU(void)
{
	m_UserInfoRecord._match		= 0; 
	m_UserInfoRecord._smatch	= 0; 
	m_UserInfoRecord._win		= 0; 
	m_UserInfoRecord._swin		= 0; 
	m_UserInfoRecord._lose		= 0; 
	m_UserInfoRecord._slose		= 0; 
	m_UserInfoRecord._draw		= 0; 
	m_UserInfoRecord._sdraw		= 0;

	ChangeDataBaseInfo( SAVE_DATABASE_RECORD );
}

void CUserSession::_ResetDisconnect_MAU(void)
{
	m_UserInfoRecord._dis		= 0;
	m_UserInfoRecord._sdis		= 0;
	m_UserInfoRecord._dmatch	= 0;
	m_UserInfoRecord._sdmatch	= 0;

	ChangeDataBaseInfo( SAVE_DATABASE_RECORD );
}

void CUserSession::_ResetKillDeath_MAU(void)
{
	m_UserInfoRecord._death		= 0; 
	m_UserInfoRecord._sdeath	= 0; 
	m_UserInfoRecord._killcount	= 0; 
	m_UserInfoRecord._skillcount= 0; 	
	m_UserInfoRecord._AssistCount = 0;
	m_UserInfoRecord._sAssistCount = 0;

	ChangeDataBaseInfo( SAVE_DATABASE_RECORD );
}
//----------------------------------------------------------------------------------------------------------------
// public
void CUserSession::CheckRankUp( INT32 i32WorkThread, RANKUP_CHECK eCheckType )
{
	// GM계정은 랭크업 처리 하지 않습니다.
	if( CheckAuthLevel() )	return;

	UINT32 ui32Rank = m_UserInfoBasic.m_ui32Rank;
	
	BOOL bRankUp = FALSE;

	// 랭크업 체크.
	switch ( eCheckType ) 
	{
	case RANKUP_CHECK_ABSOLUTE : // 절대평가. 0~46
		{ 
			// 영웅의 경험치를 얻으면 영웅계급으로 승급시킨다.
			UINT32 ui32HeroExp = m_pDesign->GetExpValue( RANK_51 );

			if( ui32HeroExp <= m_UserInfoBasic.m_ui32Exp)
			{	  
				if( ui32Rank >= RANK_51) return;

				ui32Rank = RANK_51;
				bRankUp = TRUE;
				break;
			}

			if( ui32Rank > RANK_45) break;	// 준장이상이면 상대평가에 의해 랭크업 됩니다.

			bool bLoop = true;

			
			while( m_pDesign->GetExpValue( ui32Rank + 1 ) <= m_UserInfoBasic.m_ui32Exp && bLoop )
			{	
				ui32Rank = ui32Rank + 1; 
				bRankUp = TRUE;
			
				if( ui32Rank > RANK_45)	bLoop = false; // 준장까지만 진급시킵니다.
			}
		}
		break;
	case RANKUP_CHECK_COMPARE : // 상대평가. 47~50
		{
			// 영웅이상, 준장 이하는 검사 대상에서 제외
			if(RANK_51 <= ui32Rank || RANK_47 > ui32Rank ) break; 

			// 이전에 보상을 받았다면 제외.
			if( m_UserInfoBasic.m_ui32KeepRank >= ui32Rank ) break; 

			bRankUp = TRUE;
		}
		break;

	default:
		return;
	}

	//장성계급 변동이 있는경우
	if( 0 != m_ui32ChangedRank )
	{		

		INVEN_BUFFER_SET	stGeneralItemSet;
		stGeneralItemSet.ui8ChangeType = ITEM_INSERT_REQUEST_GENERAL;

		if( RANK_46 == m_ui32ChangedRank && ui32Rank > RANK_46 ) //준장으로 강등된경우 지급된 장성급아이템을 회수한다.
		{
			for( int i = 0 ; i < g_pContextMain->m_ui8GeneralItemCount ; ++i )
			{
				T_ItemDBIdx TItemDBidx;
				g_MakeBasicItemDBIdx( TItemDBidx, g_pContextMain->m_aaui32GeneralItem[i] );
				OnDeleteItem( i32WorkThread, TItemDBidx, &stGeneralItemSet );
			}
		}
		else if( RANK_46 < m_ui32ChangedRank &&  ui32Rank < RANK_47 ) //소장 이상으로 진급한 경우 장성급 아이템을 지급한다.
		{
			//stGeneralItemSet.ui8ChangeType = ITEM_CHANGE_TYPE_INSERT;

			for( int i = 0 ; i < g_pContextMain->m_ui8GeneralItemCount ; ++i )
			{
				INVEN_BUFFER		stInvenBuffer;
				g_MakeBasicItemDBIdx( stInvenBuffer._TItemWareDBIdx, g_pContextMain->m_aaui32GeneralItem[i] );
				stInvenBuffer._ui32ItemID	= g_pContextMain->m_aaui32GeneralItem[i];
				stInvenBuffer._ui8ItemType	= ITEM_ATTR_TYPE_BASIC;
				stInvenBuffer._ui32ItemArg	= 1;				

				OnInsertItem( &stInvenBuffer, &stGeneralItemSet );
			}
		}

		SendChangeInven( &stGeneralItemSet );
		CheckEquipment();

		ui32Rank = m_ui32ChangedRank;
		m_ui32ChangedRank = 0;
		bRankUp = TRUE;
	}

	if( !bRankUp ) return;

	UINT32 ui32rankUpPoint = 0;

	

	UINT32 ui32KeepRank = m_UserInfoBasic.m_ui32KeepRank; 

	// KeepRank와 비교하여 보상지급여부를 결정합니다. m_UserInfoBasic._keep_rank ~ ui32Rank 사이의 모든 보상을 지급합니다.
	while( ui32KeepRank < ui32Rank ) 
	{
		switch(  eCheckType )
		{
		case RANKUP_CHECK_COMPARE : // 상대평가. 47~50
			{
				// 장성 계급은 웹툴로 랭크업 보너스 셋팅을 하지 않습니다. 기획의도.				
			}
			//break;	// 장성계급도 웹툴에서 셋팅된 랭크업 보너스 아이템을 받게하려면 break 주석 처리 해주세요.
		case RANKUP_CHECK_ABSOLUTE : // 절대평가. 0~46
			{
				REWARD_ITEM stReward;
				stReward.m_ui8GoodsCount = 0;
				stReward.m_ui8RequestType = ITEM_INSERT_REQUEST_RANKUP;
				
				for( INT32 i = 0; i < MAX_RANKUP_ITEM_COUNT; ++i)
				{
					// m_aRankUpItem 0번배열에 1랭크의 보상이 셋팅되어있으므로.
					UINT32 ui32GoodsID =  g_pContextMain->m_aRankUpItem[ui32KeepRank].m_ui32GoodsID[i];	

					if( 0 == ui32GoodsID ) continue;

					stReward.m_aTGoodsID[stReward.m_ui8GoodsCount] = ui32GoodsID;
					++stReward.m_ui8GoodsCount;
				}

				g_pModuleTrans->AuthSendRewardItem( i32WorkThread, this, &stReward ); 
  
			}
			break;
		}

		++ui32KeepRank;
	}
	 
 	// Rank, KeepRank 변경
	m_UserInfoBasic.m_ui32Rank		= ui32Rank;
	
	if( ui32Rank > m_UserInfoBasic.m_ui32KeepRank )	//KeepRank는 기존값보다 큰경우에만 갱신해 준다.
		m_UserInfoBasic.m_ui32KeepRank	= ui32Rank;

	// 메신저, 클랜, 룸의 계급을 변경
	SetRankUp(i32WorkThread);

	i3NetworkPacket packet(PROTOCOL_BASE_RANK_UP_ACK);
	packet.WriteData(&m_UserInfoBasic.m_ui32Rank,			sizeof(UINT32));
	packet.WriteData(&m_UserInfoBasic.m_ui32KeepRank,		sizeof(UINT32));
	packet.WriteData(&ui32rankUpPoint,						sizeof(UINT32));
	SendPacketMessage(&packet);

	// 로그 전송
	g_pModuleContextNC->LogSendMessage( i32WorkThread, LOG_MESSAGE_U_LEVEL_UP, this );

	ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO);
}

void CUserSession::SetPosition(INT32 i32Position)
{
	if(m_i32MainTask == i32Position)return;

	//Now Position
	switch( m_i32MainTask )
	{
	case GAME_TASK_LOBBY:	g_pUserManager->OnLeaveLobby( this, m_i32ChannelNum );	break;
	default: break; 
	}

	//New Postion 
	switch( i32Position )
	{
	case GAME_TASK_LOBBY: g_pUserManager->OnEnterLobby(this, m_i32ChannelNum); break;
	default: break; 
	}

	m_i32MainTask = i32Position;
	return; 
}

//Find Quest Slot
INT8 CUserSession::_FindInsertQuestSlot( UINT8 ui8CardType )
{
	// 같은 카드셋이 있는지 확인한다.
	for( INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; i++ )
	{
		if( ui8CardType == m_QuestIngInfo.m_pCardSetType[i] )
		{
            return -1;
		}
	}

	// 없으면 빈슬롯을 찾는다.
	// 마지막 슬롯은 Event 전용이기때문에 검사하지 않는다.
	for( INT32 i = 0; i < EVENT_CARDSET_SLOT ; i++ )
	{
		if( QUEST_CARDSET_TYPE_NONE != m_QuestIngInfo.m_pCardSetType[i] ) continue;
		
		return ((INT8)i);
	}

	return -1; 
}

T_RESULT CUserSession::_CheckUseNickName( TTCHAR* strNick )
{
	// 닉네임에 GM, Gm, gM, gm 이 들어가 있으면 무조건 실패
	char strNickname[NET_NICK_NAME_SIZE];
	char strGMCheck[3];

	i3String::CopyWCHARtoASCII( strNickname, NET_NICK_NAME_SIZE, strNick, NET_NICK_NAME_SIZE );

	i3String::ToLower( strGMCheck, 2, strNickname );
	if ('g' == strGMCheck[0] && 'm' == strGMCheck[1])
	{
		return EVENT_ERROR_FAIL;
	}

	TTCHAR* pNickTemp = strNick;
	UINT8 ui8Check = 0;
	while( '\0' != *pNickTemp )
	{	//	닉네임에 공백이 들어가면 무조건 실패
		if( ' '		== *pNickTemp )							return EVENT_ERROR_FAIL;		// 공백일때
		if( '　'	== *pNickTemp )							return EVENT_ERROR_FAIL;		// 한자 전각 스페이스.　
		if( '	'	== *pNickTemp )							return EVENT_ERROR_FAIL;		// 탭 일때
		if( '\r'	== *pNickTemp )							return EVENT_ERROR_FAIL;		// 엔터 일때
		if( '\n'	== *pNickTemp )							return EVENT_ERROR_FAIL;		// 엔터 일때
		if( -95		== *pNickTemp && '\0' != *(pNickTemp+1)  )								// ㄱ한자 공백 ( 2바이트입니다.)
		{
			if( -95 == *(pNickTemp+1) )						return EVENT_ERROR_FAIL;
		}

		if( 1 <= ui8Check  )
		{	// ZLOG 방지 &,	 NZLOG 방지 ,\t (탭은 위에서 막혀있다.)
			if( ( *(pNickTemp) == ',' ) && ( *(pNickTemp-1) == '&' ) )		return EVENT_ERROR_FAIL;
		}

		if( 3 <= ui8Check )
		{
			if( (*(pNickTemp-3) == '{')								&&
				(*(pNickTemp-2) == 'C' || *(pNickTemp-2) == 'c' )	&&
				(*(pNickTemp-1) == 'O' || *(pNickTemp-1) == 'o' )	&&
				(*pNickTemp     == 'L' || *pNickTemp     == 'l' ) )	return EVENT_ERROR_FAIL; // 컬러 스크립트 사용시 - {COL 대소문자 구분이 없어 대소문자 둘다 검사
		}

		if( 4 <= ui8Check )
		{
			if( (*(pNickTemp-4) == '{' )							&&
				(*(pNickTemp-3) == '/' )							&&
				(*(pNickTemp-2) == 'C' || *(pNickTemp-2) == 'c' )	&&
				(*(pNickTemp-1) == 'O' || *(pNickTemp-1) == 'o' )	&&
				(*pNickTemp     == 'L' || *pNickTemp     == 'l' ) )	return EVENT_ERROR_FAIL; // 컬러 스크립트 사용시 - {/COL 대소문자 구분이 없어 대소문자 둘다 검사
		}

		ui8Check++;
		pNickTemp++;
	}

	pNickTemp = strNick;
	switch( m_ui8LocaleCode )
	{
	case LOCALE_CODE_INDONESIA:
		{
			// 인니일 경우 0~9, A~Z, a~z, _ 만 허용	
			while( '\0' != *pNickTemp )
			{
				// 여러가지 검색법이 있겠지만.. 알아보기 쉽고 퍼포먼스 생각해서 이렇게 작업합니다.
				// 가장 많은 검색이 있을듯한 순서대로 검색합니다.알파벳->숫자->특수문자		20100827 김현우..
				if( ( 'a' <= *pNickTemp && 'z' >= *pNickTemp )	||
					( 'A' <= *pNickTemp && 'Z' >= *pNickTemp )	||
					( '0' <= *pNickTemp && '9' >= *pNickTemp )	||
					( '_' == *pNickTemp ) )
				{
					pNickTemp++;
				}
				else
				{
					return EVENT_ERROR_FAIL;
				}
			}
		}
		break;
	case LOCALE_CODE_TURKEY:		// 터키의 경우 닉네임 앞부분을 GM이나 MOD_로 생성할 수 없음 - 2012-09-07 김용수
		{
			if ( (strNick[0] == 'M') && (strNick[1] == 'O') && (strNick[2] == 'D') && (strNick[3] == '_') )
			{
				return EVENT_ERROR_FAIL;
			}

			while( '\0' != *pNickTemp )
			{
				// 여러가지 검색법이 있겠지만.. 알아보기 쉽고 퍼포먼스 생각해서 이렇게 작업합니다.
				// 가장 많은 검색이 있을듯한 순서대로 검색합니다.알파벳->숫자->특수문자		20100827 김현우..
				if( ( '!' <= *pNickTemp && '~' >= *pNickTemp )	||				
					( -16 == *pNickTemp )					||
					( -4 == *pNickTemp )					||
					( -3 == *pNickTemp )					||
					( -2 == *pNickTemp )					||
					( -10 == *pNickTemp )					||
					( -25 == *pNickTemp )					|| 
					( -48 == *pNickTemp )					||
					( -36 == *pNickTemp )					||
					( -34 == *pNickTemp )					||
					( -35 == *pNickTemp )					||
					( -42 == *pNickTemp )					||
					( -57 == *pNickTemp ) )
				{
					pNickTemp++;
				}
				else
				{
					return EVENT_ERROR_FAIL;
				}
			}
		}
		break;
	case LOCALE_CODE_NORTHAMERICA :
		{
			while( '\0' != *pNickTemp )
			{
				// 여러가지 검색법이 있겠지만.. 알아보기 쉽고 퍼포먼스 생각해서 이렇게 작업합니다.
				// 가장 많은 검색이 있을듯한 순서대로 검색합니다.알파벳->숫자->특수문자		20100827 김현우..
				if( ( 'a' <= *pNickTemp && 'z' >= *pNickTemp )	||
					( 'A' <= *pNickTemp && 'Z' >= *pNickTemp )	||
					( '0' <= *pNickTemp && '9' >= *pNickTemp )	||
					'!' == *pNickTemp || '@' == *pNickTemp || '#' == *pNickTemp || '$' == *pNickTemp ||
					'%' == *pNickTemp || '^' == *pNickTemp || '&' == *pNickTemp || '*' == *pNickTemp ||
					'(' == *pNickTemp || ')' == *pNickTemp || '[' == *pNickTemp || ']' == *pNickTemp ||
					'{' == *pNickTemp || '}' == *pNickTemp )
				{
					pNickTemp++;
				}
				else
				{
					return EVENT_ERROR_FAIL;
				}
			}
		}
		break;
	//case TRANS_METHOD_MIDDLEEAST :  // 중동은 아랍어 사용가능
	case LOCALE_CODE_PHILIPPINES :
		{
			while( '\0' != *pNickTemp )
			{
				// 여러가지 검색법이 있겠지만.. 알아보기 쉽고 퍼포먼스 생각해서 이렇게 작업합니다.
				// 가장 많은 검색이 있을듯한 순서대로 검색합니다.알파벳->숫자->특수문자		20100827 김현우..
				if( ( 'a' <= *pNickTemp && 'z' >= *pNickTemp )	||
					( 'A' <= *pNickTemp && 'Z' >= *pNickTemp )	||
					( '0' <= *pNickTemp && '9' >= *pNickTemp )	)
				{
					pNickTemp++;
				}
				else
				{
					return EVENT_ERROR_FAIL;
				}
			}
		}
		break;
	case LOCALE_CODE_RUSSIA:
	{
		enum TYPE { NONE = -1, SYMBOL, LATINE, CYRILLIC };	// 기호, 알파벳, 러시아어
		TYPE nickType = NONE;
		TYPE charType = NONE;
		// 러시아일 경우 0~9, A~Z, a~z, !, *,  а~я, А~Я (러시아 문자) 만 허용 (+ 영어와 러시아문자 혼합 사용 불가2016.06.29)
		while ('\0' != *pNickTemp)
		{
			// 여러가지 검색법이 있겠지만.. 알아보기 쉽고 퍼포먼스 생각해서 이렇게 작업합니다.
			// 가장 많은 검색이 있을듯한 순서대로 검색합니다.알파벳->숫자->특수문자		20100827 김현우..
			if (('a' <= *pNickTemp && 'z' >= *pNickTemp) ||
				('A' <= *pNickTemp && 'Z' >= *pNickTemp))
			{
				charType = LATINE;
				goto CHECK_CHAR_TYPE;
			}
			if (('0' <= *pNickTemp && '9' >= *pNickTemp) ||
				('!' == *pNickTemp) ||
				('*' == *pNickTemp))
			{
				charType = SYMBOL;
				goto CHECK_CHAR_TYPE;
			}
			if ((L'а' <= *pNickTemp && L'я' >= *pNickTemp) ||
				(L'А' <= *pNickTemp && L'Я' >= *pNickTemp) || 
				(L'ё' == *pNickTemp) ||
				(L'Ё' == *pNickTemp))
			{
				charType = CYRILLIC;
			}

		CHECK_CHAR_TYPE:
			switch (charType)
			{
			case SYMBOL:
				break;
			case LATINE:
				if (CYRILLIC == nickType)
				{
					return EVENT_ERROR_FAIL;
				}
				nickType = LATINE;
				break;
			case CYRILLIC:
				if (LATINE == nickType)
				{
					return EVENT_ERROR_FAIL;
				}
				nickType = CYRILLIC;
				break;
			default:
				return EVENT_ERROR_FAIL;
			}

			pNickTemp++;
			charType = NONE;
		}
	}
	break;
	}
	return EVENT_ERROR_SUCCESS;
}

BOOL CUserSession::_CheckClanName( TTCHAR* pClanName )
{
	switch( m_ui8LocaleCode )
	{
	case LOCALE_CODE_TURKEY:
		{
			while( '\0' != *pClanName )
			{
				// 여러가지 검색법이 있겠지만.. 알아보기 쉽고 퍼포먼스 생각해서 이렇게 작업합니다.
				// 가장 많은 검색이 있을듯한 순서대로 검색합니다.알파벳->숫자->특수문자		20100827 김현우..
				if( ( 'a' <= *pClanName && 'z' >= *pClanName )	||
					( 'A' <= *pClanName && 'Z' >= *pClanName )	||
					( '0' <= *pClanName && '9' >= *pClanName ) )
				{
					pClanName++;
				}
				else
				{
					return FALSE;
				}
			}
		}
		break;
	}

	return TRUE;
}

void CUserSession::SendNewEventCardSet(void)
{
	i3NetworkPacket SendPacket(PROTOCOL_SERVER_MESSAGE_NEW_EVENT_CARDSET);
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendWhisper(TTCHAR* senderNick, TTCHAR* message, UINT16 ui16messageSize, bool bIsGMUser, UINT8 ui8NickColor )
{
	i3NetworkPacket packet(PROTOCOL_AUTH_RECV_WHISPER_ACK);
	
	packet.WriteData(senderNick,		sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	packet.WriteData(&bIsGMUser,		sizeof(bool));
	packet.WriteData(&ui8NickColor,		sizeof(UINT8));
	packet.WriteData(&ui16messageSize,	sizeof(UINT16));
	packet.WriteData(message,			sizeof(TTCHAR) * ui16messageSize);
	

	SendPacketMessage(&packet);
}

void CUserSession::SendWhisperError(INT32 i32result, TTCHAR* receiverNick, UINT8 ui8NickColor)
{
	i3NetworkPacket packet(PROTOCOL_AUTH_SEND_WHISPER_ACK);

	packet.WriteData( &i32result,		sizeof(INT32));
	packet.WriteData( &ui8NickColor,	sizeof(UINT8) );
	packet.WriteData( receiverNick,		sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	

	SendPacketMessage(&packet);
}

void CUserSession::SendFindUser(INT32 i32Result, NEW_FIND_USER_INFO* pNewFindUserInfo )
{
	i3NetworkPacket packet(PROTOCOL_AUTH_FIND_USER_ACK);
	packet.WriteData(&i32Result,		 sizeof(INT32));
	if (EV_SUCCESSED(i32Result))
	{
		packet.WriteData(pNewFindUserInfo,	 sizeof(NEW_FIND_USER_INFO));
	}
	SendPacketMessage(&packet);
}

void CUserSession::SendFriendList( UINT8 ui8FriendCount, FRIEND_INFO_STRUCT* pFriendInfo )
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_FRIEND_INFO_ACK );

	SendPacket.WriteData(&ui8FriendCount,	sizeof(UINT8));
	for(UINT8 i = 0; i < ui8FriendCount; i++)
	{
		SendPacket.WriteData( &pFriendInfo[i]._NickSize,	sizeof(UINT8)); 
		SendPacket.WriteData( pFriendInfo[i]._NickName,		sizeof(TTCHAR) * pFriendInfo[i]._NickSize ); 
		SendPacket.WriteData( &pFriendInfo[i]._BasicInfo,	sizeof(FRIEND_INFO));
	}
	SendPacketMessage(&SendPacket); 
	return; 
}

void CUserSession::SetFriendChangeInfo_A(UINT8 ui8FriendIdx, UINT8 ui8State, UINT8 NickNameSize, TTCHAR  * NickName, FRIEND_INFO * pInfo)
{
	//0. Check 
	if( m_ui8FriendCount < ui8FriendIdx )//Insert가 있어서 이렇게 해야 한다. 
	{
		I3TRACE("SetFriendChangeInfo_A FRIEND IDX ERROR"); 
		return; 
	}

	//1. Setting
	switch(ui8State)
	{
	case FRIEND_CHANGE_STATE_INSERT	:
		m_ui8FriendCount = ui8FriendIdx + 1; 
		//g_pModuleContextNC->LogSendFriendInsert( MODULE_MESSENGER_THREAD_IDX, this, NickName, pInfo);
		break; 
	case FRIEND_CHANGE_STATE_ACCEPT	:
		//g_pModuleContextNC->LogSendFriendAccept(MODULE_MESSENGER_THREAD_IDX, this, NickName, pInfo);
		break; 
	case FRIEND_CHANGE_STATE_DELETE	:
		//g_pModuleContextNC->LogSendFriendDelete(MODULE_MESSENGER_THREAD_IDX, this, NickName, pInfo);
		break;	 
	case FRIEND_CHANGE_STATE_CHANGE	:
		break; 
	default : I3TRACE("Friend State Error"); return; break; 
	}
 
	//2. Send Message 
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_FRIEND_INFO_CHANGE_ACK );
	SendPacket.WriteData( &ui8State,		sizeof(UINT8) );		//상태
	SendPacket.WriteData( &ui8FriendIdx,	sizeof(UINT8) );		//친구인덱스
	SendPacket.WriteData( &NickNameSize,	sizeof(UINT8)); 
	SendPacket.WriteData( NickName,			sizeof(TTCHAR) * NickNameSize); 
	SendPacket.WriteData( pInfo,			sizeof(FRIEND_INFO));	//친구정보


	SendPacketMessage(&SendPacket); 
	return; 
}

void CUserSession::SendFriendAcceptError(INT32	i32ErrorCode)
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_FRIEND_ACCEPT_ACK );
	SendPacket.WriteData( &i32ErrorCode,	sizeof(INT32) );
	SendPacketMessage(&SendPacket); 
	return; 
}

void CUserSession::SendFriendInsertError(INT32 i32ErrorCode)
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_FRIEND_INSERT_ACK );
	SendPacket.WriteData( &i32ErrorCode,	sizeof(INT32) );
	SendPacketMessage(&SendPacket); 
	return; 
}

void CUserSession::SendFriendDeleteError(INT32 i32ErrorCode)
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_FRIEND_DELETE_ACK );
	SendPacket.WriteData( &i32ErrorCode,	sizeof(INT32) );
	SendPacketMessage(&SendPacket); 
	return; 
}

void CUserSession::SendFriendInvitedError(INT32 i32ErrorCode)
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_FRIEND_INVITED_ACK );
	SendPacket.WriteData( &i32ErrorCode,	sizeof(INT32) );
	SendPacketMessage(&SendPacket); 
	return;
}

void CUserSession::SendFriendInvited(UINT8 ui8FriendIdx)
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_FRIEND_INVITED_REQUEST_ACK );
	SendPacket.WriteData( &ui8FriendIdx, sizeof(UINT8) );
	SendPacketMessage(&SendPacket); 
	return;
}

void CUserSession::OnInsertItem( INVEN_BUFFER* pInvenItem, INVEN_BUFFER_SET* pInvenSet )
{
	m_Inventory.InsertItem( pInvenItem, pInvenSet );
}

UINT32 CUserSession::OnDeleteItem( INT32 i32ThreadIdx, T_ItemDBIdx& TItemDBIdx, INVEN_BUFFER_SET* pInvenSet )
{
	UINT32 ui32ItemID = m_Inventory.DeleteItemByDBIdx( TItemDBIdx, pInvenSet );
	CheckUserInfoByInvenItem( i32ThreadIdx, ui32ItemID );
	return ui32ItemID;
}

UINT32 CUserSession::OnDeleteItemID( INT32 i32ThreadIdx, T_ItemID TItemID, INVEN_BUFFER_SET* pInvenSet )
{
	UINT32 ui32ItemID = m_Inventory.DeleteItemByItemID( TItemID, pInvenSet );
	CheckUserInfoByInvenItem( i32ThreadIdx, ui32ItemID );
	return ui32ItemID;
}

void CUserSession::SendChangeInven( INVEN_BUFFER_SET* pInvenSet )
{
	PACKET_INVENTORY_GET_INFO_ACK Packet;
	Packet.m_ui8ChangeType		= pInvenSet->ui8ChangeType;
	Packet.m_Inventory.SetValue( pInvenSet->m_Inventory, 0, pInvenSet->m_ui16InvenCount );
	S2MOSendEvent( &Packet );
}

/*------------------------------------------------------
리턴값 0x00000001		//Chara 변경 됨 
리턴값 0x00000002		//Weapon 변경 됨
리턴값 0x00000004		//Item 변경 됨
리턴값 0x00000010		//Chara Equip 변경 됨 
리턴값 0x00000020		//Weapon Equip 변경 됨 
리턴값 0x00000040		//Item Equip 변경 됨 - Item은 장비할 수 없음으로 사용하지 않는다.
------------------------------------------------------*/
void CUserSession::CheckInventory_MAU( INT32 i32ThreadIdx, INVEN_CHECK eCheckType )
{
	INVEN_BUFFER_SET InvenSet;
	InvenSet.ui8ChangeType = ITEM_INSERT_REQUEST_AUTH;

	CShop* pShop = g_pContextMain->GetShop();

	UINT16			ui16InvenCount;
	INVEN_BUFFER	Inventory[MAX_INVEN_COUNT];
	m_Inventory.GetInventory( &ui16InvenCount, Inventory );

	UINT16		ui16DeleteItemCount = 0;
	T_ItemDBIdx aTItemDBIdx[MAX_INVEN_COUNT];

	BOOL bEquip = FALSE;
	BOOL bChange = FALSE;
	for( INT32 i = 0 ; i < ui16InvenCount ; i++ )
	{
		if( 0 == Inventory[i]._ui32ItemID )										continue;
		if( ITEM_ATTR_TYPE_AUTH != Inventory[i]._ui8ItemType )					continue; 
		if( i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() < Inventory[i]._ui32ItemArg )		continue;

		// 장착중인 무기/파츠는 기간이 만료되었더라도 체크 대상에서 제외합니다. 기획의도.
		bEquip = FALSE;
		if( INVEN_CHECK_DEL_PERIOD_NO_EQUIP == eCheckType )
		{
			for( INT32 k = 0 ; k < CHAR_EQUIPMENT_WEAPON_COUNT ; k++ )
			{
				if( Inventory[i]._ui32ItemID == m_aTBattleEquipWeapon[k].m_TItemID ) 
				{
					bEquip = TRUE;
					break;
				}
			}

			for (INT32 h = 0; h < CHAR_EQUIPMENT_PARTS_COUNT; h++)
			{
				if (Inventory[i]._ui32ItemID == m_aTBattleEquipParts[h].m_TItemID)
				{
					bEquip = TRUE;
					break;
				}
			}
		} 

		if( bEquip ) continue;
			
		SHOP_ITEM_BASE*	pItem = pShop->GetItemData( Inventory[i]._ui32ItemID );
		if( NULL == pItem )														continue;
		if( ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType )					continue;

		// 캐릭터는 멀티슬롯에서 제거되어야 하므로 분리합니다.
		if (ITEM_TYPE_CHARA == GET_ITEM_TYPE(Inventory[i]._ui32ItemID))
		{
			INT32 i32DeleteSlotIdx = m_MultiSlot.FindSlotIdxByCharaDBIdx(Inventory[i]._TItemWareDBIdx);
			if (i32DeleteSlotIdx != -1)
			{
				g_pModuleTrans->AuthSendCharaDelete(i32ThreadIdx, this, (UINT8)i32DeleteSlotIdx, Inventory[i]._TItemWareDBIdx);
			}
			
			continue;
		}
				
		OnDeleteItem( i32ThreadIdx, Inventory[i]._TItemWareDBIdx,  &InvenSet );

		bChange = TRUE;

		aTItemDBIdx[ui16DeleteItemCount] = Inventory[i]._TItemWareDBIdx;
		++ui16DeleteItemCount;
	}

	// 기간 만료 아이템을 DB에서도 삭제 합니다.
	if (ui16DeleteItemCount > 0)
	{
		g_pModuleTrans->AuthSendExpireDeleteItem(i32ThreadIdx, this, ui16DeleteItemCount, aTItemDBIdx);
	}

	if ( m_ui8OldPCCafe != m_ui8NewPCCafe )
	{
		DEFAULT_ITEM_SET* pPCCafeOld = g_pContextMain->GetDefaultItemSet( DEFAULT_ITEM_TYPE_PCCAFE, m_ui8OldPCCafe );
		DEFAULT_ITEM_SET* pPCCafeNew = g_pContextMain->GetDefaultItemSet( DEFAULT_ITEM_TYPE_PCCAFE, m_ui8NewPCCafe );

		bool abCompOld[DEFAULT_ITEM_COUNT] = { false, };
		bool abCompNew[DEFAULT_ITEM_COUNT] = { false, };

		if( pPCCafeOld && pPCCafeNew )
		{
			for( INT32 i = 0 ; i < pPCCafeOld->m_ui8Count ; i++ )
			{
				for( INT32 j = 0 ; j < pPCCafeNew->m_ui8Count ; j++ )
				{
					if ( pPCCafeOld->m_TItemID[i] == pPCCafeNew->m_TItemID[j] )
					{
						abCompOld[i] = true;
						abCompNew[j] = true;
						break;
					}
				}
			}
		}

		if( pPCCafeOld )
		{
			for( INT32 i = 0 ; i < pPCCafeOld->m_ui8Count ; i++ )
			{
				if ( abCompOld[i] )		continue;
				m_Inventory.DeleteItemByItemID( pPCCafeOld->m_TItemID[i], &InvenSet );
				bChange = TRUE;
			}
		}
		
		if( pPCCafeNew )
		{
			for( INT32 i = 0 ; i < pPCCafeNew->m_ui8Count ; i++ )
			{
				if ( abCompNew[i] )		continue;
				_ExceptionInsert( pPCCafeNew->m_TItemID[i], ITEM_ATTR_TYPE_PC_1, &InvenSet );
				bChange = TRUE;
			}
		}

		m_ui8OldPCCafe = m_ui8NewPCCafe;
	}

	if( bChange )
	{
		SendChangeInven( &InvenSet );
		CheckEquipment(  );
	}
}

void CUserSession::CheckEquipment()
{
	//장착 확인하기 
	INT32 i32Rv = _CheckEquipment();
	if( 0 == i32Rv  )	return;
	
	// 아이템 장착 실패 시에 기본 세팅으로 변경하여 내려줍니다.	
	_SendEquipedInfo();
}

BOOL CUserSession::SendPacketMessage(i3NetworkPacket * pPacket)
{
	if(m_ui32ATIsKill > 0)	return FALSE; //차후 Disconnect 되는 녀석 입니다. 더이상 패킷을 보내지 않습니다. 

	if(m_bSendPacket)
	{
		if( i3NetworkSession::SendPacketMessage( pPacket ) )
		{
			m_i32SendErrorCount = 0;
		}
		else
		{
			m_i32SendErrorCount++; 
			if(m_i32SendErrorCount > 1)
			{
				// 서동권
				// 이 부분의 경우 굉장히 많이 호출되는 부분이고 여러 스레드에서 호출될 수 있어, 
				// 스레드 인댁스를 매개변수로 받는거보다 이부분만 예전 방식으로 처리하는게 효율적이어서 예정 방식으로 둠
				//_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_EVENT , 0, KILL_REASON_SENDPACKET_ERROR, "SendPacket Error" );
				m_ui32ATIsKill		= i3ThreadTimer::GetServerTime();
				m_ui32KillReason	= KILL_REASON_SENDPACKET_ERROR;
			}
			return FALSE;
		}
	}

	return TRUE; 
}

BOOL CUserSession::S2MOSendEvent( S2MOPacketBase* pPacket )
{
	i3NetworkPacket SendPacket;
	if( FALSE == S2MOMakePacket( pPacket, SendPacket.GetPacketBuffer() ))	return FALSE;
	return SendPacketMessage( &SendPacket );
}

void CUserSession::KickUser_Module(T_UID i64UID, UINT8 ui8Reason)
{
	if( FALSE == GetIsActive() )	return;
	if( i64UID != m_i64UID )		return;
		
	i3NetworkPacket SendPacket(PROTOCOL_AUTH_ACCOUNT_KICK_ACK); 
	SendPacket.WriteData(&ui8Reason, sizeof(UINT8));        
	SendPacketMessage( &SendPacket ); 

	if( 0 == m_ui32ATIsKill )
	{
		// ModuleTransSocket 에서 호출
		wchar_t wstrLog[MAX_LOG_STRING_COUNT];
		i3String::Format( wstrLog, MAX_LOG_STRING_COUNT, L"KickUser_Module Reason:%d", ui8Reason );
		_SendErrorMessage( MODULE_TRANS_THREAD_IDX, EVENT_ERROR_EVENT, PROTOCOL_GSM_ACCOUNT_KICK_REQ, KILL_REASON_USERKICK, wstrLog );
	}

	return; 
}

void CUserSession::SetChangeNick( INT32 i32WorkThreadIdx, TTCHAR* pstrOrgNick, TTCHAR* pstrNewNick )
{
	// 변수의 값을 바꿔준다.
	TTCHAR strTemp[ NET_NICK_NAME_SIZE ];

	i3String::Copy( strTemp, pstrOrgNick, NET_NICK_NAME_SIZE );

	strTemp[ NET_NICK_NAME_SIZE-1 ] = '\0';

	i3String::Copy( pstrOrgNick, pstrNewNick, NET_NICK_NAME_SIZE );

	pstrOrgNick[NET_NICK_NAME_SIZE - 1] = '\0';

	i3String::Copy( pstrNewNick, strTemp, NET_NICK_NAME_SIZE );

	pstrNewNick[NET_NICK_NAME_SIZE - 1] = '\0';
	
	// 클랜서버에도 닉네임 변경 통보
	g_pModuleClan->ClanSendChangeNickname(i32WorkThreadIdx, this, pstrOrgNick );
	// 메신저서버에도 닉네임 변경 통보
	if (0 < m_ui8FriendCount)
	{
		g_pModuleMessenger->MessengerSendChangeNickname(i32WorkThreadIdx, this, pstrOrgNick );
	}
	else
	{
		g_pModuleMessenger->MessengerChangeNickname(i32WorkThreadIdx, this);
	}

	// 룸에도 닉네임 변경 통보
	if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
	{
		m_pRoom->SendChangeNickname(m_i32SlotIdx);
	}

	// 닉네임 변경 전달
	i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_MYINFO_BASIC_ACK ); 
	SendPacket.WriteData(&m_UserInfoBasic, sizeof(USER_INFO_BASIC));
	SendPacketMessage(&SendPacket);
}

void CUserSession::SetChangeRank( INT32 i32WorkThreadIdx, UINT32 ui32ItemID )
{
	//ViewRank를 위장게급 변수로 변경 합니다.
	UINT32 ui32MaintenenceID = MAKE_ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, GET_ITEM_NUMBER(ui32ItemID));
	switch(ui32MaintenenceID)
	{
	case CASHITEM_ITEMID_DISGUISE_RANK:
		{
			//만우절 계급이 사용중이라면 갱신하지 않습니다.
			if( RANK_NONE != m_UserInfoInvItemData._wAprilFoolRank) return;

			m_ui32ViewRank = m_UserInfoInvItemData._wRank;
		}
		break;
	case CASHITEM_ITEMID_DISGUISE_RANK_APRIL_FOOL:
		m_ui32ViewRank = m_UserInfoInvItemData._wAprilFoolRank;
		break;
	case 0://위장계급이 만료되어 원래 계급으로 원복
		m_ui32ViewRank = m_UserInfoBasic.m_ui32Rank;
		break;
	}
		
	// 클랜서버에도 계급 변경 통보
	g_pModuleClan->ClanSendChangeRank(i32WorkThreadIdx, this, (UINT8)m_ui32ViewRank);
	

	// 메신저서버에도 계급 변경 통보
	g_pModuleMessenger->MessengerChangeRank(i32WorkThreadIdx, this);

	// 룸에도 계급 변경 통보
	if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
	{
		m_pRoom->SendChangeRank(m_i32SlotIdx);
	}

	// 유저에게도 계급 변경 통보
	InvItemData();
}


void CUserSession::SetRankUp(INT32 i32WorkThreadIdx)
{
	UINT32 ui32Rank = m_UserInfoBasic.m_ui32Rank;

	// 임시 계급을 사용 중이 아닐때에만 진급처리
	if( NULL == GetItemByItemID( CASHITEM_ITEMID_DISGUISE_RANK ) &&
		NULL == GetItemByItemID( CASHITEM_ITEMID_DISGUISE_RANK_APRIL_FOOL ))
	{
		m_ui32ViewRank = m_UserInfoBasic.m_ui32Rank; // 검사를 위장 계급으로 함으로 임시 계급을 사용 중이 아닐때 조건이라 변수값을 바꿔줘도 된다.

		// 클랜서버에도 계급 변경 통보
		if (0 < m_UserInfoBasic.m_ui32Clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < m_UserInfoBasic.m_ui32ClanState)
		{
			g_pModuleClan->ClanSendChangeRank(i32WorkThreadIdx, this, (UINT8)ui32Rank);
		}

		// 메신저서버에도 계급 변경 통보
		g_pModuleMessenger->MessengerChangeRank(i32WorkThreadIdx, this);

		// 룸에도 계급 변경 통보

		if (m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
		{
			m_pRoom->SendChangeRank(m_i32SlotIdx);
		}
	}
}

void CUserSession::SetChangeColorNick( INT32 i32WorkThreadIdx, UINT8 ui8Color )
{	
	if (0 < m_UserInfoBasic.m_ui32Clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < m_UserInfoBasic.m_ui32ClanState)
	{
		g_pModuleClan->ClanSendChangeColorNick(i32WorkThreadIdx, this, ui8Color );
	}

	g_pModuleMessenger->MessengerChangeColorNick(i32WorkThreadIdx, this);

	if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
	{
		m_pRoom->SendChangeColorNick(m_i32SlotIdx);
	}

	InvItemData();
}

void CUserSession::SetChangeColorMuzzleFlash()
{
	if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
	{
		m_pRoom->SendChangeColorMuzzleFlash(m_i32SlotIdx);
	}

	InvItemData();
}

void CUserSession::InvItemData()
{
	ChangeDataBaseInfo( SAVE_DATABASE_ITEM );
	i3NetworkPacket SendPacket( PROTOCOL_BASE_INV_ITEM_DATA_ACK ); 
	SendPacket.WriteData(&m_UserInfoInvItemData, sizeof(USER_INFO_INVITEM_DATA));
	SendPacketMessage(&SendPacket);
}

BOOL CUserSession::CheckGameGuard_M( UINT32 ui32GameGuardTimeOut, UINT32 ui32GameGuardSendTime )
{
	switch( g_pContextMain->m_i32GameGuard )
	{
	case GAMEGUARD_NPROTECT:
	case GAMEGUARD_HACKSHIELD:
	case GAMEGUARD_XTRAP:
	case GAMEGUARD_GPK:
	case GAMEGUARD_USG:
		{
			if( m_ui32ATGameGuardSend > m_ui32ATGameGuardRecv )
			{
				if( m_ui32ATGameGuardRecv < ui32GameGuardTimeOut )
				{
					switch( g_pContextMain->m_i32GameGuard  )
					{
					case GAMEGUARD_NPROTECT:
					case GAMEGUARD_XTRAP:
						break;
					default://GAMEGUARD_HACKSHIELD, GAMEGUARD_GPK
						{
							_SendErrorMessage( MAIN_THREAD_IDX, EVENT_ERROR_GAMEGUARD_ERROR, 0, KILL_REASON_GAME_GUARD );
							return FALSE;
						}
					}
				}
			}
#if defined( USE_NPROTECT )
			if( m_ui32ATGameGuardSend < ui32GameGuardSendTime )
#else
			else if( m_ui32ATGameGuardSend < ui32GameGuardSendTime )
#endif
			{
				SendGameGuardPacket_MU( MAIN_THREAD_IDX );
			}
		}
		break;
	}
	return TRUE;
}

void CUserSession::SendGameGuardPacket_MU( INT32 i32WorkThread )
{
//Send Message
	switch( g_pContextMain->m_i32GameGuard )
	{
		case GAMEGUARD_NPROTECT:
			{
			#if defined( USE_NPROTECT )
				UINT32 ui32ReturnedPacketSize = 0;
				UINT32 dwRet = m_pCsa.Get( &ui32ReturnedPacketSize ); 

				if( dwRet >= 3000 )
				{
					_SendErrorMessage( i32WorkThread, EVENT_ERROR_GAMEGUARD_ERROR, 0, KILL_REASON_GAME_GUARD );
				}
				else
				{
					i3NetworkPacket GameGuardPack( PROTOCOL_BASE_GAMEGUARD_REQ ); 
					GameGuardPack.WriteData(&ui32ReturnedPacketSize, sizeof(UINT32) );
					GameGuardPack.WriteData(&m_pCsa.packet, ui32ReturnedPacketSize );
					SendPacketMessage(&GameGuardPack); 
				}
			#endif
			}
			break;
		case GAMEGUARD_HACKSHIELD:
			{
				if ( ANTICPX_INVALID_HANDLE_VALUE == m_hClient )
				{
					g_pLog->WriteLogSession( this, L"[ERROR]_AhnHS_MakeRequest INVALID HANDLE : %I64d", m_i64UID );
					break;
				}

				// 해당 유저의 핸들을 이용해 요청 메세지 작성
				// 각 유저의 요청 메세지는 해당 유저의 핸들을 이용해서 작성
				unsigned long ulRet = 0;

				ulRet = g_pAhnHS_MakeRequest( m_hClient, &m_stRequestBuf );

				if ( ERROR_SUCCESS != ulRet )
				{
					g_pLog->WriteLogSession( this, L"[ERROR]_AhnHS_MakeRequest ulRet:%x", ulRet );
				}
				else
				{
					// 3 요청 메시지를 클라이언트로 전송합니다.
					i3NetworkPacket GameGuardPack( PROTOCOL_BASE_GAMEGUARD_REQ ); 
					GameGuardPack.WriteData( &m_stRequestBuf.nLength, sizeof( UINT16 ) );
					GameGuardPack.WriteData( m_stRequestBuf.byBuffer, m_stRequestBuf.nLength );
					SendPacketMessage( &GameGuardPack );
				}
			}
			break;

		case GAMEGUARD_XTRAP:
			{
				char pXTrapBuffer[ XTRAP_SETINFO_PACKETBUFF_SIZE ];
				UINT32 ui32Res = XTrap_CS_Step1( m_pXtrapSessionBuf, pXTrapBuffer ) ;

				i3NetworkPacket GameGuardPack( PROTOCOL_BASE_GAMEGUARD_REQ ); 
				GameGuardPack.WriteData( pXTrapBuffer,				XTRAP_SETINFO_PACKETBUFF_SIZE );
				SendPacketMessage( &GameGuardPack );
				
				switch( ui32Res )
				{
				case XTRAP_API_RETURN_OK		:	// 성공
					break;
				case XTRAP_API_RETURN_ERROR					:	// 오류 발생
				case XTRAP_API_RETURN_UNKNOWN				:	// 알 수 없는 오류 발생
				case XTRAP_API_RETURN_INVALID_PARAMETER	:	// 잘못된 매개변수 정보
				case XTRAP_API_RETURN_INVALID_CRC			:	// CheckSum 오류
				case XTRAP_API_RETURN_TIMEOUT				:	// 시간 내에 응답이 없음
				case XTRAP_API_RETURN_DETECTHACK			:	// Hack 탐
				default:
					{
						g_pLog->WriteLogSession( this, L"[ERROR]XTrap_CS_Step1 / res : %x", ui32Res );
						_SendErrorMessage( i32WorkThread, (EVENT_ERROR)ui32Res, 0, KILL_REASON_GAME_GUARD );
					}
					break;
				}
			}
			break;
		case GAMEGUARD_GPK:
			{
			}
			break;
		case GAMEGUARD_USG:
		{
			i3NetworkPacket USGPack(PROTOCOL_BASE_GAMEGUARD_REQ);			
			SendPacketMessage(&USGPack);
		}
		break;
	}

	m_ui32ATGameGuardSend = i3ThreadTimer::GetServerTime();
	m_ui32ATGameGuardRecv = m_ui32ATGameGuardSend-1;

	return; 
}

void CUserSession::SendClanMemberInfo( UINT8 ui8TotalCount, CLANMEMBER_INFO_STRUCT* pClanMember )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MEMBER_INFO_ACK );
	SendPacket.WriteData( &ui8TotalCount,		sizeof(UINT8) );

	for( UINT8 i = 0 ; i < ui8TotalCount ; i++ )
	{
		SendPacket.WriteData( &pClanMember[i]._NickSize,	sizeof(UINT8) );
		SendPacket.WriteData( pClanMember[i]._NickName,		sizeof(TTCHAR) * pClanMember[i]._NickSize );
		SendPacket.WriteData( &pClanMember[i]._BasicInfo,	sizeof(CLANMEMBER_INFO) );
	}

	SendPacketMessage(&SendPacket);
}

void CUserSession::SendClanMemberInfoChange( T_UID i64UID, UINT64 ui64State )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MEMBER_INFO_CHANGE_ACK );
	SendPacket.WriteData( &i64UID,		sizeof(T_UID) );
	SendPacket.WriteData( &ui64State,	sizeof(UINT64) );	
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendClanMemberInfoInsert( CLANMEMBER_INFO_STRUCT* pClanMember )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MEMBER_INFO_INSERT_ACK );
	SendPacket.WriteData( &pClanMember->_NickSize,		sizeof(UINT8) );
	SendPacket.WriteData( pClanMember->_NickName,		sizeof(TTCHAR) * pClanMember->_NickSize );
	SendPacket.WriteData( &pClanMember->_BasicInfo,		sizeof(CLANMEMBER_INFO) );
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendClanMemberInfoDelete( T_UID i64UID )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MEMBER_INFO_DELETE_ACK );
	SendPacket.WriteData( &i64UID,		sizeof(T_UID) );
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendClanCreate_C(INT32 i32Result, CLAN_DETAIL_INFO * pDetailInfo)
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CREATE_CLAN_ACK );
	SendPacket.WriteData( &i32Result,	sizeof(INT32) );

	if( i32Result > SERVER_VALUE_UNKNOWN )
	{//클랜 만들기 성공
		//DataBase에는 넣지 않음
		m_UserInfoBasic.m_ui32Clanidx		= pDetailInfo->m_ui32CID;
		m_UserInfoBasic.m_ui32ClanState		= CLAN_MEMBER_LEVEL_MASTER;
		m_UserInfoBasic.m_ui8ClanExpRank	= pDetailInfo->m_expRank;
		m_UserInfoBasic.m_ui8ClanPerRank	= pDetailInfo->m_personRank;
		m_UserInfoBasic.m_ui32ClanMark		= pDetailInfo->m_mark;
		m_UserInfoBasic.m_ui8ClanNamecolor	= pDetailInfo->m_color;

		i3String::Copy( m_UserInfoBasic.m_strClanName, pDetailInfo->m_name, NET_CLAN_NAME_SIZE );

		INT32 i32SlotIdx = SERVER_VALUE_UNKNOWN;
		if( m_i32SlotIdx != SERVER_VALUE_UNKNOWN )		i32SlotIdx = m_i32SlotIdx;

		if( m_pRoom && m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
		{
			SLOT_INFO*	pSlotInfo =	m_pRoom->GetSlotInfo();
			if( pSlotInfo && i32SlotIdx != SERVER_VALUE_UNKNOWN )
			{
				pSlotInfo[ i32SlotIdx ]._clanidx		= pDetailInfo->m_ui32CID;
				pSlotInfo[ i32SlotIdx ]._clanstate	= CLAN_MEMBER_LEVEL_MASTER;
				pSlotInfo[ i32SlotIdx ]._clanExpRank	= pDetailInfo->m_expRank;
				pSlotInfo[ i32SlotIdx ]._clanMark		= pDetailInfo->m_mark;
			}
		}

		//포인트 줄이기 
		if(m_UserInfoBasic.m_ui32Point > (UINT32)g_pContextMain->m_i32ClanCreatePoint)
			m_UserInfoBasic.m_ui32Point -= g_pContextMain->m_i32ClanCreatePoint; 
		else 
			m_UserInfoBasic.m_ui32Point = 0; 		
		
		SendPacket.WriteData( pDetailInfo,	sizeof(CLAN_DETAIL_INFO) );
		SendPacket.WriteData( &m_UserInfoBasic.m_ui32Point ,	sizeof(UINT32) );

		ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO | SAVE_DATABASE_CLAN );

		// LOG_MESSAGE_U_CREATE_CLAN 하나로 두번 호출하지만 ui8Type으로 클랜 생성과 포인트소모 - 클랜생성 두개를 다 남깁니다.
		g_pModuleContextNC->LogSendMessage( MODULE_CLAN_THREAD_IDX, LOG_MESSAGE_U_CREATE_CLAN, this );
		UINT8 ui8Type = 1;
		g_pModuleContextNC->LogSendMessage( MODULE_CLAN_THREAD_IDX, LOG_MESSAGE_U_CREATE_CLAN, this, &ui8Type );
	}

	SendPacketMessage(&SendPacket); 
	return; 
}

void CUserSession::SendClanJoinRequest(INT32 i32Result, INT32 i32ClanIdx)
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_JOIN_REQUEST_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));

	if( i32Result > SERVER_VALUE_UNKNOWN )
	{
		m_UserInfoBasic.m_ui32Clanidx	= i32ClanIdx;							//가입되었음
		m_UserInfoBasic.m_ui32ClanState	= CLAN_MEMBER_LEVEL_UNKNOWN;			//가입신청 대기중
		SendPacket.WriteData(&i32ClanIdx, sizeof(INT32));

		g_pModuleContextNC->LogSendMessage( MODULE_CLAN_THREAD_IDX, LOG_MESSAGE_U_CLAN_JOIN_REQ, this ); 

		ChangeDataBaseInfo( SAVE_DATABASE_CLAN );
	}
	SendPacketMessage(&SendPacket); 
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanCancleRequest(INT32 i32Result)
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CANCEL_REQUEST_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));

	if( i32Result > SERVER_VALUE_UNKNOWN )
	{
		ResetClanInfo();	
	}
	SendPacketMessage(&SendPacket); 
	return; 
}

/*-----------------------------------------------------------
Name : SendClanListDetailInfo
Desc : 클랜 리스트에서 특정 클랜 선택시 해당 클랜의 상세정보 요청
-----------------------------------------------------------*/
void CUserSession::SendClanListDetailInfo(INT32 i32Result, CLAN_DETAIL_INFO * pDetailInfo)
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	if( EV_SUCCESSED( i32Result ) )
	{
		SendPacket.WriteData(pDetailInfo, sizeof(CLAN_DETAIL_INFO));
	}
	SendPacketMessage(&SendPacket); 
	return; 
}

/*-----------------------------------------------------------
Name : SendClanDetailInfo
Desc : 클랜 상세정보_클랜정보
-----------------------------------------------------------*/
void CUserSession::SendClanDetailInfo(INT32 i32Result, CLAN_DETAIL_INFO * pDetailInfo, CLAN_TOP_MEMBER * pTopMember, CLAN_NEWS * pNews)
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_DETAIL_INFO_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	if( EV_SUCCESSED( i32Result ) )
	{
		SendPacket.WriteData(pDetailInfo,	sizeof(CLAN_DETAIL_INFO));
		SendPacket.WriteData(pTopMember,	sizeof(CLAN_TOP_MEMBER));
		SendPacket.WriteData(pNews,			sizeof(CLAN_NEWS));
	}
	SendPacketMessage(&SendPacket); 
	return; 
}

/*-----------------------------------------------------------
Name : SendClanMemberContext
Desc : 멤버 Context
-----------------------------------------------------------*/
void CUserSession::SendClanMemberContext(INT32 i32Result, CLAN_MEMBER_CONTEXT * pMemberContext)
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MEMBER_CONTEXT_ACK ); 
	SendPacket.WriteData(&i32Result,	sizeof(INT32));
	SendPacket.WriteData(pMemberContext, sizeof(CLAN_MEMBER_CONTEXT));
	SendPacketMessage(&SendPacket); 
	return; 
}

/*-----------------------------------------------------------
Name : SendClanMemberList
Desc : 멤버 리스트
-----------------------------------------------------------*/
void CUserSession::SendClanMemberList(INT32 i32Result, UINT8 ui8section, UINT8 ui8count, CLAN_MEMBER_BASIC_INFO * pMemberlist)		//멤버리스트
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MEMBER_LIST_ACK ); 
	SendPacket.WriteData(&i32Result,	sizeof(INT32));
	SendPacket.WriteData(&ui8section,	sizeof(UINT8)); 
	SendPacket.WriteData(&ui8count,		sizeof(UINT8)); 
	SendPacket.WriteData(pMemberlist,	(sizeof(CLAN_MEMBER_BASIC_INFO) * ui8count)); 
	SendPacketMessage(&SendPacket); 
	return; 
}

/*-----------------------------------------------------------
Name : SendClanMemberDetailInfo
Desc : 멤버 상세 정보
-----------------------------------------------------------*/
void CUserSession::SendClanMemberDetailInfo(INT32 i32Result, USER_INFO_RECORD* pRecord )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MEMBER_DETAIL_INFO_ACK ); 
	SendPacket.WriteData(&i32Result,	sizeof(INT32));
	SendPacket.WriteData(pRecord,		sizeof(USER_INFO_RECORD));
	SendPacketMessage(&SendPacket); 
	return; 
}

/*-----------------------------------------------------------
Name : SendClanMatchResultContext
Desc : 클랜 매치 결과 Context
-----------------------------------------------------------*/
void CUserSession::SendClanMatchResultContext(PACKET_H_CLAN_ACK* pPacketAckH, PACKET_D_CS_CLAN_MATCH_RESULT_CONTEXT* pPacketAckD)
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CLAN_MATCH_RESULT_CONTEXT_ACK ); 
	SendPacket.WriteData(pPacketAckH, sizeof(PACKET_H_CLAN_ACK));
	SendPacket.WriteData(pPacketAckD, sizeof(PACKET_D_CS_CLAN_MATCH_RESULT_CONTEXT));
	SendPacketMessage(&SendPacket); 
	return; 
}

/*-----------------------------------------------------------
Name : SendClanMatchResultList
Desc : 클랜 매치 결과 정보
-----------------------------------------------------------*/
void CUserSession::SendClanMatchResultList(PACKET_H_CLAN_ACK* pPacketAckH, PACKET_D_CS_CLAN_MATCH_RESULT_LIST_CNT* pPacketAckDCnt, PACKET_D_CS_CLAN_MATCH_RESULT_LIST* pPacketAckD)
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_ACK ); 
	SendPacket.WriteData( pPacketAckH,		sizeof(PACKET_H_CLAN_ACK) );
	SendPacket.WriteData( pPacketAckDCnt,	sizeof(PACKET_D_CS_CLAN_MATCH_RESULT_LIST_CNT) ); 	
	SendPacket.WriteData( pPacketAckD,		(sizeof(PACKET_D_CS_CLAN_MATCH_RESULT_LIST) * pPacketAckDCnt->m_ui8iSection) ); 
	SendPacketMessage( &SendPacket ); 
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanRequestContext(INT32 i32Result, CLAN_REQUEST_CONTEXT * pMemberContext)								//요청 멤버 
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REQUEST_CONTEXT_ACK ); 
	SendPacket.WriteData(&i32Result,		sizeof(INT32));
	SendPacket.WriteData(pMemberContext,	sizeof(CLAN_REQUEST_CONTEXT));
	SendPacketMessage(&SendPacket); 
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanRequestList(INT32 i32Result, UINT8 ui8section, UINT8 ui8count, CLAN_REQUEST_BASIC_INFO * pMemberlist)	//요청 멤버 리스트 
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REQUEST_LIST_ACK ); 
	SendPacket.WriteData(&i32Result,	sizeof(INT32));
	SendPacket.WriteData(&ui8section,	sizeof(UINT8)); 
	SendPacket.WriteData(&ui8count,		sizeof(UINT8)); 
	SendPacket.WriteData(pMemberlist,	(sizeof(CLAN_REQUEST_BASIC_INFO) * ui8count)); 
	SendPacketMessage(&SendPacket); 
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanRequestDetailInfo(INT32 i32Result, CLAN_REQUEST_DETAIL_INFO * pClanRequestDetailInfo)				//요청 멤버 정보 디테일
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REQUEST_INFO_ACK ); 
	SendPacket.WriteData(&i32Result,				sizeof(INT32));
	SendPacket.WriteData(pClanRequestDetailInfo,	sizeof(CLAN_REQUEST_DETAIL_INFO));
	SendPacketMessage(&SendPacket);
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanAcceptRequest(INT32 i32Result)	//가입신청서 수락
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_ACCEPT_REQUEST_ACK ); 
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanDenialRequest(INT32 i32Result)	
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_DENIAL_REQUEST_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanChatingRequest( UINT8 ui8NickSize, TTCHAR* pstrNick, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser, UINT8 ui8NickColor )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CHATTING_ACK );	
	SendPacket.WriteData(&ui8NickSize,	sizeof(UINT8) );
	SendPacket.WriteData(pstrNick,		sizeof(TTCHAR) * ui8NickSize );
	SendPacket.WriteData(&bIsGMUser,	sizeof(bool) );
	SendPacket.WriteData(&ui8NickColor,	sizeof(UINT8) );
	SendPacket.WriteData(&ui8ChatSize,	sizeof(UINT8) );
	SendPacket.WriteData(pstrChating,	sizeof(TTCHAR) * ui8ChatSize );

	SendPacketMessage(&SendPacket);

	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanPageChatingRequest( UINT8 ui8NickSize, TTCHAR* pstrNick, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser, UINT8 ui8NickColor )
{
	INT32	i32State = g_ai32ChatLimitTime[CHAT_LIMIT_STATE_NORMAL];
	i3NetworkPacket SendPacket( PROTOCOL_CS_PAGE_CHATTING_ACK );
	SendPacket.WriteData(&i32State,		sizeof(INT32) );
	SendPacket.WriteData(&ui8NickSize,	sizeof(UINT8) );
	SendPacket.WriteData(pstrNick,		sizeof(TTCHAR) * ui8NickSize );	
	SendPacket.WriteData(&bIsGMUser,	sizeof(bool) );
	SendPacket.WriteData(&ui8NickColor,	sizeof(UINT8) );
	SendPacket.WriteData(&ui8ChatSize,	sizeof(UINT8) );
	SendPacket.WriteData(pstrChating,	sizeof(TTCHAR) * ui8ChatSize );

	SendPacketMessage(&SendPacket);

	return; 
}


/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanSecessionRequest( INT32 i32Result )	
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_SECESSION_CLAN_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);

	if( EV_SUCCESSED( i32Result ) )
	{
		ResetClanInfo();

		INT32 nSlotIdx = SERVER_VALUE_UNKNOWN;
		if( m_i32SlotIdx != SERVER_VALUE_UNKNOWN )		nSlotIdx = m_i32SlotIdx;

		if( m_pRoom && m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
		{
			SLOT_INFO*	pSlotInfo =	m_pRoom->GetSlotInfo();
			if( pSlotInfo && nSlotIdx != SERVER_VALUE_UNKNOWN )
			{
				pSlotInfo[ nSlotIdx ]._clanidx		= 0;
				pSlotInfo[ nSlotIdx ]._clanstate	= CLAN_MEMBER_LEVEL_UNKNOWN;
				pSlotInfo[ nSlotIdx ]._clanMark		= CLAN_MARK_DEFAULT;
				pSlotInfo[ nSlotIdx ]._ClanName[0]	= '\0';
			}
		}
	}
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanCloseRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CLOSE_CLAN_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);

	if( EV_SUCCESSED( i32Result ) )
	{
		ResetClanInfo();

		INT32 nSlotIdx = SERVER_VALUE_UNKNOWN;
		if( m_i32SlotIdx != SERVER_VALUE_UNKNOWN )		nSlotIdx = m_i32SlotIdx;

		if( m_pRoom && m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
		{
			SLOT_INFO*	pSlotInfo =	m_pRoom->GetSlotInfo();
			if( pSlotInfo && nSlotIdx != SERVER_VALUE_UNKNOWN )
			{
				pSlotInfo[ nSlotIdx ]._clanidx		= 0;
				pSlotInfo[ nSlotIdx ]._clanstate	= CLAN_MEMBER_LEVEL_UNKNOWN;
				pSlotInfo[ nSlotIdx ]._clanMark		= CLAN_MARK_DEFAULT;
				pSlotInfo[ nSlotIdx ]._ClanName[0]	= '\0';
			}
		}
	}
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanCommissionMasterRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_COMMISSION_MASTER_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
	
	if( EV_SUCCESSED( i32Result ) )
	{
		m_UserInfoBasic.m_ui32ClanState	= CLAN_MEMBER_LEVEL_STAFF;
		ChangeDataBaseInfo( SAVE_DATABASE_CLAN );
	}

	return; 
}

void CUserSession::SendClanCommissionStaffRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_COMMISSION_STAFF_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);

	return; 
}

void CUserSession::SendClanCommissionRegularRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_COMMISSION_REGULAR_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);

	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanDeportationRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_DEPORTATION_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
	
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanReplaceNoticeRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_NOTICE_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
	
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanReplaceIntroRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_INTRO_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
	
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanNoteRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_NOTE_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
	
	return; 
}

//-----------------------------------------------------------
// Name : SendClanReplaceNameRequest
// Desc : Mark
//-----------------------------------------------------------
void CUserSession::SendClanReplaceNameRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_NAME_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
	
	return; 
}

//-----------------------------------------------------------
// Name : SendClanCheckMarkRequest
// Desc : Mark
//-----------------------------------------------------------
void CUserSession::SendClanCheckMarkRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CHECK_MARK_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
	
	return; 
}

//-----------------------------------------------------------
// Name : SendClanChangeExpRankRequest
// Desc : Exp 변경
//-----------------------------------------------------------
void CUserSession::SendClanChangeExpRankRequest( UINT8 ui8Rank )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CHANGE_CLAN_EXP_RANK_ACK );
	SendPacket.WriteData(&ui8Rank, sizeof(UINT8));
	SendPacketMessage(&SendPacket);

	m_UserInfoBasic.m_ui8ClanExpRank = ui8Rank;
	
	INT32 i32SlotIdx = SERVER_VALUE_UNKNOWN;
	if( m_i32SlotIdx != SERVER_VALUE_UNKNOWN )		i32SlotIdx = m_i32SlotIdx;

	if( m_pRoom && m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
	{
		SLOT_INFO*	pSlotInfo =	m_pRoom->GetSlotInfo();
		if( pSlotInfo && i32SlotIdx != SERVER_VALUE_UNKNOWN )
		{
			pSlotInfo[ i32SlotIdx ]._clanExpRank	= ui8Rank;
		}
	}
}

//-----------------------------------------------------------
// Name : SendClanChangePerRankRequest
// Desc : Per 변경
//-----------------------------------------------------------
void CUserSession::SendClanChangePerRankRequest( UINT8 ui8Rank )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CHANGE_CLAN_PER_RANK_ACK );
	SendPacket.WriteData(&ui8Rank, sizeof(UINT8));
	SendPacketMessage(&SendPacket);

	m_UserInfoBasic.m_ui8ClanPerRank = ui8Rank;
}

//-----------------------------------------------------------
// Name : SendClanReplaceMarkResltRequest
// Desc : Mark 변경
//-----------------------------------------------------------
void CUserSession::SendClanReplaceMarkResltRequest( UINT32 ui32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_MARK_RESULT_ACK );
	SendPacket.WriteData(&ui32Result, sizeof(UINT32));
	SendPacketMessage(&SendPacket);

	m_UserInfoBasic.m_ui32ClanMark = ui32Result;

	INT32 i32SlotIdx = SERVER_VALUE_UNKNOWN;
	if( m_i32SlotIdx != SERVER_VALUE_UNKNOWN )		i32SlotIdx = m_i32SlotIdx;

	if( m_pRoom && m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
	{
		SLOT_INFO*	pSlotInfo =	m_pRoom->GetSlotInfo();
		if( pSlotInfo && i32SlotIdx != SERVER_VALUE_UNKNOWN )
		{
			pSlotInfo[ i32SlotIdx ]._clanMark	= ui32Result;
		}
	}
}

//-----------------------------------------------------------
// Name : SendClanReplaceManagementResltRequest
// Desc : 클랜정보 변경
//-----------------------------------------------------------
void CUserSession::SendClanReplaceManagementResltRequest( UINT8 ui8Authority )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_MANAGEMENT_RESULT_ACK );
	SendPacket.WriteData(&ui8Authority, sizeof(UINT8));
	SendPacketMessage(&SendPacket);
}

//-----------------------------------------------------------
// Name : SendClanReplaceNameResultRequest
// Desc : Name 변경
//-----------------------------------------------------------
void CUserSession::SendClanReplaceNameResultRequest( TTCHAR* pstrName )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_NAME_RESULT_ACK );
	SendPacket.WriteData(pstrName, sizeof(TTCHAR) * NET_CLAN_NAME_SIZE);
	SendPacketMessage(&SendPacket);

	i3String::Copy( m_UserInfoBasic.m_strClanName, pstrName, NET_CLAN_NAME_SIZE );
}

//-----------------------------------------------------------
// Name : SendClanReplaceMarkRequest
// Desc : Mark
//-----------------------------------------------------------
void CUserSession::SendClanReplaceMarkRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_MARK_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
	
	return; 
}

//-----------------------------------------------------------
// Name : SendClanReplacePersonMaxRequest
// Desc : Mark
//-----------------------------------------------------------
void CUserSession::SendClanReplacePersonMaxRequest( INT32 i32Result, UINT8 ui8ClanMaxCount )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_PERSONMAX_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));

	if( EV_SUCCESSED(i32Result) )
	{
		SendPacket.WriteData(&ui8ClanMaxCount, sizeof(UINT8));
	}

	SendPacketMessage(&SendPacket);
	
	return; 
}

//-----------------------------------------------------------
// Name : SendClanReplaceManagementRequest
// Desc : Mark
//-----------------------------------------------------------
void CUserSession::SendClanReplaceManagementRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_MANAGEMENT_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
	
	return; 
}

//-----------------------------------------------------------
// Name : SendClanCheckJoinAuthorityRequest
// Desc : Mark
//-----------------------------------------------------------
void CUserSession::SendClanCheckJoinAuthorityRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CHECK_JOIN_AUTHORITY_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
	
	return; 
}

//-----------------------------------------------------------
// Name : SendClanRecordResetRequest
// Desc : Mark
//-----------------------------------------------------------
void CUserSession::SendClanRecordResetRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_RECORD_RESET_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
	
	return; 
}

//-----------------------------------------------------------
// Name : SendClanAcceptRequestError
// Desc : Mark
//-----------------------------------------------------------
void CUserSession::SendClanAcceptRequestError( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_INVITE_ACCEPT_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);

	return; 
}

//-----------------------------------------------------------
// Name : 
// Desc : 
//-----------------------------------------------------------
void CUserSession::SendClanAcceptResultRequest( CLAN_DETAIL_INFO* pDetailInfo )	
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_ACCEPT_REQUEST_RESULT_ACK );
	SendPacket.WriteData(pDetailInfo,	sizeof(CLAN_DETAIL_INFO));	

	m_bVisitClanPageEnter = false;	// 이제 클랜원이 되었으니

	m_UserInfoBasic.m_ui32Clanidx		= pDetailInfo->m_ui32CID;
	m_UserInfoBasic.m_ui32ClanState		= CLAN_MEMBER_LEVEL_REGULAR;
	m_UserInfoBasic.m_ui8ClanExpRank	= pDetailInfo->m_expRank;
	m_UserInfoBasic.m_ui8ClanPerRank	= pDetailInfo->m_personRank;
	m_UserInfoBasic.m_ui32ClanMark		= pDetailInfo->m_mark;
	m_UserInfoBasic.m_ui8ClanNamecolor	= pDetailInfo->m_color;

	i3String::Copy( m_UserInfoBasic.m_strClanName, pDetailInfo->m_name, NET_CLAN_NAME_SIZE );


	INT32 i32SlotIdx = SERVER_VALUE_UNKNOWN;
	if( m_i32SlotIdx != SERVER_VALUE_UNKNOWN )		i32SlotIdx = m_i32SlotIdx;

	//대기방에 있는경우 슬롯정보를 갱신합니다.
	if (m_pRoom && m_i32RoomIdx != SERVER_VALUE_UNKNOWN)
	{
		SLOT_INFO*	pSlotInfo = m_pRoom->GetSlotInfo(i32SlotIdx);
		if (pSlotInfo)
		{
			pSlotInfo->_clanidx = pDetailInfo->m_ui32CID;
			pSlotInfo->_clanstate = CLAN_MEMBER_LEVEL_REGULAR;
			pSlotInfo->_clanExpRank = pDetailInfo->m_expRank;
			pSlotInfo->_clanMark = pDetailInfo->m_mark;

			//대기방의 모든유저에게 가입한 클랜정보를 뿌린다.
			PACKET_ROOM_GET_SLOTONEINFO_ACK	Packet;
			S2_SLOT_TOTAL_INFO* pPacketSlot = &Packet.m_SlotTotalInfo;

			i3mem::Copy(pPacketSlot, pSlotInfo, sizeof(SLOT_INFO));
			pPacketSlot->m_ui8SlotIdx = (UINT8)i32SlotIdx;
			i3String::Copy(pPacketSlot->m_strNickName, m_UserInfoBasic.m_strNick, NET_NICK_NAME_SIZE);
			pPacketSlot->m_ui8NickColor = m_UserInfoInvItemData._ui8NickColor;
			pPacketSlot->m_ui16MuzzleFlashColor = m_UserInfoInvItemData._ui16MuzzleFlashColor;

			for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
			{
				if (m_pRoom->m_Info._aSlotInfo[i]._State <= SLOT_STATE_CLOSE)	continue;

				m_pRoom->m_Info._pSession[i]->S2MOSendEvent(&Packet);
			}

		}
	}
	ChangeDataBaseInfo( SAVE_DATABASE_CLAN );

	SendPacketMessage(&SendPacket);

	return; 
}

//-----------------------------------------------------------
// Name : 
// Desc : 
//-----------------------------------------------------------
void CUserSession::SendClanDenialResultRequest()	
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_DENIAL_REQUEST_RESULT_ACK );	
	SendPacketMessage(&SendPacket);

	m_UserInfoBasic.m_ui32Clanidx	= 0;
	m_UserInfoBasic.m_ui32ClanState	= CLAN_MEMBER_LEVEL_UNKNOWN;

	ChangeDataBaseInfo( SAVE_DATABASE_CLAN );
	
	return; 
}

//-----------------------------------------------------------
// Name : 
// Desc : 
//-----------------------------------------------------------
void CUserSession::SendClanDeportationResultRequest()	
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_DEPORTATION_RESULT_ACK );	
	SendPacketMessage(&SendPacket);

	ResetClanInfo();

	INT32 i32SlotIdx = SERVER_VALUE_UNKNOWN;
	if( m_i32SlotIdx != SERVER_VALUE_UNKNOWN )		i32SlotIdx = m_i32SlotIdx;

	if( m_pRoom && m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
	{
		SLOT_INFO*	pSlotInfo =	m_pRoom->GetSlotInfo();
		if( pSlotInfo && i32SlotIdx != SERVER_VALUE_UNKNOWN )
		{
			pSlotInfo[ i32SlotIdx ]._clanidx		= 0;
			pSlotInfo[ i32SlotIdx ]._clanstate		= CLAN_MEMBER_LEVEL_UNKNOWN;
			pSlotInfo[ i32SlotIdx ]._clanMark		= CLAN_MARK_DEFAULT;
			pSlotInfo[ i32SlotIdx ]._ClanName[0]	= '\0';
		}
	}
	
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanMasterCommissionResultRequest( )	
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_COMMISSION_MASTER_RESULT_ACK );	
	SendPacketMessage(&SendPacket);

	// 마스터 임명
	m_UserInfoBasic.m_ui32ClanState	= CLAN_MEMBER_LEVEL_MASTER;	
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanStaffCommissionResultRequest( )	
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_COMMISSION_STAFF_RESULT_ACK );
	SendPacketMessage(&SendPacket);

	// 스텝 임명
	m_UserInfoBasic.m_ui32ClanState	= CLAN_MEMBER_LEVEL_STAFF;
	
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanRegularCommissionResultRequest( )	
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_COMMISSION_REGULAR_RESULT_ACK );
	SendPacketMessage(&SendPacket);

	// 일반 임명
	m_UserInfoBasic.m_ui32ClanState	= CLAN_MEMBER_LEVEL_REGULAR;

	ChangeDataBaseInfo( SAVE_DATABASE_CLAN );
	
	return; 
}

//-----------------------------------------------------------
// Name : SendClanRoomInvitedRequest
// Desc : Mark
//-----------------------------------------------------------
void CUserSession::SendClanRoomInvitedRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_ROOM_INVITED_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
	
	return; 
}

/*-----------------------------------------------------------
Name : SendClanRoomInvitedResultRequest()
Desc : 클랜원을 배틀룸에 초대시 초대 성공시 수신자에게 초대 메시지 전달
-----------------------------------------------------------*/
void CUserSession::SendClanRoomInvitedResultRequest( T_UID TSenderUID )	
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_ROOM_INVITED_RESULT_ACK );
	SendPacket.WriteData(&TSenderUID, sizeof(T_UID));
	SendPacketMessage(&SendPacket);

	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanReplaceMemberBasicResultRequest( CLAN_MEMBER_BASIC_INFO* pMemberBasicInfo )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_MEMBER_BASIC_RESULT_ACK );
	SendPacket.WriteData(pMemberBasicInfo,	sizeof(CLAN_MEMBER_BASIC_INFO));
	SendPacketMessage(&SendPacket);

	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanRecordResetResultRequest( )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_RECORD_RESET_RESULT_ACK );
	SendPacketMessage(&SendPacket);

	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanPointResetResultRequest()
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_POINT_RESET_RESULT_ACK );
	SendPacketMessage(&SendPacket);

	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanReplacColorNameResultRequest( UINT8 ui8Color )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_COLOR_NAME_RESULT_ACK );
	SendPacket.WriteData(&ui8Color,	sizeof(UINT8));
	SendPacketMessage(&SendPacket);

	m_UserInfoBasic.m_ui8ClanNamecolor = ui8Color;

	return; 
}

/*-----------------------------------------------------------
Name : SendClanMatchResultEmotionSave
Desc : 클랜전 매치 정보 감정표현(좋아요) 응답
-----------------------------------------------------------*/
void CUserSession::SendClanMatchResultEmotionSave(PACKET_H_CLAN_ACK* pPacketAckH, PACKET_D_CS_CLAN_MATCH_RESULT_EMOTION_SAVE* pPacketAckD)
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_ACK ); 
	SendPacket.WriteData(pPacketAckH,	sizeof(PACKET_H_CLAN_ACK));
	SendPacket.WriteData(pPacketAckD,	sizeof(PACKET_D_CS_CLAN_MATCH_RESULT_EMOTION_SAVE)); 	
	SendPacketMessage(&SendPacket); 
	return; 
}
void CUserSession::SendClanMatchResultEmotionSaveFail(PACKET_H_CLAN_ACK* pPacketAckH)
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_ACK ); 
	SendPacket.WriteData(pPacketAckH,	sizeof(PACKET_H_CLAN_ACK));
	SendPacketMessage(&SendPacket); 
	return; 
}

/*-----------------------------------------------------------
Name : SendClanRequestAutoAcceptOption
Desc : 클랜 가입신청서 (자동,수동) 승인 설정 응답(클랜마스터만 가능)
-----------------------------------------------------------*/
void CUserSession::SendClanRequestAutoAcceptOption(PACKET_H_CLAN_ACK* pPacketAckH)
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_ACK ); 
	SendPacket.WriteData(pPacketAckH,	sizeof(PACKET_H_CLAN_ACK));
	SendPacketMessage(&SendPacket); 
	return; 
}

// -----------------------------------------------------------
// Name : SendClanMatchTeamContextRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchTeamContextRequest( CLAN_BATTLE_TEAM_CONTEXT* pTeamContext )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_TEAM_CONTEXT_ACK );
	SendPacket.WriteData(pTeamContext,	sizeof(CLAN_BATTLE_TEAM_CONTEXT));
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchAllTeamContextRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchAllTeamContextRequest( CLAN_BATTLE_ALL_TEAM_CONTEXT* pAllTeamContext )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_ALL_TEAM_CONTEXT_ACK );
	SendPacket.WriteData(pAllTeamContext,	sizeof(CLAN_BATTLE_ALL_TEAM_CONTEXT));
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchAllTeamListRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchAllTeamListRequest( UINT16 ui16TotalCount, UINT16 ui16SectionCount, UINT16 ui16Section, UINT8 ui8Count, CLAN_BATTLE_TEAM_INFO* pBattleTeamInfo )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK );
	SendPacket.WriteData(&ui16TotalCount,	sizeof(UINT16));
	SendPacket.WriteData(&ui16SectionCount,	sizeof(UINT16));
	SendPacket.WriteData(&ui16Section,		sizeof(UINT16));
	SendPacket.WriteData(&ui8Count,			sizeof(UINT8));
	SendPacket.WriteData(pBattleTeamInfo,	sizeof(CLAN_BATTLE_TEAM_INFO) * ui8Count );
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchTeamDetailRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchTeamDetailRequest( INT32 i32Result, CLAN_DETAIL_INFO* pDetail )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_ACK );
	SendPacket.WriteData(&i32Result,	sizeof(INT32));
	SendPacket.WriteData(pDetail,		sizeof(CLAN_DETAIL_INFO));
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchTeamCreateRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchTeamCreateRequest( INT32 i32Success, UINT32 ui32Idx, CLAN_BATTLE_TEAM* pTeam )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_TEAM_CREATE_ACK );
	SendPacket.WriteData(&i32Success,	sizeof(INT32));
	SendPacket.WriteData(&ui32Idx,		sizeof(UINT32));
	SendPacket.WriteData(pTeam,			sizeof(CLAN_BATTLE_TEAM));
	SendPacketMessage(&SendPacket);

	//if( EV_SUCCESSED( i32Success ) )
	//{
	//	m_TTeamID = 1;
	//}
}
// -----------------------------------------------------------
// Name : SendClanMatchTeamLeaveRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchTeamLeaveRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_TEAM_LEAVE_ACK );
	SendPacket.WriteData(&i32Result,	sizeof(INT32));
	SendPacketMessage(&SendPacket);

	//if( EV_SUCCESSED( i32Result ) )
	//{
	//	m_TTeamID = 1;
	//}
}

// -----------------------------------------------------------
// Name : SendClanMatchTeamChangePerRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchTeamChangePerRequest( INT32 i32Result, UINT8 ui8PerMax )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK );
	SendPacket.WriteData(&i32Result,	sizeof(INT32));
	SendPacket.WriteData(&ui8PerMax,	sizeof(UINT8));
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchTeamChangePerResultRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchTeamChangePerResultRequest( UINT8 ui8PerMax )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_RESULT_ACK );
	SendPacket.WriteData(&ui8PerMax,	sizeof(UINT8));
	SendPacketMessage(&SendPacket);
}

//// -----------------------------------------------------------
//// Name : SendClanMatchTeamInfoRequest
//// Desc : 
//// -----------------------------------------------------------
//void CUserSession::SendClanMatchTeamInfoRequest( CLAN_BATTLE_TEAM* pTeam, CLAN_BATTLE_MEMBER* pMemberInfo )
//{
//	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_TEAM_INFO_ACK );
//	SendPacket.WriteData(pTeam,			sizeof(CLAN_BATTLE_TEAM));
//	SendPacket.WriteData(pMemberInfo,	sizeof(CLAN_BATTLE_MEMBER) * pTeam->_PerMax);
//	SendPacketMessage(&SendPacket);
//}

// -----------------------------------------------------------
// Name : SendClanMatchFightRequestRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchFightRequestRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK );
	SendPacket.WriteData(&i32Result,	sizeof(INT32));
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchChatingRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchChatingRequest(UINT8 ui8NickSize, TTCHAR* pstrNick, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser, UINT8 ui8NickColor )
{
	INT32	i32State = g_ai32ChatLimitTime[CHAT_LIMIT_STATE_NORMAL];
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_CHATING_ACK );
	SendPacket.WriteData(&i32State,			sizeof(INT32));
	SendPacket.WriteData(&ui8NickSize,		sizeof(UINT8));
	SendPacket.WriteData(pstrNick,			sizeof(TTCHAR) * ui8NickSize);
	SendPacket.WriteData(&bIsGMUser,		sizeof(bool));	
	SendPacket.WriteData(&ui8NickColor,		sizeof(UINT8));	
	SendPacket.WriteData(&ui8ChatSize,		sizeof(UINT8));	
	SendPacket.WriteData(pstrChating,		sizeof(TTCHAR) * ui8ChatSize);
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchFightResultRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchFightResultRequest( CLAN_BATTLE_TEAM_INFO* pTeamInfo )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_FIGHT_REQUEST_RESULT_ACK );
	SendPacket.WriteData(pTeamInfo,	sizeof(CLAN_BATTLE_TEAM_INFO));
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchFightAccectRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchFightAccectRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_FIGHT_ACCECT_ACK );
	SendPacket.WriteData(&i32Result,	sizeof(INT32));
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchFightAccectResultRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchFightAccectResultRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_FIGHT_ACCECT_RESULT_ACK );
	SendPacket.WriteData(&i32Result,	sizeof(INT32));
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchFightAccectRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchFightIfoResultRequest( CLAN_BATTLE_TEAM* pBattleTeam, CLAN_BATTLE_CLAN* pBattleClan)
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_FIGHT_INFO_RESULT_ACK );
	SendPacket.WriteData(pBattleTeam,	sizeof(CLAN_BATTLE_TEAM));
	SendPacket.WriteData(pBattleClan,	sizeof(CLAN_BATTLE_CLAN));
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchFightAccectRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanBattleChangeClanInfo( CLAN_BATTLE_CLAN* pBattleClan )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_FIGHT_CHANGE_INFO_ACK );
	SendPacket.WriteData(pBattleClan,	sizeof(CLAN_BATTLE_CLAN));
	SendPacketMessage(&SendPacket);
}

/*-----------------------------------------------------------
Name : SendClanBattleRoomCreate
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanBattleRoomCreate( UINT32 ui32TeamIdx, UINT32 ui32RequsetTeamIdx, UINT8 ui8PerMax )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_ROOM_CREATE_GS_REQ );
	SendPacket.WriteData( &ui32TeamIdx,			sizeof(UINT32) );
	SendPacket.WriteData( &ui32RequsetTeamIdx,	sizeof(UINT32) );
	SendPacket.WriteData( &ui8PerMax,			sizeof(UINT8) );
	SendPacketMessage(&SendPacket);
}

/*-----------------------------------------------------------
Name : SendClanBattleRoomCreate
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanBattleRoomJoin( UINT32 ui32RoomIdx, UINT32 ui32MatchTeam )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_ROOM_JOIN_GS_REQ );
	SendPacket.WriteData( &ui32RoomIdx,		sizeof(UINT32) );
	SendPacket.WriteData( &ui32MatchTeam,	sizeof(UINT32) );
	SendPacketMessage(&SendPacket);
}

// End of Clan
//-----------------------------------------------------------

void CUserSession::SendMessengerSendNote(INT32 i32result)
{
	i3NetworkPacket SendPacket(PROTOCOL_MESSENGER_NOTE_SEND_ACK);
	SendPacket.WriteData(&i32result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendMessengerClanNote(INT32 i32result)
{
	i3NetworkPacket SendPacket(PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK);
	SendPacket.WriteData(&i32result, sizeof(INT32));
	SendPacketMessage(&SendPacket);
}


void CUserSession::SendMessengerNoteList(UINT8 ui8pageCount, UINT8 ui8noteCount, NOTE_HEADER * pnoteHeaderList, NOTE_BODY * pnoteBodyList)
{
	i3NetworkPacket SendPacket(PROTOCOL_MESSENGER_NOTE_LIST_ACK);
	SendPacket.WriteData(&ui8pageCount, sizeof(UINT8));
	SendPacket.WriteData(&ui8noteCount, sizeof(UINT8));
	SendPacket.WriteData(pnoteHeaderList, sizeof(NOTE_HEADER) * ui8noteCount);

	for(INT32 i = 0; i < ui8noteCount; i++)
	{
		NOTE_BODY* body = &pnoteBodyList[i];
		
		SendPacket.WriteData(&body->_senderSize,	sizeof(UINT8));
		SendPacket.WriteData(&body->_messageSize,	sizeof(UINT8));
		SendPacket.WriteData(body->_sender,			sizeof(TTCHAR) * body->_senderSize);
		if( 0 < body->_messageSize )
		{
            SendPacket.WriteData(body->_message,	sizeof(TTCHAR) * body->_messageSize);
		}
		else
		{
			SendPacket.WriteData(&body->_message[0], sizeof(TTCHAR));
			SendPacket.WriteData(&body->_message[1], sizeof(TTCHAR) * body->_message[0]);
		}
	}

	SendPacketMessage(&SendPacket);
}

void CUserSession::SendMessengerCheckReadedNote(UINT8 ui8readedCount, INT32* pi32noteIdList)
{
	i3NetworkPacket SendPacket(PROTOCOL_MESSENGER_NOTE_CHECK_READED_ACK);
	SendPacket.WriteData(&ui8readedCount,	sizeof(UINT8));
	SendPacket.WriteData(pi32noteIdList,	sizeof(INT32) * ui8readedCount);
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendMessengerDeleteNote(INT32 i32result, UINT8 ui8deleteCount, INT32* pi32noteIdList)
{
	i3NetworkPacket SendPacket(PROTOCOL_MESSENGER_NOTE_DELETE_ACK);
	SendPacket.WriteData(&i32result,		sizeof(INT32));
	SendPacket.WriteData(&ui8deleteCount,	sizeof(UINT8));
	SendPacket.WriteData(pi32noteIdList,	sizeof(INT32) * ui8deleteCount);
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendMessengerReceiveNote(NOTE_HEADER noteHeader, UINT8 ui8senderSize, UINT8 ui8messageSize, TTCHAR * sender, TTCHAR * message)
{
	i3NetworkPacket SendPacket(PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK);
	SendPacket.WriteData(&noteHeader,		sizeof(NOTE_HEADER));
	SendPacket.WriteData(&ui8senderSize,	sizeof(UINT8));
	SendPacket.WriteData(&ui8messageSize,	sizeof(UINT8));
	SendPacket.WriteData(sender,			sizeof(TTCHAR) * ui8senderSize);
	if( 0 < ui8messageSize )
	{
		SendPacket.WriteData(message,		sizeof(TTCHAR) * ui8messageSize);
	}
	else
	{
		SendPacket.WriteData(&message[0],	sizeof(TTCHAR));
		SendPacket.WriteData(&message[1],	sizeof(TTCHAR) * message[0]);
	}

	SendPacketMessage(&SendPacket);
}

void CUserSession::SendGetRemainMoney( INT32 i32Result, UINT32 ui32Point, UINT32 ui32Cash )
{	
	i3NetworkPacket packet(PROTOCOL_AUTH_GET_POINT_CASH_ACK);

	packet.WriteData(&i32Result,	sizeof(INT32));

	if (EV_SUCCESSED(i32Result))
	{
		// Point 는 게임서버에서 관리하기 때문에 더해줄 필요가 없습니다.
		//m_UserInfoBasic.m_ui32Point	+= ui32Point;
		m_UserInfoBasic.m_ui32Cash	= ui32Cash;

		//ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO); 

		packet.WriteData(&m_UserInfoBasic.m_ui32Point,	sizeof(UINT32));
		packet.WriteData(&m_UserInfoBasic.m_ui32Cash,	sizeof(UINT32));
	}

	SendPacketMessage(&packet);
}


void CUserSession::SendGetPoint( INT32 i32ThreadIdx, UINT32 ui32Point, UINT8 ui8GetRoute )
{	
	if( 0 == ui32Point )	return; 

	m_UserInfoBasic.m_ui32Point	+= ui32Point;
	ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO);

	INT32 i32GetRoute	= ui8GetRoute;
	g_pModuleContextNC->LogSendMessage( i32ThreadIdx, LOG_MESSAGE_U_GET_POINT, this, &ui32Point, &i32GetRoute );
	
	PACKET_SHOP_PLUS_POINT_ACK Packet;
	Packet.m_ui32GetPoint	= ui32Point;
	Packet.m_ui32Point		= m_UserInfoBasic.m_ui32Point;
	Packet.m_ui8GetRoute	= ui8GetRoute;
	S2MOSendEvent( &Packet );
}

void CUserSession::SendCheckNickName( INT32 i32Rv )
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_ACK );
	SendPacket.WriteData(&i32Rv,				sizeof(INT32));
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendNickName( INT32 i32WorkIdx, TTCHAR* strNickName )
{	
	if( '\0' == *strNickName )	return;

	TTCHAR strNickTemp[ NET_NICK_NAME_SIZE ];

	i3String::Copy( strNickTemp, m_UserInfoBasic.m_strNick, NET_NICK_NAME_SIZE );
	i3String::Copy( m_UserInfoBasic.m_strNick, strNickName, NET_NICK_NAME_SIZE );

	ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO);
	
	// 클랜서버에 닉네임 변경 통보
	g_pModuleClan->ClanSendChangeNickname( i32WorkIdx, this, m_UserInfoBasic.m_strNick );
	// 메신저서버에 닉네임 변경 통보
	g_pModuleMessenger->MessengerChangeNickname( i32WorkIdx, this );

	// 룸에도 닉네임 변경 통보
	if( SERVER_VALUE_UNKNOWN < m_i32RoomIdx )
	{
		m_pRoom->SendChangeNickname(m_i32SlotIdx);
	}

	g_pModuleContextNC->LogSendMessage( i32WorkIdx, LOG_MESSAGE_U_CHANGE_NICK, this, strNickTemp ); 

	// 클라이언트에 전달
	UINT8 ui8Size = (UINT8)i3String::Length( m_UserInfoBasic.m_strNick ) + 1;
	ui8Size *= sizeof(TTCHAR);
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_CHANGE_NICKNAME_ACK );
	SendPacket.WriteData(&ui8Size,				sizeof(UINT8));
	SendPacket.WriteData(m_UserInfoBasic.m_strNick,	sizeof(TTCHAR) * ui8Size );
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendUserRecord( USER_INFO_RECORD* pUserRecord )
{
	i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_RECORD_INFO_DB_ACK );
	SendPacket.WriteData(pUserRecord,		sizeof(USER_INFO_RECORD));
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : LogIn_MA
// Desc : 로그인 결과값입니다.
// -----------------------------------------------------------
void CUserSession::LogIn_MAU( INT32 i32WorkThread, INT32 i32Rv, UINT32 ui32BlockDate, LOGIN_INFO* pLoginInfo )
{
	//1. 로그인을 요청했는지
	if( 0 == m_ui32ATLogin )
	{	// 해킹으로 간주하고 끊어버립니다.
		_SendErrorMessage( i32WorkThread, EVENT_ERROR_LOGIN, 0, KILL_REASON_HACK_PACKET );
	}
	else
	{
		if( EV_SUCCESSED( i32Rv ) )
		{		
			m_i64UID	= pLoginInfo->m_TUID;	//정상 로그인
			i3mem::Copy( m_strId, pLoginInfo->m_strID, NET_ID_SIZE );

			// Trans 로 부터 Login ACK 를 받았을때만 메신저와 Clan 에 로그인 성공 패킷을 보냅니다.
			// 쓰레드 주의해 주세요.	
			// ClanServer LogIn - 위치 변경하지 말아야 합니다.
			if( FALSE == g_pModuleClan->ClanSendLogIn( i32WorkThread, this ) )
			{
				m_abGetInfoToServer[ GET_INFO_TO_SERVER_CLAN ] = true;
			}

			m_bCheckLogout = FALSE;

			if( DEF_TRANS_LINK_COUNT_MAX > pLoginInfo->m_ui8LinkThreadIdx )
			{
				g_pUserManager->m_ui32LinkLoginCount[ pLoginInfo->m_ui8LinkThreadIdx ]++;
				g_pUserManager->m_ui32LinkLoginTimeArg[ pLoginInfo->m_ui8LinkThreadIdx ] = pLoginInfo->m_ui32LoginDelayTime;
			}

			m_ui32ATLastRecv			= i3ThreadTimer::GetServerTime();
			m_ui32ATGameGuardSend		= i3ThreadTimer::GetServerTime();
			m_ui32ATGameGuardRecv		= m_ui32ATGameGuardSend;
			m_ui8LocaleCode				= pLoginInfo->m_ui8LocaleCode;

			SetPosition(GAME_TASK_LOGIN);
		}
		else
		{
			SetPosition(GAME_TASK_NONE);
		}

		PACKET_LOGIN_ACK Packet;

		Packet.m_TResult		= i32Rv;
		Packet.m_ui32BlockDate	= ui32BlockDate;
		if( EV_SUCCESSED( i32Rv ) )
		{
			Packet.m_TUID		= pLoginInfo->m_TUID;
			Packet.m_strID		= pLoginInfo->m_strID;
			Packet.m_strCustom	= pLoginInfo->m_strcustom;
			Packet.m_strUID		= pLoginInfo->m_strBillingID;
			Packet.m_strExtUID  = pLoginInfo->m_strExtUID;
			Packet.m_strWebLoginKey = pLoginInfo->m_strWebLoginKey;
		}
		else if (EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT == i32Rv)
		{
			Packet.m_i32PenaltyAmount		= pLoginInfo->m_i32PenaltyAmount;
			Packet.m_strUserNoticeComment	= pLoginInfo->m_strUserNoticeComment;
		}
		S2MOSendEvent( &Packet );
	}

	m_ui32ATLogin		= 0;
	return; 
}

// -----------------------------------------------------------
// Name : GetMyInfo_A
// Desc : 유저 정보를 받습니다.
// -----------------------------------------------------------
void CUserSession::GetMyInfo_A( INT32 i32WorkThread, INT32 i32Rv, i3NetworkPacket* pPacket )
{
	if( EV_SUCCESSED( i32Rv ) )
	{
		_GetMyInfoPacket( i32WorkThread, pPacket, FALSE );
	}
	else
	{	// 실패시, 서버 버전에 따라 유저 버전을 할당합니다.
		switch( g_pContextMain->m_eServerVersion )
		{	
		case PB_VER_V10:
			m_pDesign  = m_pDesignV1;
			break; 
		default: 
			m_pDesign  = m_pDesignV2;
			break; 
		}
	}

	m_abGetInfoToServer[ GET_INFO_TO_SERVER_BASIC ] = true;
	SendMyInfo( i32WorkThread, i32Rv );
}

void CUserSession::SendMyInfo( INT32 i32WorkThread, INT32 i32Result )
{
	if( EV_FAILED( i32Result ) )
	{
		m_ui32ATInfo = 0;

		PACKET_BASE_GET_USER_INFO_ACK Packet;
		Packet.m_TResult = i32Result;
		S2MOSendEvent( &Packet );
		return;
	}

	for( INT32 i = 0 ; i < GET_INFO_TO_SERVER_MAX ; i++ )
	{
		if( false == m_abGetInfoToServer[ i ] )	return;
	}

	m_ui32ATInfo = 0;

	// 각 서버에 로그인 패킷을 보냅니다.
	// NC Login Message
	g_pModuleContextNC->LogSendMessage(i32WorkThread, LOG_MESSAGE_U_LOGIN, this );		//LOG_D
	// MessengerServer Login
	g_pModuleMessenger->MessengerLogin( i32WorkThread , this );

	CheckRankUp(i32WorkThread, RANKUP_CHECK_COMPARE);

	// Set Packet
	PACKET_BASE_GET_USER_INFO_ACK Packet;
	Packet.m_TResult				= EVENT_ERROR_SUCCESS;
	Packet.m_ui8Age					= m_ui8Age;
	Packet.m_ui32TotalConnectCount	= m_ui32AccConnectCount;

	Packet.m_bGetEventItem			= false;
	Packet.m_ui32ReportServerTime	= i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
	Packet.m_ui32ClientIP			= m_ConIp;

	Packet.m_UserInfoBasic			= m_UserInfoBasic;
	Packet.m_UserInfoRecord			= m_UserInfoRecord;
	Packet.m_UserInfoInvItemData	= m_UserInfoInvItemData;

	Packet.m_r32PCCafeExp	= 0.f;
	Packet.m_r32PCCafePoint	= 0.f;
	g_pContextMain->GetRoomBonus( m_ui8LocaleCode, EVENT_BONUS_PC_CAFE, m_UserInfoBasic.m_ui8PCCafe, &Packet.m_r32PCCafeExp, &Packet.m_r32PCCafePoint );

	_SyncEquipIdx(TRUE);	// 공용파츠만 인벤에서 ItemID 검색. 일단 떔빵 처리 합니다.
	m_MultiSlot.GetPacketData( &Packet.m_MultiSlotInfo );
	
	for (INT32 i = 0; i < ATTENDANCE_TYPE_MAX; i++)
	{
		Packet.m_stAttendance[i] = (ATTENDANCE_USER)m_stAttendance[i];
	}
	Packet.m_stAttendanceInfo.SetValue(m_stAttendanceInfo, 0, ATTENDANCE_TYPE_MAX);

	Packet.m_NotifyMedal.SetValue( m_MedalAllInfo.m_aNotifyMedal, 0, MAX_NOTIFY_MADAL_COUNT  ) ;
	Packet.m_UserInfoDaily			= m_UserInfoDaily;
	Packet.m_aQuickJoinInfo.SetValue( m_aQuickJoinInfo, 0, S2MO_MAX_QUICK_JOIN_INFO_COUNT );
 
	Packet.m_ui32Miniature			= m_ui32Miniature;
	Packet.m_ui32Insignia			= m_ui32Insignia;
	Packet.m_ui32Order				= m_ui32Order;
	Packet.m_ui32Master				= m_ui32Master; 

	Packet.m_ui64GainUserTitle		= m_ui64GainUserTitle;
	Packet.m_i32LimitUserTitleCount	= m_i32LimitUserTitleCount;

	Packet.m_QuestIngInfo			= m_QuestIngInfo;
	for( INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; i++ )
	{
		Packet.m_aQuestCtxActive.SetValue( m_aaui8QuestCtxActive[i], i*MAX_QUEST_PER_CARDSET_NEW, MAX_QUEST_PER_CARDSET_NEW );
	}
	Packet.m_aui8EquipUserTitle.SetValue( m_aui8EquipUserTitle, 0, MAX_EQUIP_USER_TITLE );

	Packet.m_bGiftAutoAuth			= m_bGiftAutoAuth;
	Packet.m_ui8Country				= m_ui8Country; 

	S2MOSendEvent( &Packet );


	///////////////////////////////////////////////////////////////////
	//// 멀티슬롯 캐릭터 정보 별도 패킷으로 전달
	// 활성화된 슬롯 체크.
	INT32 i32ActiveSlotCount = 0;
	INT32 i32ActiveSlot[S2MO_MULTI_SLOT_COUNT] = {0,};
	for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
	{
		if( FALSE == m_MultiSlot.IsActiveChar( i ) )	continue;
		i32ActiveSlot[i32ActiveSlotCount] = i;
		++i32ActiveSlotCount;
	}


	INT32 i32Count = 0;

	PACKET_BASE_GET_CHARA_INFO_ACK	CharaPacket;
	S2MO_CHAR_INFO					aCharInfo[S2MO_MULTI_SLOT_PACKET_COUNT];
	//UINT8		ui8PacketCount =  ( i32CharaCount / S2MO_MULTI_SLOT_PACKET_COUNT  ) + ( ( i32CharaCount % S2MO_MULTI_SLOT_PACKET_COUNT ) ? 1 : 0  ) ;
	UINT8		ui8PacketIdx = 0;
	//CharaPacket.m_ui8PacketCount = ui8PacketCount;
	for( INT32 i = 0; i < i32ActiveSlotCount ; ++i )
	{
		if(  i32Count > 0 && 0 == ( i32Count % S2MO_MULTI_SLOT_PACKET_COUNT ) )
		{
			CharaPacket.m_ui8PacketIdx	= ui8PacketIdx;
			CharaPacket.m_aChara.SetValue( aCharInfo, 0, i32Count );
			S2MOSendEvent(&CharaPacket);

			CharaPacket.m_aChara.SetCount(0);
			i32Count = 0;
			++ui8PacketIdx;
		}

		aCharInfo[i32Count].m_ui8SlotIdx = (UINT8)i32ActiveSlot[i];
		aCharInfo[i32Count].m_ui8SlotState = MULTI_SLOT_STATE_NONE;
		m_MultiSlot.GetPacketData(  aCharInfo[i32Count].m_ui8SlotIdx,  &aCharInfo[i32Count] );

		++i32Count;
	}

	CharaPacket.m_ui8PacketIdx = ui8PacketIdx;
	CharaPacket.m_aChara.SetValue( aCharInfo, 0, i32Count );
	S2MOSendEvent(&CharaPacket);
	///////////////////////////////////////////////////////////////////


	SetPosition( GAME_TASK_INFO );
}

void CUserSession::SetBirthDay_A( INT32 i32Rv, UINT32 ui32BirthDay )
{
	i3NetworkPacket SendPacket( PROTOCOL_LOBBY_SET_BIRTHDAY_ACK );
	SendPacket.WriteData( &i32Rv,	sizeof(INT32) );

	if( EV_SUCCESSED( i32Rv ) )
	{
		// 나이 다시 설정
		m_ui32BirthDay = ui32BirthDay;
		_SetAge();
		SendPacket.WriteData( &m_ui8Age, sizeof(UINT8) );
	}
	else
	{
		m_ui32BirthDay = 0;
	}
	SendPacketMessage( &SendPacket );
}

void CUserSession::SetCharaSkill( UINT8 ui8Slot, UINT8 ui8Class )
{
	S2MO_CHAR_SKILL_INFO Skill_Info;
	m_MultiSlot.GetCharSkillInfo( ui8Slot, &Skill_Info );

	Skill_Info.m_ui8Class = ui8Class;

	m_MultiSlot.SetCharSkillInfo( ui8Slot, &Skill_Info );

}

// -----------------------------------------------------------
// Name : ServerEnter_A
// Desc : 새로운 서버에 접속합니다.
// -----------------------------------------------------------
void CUserSession::ServerEnter_A( INT32 i32Rv, i3NetworkPacket* pPacket )
{
	if( EV_SUCCESSED( i32Rv ) )
	{
		pPacket->ReadData( &m_ui8LocaleCode, sizeof(UINT8) );
		pPacket->ReadData( m_strId,				NET_ID_SIZE );
		_GetMyInfoPacket( MODULE_TRANS_THREAD_IDX, pPacket, TRUE );

		// 메신저, 클랜서버에 서버이동알린다.
		g_pModuleMessenger->MessengerSendServerEnter(	MODULE_TRANS_THREAD_IDX, this );
		g_pModuleClan->ClanSendServerEnter( MODULE_TRANS_THREAD_IDX, this );

		m_bCheckLogout = FALSE;	
	}
	else 
	{	// 실패시, 서버 버전에 따라 유저 버전을 할당합니다.
		switch( g_pContextMain->m_eServerVersion )
		{	
		case PB_VER_V10:
			m_pDesign  = m_pDesignV1;
			break; 
		default: 
			m_pDesign  = m_pDesignV2;
			break; 
		}
	}

	PACKET_BASE_USER_ENTER_ACK Packet;
	Packet.m_TResult = i32Rv;
	S2MOSendEvent( &Packet );

	I3TRACE( "ENTER USER ACK\n" );
}

// -----------------------------------------------------------
// Name : ServerLeave_A
// Desc : 서버에서 나갑니다.
// -----------------------------------------------------------
void CUserSession::ServerLeave_A( INT32 i32Rv )
{
	PACKET_BASE_USER_LEAVE_ACK Packet;
	Packet.m_TResult = i32Rv;
	S2MOSendEvent( &Packet );

	// m_iIsKillST 때문에 Send 다음에 위치해야 합니다.
	if( EV_SUCCESSED( i32Rv ) )
	{
		//서버를 이동한다는 메시지를 각 상위 서버에 보냄  -- 다시 생각해 봅시다. 꼭 보낼 필요가 있을지. 한가지 걸리는건 귓말이나, 찾아가기 초대하기 정도? -- 현우 일해라 --
		g_pModuleClan->ClanSendServerLeave( MODULE_TRANS_THREAD_IDX, this );
		g_pModuleMessenger->MessengerLeaveUser( MODULE_TRANS_THREAD_IDX, this );

		DisconnectUserDelay( EVENT_ERROR_EVENT, KILL_REASON_SERVER_MOVE );
		//m_iIsKillST		= g_pContextMain->GetServerTime();
		//m_iKillReason	= KILL_REASON_SERVER_MOVE;
		m_i64UID		= SERVER_VALUE_UNKNOWN;											// DB 를 날림으로 유저가 이동할 것이라고 알려줍니다.

		SetPosition( GAME_TASK_LEAVE );
	}
}

// -----------------------------------------------------------
// Name : CreateNickName_A
// Desc : 닉네임이 생성 되었습니다.
// -----------------------------------------------------------
void CUserSession::CreateNickName_A( INT32 i32Rv, UINT32 ui32NewPoint, TTCHAR* strNickName )
{
	PACKET_BASE_CREATE_NICK_ACK Packet;
	Packet.m_TResult = i32Rv;

	if( EV_SUCCESSED( i32Rv ) )
	{	// 신규유저 만들기 성공..

		// 닉네임 세팅
		i3String::Copy( m_UserInfoBasic.m_strNick, strNickName, NET_NICK_NAME_SIZE );

		// 신규 유저 포인트 지급
		m_UserInfoBasic.m_ui32Point = 0;// 포인트 복사 방지
		SendGetPoint( MODULE_TRANS_THREAD_IDX, ui32NewPoint, (UINT8)GET_POINT_ROUTE_CREATE_CHARA );

		for( INT32 i = 0 ; i < CHAR_EQUIPMENT_COMMON_COUNT ; i++ )
		{
			T_ItemDBIdx TItemDBIdx;
			g_MakeBasicItemDBIdx( TItemDBIdx, g_pContextMain->m_TBasicCommonEquip[ m_ui8LocaleCode ][ i ] );

			m_MultiSlot.SetCommonEquipment( i, TItemDBIdx, g_pContextMain->m_TBasicCommonEquip[ m_ui8LocaleCode ][ i ] );
		}

		// 튜토리얼 임무카드 
		g_pContextMain->GetQuest( QUEST_CARDSET_TYPE_TUTORIAL, TUTORIAL_CARDSET_SLOT, &m_QuestIngInfo, m_aaui8QuestCtxActive[ TUTORIAL_CARDSET_SLOT ], m_ui8LocaleCode );
		_SendQuestInfoPacket( TUTORIAL_CARDSET_SLOT );
		

		//Zepetto Module에 통보 합니다.
		g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_CREATE_NICK, this );
		
		// MessengerServer Login
		g_pModuleMessenger->MessengerChangeNickname( MODULE_TRANS_THREAD_IDX, this );
		g_pModuleMessenger->MessengerLogin(MODULE_TRANS_THREAD_IDX, this);

		// 친구 목록 초기화
		g_pModuleMessenger->MessengerSendResetFriendInfo( MODULE_TRANS_THREAD_IDX, m_i64UID );

		ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO | SAVE_DATABASE_CHARA_COMMON);

		ITEM_INFO pCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];
		m_MultiSlot.GetCommonEquipmentAll( pCommonEquips );
		Packet.m_aCommonEquipment.SetValue( pCommonEquips, 0, CHAR_EQUIPMENT_COMMON_COUNT );
		Packet.m_strNickName	= strNickName;
	}
	else
	{	// 닉네임을 다시 초기화 해줍니다.
		m_UserInfoBasic.m_strNick[0] = '\0';
	}

	S2MOSendEvent( &Packet );
}


void CUserSession::CreateChar_A(PACKET_CHAR_CREATE_CHARA_ACK* pPacket, T_GoodsID TGoodsID, INVEN_BUFFER* pCharaInven)
{	
	T_ItemDBIdx			TItemDBIdx;
	INVEN_BUFFER_SET	InvenSet;

	InvenSet.ui8ChangeType	= ITEM_INSERT_REQUEST_BUY;

	switch (pPacket->m_ui8SlotState)
	{
		case MULTI_SLOT_STATE_NORMAL:
		case MULTI_SLOT_STATE_UNUSE:
			break;
		default:
		{	// 에러.
			// 로그 남기고 리턴.
			g_pLog->WriteLog(L"[ERROR] Create Chara Slot State / UID : %I64d / Slot State : %d / Rv : %d / Type : %d / SlotIdx : %d",
				m_i64UID, (UINT8)pPacket->m_ui8SlotState, (INT32)pPacket->m_TResult, (UINT8)pPacket->m_ui8CreateType, (UINT8)pPacket->m_ui8SlotIdx);

			pPacket->m_TResult = EVENT_ERROR_CHARA_SLOT_STATE;
			S2MOSendEvent(pPacket);
			return;
		}
		break;
	}

	if (S2MO_IS_SUCCESS(pPacket->m_TResult))
	{
		// 기본 캐릭 생성 예외처리
		if (pCharaInven->_TItemWareDBIdx & DEFAULT_ITEM_DBIDX_MIN)
		{
			pCharaInven->_ui32ItemID		= TGoodsID / 100;
			pCharaInven->_ui32ItemArg		= 1;
			if (ITEM_ATTR_TYPE_PC_1 == pCharaInven->_ui8ItemType) pCharaInven->_ui8ItemType = ITEM_ATTR_TYPE_PC_1;
			else pCharaInven->_ui8ItemType		= ITEM_ATTR_TYPE_BASIC;
		}

		// 구매 이력 기록.
		switch (pPacket->m_ui8CreateType)
		{
		case GOODS_BUY_CASH: 
				m_UserInfoServer.m_ui32LastBuyCash = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
			//	break;	// 주석이 맞음
		case GOODS_BUY_POINT:
			{
			MedalMissionCheck_Shop(MODULE_TRANS_THREAD_IDX, pPacket->m_ui8CreateType, pCharaInven->_ui32ItemID);

				//ZLog 에 기록 상점 로그 보내기 (구입)
				INT32 i32SpendType = (INT32)SPEND_TYPE_BUY_GOODS;
			g_pModuleContextNC->LogSendMessage(MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_SHOP_BUY, this, &TGoodsID, &pPacket->m_ui8CreateType, &i32SpendType);
				
			}
			break;
		}

		UINT8 ui8SlotState = pPacket->m_ui8SlotState;
		if (pPacket->m_bAddCharaSlot && TRUE == m_MultiSlot.InsertCharInfoAll(pPacket->m_ui8SlotIdx, (MULTI_SLOT_STATE)ui8SlotState, &pPacket->m_BaseInfo))
		{
			m_MultiSlot.SetCharSkillInfo(pPacket->m_ui8SlotIdx, &pPacket->m_SkillInfo);

			// 이곳에서 초기 장비를 세팅 합니다.
			// Weapon 편
			T_ItemID* pTDefaultWeapon;
			pTDefaultWeapon = g_pContextMain->GetDefaultCharWeaponSetting(m_ui8LocaleCode);

			if (pTDefaultWeapon)
			{
				for (INT32 i = 0; i < CHAR_EQUIPMENT_WEAPON_COUNT; i++)
				{
					g_MakeBasicItemDBIdx(TItemDBIdx, pTDefaultWeapon[i]);
					m_MultiSlot.SetCharEquipment(pPacket->m_ui8SlotIdx, i, TItemDBIdx, pTDefaultWeapon[i]);
				}
			}

			// Parts 편
			T_ItemID	TPartId;
			ITEM_INFO*	pTDefaultEquip = g_pContextMain->GetDefaultCharEquipSetting();
			if (pTDefaultEquip)
			{
				for (INT32 i = 0; i < CHAR_EQUIPMENT_PARTS_COUNT; i++)
				{
					// 캐릭터는 Trans서버로 부터 받은 DB IDX를 사용합니다.
					switch (i)
					{ 
					case CHAR_EQUIPMENT_PARTS_CHARA: 
						{
							TItemDBIdx	= pCharaInven->_TItemWareDBIdx;
							TPartId		= pCharaInven->_ui32ItemID;
						}
						break;
					default:
						{	 
							TItemDBIdx	= pTDefaultEquip[i].m_TItemDBIdx;
							TPartId		= pTDefaultEquip[i].m_TItemID;
						}
						break;
					}

					m_MultiSlot.SetCharEquipment(pPacket->m_ui8SlotIdx, (CHAR_EQUIPMENT_WEAPON_COUNT + i), TItemDBIdx, TPartId);
				}
			}

			if (GOODS_BUY_NONE == pPacket->m_ui8CreateType)
			{	// PCCafe 캐릭터 예외처리 
				return;
			}

			if (GOODS_BUY_CHAR_FREE == pPacket->m_ui8CreateType)
			{	// 기본 캐릭터 생성이면 메인슬롯 설정
				m_pDesign->SetTeamSlot(m_MultiSlot, pPacket->m_ui8SlotIdx, pCharaInven->_ui32ItemID);
			}

			ChangeDataBaseInfo(SAVE_DATABASE_CHARA_COMMON);
			ChangeCharaDataBaseInfo(((UINT64)0x01 << pPacket->m_ui8SlotIdx));
		}

		// 캐릭터를 인벤토리에 추가합니다.
		OnInsertItem(pCharaInven, &InvenSet);
		SendChangeInven(&InvenSet);		// 추가되는 아이템을 전달 합니다.
	}

	m_MultiSlot.GetCharEquipInfo(pPacket->m_ui8SlotIdx, &pPacket->m_CharEquipInfo);

	UINT32 ui32CheckList = (0x1 << CHAR_EQUIPMENT_CHARA);	// 기간제 캐릭터는 아직 미인증 상태이므로 건너뜁니다.
	if ( pPacket->m_bAddCharaSlot && FALSE == _CheckCharEquipment((INT32)pPacket->m_ui8SlotIdx, &pPacket->m_CharEquipInfo, ui32CheckList))
	{
		// 로그 남기고 리턴.
		g_pLog->WriteLog(L"[ERROR] Create Chara Equip Info / UID : %I64d / Slot State : %d / Rv : %d / Type : %d / SlotIdx : %d / Check : %d",
			m_i64UID, (UINT8)pPacket->m_ui8SlotState, (INT32)pPacket->m_TResult, (UINT8)pPacket->m_ui8CreateType, (UINT8)pPacket->m_ui8SlotIdx, ui32CheckList);

		pPacket->m_TResult = EVENT_ERROR_EQUIPMENT_WRONG_ITEM_ID;
	}

	S2MOSendEvent( pPacket );
}


// -----------------------------------------------------------
// Name : SetClan_C
// Desc : 클랜 정보를 받습니다.
// -----------------------------------------------------------
void CUserSession::SetClan_C( INT32 i32Result, UINT32 ui32CID, UINT8 ui8ClanState, UINT8 ui8ExpRank, UINT8 ui8PerRank, UINT32 ui32Mark, TTCHAR* strClanName )
{
	if( EV_SUCCESSED( i32Result ) )
	{
		m_UserInfoBasic.m_ui32Clanidx		= ui32CID;
		m_UserInfoBasic.m_ui32ClanState		= ui8ClanState;	
		if( CLAN_MEMBER_LEVEL_UNKNOWN < ui8ClanState )
		{
			m_UserInfoBasic.m_ui8ClanExpRank	= ui8ExpRank;
			m_UserInfoBasic.m_ui8ClanPerRank	= ui8PerRank;
			m_UserInfoBasic.m_ui32ClanMark		= ui32Mark;
			m_UserInfoBasic.m_ui8ClanNamecolor	= 0; 
			i3String::Copy( m_UserInfoBasic.m_strClanName, strClanName, NET_CLAN_NAME_SIZE );
			m_UserInfoBasic.m_strClanName[NET_CLAN_NAME_SIZE-1] = '\0';
		}
		ChangeDataBaseInfo( SAVE_DATABASE_CLAN );
	}
	m_abGetInfoToServer[ GET_INFO_TO_SERVER_CLAN ] = true;
	// 클랜 정보가 이상해도 로그인은 시킵니다.
	SendMyInfo( MODULE_CLAN_THREAD_IDX, EVENT_ERROR_SUCCESS );
}

void CUserSession::SendBuyQuestCardResult( INT32 i32Rv, UINT32 ui32GoodsID, INT32 i32ThreadIdx, T_ItemDBIdx TItemWareDBIdx, UINT8 ui8BuyType )
{
	INT32 i32Result = EVENT_ERROR_SUCCESS;
	INT8 i8QuestSlotIdx = 0;
	if(EV_FAILED( i32Rv ))	i32Result = EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL; // 클라이언트 작업 안되있어서 막습니다 i32Result = EVENT_ERROR_EVENT_BUY_CARD_NETWORK;
	else
	{
		INT32 i32SpendType	= (INT32)SPEND_TYPE_BUY_MISSIONCARD;
		//상점 로그 보내기 (구입)
		g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_SHOP_BUY, this, &ui32GoodsID, &ui8BuyType, &i32SpendType );

		// GoodsID로 부터 임무 카드 종류를 알아낸다.
		UINT8 ui8CardType = (UINT8)GET_GOODS_FLAG_NUMBER( ui32GoodsID ) ;

		i8QuestSlotIdx = _FindInsertQuestSlot( ui8CardType );

		// 계급 제한이 있는 카드의 경우 확인하여 맞지 않을 경우 실패 처리한다. ( 클라이언트 해킹 ) -- 서동권
		switch( ui8CardType )
		{
			// 사관
		case QUEST_CARDSET_TYPE_COMMISSIONED_O:
			{
				if( RANK_05 > m_UserInfoBasic.m_ui32Rank )
					i8QuestSlotIdx = -1;
			}
			break;
			// 위관
		case QUEST_CARDSET_TYPE_COMPANY_O:
			{
				if( RANK_17 > m_UserInfoBasic.m_ui32Rank )
					i8QuestSlotIdx = -1;
			}
			break;
			// 영관
		case QUEST_CARDSET_TYPE_FIELD_O:
			{
				if( RANK_31 > m_UserInfoBasic.m_ui32Rank )
					i8QuestSlotIdx = -1;
			}
			break;
		}

		if( EV_SUCCESSED( i8QuestSlotIdx ) )
		{
			g_pContextMain->GetQuest( ui8CardType, i8QuestSlotIdx, &m_QuestIngInfo, m_aaui8QuestCtxActive[ i8QuestSlotIdx ], m_ui8LocaleCode );

			ChangeDataBaseInfo(SAVE_DATABASE_QUEST | SAVE_DATABASE_BASEINFO );

			//// 아이템 인증
			g_pModuleTrans->AuthSendAuthItem(i32ThreadIdx, this, TItemWareDBIdx, 1 );

		}
		else	// 여기서 실패하면 반드시 로그를 남깁니다. 임무카드 금액이 차감 되지만 복구는 안되기 때문
		{	
			i32Result = EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL; 

			g_pLog->WriteLogSession( this, L"[SendBuyQuestCardResult] Quest Card Buy Error or Client hacking ( Rank ) / UID : %I64d / CardType : %d", m_i64UID, ui8CardType ) ; 
		}
	}
	  	

	i3NetworkPacket pPacket( PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK );
	pPacket.WriteData(&i32Result,								sizeof(INT32)); 
	if(EV_SUCCESSED( i32Result ))
	{
		pPacket.WriteData(&m_UserInfoBasic.m_ui32Point,				sizeof(UINT32)); 
		pPacket.WriteData(&i8QuestSlotIdx,						sizeof(INT8));				
		pPacket.WriteData(&m_QuestIngInfo,						sizeof(QUESTING_INFO)); 
	}

	SendPacketMessage( &pPacket ); 
}

void CUserSession::SendBlockList( UINT8 ui8BlockCount, BLOCK_INFO_STRUCT* pBlockInfo )
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_BLOCK_INFO_ACK );

	SendPacket.WriteData(&ui8BlockCount,	sizeof(UINT8)); 
	for(UINT8 i = 0; i < ui8BlockCount; i++)
	{
		SendPacket.WriteData( &pBlockInfo[i]._NickSize,		sizeof(UINT8)); 
		SendPacket.WriteData( pBlockInfo[i]._NickName,		sizeof(TTCHAR) * pBlockInfo[i]._NickSize ); 
		SendPacket.WriteData( &pBlockInfo[i]._CommentSize,	sizeof(UINT8)); 
		SendPacket.WriteData( pBlockInfo[i]._Comment,		sizeof(TTCHAR) * pBlockInfo[i]._CommentSize ); 
		SendPacket.WriteData( &pBlockInfo[i]._BasicInfo,	sizeof(BLOCK_INFO));
	}
	SendPacketMessage(&SendPacket); 
	return; 
}

void CUserSession::SendBlockInsertResult(INT32 i32Result, INT32 i32Idx, UINT8 ui8Rank, UINT8 ui8NickSize, TTCHAR *strNick, UINT8 ui8CommentSize, TTCHAR *strComment)
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_BLOCK_INSERT_ACK );
	SendPacket.WriteData( &i32Result,		sizeof(INT32) );
	if( EV_SUCCESSED( i32Result ) )
	{
		SendPacket.WriteData( &i32Idx,			sizeof(INT32)); 
		SendPacket.WriteData( &ui8Rank,			sizeof(UINT8)); 
		SendPacket.WriteData( &ui8NickSize,		sizeof(UINT8)); 
		SendPacket.WriteData( strNick,			sizeof(TTCHAR) * ui8NickSize ); 
		SendPacket.WriteData( &ui8CommentSize,	sizeof(UINT8)); 
		SendPacket.WriteData( strComment,		sizeof(TTCHAR) * ui8CommentSize ); 
	}
	SendPacketMessage(&SendPacket); 
	return; 
}

void CUserSession::SendBlockDeleteResult(INT32 i32Result, INT32 i32Idx )
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_BLOCK_DELETE_ACK );
	SendPacket.WriteData( &i32Result,		sizeof(INT32) );
	SendPacket.WriteData( &i32Idx,			sizeof(INT32) );
	SendPacketMessage(&SendPacket); 
	return; 
}

BOOL CUserSession::_CheckIsSameClan( INT32 i32SessionIdx )
{
	BOOL bResult = FALSE;	// 다른 클랜.

	CUserSession *	pSession = g_pUserManager->GetSession( i32SessionIdx );
	if ( NULL != pSession && 0 != m_UserInfoBasic.m_ui32Clanidx )
	{
		if( m_UserInfoBasic.m_ui32Clanidx == pSession->GetClanIdx() ) bResult = TRUE;
	}

	return bResult;
}

void CUserSession::SendMyItemInfo( CUserSession * pSession, UINT8 ui8Type  )
{
	switch( ui8Type )
	{
	case VIEW_USER_ITEM_INFO_TYPE_LOBBY :
		{
			PACKET_LOBBY_VIEW_USER_ITEM_ACK Packet;

			// 캐릭터 장비정보
			ITEM_INFO	CharItemInfo[CHAR_EQUIPMENT_COUNT];
			m_pDesign->GetMainCharEquipmentAll(&m_MultiSlot, CharItemInfo);	// 로비라 팀을 알수 없음.
			for( INT32 i = 0 ; i < CHAR_EQUIPMENT_COUNT; ++ i )
			{
				Packet.m_aCharaEquip[i] = CharItemInfo[i].m_TItemID;
			}

			Packet.m_TCharItemID[TEAM_RED] = CharItemInfo[CHAR_EQUIPMENT_CHARA].m_TItemID;

			// 공통 파츠 장비정보
			ITEM_INFO	CommonItemInfo[CHAR_EQUIPMENT_COMMON_COUNT];
			m_MultiSlot.GetCommonEquipmentAll( CommonItemInfo );
			for( INT32 i = 0 ; i < CHAR_EQUIPMENT_COMMON_COUNT; ++ i )
			{
				Packet.m_aCommonEquips[i] = CommonItemInfo[i].m_TItemID;
			}

			// 블루팀 캐릭터 정보
			ITEM_INFO	BlueItemInfo;
			INT32 i32BlueTeamSlot = m_MultiSlot.GetTeamSlot(TEAM_BLUE);
			m_MultiSlot.GetCharEquipment( i32BlueTeamSlot, CHAR_EQUIPMENT_CHARA, &BlueItemInfo);
			Packet.m_TCharItemID[TEAM_BLUE] = BlueItemInfo.m_TItemID;

			UINT16 ui16Count;
			T_ItemID TItemID[ MAX_INVEN_COUNT ];
			m_Inventory.GetInventoryItemItemID( &ui16Count, TItemID );

			Packet.m_aTItemID.SetValue( TItemID, 0, ui16Count );

			pSession->S2MOSendEvent( &Packet );
		}
		break;	// 로비
	case VIEW_USER_ITEM_INFO_TYPE_ROOM :
		{
			PACKET_ROOM_GET_USER_ITEM_ACK Packet;
	
			S2MO_CHAR_EQUIP_INFO	CharEquipInfo;
			ITEM_INFO				aCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];
			UINT16					ui16ItemCount;
			T_ItemID				m_aTItemID[ MAX_INVEN_COUNT ];

			m_pDesign->GetMainCharEquipInfo(&m_MultiSlot, &CharEquipInfo, m_i32SlotIdx % TEAM_COUNT);	

			m_MultiSlot.GetCommonEquipmentAll( aCommonEquips );
			
			m_Inventory.GetInventoryItemItemID( &ui16ItemCount, m_aTItemID );
			 
			Packet.m_CharEquipInfo = CharEquipInfo;
			Packet.m_aCommonEquips.SetValue( aCommonEquips, 0, CHAR_EQUIPMENT_COMMON_COUNT );
			Packet.m_aTItemID.SetValue( m_aTItemID, 0, ui16ItemCount );

			pSession->S2MOSendEvent( &Packet );
		}
		break; // 룸
	// case VIEW_USER_ITEM_INFO_TYPE_BATTLE :	break; // 배틀
	default:  
		break;
	}
}

// -----------------------------------------------------------------
// 출석체크 이벤트 완료 보상 요청에 대한 결과값을 보냅니다. - 서동권
// -----------------------------------------------------------------
void CUserSession::SendAttendanceItemResult( INT32 i32Rv )
{
	i3NetworkPacket packet( PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_ACK );
	packet.WriteData( &i32Rv,	sizeof(INT32) );
	SendPacketMessage( &packet );	
}

// -----------------------------------------------------------------
// 출석체크 이벤트 출석 요청에 대한 결과값을 보냅니다. - 서동권
// -----------------------------------------------------------------
void CUserSession::SendAttendanceCheckResult( INT32 i32Rv, ATTENDANCE_USER_SVR* pstAttendance )
{
	i3NetworkPacket packet( PROTOCOL_BASE_ATTENDANCE_ACK );
	packet.WriteData( &i32Rv,	sizeof(INT32) );
	if( EVENT_ERROR_ATTENDANCE_SUCCESS == i32Rv )
	{
		packet.WriteData( pstAttendance,	sizeof(ATTENDANCE_USER) );
	}
	SendPacketMessage( &packet );	
}

// 채팅내용 검사 (중복채팅을 시도했을 경우 카운트를 세주거나 제한 시작, GM Pause중에 발신한 것인지 확인)
// 인자 : 채팅내용, 채팅한 위치
// 반환 : 채팅 가능 여부. TRUE : 가능
BOOL CUserSession::_CheckChat(TTCHAR* strChat, INT32 i32ChatType, INT16 i16Size, PROTOCOL ui16Protocol )
{
#ifdef USE_GM_PAUSE
	if ( m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
	{
		if ( m_pRoom->IsGMPause() )
		{
			if ( !CheckAuthLevel() ) return FALSE;
		}
	}
#endif

	DATE32	dt32CurrentTime = i3ThreadTimer::LocalTime(); // 현재 시간
	INT32	i32ChatLimitTime = 0;

	//채팅 제한 시간이 지났는지.
	if( m_dt32HackChatBlockDate <= dt32CurrentTime )
	{
		if( m_eChatLimitState == CHAT_LIMIT_STATE_ING ) 
			m_dt32ChatEndTime = dt32CurrentTime; // 다시 채팅 제한이 풀리는 경우는 카운트 시간 초기화
		m_eChatLimitState = CHAT_LIMIT_STATE_NORMAL;
	}
	else
	{
		i32ChatLimitTime = m_dt32HackChatBlockDate.DiffTime(dt32CurrentTime);
		
		if ((INT32)g_pContextMain->m_chlChatHackLimit.m_ui32LimitTime < i32ChatLimitTime) // 채팅 블럭 전체 시간이 현재 남은 채팅 시간보다 클수 없다. 큰 경우는 DB 데이터 참조바람.
		{
			m_dt32HackChatBlockDate = dt32CurrentTime; // 채팅 블럭 시간을 현재 시간으로 변경.
			m_UserInfoServer.m_dt32HackChatBlockDate = dt32CurrentTime;
			m_eChatLimitState = CHAT_LIMIT_STATE_NORMAL;
		}
		else
			m_eChatLimitState = CHAT_LIMIT_STATE_ING;
	}

	//------------------------------------------------------------------------------------------------------------

	if( dt32CurrentTime >= m_dt32ChatEndTime ) //재 입력 시간 시간이끝날경우 입력 회수 초기화, 재 입력 시간 리셋
	{
		m_i32ChatCount = 0;
		m_dt32ChatEndTime =  dt32CurrentTime;
		m_dt32ChatEndTime.AddTime( I3DATE_ADD_TYPE_SECOND, (INT32)g_pContextMain->m_chlChatHackLimit.m_ui8Time);
	}
	
	//------------------------------------------------------------------------------------------------------------

	++m_i32ChatCount; //채팅을 쳐서 들어왔기 때문에 카운트 증가	

	//------------------------------------------------------------------------------------------------------------

	if( m_i32ChatCount > g_pContextMain->m_chlChatHackLimit.m_ui8Count && // 횟수 초과 AND 채팅 사이즈 초과
			i16Size > g_pContextMain->m_chlChatHackLimit.m_ui8Size)
	{
		switch( m_eChatLimitState )
		{
			case CHAT_LIMIT_STATE_NORMAL:
				{
					m_eChatLimitState						= CHAT_LIMIT_STATE_START;
					m_dt32HackChatBlockDate	= dt32CurrentTime;
					m_dt32HackChatBlockDate.AddTime( I3DATE_ADD_TYPE_SECOND, (INT32)g_pContextMain->m_chlChatHackLimit.m_ui32LimitTime);
					m_dt32ChatEndTime						= dt32CurrentTime;
					i32ChatLimitTime						= (INT32)g_pContextMain->m_chlChatHackLimit.m_ui32LimitTime;
					m_UserInfoServer.m_dt32HackChatBlockDate = m_dt32HackChatBlockDate;
					ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO); // 채팅 블럭 데이트 값이 변경되서 필요합니다.
				}
				break;
			case CHAT_LIMIT_STATE_ING:
				{
					// ZLog에 남긴다 (도배한 곳, 채팅내용)
					g_pModuleContextNC->LogSendMessage(m_WorkThreadIdx, LOG_MESSAGE_U_CHATTING_HACK, this, &i32ChatType, strChat);
				}
				break;
		}
	}

	//------------------------------------------------------------------------------------------------------------

	switch( m_eChatLimitState ) //결과 적으로 이 채팅이 성공적인지 여부
	{
		case CHAT_LIMIT_STATE_NORMAL:
			return TRUE;
			break;
		case CHAT_LIMIT_STATE_START: // 도배 금지 남은 시간 전송.
		case CHAT_LIMIT_STATE_ING:
		{
			INT8 i8State = (INT8)m_eChatLimitState;
			i3NetworkPacket SendChattingPacket(ui16Protocol);
			SendChattingPacket.WriteData(&i8State, sizeof(INT8));
			SendChattingPacket.WriteData(&i32ChatLimitTime, sizeof(INT32));
			SendPacketMessage(&SendChattingPacket);
			return FALSE;
		}
		break;
	}

	return TRUE;	
}

void CUserSession::SetLastUserCount( TYPE_ROOM_INFO* pRoom )
{
	m_ui32LastRoomUserCount	= 0;
	for( INT32 i = 0; i < SLOT_MAX_COUNT; ++i )
	{
		if( SLOT_STATE_BATTLE == pRoom->_aSlotInfo[i]._State )
			++m_ui32LastRoomUserCount;
	}
}

void CUserSession::SendMedalInfo(void)
{
	PACKET_H_GET_MEDAL_INFO_ACK	PacketHeader;
	i3mem::FillZero( &PacketHeader, sizeof( PACKET_H_GET_MEDAL_INFO_ACK )  );
	PacketHeader.m_i32Rv = EVENT_ERROR_SUCCESS;

	if( !g_pContextMain->IsContentsEnabled( CONTENTS_MEDAL ) )	
	{	// 메달 시스템 OFF 면 빈패킷.
		PacketHeader.m_ui16Idx	= 0;
		PacketHeader.m_ui16Count= 0;
		i3NetworkPacket	EmptyPacket( PROTOCOL_GET_MEDAL_INFO_ACK );
		EmptyPacket.WriteData( &PacketHeader,		sizeof( PACKET_H_GET_MEDAL_INFO_ACK ) );
		SendPacketMessage(&EmptyPacket); 
		return;
	}

	PacketHeader.m_ui16TotalCount = 0;
	for( INT32 Type = MEDAL_MC_CHECK_POSITION_START ; Type < MEDAL_MC_CHECK_POSITION_MAX ; Type++ )
	{
		PacketHeader.m_ui16TotalCount += m_MedalAllInfo.m_aui16MedalCount[Type];
	}

	PacketHeader.m_ui16TotalCount = MIN( PacketHeader.m_ui16TotalCount, MAX_MEDAL_COUNT );

	if( 0 == PacketHeader.m_ui16TotalCount ) 
	{
		PacketHeader.m_ui16Idx	= 0;
		PacketHeader.m_ui16Count= 0;
		// 소지한 메달이 0개이면, 바로 Send
		i3NetworkPacket	MedalPacket( PROTOCOL_GET_MEDAL_INFO_ACK );
		MedalPacket.WriteData( &PacketHeader,						sizeof( PACKET_H_GET_MEDAL_INFO_ACK ) );
		SendPacketMessage(&MedalPacket); 
	}
	else
	{
		// 일반 메달 전체 데이터 용 구조체에 담는다.
		MEDAL_GT AllMedal[MEDAL_MC_CHECK_POSITION_MAX*MAX_MEDAL_COUNT];
		INT32 i32AllMedalIdx = 0; 
		for( INT32 Type = MEDAL_MC_CHECK_POSITION_START ; Type < MEDAL_MC_CHECK_POSITION_MAX ; Type++ )
		{
			for( INT32 ArrIdx = 0 ; ArrIdx < MAX_MEDAL_COUNT ; ArrIdx++ )
			{
				if( INIT_MEDAL_NUMBER == m_MedalAllInfo.m_aMedal[Type][ArrIdx].m_Medal.m_ui16Idx ) continue;

				AllMedal[i32AllMedalIdx].m_Medal = m_MedalAllInfo.m_aMedal[Type][ArrIdx].m_Medal;
				i32AllMedalIdx++;
			}
		}

		INT32	i32PacketSize		= PACKET_BUFFERSIZE_MTU - sizeof( PACKET_H_GET_MEDAL_INFO_ACK ) ;  
		UINT32	ui32RemainCount		= PacketHeader.m_ui16TotalCount ;	// 메달 총갯수

		UINT32	ui32CurItemCount	= 0;	// 이번 패킷에 보낼 메달 갯수
		UINT32	ui32StartIdx		= 0;	// 시작 위치

		UINT32 ui32MaxCount		= (i32PacketSize / sizeof( MEDAL ));	// 패킷 한번에 보낼수 있는 최대 메달 갯수.

		while( 0 < ui32RemainCount )
		{
			ui32StartIdx			+= ui32CurItemCount;

			if( ui32RemainCount > ui32MaxCount )	ui32CurItemCount	= ui32MaxCount;
			else									ui32CurItemCount	= ui32RemainCount;

			ui32RemainCount -= ui32CurItemCount;
			i3NetworkPacket	MedalPacket( PROTOCOL_GET_MEDAL_INFO_ACK );

			PacketHeader.m_ui16Count = (UINT16)ui32CurItemCount;
			PacketHeader.m_ui16Idx = (UINT16)ui32StartIdx;

			MedalPacket.WriteData( &PacketHeader,						sizeof( PACKET_H_GET_MEDAL_INFO_ACK ) );
			for( UINT32 i = ui32StartIdx ; i < ui32StartIdx + ui32CurItemCount ; i++ )
			{
				MedalPacket.WriteData( &AllMedal[i].m_Medal,	sizeof( MEDAL ) );
			}
			SendPacketMessage(&MedalPacket); 
		}
	}
	return;
}

void CUserSession::SendCurMedalSetInfo(void)
{
	PACKET_H_GET_CUR_MEDAL_SET_INFO_ACK	PacketHeader;
	PacketHeader.m_i32Rv = EVENT_ERROR_SUCCESS;

	if( !g_pContextMain->IsContentsEnabled( CONTENTS_MEDAL ) )	
	{	// 메달 시스템 OFF 면 빈패킷 전송
		PacketHeader.m_ui16Count = 0;
		PacketHeader.m_ui16Idx = 0;
		PacketHeader.m_ui16TotalCount = 0;

		i3NetworkPacket	EmptyPacket( PROTOCOL_GET_CUR_MEDAL_SET_INFO_ACK );
		EmptyPacket.WriteData( &PacketHeader,		sizeof( PACKET_H_GET_CUR_MEDAL_SET_INFO_ACK ) );
		SendPacketMessage(&EmptyPacket); 
		return;
	}

	PacketHeader.m_ui16TotalCount = MAX_CUR_MEDAL_SET_COUNT;

	INT32	i32PacketSize		= PACKET_BUFFERSIZE_MTU - sizeof( PACKET_H_GET_CUR_MEDAL_SET_INFO_ACK ) ;  
	UINT32	ui32RemainCount		= PacketHeader.m_ui16TotalCount ;	// 메달 총갯수
	UINT32	ui32CurItemCount	= 0;	// 이번 패킷에 보낼 메달 갯수
	UINT32	ui32StartIdx		= 0;	// 시작 위치

	UINT32 ui32MaxCount		= (i32PacketSize / sizeof( CUR_MEDAL_SET ));	// 패킷 한번에 보낼수 있는 최대 메달 셋트 갯수.

	while( 0 < ui32RemainCount )
	{
		ui32StartIdx			+= ui32CurItemCount;

		if( ui32RemainCount > ui32MaxCount )	ui32CurItemCount	= ui32MaxCount;
		else									ui32CurItemCount	= ui32RemainCount;

		ui32RemainCount -= ui32CurItemCount;
		i3NetworkPacket	MedalSetPacket( PROTOCOL_GET_CUR_MEDAL_SET_INFO_ACK );

		PacketHeader.m_ui16Count = (UINT16)ui32CurItemCount;
		PacketHeader.m_ui16Idx = (UINT16)ui32StartIdx;

		MedalSetPacket.WriteData( &PacketHeader,						sizeof( PACKET_H_GET_CUR_MEDAL_SET_INFO_ACK ) );

		for( UINT32 i = ui32StartIdx ; i < ui32StartIdx + ui32CurItemCount ; i++ )
		{
			MedalSetPacket.WriteData( &m_MedalAllInfo.m_aCurMedalSet[i],	 sizeof( CUR_MEDAL_SET )  );
		}
		SendPacketMessage(&MedalSetPacket); 
	}
	return;
}

void CUserSession::SendComMedalSetInfo(void)
{
	PACKET_H_GET_COM_MEDAL_SET_INFO_ACK	PacketHeader;
	PacketHeader.m_i32Rv = EVENT_ERROR_SUCCESS;
	PacketHeader.m_ui16Count = 0;
	PacketHeader.m_ui16Idx = 0;
	PacketHeader.m_ui16TotalCount = 0;

	i3NetworkPacket	EmptyPacket( PROTOCOL_GET_COM_MEDAL_SET_INFO_ACK );
	EmptyPacket.WriteData( &PacketHeader,		sizeof( PACKET_H_GET_COM_MEDAL_SET_INFO_ACK ) );
	SendPacketMessage(&EmptyPacket); 
	return;
}

MEDAL_GT*	CUserSession::GetMedal_UserDataType(UINT16 ui16MedalIdx, MEDAL_MC_CHECK_POSITION eType)	// 메달 고유 인덱스로 유저의 메달 데이터 가져오기. = 타입별 
{
	for( INT32 ArrIdx = 0 ; ArrIdx < m_MedalAllInfo.m_aui16MedalCount[eType] ; ArrIdx++ )
	{
		if( ui16MedalIdx == m_MedalAllInfo.m_aMedal[eType][ArrIdx].m_Medal.m_ui16Idx )  return &m_MedalAllInfo.m_aMedal[eType][ArrIdx];
	}

	return NULL;	// 검색 실패.
}

// 메달 고유 인덱스로 유저의 메달 데이터 가져오기. = 어느 메모리 타입인지 모를때.
MEDAL_GT*	CUserSession::GetMedal_UserDataAll(UINT16 ui16MedalIdx)
{
	for( INT32 Type = MEDAL_MC_CHECK_POSITION_START ; Type < MEDAL_MC_CHECK_POSITION_MAX; Type++ )
	{
		for( INT32 ArrIdx = 0 ; ArrIdx < m_MedalAllInfo.m_aui16MedalCount[Type] ; ArrIdx++ )
		{
			if( ui16MedalIdx == m_MedalAllInfo.m_aMedal[Type][ArrIdx].m_Medal.m_ui16Idx )  return &m_MedalAllInfo.m_aMedal[Type][ArrIdx];
		}
	}

	return NULL;	// 검색 실패.
}


// 메달 셋트 고유 인덱스로 유저의 메달 셋트 데이터 가져오기. = 진행중
CUR_MEDAL_SET*	CUserSession::GetCurMedalSet_UserData(UINT16 ui16MedalSetIdx)	
{
	// 진행중인 메달 셋트
	for( UINT16 ArrIdx = 0 ; ArrIdx < MAX_CUR_MEDAL_SET_COUNT ; ArrIdx++ )
	{
		if( ui16MedalSetIdx == m_MedalAllInfo.m_aCurMedalSet[ArrIdx].m_ui16Idx ) 
		{
			return  &m_MedalAllInfo.m_aCurMedalSet[ArrIdx];
		}
	}

	return NULL;	// 검색 실패.
}

void CUserSession::SendGetMedalReward( PACKET_H_GET_MEDAL_REWARD_ACK* pPacketAckH, PACKET_D_GET_MEDAL_REWARD* pPacketAckD )
{
	i3NetworkPacket SendPacket(PROTOCOL_GET_MEDAL_REWARD_ACK);
	SendPacket.WriteData( pPacketAckH, sizeof( PACKET_H_GET_MEDAL_REWARD_ACK ) );
	SendPacket.WriteData( pPacketAckD, sizeof( PACKET_D_GET_MEDAL_REWARD ) * pPacketAckH->m_ui8Count );
	SendPacketMessage( &SendPacket );
	return;
}



void	CUserSession::MedalMissionCheck_Shop( INT32 i32ThreadIdx, UINT8 ui8BuyType, UINT32 ui32ItemID   ) // 메달 미션 체크 - 상점.
{
	// 메달 시스템 비활성화시 강제 리턴
	if( !g_pContextMain->IsContentsEnabled( CONTENTS_MEDAL ) )	 return;
	if (m_ui8LocaleCode >= LOCALE_CODE_MAX) return;

	// 완료된 메달 정보.( 클라이언트 Packet Send 용 ) 
	MMC_INFO MMCInfo;
	MMCInfo.m_ui32UseItemID = ui32ItemID;
	MMCInfo.m_ui8BuyType	= ui8BuyType;
	UINT32 ui32DBSaveFlag	= 0;
	UINT16 ui16CompleteMedalCount = 0;
	MQF_MEDAL_INFO* pMqfMedal;
	
	// 미션달성(카운팅) 된 메달 정보를 담을 구조체.
	MEDAL_STATE		MedalComplete[MAX_MEDAL_COUNT];
	i3mem::FillZero( MedalComplete, sizeof( MEDAL_STATE ) * MAX_MEDAL_COUNT );

	MQF_ALL_MEDAL_INFO* pServerMQF = &g_pContextMain->m_MQFMedal[m_ui8LocaleCode];

	INT32 i32MedalCount = (INT32)pServerMQF->m_aui16MQFMedalCount[MEDAL_MC_CHECK_POSITION_SHOP];

	// 1. 일반 메달.
	for( INT32 MedalArrIdx = 0; MedalArrIdx < i32MedalCount ; MedalArrIdx++ )
	{
		// 주의 : 게임서버가 로드한 메달 정보와 유저가 DB에서 로드한 메달 정보가 메달 고유 인덱스의 오름차순으로 동일하게 정렬되어있다는 가정하에 진행됩니다.
		pMqfMedal = &pServerMQF->m_aMedalInfo[MEDAL_MC_CHECK_POSITION_SHOP][MedalArrIdx];
		if( NULL == pMqfMedal  ) // 나오면 안됨.
		{
			I3ASSERT(FALSE);
			continue;
		}

		// 액션이 구입관련이 아니면 continue
		if( pMqfMedal->m_eActionID < MEDAL_MC_ACTION_ID_BUY || pMqfMedal->m_eActionID > MEDAL_MC_ACTION_ID_SHOP_MAX ) continue;

		MEDAL_GT* pUserMedal = &m_MedalAllInfo.m_aMedal[MEDAL_MC_CHECK_POSITION_SHOP][MedalArrIdx];
		if( NULL == pUserMedal  )	// 나오면 안됨.
		{
			I3ASSERT(0);
			continue;
		}

		// Lv4까지 모두 달성했다면 카운팅 하지 않는다.
		if(  pUserMedal->m_Medal.m_ui16Count >= pMqfMedal->m_ui16Count[MEDAL_LEVEL_MAX-1] ) continue;

		MMCInfo.m_eActionID = pMqfMedal->m_eActionID;
		MMCInfo.m_pMQFMedalInfo = pMqfMedal;

		if( TRUE == CheckMissionBuy( &MMCInfo ) ) 
		{
			// 미션 달성
			pUserMedal->m_Medal.m_ui16Count++;

			MedalComplete[ui16CompleteMedalCount].m_ui8MedalType = MEDAL_TYPE_NORMAL;				
			MedalComplete[ui16CompleteMedalCount].m_ui16MedalIdx = pMqfMedal->m_ui16Idx ;			
			MedalComplete[ui16CompleteMedalCount].m_ui16Count = pUserMedal->m_Medal.m_ui16Count;

			pUserMedal->m_ui8IsSave = 1;	// DB 저장 ㄱㄱ

			ui16CompleteMedalCount++;
			ui32DBSaveFlag |= SAVE_DATABASE_MEDAL;

			// 레벨 달성시 ZLog에 기록.
			LOG_MEDAL_LEVELUP_INFO LogInfo;
			LogInfo.m_ui16CompareCount	= pUserMedal->m_Medal.m_ui16Count;
			LogInfo.m_ui16MedalIdx		= pMqfMedal->m_ui16Idx;
			LogInfo.m_eMedalType		= MEDAL_TYPE_NORMAL;
			LogInfo.m_ui16MedalSetIdx	= 0;
			LogInfo.m_eMedalSetType		= MEDAL_SET_TYPE_NORMAL;
			g_pModuleContextNC->LogMedalLevelUp( i32ThreadIdx,this, &LogInfo );
		}
	}


	// 2. 진행중인 메달 셋트
	MQF_MEDAL_SET_INFO* pMQFMedalSetInfo = NULL;
	for( INT32 CurMedalSetIdx = 0; CurMedalSetIdx < MAX_CUR_MEDAL_SET_COUNT ; CurMedalSetIdx++ )
	{
		// 빈 슬롯은 넘어간다.
		if( INIT_MEDAL_NUMBER == m_MedalAllInfo.m_aCurMedalSet[CurMedalSetIdx].m_ui16Idx ||
			MEDAL_SET_TYPE_NORMAL == m_MedalAllInfo.m_aCurMedalSet[CurMedalSetIdx].m_ui8Type ) continue;

		// 메달 셋트내 활성화된 메달 갯수 검색
		CUR_MEDAL_SET* pCurUserMedalSet = &m_MedalAllInfo.m_aCurMedalSet[CurMedalSetIdx];	// 진행중인 유저의 메달 셋트
		if( NULL == pCurUserMedalSet ) continue;

		// 이벤트 메달 셋트는 서버에서 진행중이 아니라면 카운팅 하지 않는다.
		if( MEDAL_SET_TYPE_EVENT ==  pCurUserMedalSet->m_ui8Type && FALSE == g_pContextMain->IsEnableEventMedalSet() )  continue;

		pMQFMedalSetInfo = g_pContextMain->GetMedalSet( pCurUserMedalSet->m_ui16Idx ) ;
		if( NULL == pMQFMedalSetInfo ) 
		{
			// 나올수 없는 상황
			I3ASSERT(FALSE);
			continue; 
		}

		// 메달 셋트 내에 활성화된 메달 갯수만큼만 루프
		for( INT32 A_MedalIdx = 0; A_MedalIdx < pMQFMedalSetInfo->m_ui8ActiveMedalCount ; A_MedalIdx++ )
		{
			pMqfMedal = &pMQFMedalSetInfo->m_aMedalInfo[A_MedalIdx];
			if( NULL == pMqfMedal )		// 나오면 안됨.
			{
				I3ASSERT(FALSE);
				continue;
			}

			// 액션이 구입관련이 아니면 continue
			if( pMqfMedal->m_eActionID < MEDAL_MC_ACTION_ID_BUY || pMqfMedal->m_eActionID > MEDAL_MC_ACTION_ID_SHOP_MAX ) continue;

			// 메달 셋트는 Lv1이 한계입니다.
			// Lv1까지 모두 달성했다면 카운팅 하지 않는다. 
			if(  pCurUserMedalSet->m_ui16Count[A_MedalIdx] >= pMqfMedal->m_ui16Count[MEDAL_LEVEL_MAX_SET-1] ) continue;
			MMCInfo.m_pMQFMedalInfo = pMqfMedal;
			MMCInfo.m_eActionID		= pMqfMedal->m_eActionID;	

			if( TRUE == CheckMissionBuy(&MMCInfo) ) 
			{
				pCurUserMedalSet->m_ui16Count[A_MedalIdx]++;	// 유저가 소지한 메달의 카운트 증가

				MedalComplete[ui16CompleteMedalCount].m_ui8MedalType = MEDAL_TYPE_SET;				
				MedalComplete[ui16CompleteMedalCount].m_ui16MedalIdx = pMqfMedal->m_ui16Idx ;			
				MedalComplete[ui16CompleteMedalCount].m_ui16Count = pCurUserMedalSet->m_ui16Count[A_MedalIdx];

				ui16CompleteMedalCount++;
				ui32DBSaveFlag |= SAVE_DATABASE_CUR_MEDAL_SET;

				// 레벨 달성시 ZLog에 기록.
				LOG_MEDAL_LEVELUP_INFO LogInfo;
				LogInfo.m_ui16CompareCount	= pCurUserMedalSet->m_ui16Count[A_MedalIdx];
				LogInfo.m_ui16MedalIdx		= pMqfMedal->m_ui16Idx;
				LogInfo.m_ui16MedalSetIdx	= pCurUserMedalSet->m_ui16Idx;
				LogInfo.m_eMedalType		= MEDAL_TYPE_SET;
				LogInfo.m_eMedalSetType		= (MEDAL_SET_TYPE)pCurUserMedalSet->m_ui8Type;
				g_pModuleContextNC->LogMedalLevelUp( i32ThreadIdx,this, &LogInfo );
			}
		}
	}


	// 미션 달성.
	if( ui16CompleteMedalCount > 0 )
	{
		// 미션 달성( +1 ) 내용을 해당 유저에게 보낸다.
		i3NetworkPacket Packet( PROTOCOL_MISSION_COMPLETE_ACK );
		PACKET_H_MISSION_COMPLETE_ACK		PacketAck;

		PacketAck.m_ui16CompleteMedalCount = ui16CompleteMedalCount;
		Packet.WriteData( &PacketAck, sizeof(PACKET_H_MISSION_COMPLETE_ACK) );
		Packet.WriteData( MedalComplete, sizeof(MEDAL_STATE) * ui16CompleteMedalCount);
		SendPacketMessage( &Packet );

		ChangeDataBaseInfo( ui32DBSaveFlag );
	}

	return;
}


BOOL	CUserSession::CheckMissionBuy( MMC_INFO* pMMCInfo  ) 
{
	if( NULL == pMMCInfo ) 
	{
		I3ASSERT(FALSE);
		return FALSE;
	}

	switch( pMMCInfo->m_eActionID )
	{	
		// Action ID 별로 처리.
	//case MEDAL_MC_ACTION_ID_BUY: break;
	case MEDAL_MC_ACTION_ID_BUY_BY_POINT:
		{
			if( GOODS_BUY_POINT != pMMCInfo->m_ui8BuyType )
			{
				return FALSE;
			}
		}
		break;
	case MEDAL_MC_ACTION_ID_BUY_BY_CASH:
		{
			if( GOODS_BUY_CASH != pMMCInfo->m_ui8BuyType )
			{
				return FALSE;
			}
		}
		break;		
	}


	ITEM_TYPE eItemType = (ITEM_TYPE)GET_ITEM_TYPE( pMMCInfo->m_ui32UseItemID);
	switch( pMMCInfo->m_pMQFMedalInfo->m_eItem)
	{
	case MEDAL_MCF_ITEM_TYPE :
		{ 
			if( pMMCInfo->m_pMQFMedalInfo->m_eUseItemType != eItemType ) return FALSE;
		}
		break;
	case MEDAL_MCF_WEAPON_TYPE :
		{ 
			if( eItemType < ITEM_TYPE_PRIMARY || eItemType > ITEM_TYPE_THROWING2 || pMMCInfo->m_pMQFMedalInfo->m_eWeaponType != (INT32)GET_ITEM_SUBTYPE( pMMCInfo->m_ui32UseItemID )  ) return FALSE;
		}
		break;
	case MEDAL_MCF_ITEM_ID :
		{
			if( pMMCInfo->m_pMQFMedalInfo->m_ui32UseItemID != pMMCInfo->m_ui32UseItemID ) return FALSE;
		}
		break;
	}

	return TRUE;
}


// v1.5 Medal
// 일일 메달 셋트 지급.
void	CUserSession::RefreshDailyMedalSet()
{
	// 서버에서 로드한 일일 메달 셋트 갯수가 0 이면 리턴.
	UINT16 ui16DailyMedalSetCount = g_pContextMain->m_MQFAllDailyMedalSet.m_ui16ActiveCount;
	if( 0 == ui16DailyMedalSetCount ) return;

	// 일일 메달 셋트 인덱스 검색.
	INT32	i32DailyMedalSetSlotIdx = FindCurMedalSetSlot(MEDAL_SET_TYPE_DAILY);
	if( -1 == i32DailyMedalSetSlotIdx ) 
	{	// 일일 메달 셋트를 소지하지 않았으면 랜덤 지급.
		// 닉네임 생성시 일일 메달 셋트를 지급받지 못하면( 그당시 서버에서 로드한 일일 메달 셋트 갯수가 0개이면) 
		// 이상황이 발생활수 있습니다.
		GetDailyMedalSet();	
		return;
	}

	// 오늘 날짜와 지급받은 날짜가 같으면 return
	if( i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() / 10000 == m_MedalAllInfo.m_aCurMedalSet[i32DailyMedalSetSlotIdx].m_ui32RecvDate / 10000 ) return;	
	
	// 랜덤으로 뽑은 메달 셋트 인덱스가 이전과 동일하면 걍 ++ 
	UINT16 ui16RandNum =  rand() % ui16DailyMedalSetCount;	// 랜덤으로 뽑은 일일 메달 셋트 배열 인덱스.
	UINT16 ui16NewMedalSetIdx = g_pContextMain->m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[ui16RandNum].m_ui16Idx; 
	if( ui16NewMedalSetIdx == m_MedalAllInfo.m_aCurMedalSet[i32DailyMedalSetSlotIdx].m_ui16Idx ) 
	{	
		if( ++ui16RandNum >= ui16DailyMedalSetCount ) ui16RandNum = 0;	// 배열 최대값 체크.
	}

	ui16NewMedalSetIdx = g_pContextMain->m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[ui16RandNum].m_ui16Idx;

	RefreshCurMedalSet( (UINT8)i32DailyMedalSetSlotIdx, MEDAL_SET_TYPE_DAILY, ui16NewMedalSetIdx, MEDAL_SET_GET_TYPE_REFRESH );	

	return;
}



// 이벤트 메달 셋트 지급.
void	CUserSession::GetEventMedalSet()
{
	// 서버에서 로드한 이벤트 메달 셋트 갯수가 0 이면 리턴.
	if( 0 == g_pContextMain->m_MQFAllEventMedalSet.m_ui16ActiveCount ) return;

	// 서버에서 진행하는 이벤트 메달 셋트가 있는지 체크.
	if( !g_pContextMain->IsEnableEventMedalSet() ) return;

	// 유저가 이벤트 메달 셋트를 소지중이면 return
	if( -1 != FindCurMedalSetSlot( MEDAL_SET_TYPE_EVENT ) )	return; 

	RefreshCurMedalSet( (UINT8)FindCurMedalSetSlot(MEDAL_SET_TYPE_NORMAL), MEDAL_SET_TYPE_EVENT, g_pContextMain->m_MedalSystemInfo.m_ui16EventMedalSetIdx, MEDAL_SET_GET_TYPE_INSERT );

	return;
}

INT32	CUserSession::FindCurMedalSetSlot(MEDAL_SET_TYPE eMedalSetType)
{
	for( INT32 i = 0;  i < MAX_CUR_MEDAL_SET_COUNT ; i++ )
	{
		switch (eMedalSetType )
		{
		case MEDAL_SET_TYPE_NORMAL:	// 노말 셋트는 서버에선 쓰지 않는 개념이므로 이경우엔 빈슬롯을 찾습니다.
			{
				if( MEDAL_SET_TYPE_NORMAL == (MEDAL_SET_TYPE)m_MedalAllInfo.m_aCurMedalSet[i].m_ui8Type || 
					INIT_MEDAL_NUMBER  == m_MedalAllInfo.m_aCurMedalSet[i].m_ui16Idx ) 
				{
					return i;
				}
			}
			break;
		case MEDAL_SET_TYPE_DAILY :
			{
				if( MEDAL_SET_TYPE_DAILY == (MEDAL_SET_TYPE)m_MedalAllInfo.m_aCurMedalSet[i].m_ui8Type && 
					INIT_MEDAL_NUMBER != m_MedalAllInfo.m_aCurMedalSet[i].m_ui16Idx ) 
				{
					return i;
				}
			}
			break;
		case MEDAL_SET_TYPE_EVENT :
			{
				if( MEDAL_SET_TYPE_EVENT == (MEDAL_SET_TYPE)m_MedalAllInfo.m_aCurMedalSet[i].m_ui8Type  ) 
				{
					return i;
				}
			}
			break;
		default : 
			return -1;
			break;
		}
	}

	// 못찾으면 
	return -1;
}

void	CUserSession::RefreshCurMedalSet( UINT8 ui8ArrIdx, MEDAL_SET_TYPE eMedalSetType, UINT16 ui16MedalSetIdx, MEDAL_SET_GET_TYPE eGetType )
{ 
	if( ui8ArrIdx >= MAX_CUR_MEDAL_SET_COUNT  ) return ;

	i3mem::FillZero( &m_MedalAllInfo.m_aCurMedalSet[ui8ArrIdx] , sizeof( CUR_MEDAL_SET ) );

	m_MedalAllInfo.m_aCurMedalSet[ui8ArrIdx].m_ui8Type = (UINT8)eMedalSetType;
	m_MedalAllInfo.m_aCurMedalSet[ui8ArrIdx].m_ui16Idx = ui16MedalSetIdx;
	m_MedalAllInfo.m_aCurMedalSet[ui8ArrIdx].m_ui32RecvDate	= i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();

	ChangeDataBaseInfo(SAVE_DATABASE_CUR_MEDAL_SET);

	// 갱신된 CurMedalSet 정보를 클라이언트에 알려준다.
	PACKET_SET_REFRESH_MEDAL_SET_ACK PacketAck;
	i3mem::Copy( PacketAck.m_aCurMedalSet, m_MedalAllInfo.m_aCurMedalSet, sizeof( CUR_MEDAL_SET ) * MAX_CUR_MEDAL_SET_COUNT );
	i3NetworkPacket Packet( PROTOCOL_SET_REFRESH_MEDAL_SET_ACK  );
	Packet.WriteData( &PacketAck, sizeof(PACKET_SET_REFRESH_MEDAL_SET_ACK) );
	SendPacketMessage( &Packet );
}

void	CUserSession::SendFailMedalNotifySetting()	// 알리미 메달 등록 실패
{
	PACKET_SET_NOTIFY_MEDAL_ACK	PacketAck;
	PacketAck.m_i32Rv = EVENT_ERROR_PBV15_MEDAL_NOTIFY_SETTING_FAIL;

	i3NetworkPacket Packet( PROTOCOL_SET_NOTIFY_MEDAL_ACK  );
	Packet.WriteData( &PacketAck, sizeof(PACKET_SET_NOTIFY_MEDAL_ACK) );
	SendPacketMessage( &Packet );

	return;
}
 
void CUserSession::UpdateNotufyMedalCount(void)
{
	if( !g_pContextMain->IsContentsEnabled( CONTENTS_MEDAL ) )	 return;

	for( UINT8 i = 0; i < MAX_NOTIFY_MADAL_COUNT ; i++ )
	{
		switch( m_MedalAllInfo.m_aNotifyMedal[i].m_ui8MedalType ) 
		{
		case MEDAL_TYPE_NORMAL:	// 일반 메달.
			{
				MEDAL_GT*	pMedalGS = GetMedal_UserDataAll( m_MedalAllInfo.m_aNotifyMedal[i].m_ui16Idx);
				if( NULL == pMedalGS ) continue;
				m_MedalAllInfo.m_aNotifyMedal[i].m_ui16Count = pMedalGS->m_Medal.m_ui16Count;
			}
			break;
		case MEDAL_TYPE_SET:	// 진행중인 메달 셋트.
			{
				MQF_MEDAL_SET_INFO* pMedalSet = g_pContextMain->GetMedalSet_ByMedalIdx(m_MedalAllInfo.m_aNotifyMedal[i].m_ui16Idx); // 메달 고유 인덱스로 메달 셋트 데이터 검색.
				if( NULL == pMedalSet ) continue;

				INT8 i8ArrIdx = g_pContextMain->GetSetMedalArrIdx_ByMedalIdx( m_MedalAllInfo.m_aNotifyMedal[i].m_ui16Idx ); // 메달 고유 인덱스로 메달 셋트에서 몇번째 메달인지 검색.
				if( -1 == i8ArrIdx ) continue;

				for( UINT8 k = 0 ; k < MAX_CUR_MEDAL_SET_COUNT ; k++ )
				{
					if( pMedalSet->m_ui16Idx == m_MedalAllInfo.m_aCurMedalSet[k].m_ui16Idx ) // 일치하는 셋트면
					{
						m_MedalAllInfo.m_aNotifyMedal[i].m_ui16Count = m_MedalAllInfo.m_aCurMedalSet[k].m_ui16Count[i8ArrIdx-1];
						break;
					}
				}	 
			}
			break;
		}
	}
}
 
void	CUserSession::RecvMedalAllInfoFromTrans( i3NetworkPacket* pPacket )
{
	if( !g_pContextMain->IsContentsEnabled( CONTENTS_MEDAL ) )	 return;

	if (m_ui8LocaleCode >= LOCALE_CODE_MAX) return;

	PACKET_D_GET_USER_INFO_MEDAL	PacketMedalD;
	pPacket->ReadData( &PacketMedalD,	sizeof(PACKET_D_GET_USER_INFO_MEDAL) );	


	// 1. Medal Basic Info
	i3mem::Copy( m_MedalAllInfo.m_aNotifyMedal, PacketMedalD.m_aNotifyMedal , sizeof( NOTIFY_MEDAL ) * MAX_NOTIFY_MADAL_COUNT );

	// 2. Normal Medal Recv
	if( PacketMedalD.m_ui16MedalCount > 0 )
	{
		MEDAL		RecvMedal;
		for( UINT16 i = 0 ; i < PacketMedalD.m_ui16MedalCount ; i++ )
		{
			pPacket->ReadData(&RecvMedal,		sizeof(MEDAL));

			// MQF 데이터가 없다면 버린다. 있을수 없는일. 
			MQF_MEDAL_INFO *pMQFMedalInfo = g_pContextMain->GetMedal_MQFData(RecvMedal.m_ui16Idx, MEDAL_TYPE_NORMAL, m_ui8LocaleCode);
			if( NULL == pMQFMedalInfo ) continue;

			// 읽어들인 일반 메달 데이터를 미션 별로 구분해서 담는다.

			for( INT32 Type = MEDAL_MC_CHECK_POSITION_START ; Type < MEDAL_MC_CHECK_POSITION_MAX ; Type++ )
			{
				if( Type != pMQFMedalInfo->m_eCheckPosition ) continue;

				m_MedalAllInfo.m_aMedal[Type][m_MedalAllInfo.m_aui16MedalCount[Type]].m_Medal = RecvMedal;
				m_MedalAllInfo.m_aui16MedalCount[Type]++;
			}

		} // for
	}// 2. Normal Medal Recv End 


	// 3. Normal Medal Setting. DB에서 읽어들인 일반 메달 정보와 MQF 파일 정보를 동기화 시킨다.
	MEDAL_GT *pMedalGS;
	UINT16 ui16UserMedalCount = 0;
	UINT16 ui16MedalIdx = 0;

	//  일반 메달 동기화.
	MQF_ALL_MEDAL_INFO*	pServerMQF = &g_pContextMain->m_MQFMedal[m_ui8LocaleCode];
	for( INT32 Type = MEDAL_MC_CHECK_POSITION_START; Type < MEDAL_MC_CHECK_POSITION_MAX ; Type++ )
	{
		for (INT32 MedalIdx = 0; MedalIdx < pServerMQF->m_aui16MQFMedalCount[Type]; MedalIdx++)
		{
			ui16MedalIdx = pServerMQF->m_aMedalInfo[Type][MedalIdx].m_ui16Idx;
			pMedalGS = GetMedal_UserDataType( ui16MedalIdx, (MEDAL_MC_CHECK_POSITION)Type );
			if( NULL == pMedalGS ) // MQF에는 있는데 유저에게 없으면( 카운팅이 한번도 되지않아 DB에 데이터가 없으면) 초기화.
			{
				ui16UserMedalCount = m_MedalAllInfo.m_aui16MedalCount[Type];
				m_MedalAllInfo.m_aMedal[Type][ui16UserMedalCount].m_Medal.m_ui16Idx = ui16MedalIdx;
				m_MedalAllInfo.m_aMedal[Type][ui16UserMedalCount].m_Medal.m_ui16Count = 0 ;
				m_MedalAllInfo.m_aMedal[Type][ui16UserMedalCount].m_Medal.m_ui8GetReward = 0;
				m_MedalAllInfo.m_aui16MedalCount[Type]++;
			}
		}
	}

	// 메달 고유 인덱스 오름 차순으로 정렬.
	MEDAL_GT temp; 
	for( INT32 Type = MEDAL_MC_CHECK_POSITION_START; Type < MEDAL_MC_CHECK_POSITION_MAX ; Type++ )
	{
		for( INT32 r=0; r< m_MedalAllInfo.m_aui16MedalCount[Type] -1; r++)
		{
			for( INT32 t=0; t< m_MedalAllInfo.m_aui16MedalCount[Type]-r-1; t++)
			{
				if( m_MedalAllInfo.m_aMedal[Type][t].m_Medal.m_ui16Idx > m_MedalAllInfo.m_aMedal[Type][t+1].m_Medal.m_ui16Idx )
				{
					temp = m_MedalAllInfo.m_aMedal[Type][t+1];
					m_MedalAllInfo.m_aMedal[Type][t+1] = m_MedalAllInfo.m_aMedal[Type][t];
					m_MedalAllInfo.m_aMedal[Type][t] = temp;					
				}
			}
		}
	}


	// 3. Current Medal Set Recv
	if( PacketMedalD.m_ui16CurMedalSetCount > 0 )
	{
		CUR_MEDAL_SET		RecvMedalSet;
		for( UINT16 MedalSetCnt = 0 ; MedalSetCnt < PacketMedalD.m_ui16CurMedalSetCount ; MedalSetCnt++ )
		{
			pPacket->ReadData(&RecvMedalSet,		sizeof(CUR_MEDAL_SET));

			// MQF 데이터가 없다면 버린다.  
			MQF_MEDAL_SET_INFO *pMQFMedalSetInfo = g_pContextMain->GetMedalSet( RecvMedalSet.m_ui16Idx );
			if( NULL == pMQFMedalSetInfo ) continue;

			m_MedalAllInfo.m_aCurMedalSet[MedalSetCnt] = RecvMedalSet;
		}
	}


	// 4. Current Medal Set Refresh
	RefreshDailyMedalSet();	// 일일 메달 셋트 지급.
	GetEventMedalSet();	// 이벤트 메달 셋트 지급.

	return;
}

void	CUserSession::GetDailyMedalSet()
{
	// 서버에서 로드한 일일 메달 셋트 갯수가 0 이면 리턴.
	UINT16 ui16DailyMedalSetCount = g_pContextMain->m_MQFAllDailyMedalSet.m_ui16ActiveCount;
	if( 0 == ui16DailyMedalSetCount ) return;

	// 빈슬롯 찾기.
	INT32	i32EmptySlotIdx = FindCurMedalSetSlot(MEDAL_SET_TYPE_NORMAL);
	if( -1 == i32EmptySlotIdx )	return;
		
	// 랜덤으로 뽑은 일일 메달 셋트 배열 인덱스.
	UINT16 ui16RandNum =  rand() % ui16DailyMedalSetCount;
	RefreshCurMedalSet( (UINT8)i32EmptySlotIdx, MEDAL_SET_TYPE_DAILY, g_pContextMain->m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[ui16RandNum].m_ui16Idx, MEDAL_SET_GET_TYPE_INSERT );	
	
	return;
}

INT32 CUserSession::_ExtendItem( EXTEND_BASKET* stExtendItemData )
{
	// TWardDBIdx 해킹 체크.
	INVEN_BUFFER* pInven = GetItemByWareDBIdx( stExtendItemData->m_TWareDBIdx );
	if( NULL == pInven ) return EVENT_ERROR_SHOP_EXTEND_HACKING;	// 인벤에 없는 DB Idx.

	UINT32 ui32GoodsId = stExtendItemData->m_Basket.m_ui32GoodsID;

	UINT32 ui32GoodsType = GET_GOODS_FLAG_TYPE(ui32GoodsId);
	if(ITEM_TYPE_CHARA == ui32GoodsType || ITEM_TYPE_SET== ui32GoodsType)		return EVENT_ERROR_SHOP_EXTEND_WRONG_GOODS_TYPE;	// 캐릭터, 셋트 상품은 연장불가.( 인벤토리 기준이므로. )

	// 클라이언트가 보내온 GoodsID에 해당하는 아이템을 실제로 인벤토리에 소지하고 있는지 체크합니다>
	CShop* pShop = g_pContextMain->GetShop();
	SHOP_MATCHING_DATA*	  pMatching = pShop->GetMatchingAllData();
	UINT32	ui32MatchingCount	= pShop->GetMatchingCount();
	for( UINT32 k = 0; k < ui32MatchingCount; ++k )
	{
		if( pMatching[k]._ui32GoodsID != ui32GoodsId) continue;
		if( pMatching[k]._ui32ItemID != pInven->_ui32ItemID ) return EVENT_ERROR_SHOP_EXTEND_HACKING;
		break;
	}	
	
	SHOP_GOODS_EXPANSION* pGoods = pShop->GetGoodsData( ui32GoodsId );
	
	BUY_PRICE	stBuyPrice;
	i3mem::FillZero( &stBuyPrice, sizeof(BUY_PRICE));
	stBuyPrice.m_ePriceType = PRICE_TYPE_SHOP_REAL;
	stBuyPrice.m_eBuyType	= (SIA_GOODS_BUY)stExtendItemData->m_Basket.m_ui8BuyType;
	 
	INT32 i32Rv = _CheckBuyGoods( pGoods, &stBuyPrice );
	if( EV_FAILED( i32Rv ) )
	{
		return i32Rv;
	}

	if( FALSE == g_pModuleTrans->AuthSendExtendItem( m_WorkThreadIdx, this, stExtendItemData ))		return EVENT_ERROR_EVENT_BUY_GOODS_NETWORK;

	// 포인트 복사 방지를 위해 먼저 차감 합니다. 구매 실패시 복구함.
	if( m_UserInfoBasic.m_ui32Point > stBuyPrice.m_ui32Price[GOODS_BUY_POINT] )
	{
		m_UserInfoBasic.m_ui32Point	-= stBuyPrice.m_ui32Price[GOODS_BUY_POINT];
	}
	else m_UserInfoBasic.m_ui32Point = 0;

	ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );

	return EVENT_ERROR_SUCCESS;
}

INT32 CUserSession::_CheckBuyGoods( SHOP_GOODS_EXPANSION* pGoods, BUY_PRICE *pBuyPrice )
{	// GOOD정보, 유저 잔액 검사합니다.
	if( NULL == pGoods )												return EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_GOODS;	// 상품이 없다.
	if( GOODS_USE_DISABLE == pGoods->_ui8Enable )						return EVENT_ERROR_EVENT_BUY_GOODS_NOT_ALLOWED;		// 판매하지 않는 상품.				
	if( GOODS_SALE_EVENT == pGoods->_ui8SaleType )						return EVENT_ERROR_EVENT_BUY_GOODS_NOT_ALLOWED;		// 이벤트용 상품					 
	if( GOODS_SALE_BUY == pGoods->_ui8SaleType && PRICE_TYPE_SHOP_REAL != pBuyPrice->m_ePriceType)	return EVENT_ERROR_EVENT_BUY_GOODS_NOT_ALLOWED;		// 상점 판매용 상품
	if( GOODS_SALE_GIFT == pGoods->_ui8SaleType && PRICE_TYPE_GIFT_REAL != pBuyPrice->m_ePriceType)	return EVENT_ERROR_EVENT_BUY_GOODS_NOT_ALLOWED;		// 상점 판매용 상품
	if( GOODS_BUY_CHAR_FREE == pBuyPrice->m_eBuyType  )					return EVENT_ERROR_SUCCESS;							// 캐릭터 첫 생성은 무료.

	UINT32 ui32UserMoney = 0;

	UINT32 ui32GoodsPrice	= pGoods->GetPrice(pBuyPrice->m_eBuyType, pBuyPrice->m_ePriceType);
	 
	if( 1 > ui32GoodsPrice )											return EVENT_ERROR_SHOP_BUY_FAIL_NO_GOODS_PRICE;	// 상품 가격이 0원이하.

	pBuyPrice->m_ui32Price[pBuyPrice->m_eBuyType] +=  ui32GoodsPrice;
		
	// 구매 유형
	switch( pBuyPrice->m_eBuyType )
	{
		case GOODS_BUY_POINT:		ui32UserMoney = m_UserInfoBasic.m_ui32Point; 	break; 
		case GOODS_BUY_CASH:		ui32UserMoney = m_UserInfoBasic.m_ui32Cash;		break; 
		case GOODS_BUY_COIN:		ui32UserMoney = m_UserInfoBasic.m_ui32Coin;		break; 
		default:	return EVENT_ERROR_FAIL; 
	}
 
	if( ui32UserMoney <  pBuyPrice->m_ui32Price[pBuyPrice->m_eBuyType] )		return EVENT_ERROR_EVENT_AUTH_START_PRICE;			// 돈이 모자람

	return EVENT_ERROR_SUCCESS;
}

void CUserSession::_FailDeleteChara(INT32 i32Rv, UINT8 ui8CharaSlotIdx )
{
	i3NetworkPacket SendPacket(PROTOCOL_CHAR_DELETE_CHARA_ACK);
	SendPacket.WriteData(&i32Rv,					sizeof(INT32));
	SendPacket.WriteData(&ui8CharaSlotIdx,			sizeof(UINT8));
	SendPacketMessage( &SendPacket );
}

void CUserSession::DeleteChar_A(INT32 i32WorkThread, T_RESULT i32Result, UINT8 ui8DeleteSlotIdx)
{	
	INVEN_BUFFER_SET	InvenSet;
	ITEM_INFO			CharItemInfo;

	i3NetworkPacket SendPacket(PROTOCOL_CHAR_DELETE_CHARA_ACK);
	SendPacket.WriteData(&i32Result,				sizeof(INT32));
	SendPacket.WriteData(&ui8DeleteSlotIdx,			sizeof(UINT8));
	
	if (!S2MO_IS_SUCCESS(i32Result))
	{
		SendPacketMessage(&SendPacket);
		return;
	}

	
	m_MultiSlot.GetCharEquipment( (INT32)ui8DeleteSlotIdx, CHAR_EQUIPMENT_CHARA, &CharItemInfo );

	SendPacket.WriteData(&CharItemInfo.m_TItemDBIdx,				sizeof(T_ItemDBIdx));

	INT32 i32MainSlotIdx = -1;
	
	// 멀티 슬롯에서 제거.
	BOOL bDelete = m_pDesign->DeleteChara(m_MultiSlot, (INT32)ui8DeleteSlotIdx, i32MainSlotIdx);

	if ( bDelete )
	{	// 인벤토리에서 제거.
		OnDeleteItem(i32WorkThread, CharItemInfo.m_TItemDBIdx, &InvenSet);

		if (i32MainSlotIdx > -1) 
		{
			ChangeDataBaseInfo(SAVE_DATABASE_CHARA_COMMON); 
		}
	}

	// 변경된 메인 슬롯 정보 전달
	SendPacket.WriteData(&i32MainSlotIdx,				sizeof(INT32));

	SendPacketMessage(&SendPacket);
}

void CUserSession::SendFindDetailUserInfo( T_RESULT TResult, S2_USER_DETAIL_INFO* pUserInfo )
{
	PACKET_BASE_GET_USER_DETAIL_INFO_ACK Packet;
	
	m_i64FindUID = pUserInfo->m_TUID;

	//찾은 유저의 현재 표시 랭크를 기록(친구 추가를 위해)
	m_ui8FindUserRank		= pUserInfo->m_ui8Rank;
	m_ui8FindUserNickColor	= pUserInfo->m_InvenData._ui8NickColor;

	if( RANK_NONE != pUserInfo->m_InvenData._wRank )
		m_ui8FindUserRank = (UINT8)pUserInfo->m_InvenData._wRank;

	if( RANK_NONE != pUserInfo->m_InvenData._wAprilFoolRank )
		m_ui8FindUserRank = (UINT8)pUserInfo->m_InvenData._wAprilFoolRank;

	Packet.m_TResult = TResult;
	if( EV_SUCCESSED( TResult ))
	{
		i3mem::Copy( &Packet.m_UserDetailInfo, pUserInfo, sizeof(S2_USER_DETAIL_INFO) );
	}

	S2MOSendEvent( &Packet );
}

void CUserSession::_SetUserDetailInfo( S2_USER_DETAIL_INFO* pstInfo, CUserSession * pSession )
{
	pstInfo->m_TUID				= pSession->m_i64UID;
	pstInfo->m_ui32ClanMark		= pSession->m_UserInfoBasic.m_ui32ClanMark;	
	pstInfo->m_ui8Rank			= (UINT8)pSession->m_ui32ViewRank;
	pstInfo->m_ui32Exp			= pSession->m_UserInfoBasic.m_ui32Exp;
	pstInfo->m_ui32Match		= pSession->m_UserInfoRecord._match;
	pstInfo->m_ui32Win			= pSession->m_UserInfoRecord._win;
	pstInfo->m_ui32Draw			= pSession->m_UserInfoRecord._draw;
	pstInfo->m_ui32Lose			= pSession->m_UserInfoRecord._lose; 
	pstInfo->m_ui32DisConnect	= pSession->m_UserInfoRecord._dis;
	pstInfo->m_ui32Kill			= pSession->m_UserInfoRecord._killcount;
	pstInfo->m_ui32Death		= pSession->m_UserInfoRecord._death;
	pstInfo->m_ui32HeadShot		= pSession->m_UserInfoRecord._headshot;
	pstInfo->m_ui32ServerIdx	= g_pContextMain->m_i32ServerIdx;
	pstInfo->m_ui32ChannelNum	= pSession->m_i32ChannelNum;
	pstInfo->m_ui32Assist = pSession->m_UserInfoRecord._AssistCount;
//	pstInfo->m_i8CharCount		= (INT8)pSession->m_MultiSlot.GetCharSlotCount();
		
	i3mem::Copy( &pstInfo->m_LastGame, &pSession->m_UserInfoLastGame , sizeof(USER_INFO_LAST_GAME) );
	i3mem::Copy( &pstInfo->m_InvenData, &pSession->m_UserInfoInvItemData , sizeof(USER_INFO_INVITEM_DATA) );

	if( GAME_TASK_LOBBY < pSession->GetPosition() )
	{
		pstInfo->m_ui32State		= FRIEND_STATE_ROOM;
		pstInfo->m_ui32RoomIdx	= pSession->m_i32RoomIdx;
	}
	else
	{
		pstInfo->m_ui32State		= FRIEND_STATE_ONLINE;
	}

	S2MO_CHAR_BASE_INFO		BasicInfo;

	INT8 i8CharInfoIdx = 0;
	ITEM_INFO			CharItemInfo;

	for( INT8 i = 0 ; i < S2MO_MULTI_SLOT_COUNT; ++i )
	{
		//if (i >= S2MO_MULTI_SLOT_PACKET_COUNT) break;
		if( FALSE == pSession->m_MultiSlot.IsActiveChar(i, MULTI_SLOT_CHECK_USING) ) continue;

		pSession->m_MultiSlot.GetCharBaseInfo( i, &BasicInfo );
		pSession->m_MultiSlot.GetCharEquipment( i, CHAR_EQUIPMENT_CHARA, &CharItemInfo );

		pstInfo->m_aCharInfo[i8CharInfoIdx].m_ui8SlotIdx	= i;
		pstInfo->m_aCharInfo[i8CharInfoIdx].m_TItemID			= CharItemInfo.m_TItemID;
		pstInfo->m_aCharInfo[i8CharInfoIdx].m_ui32CreateDate		 = BasicInfo.m_ui32CreateDate;
		pstInfo->m_aCharInfo[i8CharInfoIdx].m_ui32BattleTime	 = BasicInfo.m_ui32BattleTime;
		pstInfo->m_aCharInfo[i8CharInfoIdx].m_ui32SkillRelTime = BasicInfo.m_ui32SkillRelTime;

		i3String::Copy( pstInfo->m_aCharInfo[i8CharInfoIdx].m_strCharName, BasicInfo.m_strCharName, NET_NICK_NAME_SIZE );

		pSession->m_MultiSlot.GetCharSkillInfo( i, &pstInfo->m_aCharInfo[i8CharInfoIdx].m_SkillInfo );
		
		++i8CharInfoIdx;
	}

	pstInfo->m_i8CharCount = i8CharInfoIdx;

	i3String::Copy( pstInfo->m_strNickName, pSession->m_UserInfoBasic.m_strNick, NET_NICK_NAME_SIZE );
	i3String::Copy( pstInfo->m_strClanName, pSession->m_UserInfoBasic.m_strClanName, NET_CLAN_NAME_SIZE );
	
	//찾은 유저의 UID를 기록(클랜초대를 위해)
	m_i64FindUID = pSession->m_i64UID;

	//찾은 유저의 현재 표시 랭크를 기록(친구 추가를 위해)
	m_ui8FindUserRank = (UINT8)pSession->m_ui32ViewRank;
	m_ui8FindUserNickColor = pSession->m_UserInfoInvItemData._ui8NickColor;

	if( RANK_NONE != pSession->m_UserInfoInvItemData._wRank )
		m_ui8FindUserRank = (UINT8)pSession->m_UserInfoInvItemData._wRank;

	if( RANK_NONE != pSession->m_UserInfoInvItemData._wAprilFoolRank )
		m_ui8FindUserRank = (UINT8)pSession->m_UserInfoInvItemData._wAprilFoolRank;
}

// 빠른입장 방입장.
INT32	CUserSession::_OnJoinRoomByQuickJoin( INT32 i32ChannelIdx, INT32 i32RoomIdx )
{
	// 1. 로비에서 빠른 입장시 채널 변경이 없다면 방입장.
	// 2. 로비에서 빠른 입장시 채널 변경이 있다면	1.채널 퇴장 -	2.채널 입장 - 3.로비 입장 - 4.방입장
	// 3. 채널&서버에서 빠른 입장시								1.채널 입장 - 2.로비 입장 - 3.방입장
	
	INT32 i32Rv;
	ROOM_INFO_JOIN RoomInfoJoin;
	RoomInfoJoin.m_ui8EnterType = S2MO_ROOM_ENTER_QUICK_JOIN;

	if( GAME_TASK_LOBBY == m_i32MainTask ) // 로비에서 빠른 입장이라면 현재 자신의 채널과 비교후 처리. 
	{
		// 채널 변경이 없다면 방입장만.
		if( m_i32ChannelNum == i32ChannelIdx )
		{
			return g_pRoomManager->OnJoinRoom( m_WorkThreadIdx, this, i32ChannelIdx, i32RoomIdx, &RoomInfoJoin ) ;			 
		}
		
		// 다른 채널 이동을 위해 현재 채널 퇴장
		g_pUserManager->OnLeaveChannel( this, m_i32ChannelNum);
		_OnLeaveChnnel(); 
		//메시지 보내기 
		INT32 i32Temp = 0;
		i3NetworkPacket	ChannelLeavePacket;
		ChannelLeavePacket.SetProtocol(PROTOCOL_LOBBY_LEAVE_ACK);
		ChannelLeavePacket.WriteData(&i32Temp, sizeof(INT32));
		SendPacketMessage(&ChannelLeavePacket);
	}
	
	// 신 채널 입장
	i32Rv = g_pUserManager->OnEnterChannel(this, i32ChannelIdx );
	if( EV_FAILED( i32Rv ) )	return i32Rv;

	m_i32ChannelNum		= i32ChannelIdx; 
	m_i32LogChannelNum	= ((g_pContextMain->m_i32ServerIdx*GAME_CHANNEL_COUNT) + (m_i32ChannelNum+1));

	// 신 로비 입장
	i3NetworkPacket	LobbyEnterPacket;
	LobbyEnterReq( m_WorkThreadIdx, &LobbyEnterPacket, S2MO_ROOM_ENTER_QUICK_JOIN );	// SetPosition(GAME_TASK_LOBBY) 일어남.
	SendPacketMessage( &LobbyEnterPacket );	// 로비입장 패킷 줍니다.

	// 방리스트, 채널리스트를 줍니다.
	m_bRequestRoomList = TRUE;
	g_pUserManager->SendListInfoPaket( g_pContextMain->m_i32ServerIdx, this );
	 
	// 방 입장
	return g_pRoomManager->OnJoinRoom( m_WorkThreadIdx, this, i32ChannelIdx, i32RoomIdx, &RoomInfoJoin );
}



void	CUserSession::_OnLeaveChnnel()
{
	SetPosition(GAME_TASK_CHANNEL); 			//순서 중요
	m_i32ChannelNum		= SERVER_VALUE_UNKNOWN;
	m_i32LogChannelNum	= SERVER_VALUE_UNKNOWN;

	// 계급 변화가 있는지 확인
	CheckRankUp( m_WorkThreadIdx );

	// 클랜서버에 알려준다.
	g_pModuleClan->ClanSendChannelLeave(m_WorkThreadIdx, this );
	g_pModuleTrans->AuthSendChannelLeave( m_WorkThreadIdx, this );

	g_pModuleMessenger->MessengerSendChannelLeave(m_WorkThreadIdx, this, m_i64UID );
}


void	CUserSession::LobbyEnterReq( INT32 i32WorkIdx, i3NetworkPacket* pSendPacket, S2MO_ROOM_ENTER eEnterType )
{
	INT32	i32Rv;

	//1. 방에 있으면 방에서 나가기 방어 코드 
	g_pRoomManager->OnLeaveRoom(this, m_i32ChannelNum, i32WorkIdx, ROOM_ENDTYPE_NONE);

	//2. 룸리스트를 보낸다. 
	m_bRequestRoomList		= FALSE;
	//m_bRequestClanRoomList	= FALSE;
	m_ui32UpdateRoomListPage	= 100;  // 0번 부터 받기 위해서 큰수를 사용했습니다. 
	m_UpdateUserListPage		= 100;  // 0번 부터 받기 위해서 큰수를 사용했습니다.  

	//3. 포지션 세팅
	SetPosition(GAME_TASK_LOBBY);

	//4. 정상적인 메시지를 보낸다. 
	UINT8 ui8EnterType = (UINT8)eEnterType;
	i32Rv = EVENT_ERROR_SUCCESS; 
	UINT8 ui8IsMerPenalty = IsMerPenalty();
	pSendPacket->SetProtocol( PROTOCOL_LOBBY_ENTER_ACK );
	pSendPacket->WriteData(&i32Rv,			sizeof(INT32) );
	pSendPacket->WriteData(&ui8EnterType,	sizeof(UINT8) );
	pSendPacket->WriteData(&ui8IsMerPenalty,sizeof(UINT8) );

	// 클랜서버에 알려준다.
	g_pModuleClan->ClanSendChannelEnter( i32WorkIdx, this, m_i32ChannelNum );
	g_pModuleTrans->AuthSendChannelEnter( i32WorkIdx, this );

	g_pModuleMessenger->MessengerSendChannelEnter(i32WorkIdx, this, m_i64UID, m_i32ChannelNum );

	return;
}


INT32 CUserSession::_RepairItem( T_ItemDBIdx TWareDBIdx, UINT32* pui32Point, UINT32* pui32Cash,  UINT32* pui32AuthArg,  T_ItemID* pTItemID )
{	
	INVEN_BUFFER* pInven = GetItemByWareDBIdx( TWareDBIdx );

	if( NULL == pInven )	return	EVENT_ERROR_SHOP_NOT_FIND_ITEM;

	CShop* pShop = g_pContextMain->GetShop();

	SHOP_ITEM_BASE*	pItem		= pShop->GetItemData(  pInven->_ui32ItemID );
	SHOP_ITEM_DATA*	pItemEx		= pShop->GetItemDataEx(  pInven->_ui32ItemID );

	if( NULL == pItem || NULL == pItemEx )						return	EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_GOODS;

	if( ITEM_TYPE_COUNT != pItem->_ui8AuthType )				return EVENT_ERROR_EVENT_BUY_GOODS_REPAIR; 
	else if( ITEM_EXPIRE_LEAVE != pItem->_ui8AuthExpireType )	return EVENT_ERROR_EVENT_BUY_GOODS_REPAIR; 
	else if( pItemEx->_ui32Durability <= pInven->_ui32ItemArg )	return EVENT_ERROR_EVENT_BUY_GOODS_REPAIR; 
	else if( 0 == pItemEx->_ui32Durability )					return EVENT_ERROR_EVENT_BUY_GOODS_REPAIR; 

	UINT32 ui32Arg = pItemEx->_ui32Durability - pInven->_ui32ItemArg;						

	switch( g_pContextMain->GetContentsVersion( CONTENTS_REPARE_DURABILITY ) )
	{
	case REPARE_DURABILITY_VER_1:
		{
			*pui32Point		= (ui32Arg * pItemEx->_ui32RepairPoint);
			*pui32Cash		= (ui32Arg * pItemEx->_ui32RepairCash);
		}
		break;
	case REPARE_DURABILITY_VER_2:
		{
			if( 0 < pItemEx->_ui32RepairPoint )
			{
				*pui32Point	= ((pItemEx->_ui32RepairPoint * ui32Arg)/pItemEx->_ui32Durability);
				*pui32Point	+= static_cast<UINT32>( *pui32Point * _GetRepairPremium(static_cast<UINT32>((100.f/pItemEx->_ui32Durability)*pInven->_ui32ItemArg)) );
				if( 0 == *pui32Point ) { *pui32Point = 1; }
			}
			else if( 0 < pItemEx->_ui32RepairCash )
			{
				*pui32Cash	= ((pItemEx->_ui32RepairCash * ui32Arg)/pItemEx->_ui32Durability);
				*pui32Cash	+= static_cast<UINT32>( *pui32Cash * _GetRepairPremium(static_cast<UINT32>((100.f/pItemEx->_ui32Durability)*pInven->_ui32ItemArg)) );
				if( 0 == *pui32Cash )  { *pui32Cash = 1; }
			}
		}
		break;
	}

	// 둘중 하나는 반드시 0입니다. 웹툴에서 제어.
	if( *pui32Point	> m_UserInfoBasic.m_ui32Point || *pui32Cash	> m_UserInfoBasic.m_ui32Cash )	return EVENT_ERROR_EVENT_AUTH_START_PRICE; 

	*pui32AuthArg = ui32Arg;	
	*pTItemID = pInven->_ui32ItemID;	
	
	return EVENT_ERROR_SUCCESS;
}

void	CUserSession::SendRepairResult( UINT8 ui8RepairItemCount, S2MO_SHOP_REPAIR_RESULT *pInfo )
{
 	PACKET_SHOP_REPAIR_ACK Packet;
	Packet.m_ui32Point	= m_UserInfoBasic.m_ui32Point;
	Packet.m_ui32Cash	= m_UserInfoBasic.m_ui32Cash;
	Packet.m_aRepairResult.SetValue( pInfo, 0, ui8RepairItemCount );

	S2MOSendEvent( &Packet );

	
	return;

}

void CUserSession::OnRepairItem( S2MO_REPAIR_INFO *pstInfo )
{
	INVEN_BUFFER_SET InvenSet;	

	InvenSet.ui8ChangeType = ITEM_CHANGE_TYPE_REPAIR;

	for( INT32 i = 0 ; i < pstInfo->m_ui8ItemCount ; i++ )
	{
		if( 0 == pstInfo->m_stRepairItem[i].m_TItemDBIdx ) continue;

		m_Inventory.SetItemArg( pstInfo->m_stRepairItem[i].m_TItemDBIdx, pstInfo->m_stRepairItem[i].m_ui32AuthArg, &InvenSet );
	}
	SendChangeInven( &InvenSet );

	return;
}

INVEN_BUFFER* CUserSession::GetItemByWareDBIdx( T_ItemDBIdx& TWareDBIdx )
{
	return m_Inventory.GetItemByWareDBIdx( TWareDBIdx );
}

INVEN_BUFFER* CUserSession::GetItemByItemID( T_ItemID TItemID )
{
	return m_Inventory.GetItemByItemID( TItemID );
}

UINT32 CUserSession::GetUseItemInRoom()
{
	UINT32 ui32UseItem = m_Inventory.GetUseItemInRoom();

	// Respawn 계산 처리
	if( ui32UseItem & SLOT_ITEM_SHORT_RESPAWN_100 )			m_ui32RespawnDelay = RESPAWN_TIME_SHORT_100;
	else if( ui32UseItem & SLOT_ITEM_SHORT_RESPAWN_50 )		m_ui32RespawnDelay = RESPAWN_TIME_SHORT_50;
	else if( ui32UseItem & SLOT_ITEM_SHORT_RESPAWN_30 )		m_ui32RespawnDelay = RESPAWN_TIME_SHORT_30;
	else if( ui32UseItem & SLOT_ITEM_SHORT_RESPAWN_20 )		m_ui32RespawnDelay = RESPAWN_TIME_SHORT_20;
	else													m_ui32RespawnDelay = RESPAWN_TIME_NO;

	return ui32UseItem;
}

void CUserSession::GetItemBonusInRoom( INT32 i32Hour, REAL32& r32AddExpItem, REAL32& r32AddPotItem, REAL32& r32AddSkillItem, UINT8& ui8ResultIcon )
{
	m_Inventory.GetItemBonusInRoom( i32Hour, r32AddExpItem, r32AddPotItem, r32AddSkillItem, ui8ResultIcon );
}

AAS_CATCH_METHOD CUserSession::CheckAAS()
{
	// 1. 하루이용량 제한 방식이 ON되어 있으면
	if ( m_bAASDailyUsageWay ) 
	{
		if ( GetAASAccMin() > g_pContextMain->m_AASOption.m_ui16AASDailyUsageMaxMin ) return CATCH_DAILY_USAGE;
	}
	// 2. 지정시간 금지 방식이 ON되어 있으면
	if ( m_bAASAppointedWay )
	{
		if ( g_pContextMain->m_bAASMethodAppointed ) return CATCH_APPOINTED;
	}
	// 3. 계정별 지정시간 금지 방식 검사
	SYSTEMTIME stLocalSystemTime;
	::GetLocalTime( &stLocalSystemTime );
	INT32 i32Week = stLocalSystemTime.wDayOfWeek;
	INT32 i32Hour = i3ThreadTimer::LocalTime().GetHour();
	if ( ( m_aui32AASBanTime[i32Week] & (0x00000001 << i32Hour) ) ) return CATCH_ACCOUNT_APPOINTED;

	return CATCH_NOTHING;
}

void CUserSession::AddPingTime(UINT16 ui16PingTime)
{
	if (_UI64_MAX - ui16PingTime <  m_ui64PingTime)	// UINT64 최댓값을 넘어갈경우 넘어가기 전에 초기화 시켜준다.
	{
		m_ui64PingTime = 0;
		m_ui32PingCount = 0;
	}

	m_ui64PingTime += ui16PingTime;
	m_ui32PingCount++;
}

UINT32 CUserSession::GetAveragePing()
{
	if (0 == m_ui32PingCount)	return 0; // 인게임을 한번도 하지않고 종료한 유저

	return (UINT32)( m_ui64PingTime / m_ui32PingCount );
}

BOOL	CUserSession::_CheckCompleteMedal(MEDAL_TYPE eMedalType, UINT16 ui16MedalIdx, UINT8 ui8Level )
{
	MQF_MEDAL_INFO * pMQFMedal		= NULL;
	MQF_MEDAL_SET_INFO* pMQFMedalSet= NULL;
	MEDAL_GT* pUserMedal			= NULL;
	CUR_MEDAL_SET* pUserCurMedalSet	= NULL;

	switch ( eMedalType ) 
	{
	case MEDAL_TYPE_NORMAL :
		{
			// Mqf데이터 있는지...
			pMQFMedal = g_pContextMain->GetMedal_MQFData(ui16MedalIdx, MEDAL_TYPE_NORMAL, m_ui8LocaleCode);
			if( NULL == pMQFMedal )		return FALSE;

			// 유저가 메달을 소지 했는지.
			pUserMedal = GetMedal_UserDataAll(ui16MedalIdx) ;
			if( NULL == pUserMedal )	return FALSE;

			// 카운트 조건이 만족되는지 체크. 
			if( MEDAL_LEVEL_NONE == ui8Level || ui8Level > MEDAL_LEVEL_MAX ) return FALSE;
			if( pUserMedal->m_Medal.m_ui16Count < pMQFMedal->m_ui16Count[ui8Level-1] )	return FALSE; 
		}
		break;
	case MEDAL_TYPE_SET :
		{
			// Mqf데이터 있는지 검색.
			pMQFMedal = g_pContextMain->GetMedal_MQFData(ui16MedalIdx, MEDAL_TYPE_SET ) ;
			if( NULL == pMQFMedal )		 return FALSE;

			pMQFMedalSet =  g_pContextMain->GetMedalSet_ByMedalIdx( ui16MedalIdx );
			if( NULL == pMQFMedalSet )	return FALSE;

			// 유저의 진행중인 메달 셋트에서 검색.
			pUserCurMedalSet = GetCurMedalSet_UserData(pMQFMedalSet->m_ui16Idx) ;
			if( NULL == pUserCurMedalSet )	return FALSE;	// 검색 실패

			// 몇번째 메달인지 검색
			ui8Level = g_pContextMain->GetSetMedalArrIdx_ByMedalIdx( ui16MedalIdx );
			if( 255 == ui8Level ||  0 == ui8Level )	return FALSE;	 

			// 카운트 조건이 만족되는지 체크.
			if( pUserCurMedalSet->m_ui16Count[ui8Level-1] < pMQFMedal->m_ui16Count[MEDAL_LEVEL_MAX_SET-1] )	return FALSE;	 
		}
		break;
	default : 
		return FALSE;
		break;
	}

	return TRUE;
}

void CUserSession::DaliyPlayReward( INT32 i32WorkIdx, BOOL bFreeMove, UINT32 ui32PlayTime )
{  
	if( !g_pContextMain->IsContentsEnabled( CONTENTS_TS_EVENT ) )		return;

	m_UserInfoDaily._playtime += ui32PlayTime;
	m_bFirstObserver = FALSE;

	if( MAX_DAILY_GIFT_COUNT <= m_UserInfoDaily._ui8GetItem )			return;

	m_UserInfoDaily._DGPlaytime	+= (UINT32)(bFreeMove ? ui32PlayTime : ui32PlayTime*0.5f);
	ChangeDataBaseInfo( SAVE_DATABASE_RECORD );

	if( m_UserInfoDaily._DGPlaytime < g_pContextMain->m_DailyGift.m_ui32PlayTime[m_UserInfoDaily._ui8GetItem] )	return;

	REWARD_ITEM	stReward;
	stReward.m_ui8GoodsCount	= 1;
	stReward.m_ui8RequestType	= ITEM_INSERT_REQUEST_TS_BONUS; 
	stReward.m_aTGoodsID[0]		= g_pContextMain->GetDailyGiftReward( m_UserInfoDaily._ui8GetItem );
	if( 0 == stReward.m_aTGoodsID[0] )
	{	// 보상 아이템 검색 실패. 초기화 합니다.
		m_UserInfoDaily._DGPlaytime = 0;
		++m_UserInfoDaily._ui8GetItem;
		return;  
	}

	INT32	i32Rv		= EVENT_ERROR_PBV15_REWARD_POPUP_FAIL;   
	BOOL	bTransSend	= g_pModuleTrans->AuthSendRewardItem( i32WorkIdx, this, &stReward ); 
	 
	if( bTransSend )
	{	// 데이터 저장
		m_UserInfoDaily._DGPlaytime = 0;
		++m_UserInfoDaily._ui8GetItem; 
		i32Rv = EVENT_ERROR_SUCCESS;  
	}

	// 임시 처리
	ST_ITEM aItem[MAX_SHOP_GOODS_ITEM_COUNT];
	CShop* pShop = g_pContextMain->GetShop();
	pShop->GetItemInfo_byGoodsID(stReward.m_aTGoodsID[0], aItem );

	i3NetworkPacket	SendPacket( PROTOCOL_BASE_NEW_REWARD_POPUP_ACK );
	SendPacket.WriteData( &i32Rv,						sizeof(INT32) );
	SendPacket.WriteData( &stReward.m_ui8RequestType,	sizeof(UINT8) );
	SendPacket.WriteData( &aItem[0].m_TItemID,			sizeof(UINT32) );
	SendPacket.WriteData( &aItem[0].m_TItemArg,			sizeof(UINT32) );
	SendPacketMessage( &SendPacket );
}

INT32 CUserSession::GetKillDeathRate()
{
	if( 0 == m_UserInfoRecord._killcount + m_UserInfoRecord._death ) return 0;

	REAL32  r32Killcount	= (REAL32)m_UserInfoRecord._killcount; 
	REAL32	r32Deathcount	= (REAL32)m_UserInfoRecord._death; 	
	
	return (INT32)(( r32Killcount / ( r32Killcount + r32Deathcount ) ) * 100.f);
}

T_RESULT CUserSession::CheckMercenaryRegist()
{
	// 1. 킬/뎃률 체크
	if ( GetKillDeathRate() < MERCENARY_MIN_KILL_DEATH_RATE * 100) 
	{
		return EVENT_ERROR_NOT_ENOUGH_KILL_DEATH_WEIGHT;
	}

	// 2. 클랜전 이탈 시간 체크. 
	if( IsMerPenalty() ) 
	{
		return EVENT_ERROR_CLAN_WAR_MERCENAY_DIS_TIME;
	}

	return EVENT_ERROR_SUCCESS;
}
 
BOOL CUserSession::_CheckCouponNumber( char* pstrCoupon )
{
	INT32 i32Len = i3String::Length( pstrCoupon );
	if ( i32Len <= 0 ) return FALSE;
	if ( i32Len > GIFTCOUPON_LENGTH ) i32Len = GIFTCOUPON_LENGTH;

	for( INT32 i = 0; i < i32Len; i++ )
	{
		if ( 'A' <= pstrCoupon[i] && pstrCoupon[i] <= 'Z' )	continue;
		if ( 'a' <= pstrCoupon[i] && pstrCoupon[i] <= 'z' )	continue;
		if ( '0' <= pstrCoupon[i] && pstrCoupon[i] <= '9' )	continue;
		return FALSE;
	}
	
	return TRUE;
}

BOOL CUserSession::IsInLobby()
{
	 if ( GAME_TASK_LOBBY != GetPosition() )		return FALSE;
	 if ( 0 == m_MultiSlot.GetCharSlotCount() )		return FALSE;

	 return TRUE;
}


void CUserSession::SendGachaPurchaseAck( PACKET_GACHA_SS_RESULT_HEADER* pResult, PACKET_GACHA_RESULT_DATA* pDatas, UINT8 ui8ItemCount , INVEN_BUFFER_DB *pItem  )
{
	PACKET_GACHA_RESULT_HEADER stResultHeader;
	stResultHeader.m_eErrorCode			= pResult->m_eErrorCode;
	stResultHeader.m_i32ItemIdx			= pResult->m_i32ItemIdx;
	stResultHeader.m_i32AccumPurchase	= pResult->m_i32AccumPurchase;
	stResultHeader.m_i32ResultCount		= pResult->m_i32ResultCount;
	stResultHeader.m_TGoodsID			= pResult->m_TGoodsID;
	stResultHeader.m_ui32Cash			= pResult->m_ui32Cash;
	 
	if ( GACHA_ERROR_SUCCESS == stResultHeader.m_eErrorCode )
	{
		// 유저 인벤토리 업데이트.
		INVEN_BUFFER_SET InvenSet;
		for( UINT8 i = 0 ; i < ui8ItemCount ; ++i )
		{
			INVEN_BUFFER InvenBuffer; 
			i3mem::Copy( &InvenBuffer, &pItem[i], sizeof( INVEN_BUFFER ) );
			OnInsertItem( &InvenBuffer, &InvenSet );  
		}
		SendChangeInven( &InvenSet );		// 추가되는 아이템을 전달 합니다.

		UINT32 ui32UserMoney = 0;

		switch( pResult->m_ui8BuyType ) 
		{
		case GOODS_BUY_CASH :
			{
				// 캐쉬 업데이트.
				ui32UserMoney					= pResult->m_ui32Cash; 
				m_UserInfoBasic.m_ui32Cash		= pResult->m_ui32Cash; 
				if( pResult->m_ui32Cash > m_UserInfoBasic.m_ui32NCCoin )
				{
					m_UserInfoBasic.m_ui32HappyCoin = pResult->m_ui32Cash - m_UserInfoBasic.m_ui32NCCoin;
				}
				else
				{
					m_UserInfoBasic.m_ui32HappyCoin = 0;
					m_UserInfoBasic.m_ui32NCCoin = pResult->m_ui32Cash;
				}
				 
				// 마지막 캐시 아이템 구입 날짜 저장
				m_UserInfoServer.m_ui32LastBuyCash = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
			}
			break;
		case GOODS_BUY_POINT:
			ui32UserMoney = m_UserInfoBasic.m_ui32Point;
			break;
		case GOODS_BUY_COIN:
			ui32UserMoney = m_UserInfoBasic.m_ui32Coin;
			break;
		}

		// ZLog 기록
		g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_GACHA_SHOP, this, &ui32UserMoney, pResult, pDatas );
	}
	else
	{	// 실패시 포인트, 코인 롤백 

		switch( pResult->m_ui8BuyType ) 
		{
		case GOODS_BUY_POINT :
			m_UserInfoBasic.m_ui32Point	+= pResult->m_ui32Price;
			break;
		case GOODS_BUY_COIN :
			m_UserInfoBasic.m_ui32Coin	+= pResult->m_ui32Price;
			break;
		}
	}

	ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );

	stResultHeader.m_ui32Point	= m_UserInfoBasic.m_ui32Point;
	stResultHeader.m_ui32Coin	= m_UserInfoBasic.m_ui32Coin;

	i3NetworkPacket stPacket(PROTOCOL_GACHA_PURCHASE_ACK);
	stPacket.WriteData(&stResultHeader, sizeof(PACKET_GACHA_RESULT_HEADER));
	if ( GACHA_ERROR_SUCCESS == stResultHeader.m_eErrorCode )
	{
		stPacket.WriteData(pDatas, static_cast<UINT16>(sizeof(PACKET_GACHA_RESULT_DATA) * pResult->m_i32ResultCount));
	}

	SendPacketMessage(&stPacket);
}

void CUserSession::ChangePCCafeBonus_A( UINT8 ui8PCCafe )
{
	// 다음 인벤토리 진입시 pc방 아이템 지급/제거 되도록 마킹
	m_ui8NewPCCafe = ui8PCCafe;
	
	// 경험치,포인트 보너스 적용
	m_UserInfoBasic.m_ui8PCCafe = ui8PCCafe;

	// 클라이언트에게 통지
	PACKET_BASE_CHANGE_PCCAFE_STATUS_ACK Packet;
	Packet.m_ui8PCCafe = ui8PCCafe;
	S2MOSendEvent( &Packet );

	// 레디룸이나 인게임에 있을 경우에 레디룸 유저 모두의 슬롯정보 갱신
	if ( GAME_TASK_READY_ROOM <= GetPosition() && m_pRoom )
	{
		m_pRoom->ChangePCCafeValue( m_i32SlotIdx, ui8PCCafe );
	}
}

void CUserSession::CheckInvenItemByUserInfo(INT32 i32ThreadIdx)
{	// 유저정보에 의한 인벤 아이템 체크.

	// NickColor.
	INVEN_BUFFER* pInvenBuffer = m_Inventory.GetItemByItemID( 1600006 );// MAKE_ITEM_ID(ITEM_TYPE_MAINTENANCE, 0, CASHITEM_GROUP_COLOR_NICK)
	if( !pInvenBuffer && m_UserInfoInvItemData._ui8NickColor )
	{	// 기존에 닉네임 컬러를 사용하였지만, 인벤토리에서 찾을수 없다면 정보를 업데이트 합니다.
		m_UserInfoInvItemData._ui8NickColor = 0;
		SetChangeColorNick( i32ThreadIdx, m_UserInfoInvItemData._ui8NickColor );
	}

	// MuzzleFlahsColor
	pInvenBuffer = m_Inventory.GetItemByItemID( 1600187 );// MAKE_ITEM_ID(ITEM_TYPE_MAINTENANCE, 0, CASHITEM_GROUP_COLOR_MUZZLE_FLASH)
	if( !pInvenBuffer && m_UserInfoInvItemData._ui16MuzzleFlashColor )
	{	// 기존에 컬러 총구화염 변경권을 사용하였으나 인벤에 없다면 정보를 업데이트 합니다.
		m_UserInfoInvItemData._ui16MuzzleFlashColor = 0;
		SetChangeColorMuzzleFlash();
	}

	UINT32	ui32DisguiseItemID	= 0;
	bool	bIsRankChanged		= false;

	m_ui32ViewRank = m_UserInfoBasic.m_ui32Rank;

	//위장계급 체크
	pInvenBuffer	= GetItemByItemID( 1600009 );// MAKE_ITEM_ID(ITEM_TYPE_MAINTENANCE, 0, CASHITEM_GROUP_DISGUISE_RANK)
	if( pInvenBuffer )
	{
		ui32DisguiseItemID	= pInvenBuffer->_ui32ItemID;
		m_ui32ViewRank		= m_UserInfoInvItemData._wRank;
	}
	else if( m_UserInfoInvItemData._wRank != RANK_NONE )
	{	// 만료되었다면 초기값을 저장한다
		m_UserInfoInvItemData._wRank = RANK_NONE;
		ChangeDataBaseInfo( SAVE_DATABASE_ITEM );
		bIsRankChanged = true;
	}

	//만우절 위장계급 체크
	pInvenBuffer = GetItemByItemID( 1600186 );// MAKE_ITEM_ID(ITEM_TYPE_MAINTENANCE, 0, CASHITEM_GROUP_DISGUISE_RANK_APRIL_FOOL)
	if( pInvenBuffer )
	{
		ui32DisguiseItemID	= pInvenBuffer->_ui32ItemID;
		m_ui32ViewRank		= m_UserInfoInvItemData._wAprilFoolRank;
	}
	else if( m_UserInfoInvItemData._wAprilFoolRank != RANK_NONE )
	{	// 만료되었다면 초기값을 저장한다
		m_UserInfoInvItemData._wAprilFoolRank = RANK_NONE;
		ChangeDataBaseInfo( SAVE_DATABASE_ITEM );
		bIsRankChanged = true;
	}

	if( bIsRankChanged )
	{
		SetChangeRank( i32ThreadIdx, ui32DisguiseItemID );
	}

	return;
}


void CUserSession::CheckUserInfoByInvenItem(INT32 i32ThreadIdx, T_ItemID TDeletedItemID)
{	// 삭제된 아이템이 있을경우 유저 정보 처리.

	switch( TDeletedItemID )
	{	// 삭제된 아이템이 있을경우 유저에 대한 추가 처리.
	case  1600006:	// MAKE_ITEM_ID(ITEM_TYPE_MAINTENANCE, 0, CASHITEM_GROUP_COLOR_NICK) 
		{
			m_UserInfoInvItemData._ui8NickColor = 0;
			SetChangeColorNick( i32ThreadIdx, m_UserInfoInvItemData._ui8NickColor );
		}
		break;

	case 1600009:	//위장계급
		{
			m_UserInfoInvItemData._wRank = RANK_NONE;
			ChangeDataBaseInfo( SAVE_DATABASE_ITEM );
			
			INVEN_BUFFER* pInvenBuffer = GetItemByItemID( 1600186 );

			if( pInvenBuffer )	//만우절 위장계급이 있는경우 만우절 위장계급을 적용한다.
			{
				SetChangeRank( i32ThreadIdx, pInvenBuffer->_ui32ItemID );
			}
			else				//원래 계급으로 적용한다.
			{
				SetChangeRank( i32ThreadIdx, 0 );
			}

		}
		break;

	case 1600186:	//만우절 위장계급
		{
			m_UserInfoInvItemData._wAprilFoolRank = RANK_NONE;
			ChangeDataBaseInfo( SAVE_DATABASE_ITEM );
			
			INVEN_BUFFER* pInvenBuffer = GetItemByItemID( 1600009 );

			if( pInvenBuffer )	//위장계급이 있는 경우 위장계급으로 변경한다.
			{
				SetChangeRank( i32ThreadIdx, pInvenBuffer->_ui32ItemID );
			}
			else				//원래 계급으로 적용한다.
			{
				SetChangeRank( i32ThreadIdx, 0 );
			}

		}
		break;

	case 1600187:	//컬러 총구화염 변경권
		{
			m_UserInfoInvItemData._ui16MuzzleFlashColor = 0;
			SetChangeColorMuzzleFlash();			
		}
		break;

	default:
		break;
	}
}
 
void CUserSession::SendSeasonRankingClan(void)
{
	if( !g_pContextMain->IsContentsEnabled(CONTENTS_CLAN_MATCH_NEW) )	return;

	// 현재 시즌 시작이후 한번이라도 확인했다면 패킷을 보내지 않습니다.
	if( m_ui32ClanRankingDate >= g_pContextMain->GetClanMatchSeasonStartDate(SEASON_TYPE_ONGOING)  ) return;

	m_ui32ClanRankingDate = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();

	// 세이브 플래그는 PROTOCOL_CLAN_WAR_CHECK_PRESEASON_RANKING_REQ 받으면 킴.
	// ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO); 

	PACKET_CLAN_WAR_PRESEASON_RANKING_INFO_ACK Packet;

	CLAN_MATCH_SEASON	stClanMatchSeason;
	g_pContextMain->MakePacketClanMatchSeason(SEASON_TYPE_PRE,&stClanMatchSeason);
				
	Packet.m_i32SeasonRankingCount = g_pContextMain->GetSeasonRankingCount(); 
	Packet.m_aSeasonRankingClan.SetValue( g_pContextMain->GetSeasonRankingClan(), 0,  g_pContextMain->GetSeasonRankingCount() );
	Packet.m_strSeasonName = stClanMatchSeason.m_szSeasonName;

	S2MOSendEvent( &Packet );

	return;
}

INT32 CUserSession::JoinClanMatchRoom(INT32 i32ChannelIndex, INT32 i32RoomIndex, ROOM_INFO_JOIN* pInfoJoin )
{
	if( m_i32MainTask < GAME_TASK_LOBBY )			return EVENT_ERROR_GAME_WRONG_POSITION;
	if( SERVER_VALUE_UNKNOWN == m_i32ChannelNum )	return EVENT_ERROR_GAME_WRONG_POSITION;
	if( SERVER_VALUE_UNKNOWN < m_i32RoomIdx )		return EVENT_ERROR_GAME_WRONG_POSITION;

	return g_pRoomManager->OnJoinRoom( MODULE_CLAN_THREAD_IDX, this, i32ChannelIndex, i32RoomIndex, pInfoJoin);
}

void	CUserSession::SendClanMatchTeamCount(INT32 i32ChannelNum )
{
	if(  m_ui32MatchCountCheckTime + MATCH_TEAM_COUNT_UPDATE_TIME > i3ThreadTimer::GetServerTime() ) return;

	m_ui32MatchCountCheckTime = i3ThreadTimer::GetServerTime();

	UINT8 ui8UserChannelType = g_pContextMain->m_pChannelType[i32ChannelNum] ;

	if( ui8UserChannelType != CHANNEL_TYPE_CLAN && ui8UserChannelType != CHANNEL_TYPE_PCCAFE_CLAN ) return;
	
	PACKET_CLAN_WAR_MATCH_TEAM_COUNT_ACK SendPacket;
	SendPacket.m_i32MatchedTeamCount	= g_pContextMain->GetMatchedTeamCount();;
	SendPacket.m_i32MatchingTeamCount	= g_pContextMain->GetMatchingTeamCount();
	S2MOSendEvent( &SendPacket);		

	return;
}

void	CUserSession::SetMerPenaltyTime( INT32 i32WorkIdx )
{
	if( !IsMercenary() ) return;

	m_dt32MerPenaltyTime = i3ThreadTimer::LocalTime();
	m_dt32MerPenaltyTime.AddTime( I3DATE_ADD_TYPE_MINUTE, MERCENARY_REREGISTRATION_TIME  );
	// DB에 저장
	g_pModuleClan->SaveMerPenalty(i32WorkIdx, this);
	I3TRACE("!!!! SetMerPenaltyTime \n");
}

bool	CUserSession::IsMerPenalty(void )
{
	INT32 i32DiffTime = i3ThreadTimer::LocalTime().DiffTime( m_dt32MerPenaltyTime );
	if( i32DiffTime < 0 ) 
	{
		return true;
	}
	return false;
}

INT32	CUserSession::_ChaeckCreateRoom( void )
{
	// 1. AAS Check.
	if( g_pContextMain->CheckAASBlockON(AAS_BLOCK_CREATE_ROOM) && CheckAAS() ) return EVENT_ERROR_AAS_CREATE_ROOM;

	// 2. 용병이면 실패.
	if( IsMercenary() ) return EVENT_ERROR_CLAN_WAR_MERCENAY;

	return EVENT_ERROR_SUCCESS;
}

void	CUserSession::ResetClanInfo(void)
{
	m_UserInfoBasic.m_ui32Clanidx		= 0;
	m_UserInfoBasic.m_ui32ClanState		= CLAN_MEMBER_LEVEL_UNKNOWN;
	m_UserInfoBasic.m_ui32ClanMark		= CLAN_MARK_DEFAULT;
	m_UserInfoBasic.m_strClanName[0]	= '\0';
	m_UserInfoBasic.m_ui8ClanExpRank = 0;
	m_UserInfoBasic.m_ui8ClanPerRank = 0; 
	m_UserInfoBasic.m_ui8ClanNamecolor = 0;

	//m_ui32ClanRankingDate = 0;	// 다른 타이밍에 초기화 됩니다.

	ChangeDataBaseInfo( SAVE_DATABASE_CLAN );
}

void	CUserSession::_SyncEquipIdx(BOOL bCommonOnly)
{	// 인벤토리내 아이템 DB IDX로 ItemID를 검색하여 셋팅 해줍니다.
	
	INVEN_BUFFER*	pInven;
	INVEN_BUFFER_SET	InvenSet; 
	InvenSet.ui8ChangeType	= ITEM_INSERT_REQUEST_BUY;

	INVEN_BUFFER	InvenEquip;
	InvenEquip._ui32ItemArg	= 1;
	InvenEquip._ui8ItemType = ITEM_ATTR_TYPE_BASIC;
 
	
	// 공용 장비
	ITEM_INFO aItemCommon[CHAR_EQUIPMENT_COMMON_COUNT];
	m_MultiSlot.GetCommonEquipmentAll( aItemCommon );
	for( INT32 i = 0; i < CHAR_EQUIPMENT_COMMON_COUNT ; ++i )
	{
		pInven = m_Inventory.GetItemByWareDBIdx( aItemCommon[i].m_TItemDBIdx );
		if( NULL == pInven )																
		{	// Default 아이템으로 셋팅해 줍니다.
			if( aItemCommon[i].m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN )
			{	
				aItemCommon[i].m_TItemID = GET_DEFAULT_ITEM_ID( aItemCommon[i].m_TItemDBIdx ); 
			}
		}
		else
		{	// Default 아이템이 아닐경우 유저 인벤토리에서 검색한 ItemID를 셋팅.
			aItemCommon[i].m_TItemID = pInven->_ui32ItemID;
		}

		m_MultiSlot.SetCommonEquipment( i, aItemCommon[i].m_TItemDBIdx, aItemCommon[i].m_TItemID );
		ChangeDataBaseInfo( SAVE_DATABASE_CHARA_COMMON );
	}

	if (bCommonOnly) return;

	// 캐릭터 장비
	ITEM_INFO	EquipItemInfo;
	for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
	{
		if( FALSE == m_MultiSlot.IsActiveChar(i) )	continue;

		for( INT32 k = 0; k < CHAR_EQUIPMENT_COUNT ; ++k )
		{
			m_MultiSlot.GetCharEquipment( i, k, &EquipItemInfo );

			pInven = m_Inventory.GetItemByWareDBIdx( EquipItemInfo.m_TItemDBIdx );
			if( NULL == pInven )																
			{ 
				if( EquipItemInfo.m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN )
				{	
					switch( k )
					{
					case CHAR_EQUIPMENT_PRIMARY:
					case CHAR_EQUIPMENT_SECONDARY:
					case CHAR_EQUIPMENT_MELEE:
					case CHAR_EQUIPMENT_THROWING1:
					case CHAR_EQUIPMENT_THROWING2:
						EquipItemInfo.m_TItemID = GET_DEFAULT_ITEM_ID( EquipItemInfo.m_TItemDBIdx );  
						break;
					case CHAR_EQUIPMENT_CHARA:
						{
							EquipItemInfo.m_TItemID = GET_DEFAULT_ITEM_ID( EquipItemInfo.m_TItemDBIdx );  
							InvenEquip._TItemWareDBIdx = EquipItemInfo.m_TItemDBIdx;
							InvenEquip._ui32ItemID = EquipItemInfo.m_TItemID;
							OnInsertItem( &InvenEquip, &InvenSet );
						}
						break;
					default:
						EquipItemInfo.m_TItemID  = MAKE_DEFAULT_PARTS_ITEMID( k + 1 );
						break;
					}
				}
			}
			else
			{	// Default 아이템이 아닐경우 유저 인벤토리에서 검색한 ItemID를 셋팅.
				EquipItemInfo.m_TItemID = pInven->_ui32ItemID;
			}

			m_MultiSlot.SetCharEquipment( i, k, EquipItemInfo.m_TItemDBIdx, EquipItemInfo.m_TItemID  );
		}

		//ChangeCharaDataBaseInfo( ( (UINT64)0x01 << i) );
	}
}


void CUserSession::ChangeStateChar_A( T_RESULT i32Result, UINT8 ui8SlotIdx, UINT8 ui8SlotState )
{	
	PACKET_CHAR_CHANGE_STATE_ACK	Packet;
	Packet.m_TResult = i32Result;
	Packet.m_ui8SlotIdx = ui8SlotIdx;
	Packet.m_ui8SlotState = ui8SlotState;

	if( S2MO_IS_SUCCESS( i32Result ) )
	{
		m_MultiSlot.SetActiveChar( (INT32)ui8SlotIdx, (MULTI_SLOT_STATE)ui8SlotState);

		ChangeCharaDataBaseInfo( ( (UINT64)0x01 << ui8SlotIdx) );
	}
	
	S2MOSendEvent( &Packet );
}


BOOL	CUserSession::_CheckUlsanGuard(INT32 i32ThreadIdx, PROTOCOL	ProtocolID, HACK_TYPE_BASE* pUSGBase, UINT64 *pui64DXFunc, char *pID)
{
	if (!g_pContextMain->IsContentsEnabled(CONTENTS_ULSAN_GUARD)) return FALSE;

	bool bHack = false;
	for (INT32 i = 0; i < MAX_HACK_TYPE; ++i)
	{
		if (pUSGBase->m_ui8Type[i] != 0)
		{
			bHack = true;
			break;
		}
	}

	if (!bHack) return FALSE;


	LOG_USG	stLog;
	stLog.m_ui8Kick = 0;
	stLog.m_strId[0] = 0;
	i3mem::Copy(stLog.m_strId, pID, NET_ID_SIZE);

	//srand( g_pContextMain->GetServerTime() ); 

	wchar_t wstrTemp[MAX_PATH];
	i3String::Format(wstrTemp, MAX_PATH, L"[ULSAN GUARD HackUser] %d %d %d %d %d %d %d %d ", 
			pUSGBase->m_ui8Type[0], pUSGBase->m_ui8Type[1], pUSGBase->m_ui8Type[2], pUSGBase->m_ui8Type[3],
			pUSGBase->m_ui8Type[4], pUSGBase->m_ui8Type[5], pUSGBase->m_ui8Type[6], pUSGBase->m_ui8Type[7]);

	// 0 ~100 % 확률로 자릅니다.
	if (rand() % 10 < g_pContextMain->m_pHackCheckRatio.m_aui8HackCheckRatio[GS_HACK_TYPE_DLL_CHEAT])
	{
		m_bWriteHackLog = true;

		stLog.m_ui8Kick = 1;

		// 잘리면 무조건 로그 남깁니다.
		g_pModuleContextNC->LogSendMessage(i32ThreadIdx, LOG_MESSAGE_U_HACKING, this, pUSGBase, pui64DXFunc, &stLog);

		// 30초~4분30초 사이에 자릅니다.
		DisconnectUserDelay(EVENT_ERROR_HACKING_TOOL, KILL_REASON_HACK_SERVER_G, wstrTemp);
		
		g_pLog->WriteLogSession(this, wstrTemp);

		return TRUE;
	}

	if (!m_bWriteHackLog)	// 추후 오탐 가능성이 없어지면 주석 처리 합니다.
	{
		m_bWriteHackLog = true;

		g_pModuleContextNC->LogSendMessage(i32ThreadIdx, LOG_MESSAGE_U_HACKING, this, pUSGBase, pui64DXFunc, &stLog);
	}

	return FALSE;}

T_RESULT	CUserSession::_CheckCharaDelete(INT32 i32DeleteSlotIdx)		// 캐릭터 삭제 조건 검사
{
	// 0. 캐릭터 갯수 체크
	if (m_MultiSlot.GetCharSlotCount() <= 1)
	{
		return EVENT_ERROR_SHOP_CHARA_DEL_FAIL_SLOT_COUNT;
	}

	// 1. 캐릭터 슬롯인덱스 체크
	if (i32DeleteSlotIdx >= S2MO_MULTI_SLOT_COUNT)
	{
		return EVENT_ERROR_SHOP_CHARA_DEL_FAIL_SLOT_IDX;
	}

	// 2. 소지중인 캐릭터 인지 체크.
	if (!m_MultiSlot.IsActiveChar(i32DeleteSlotIdx))
	{
		return EVENT_ERROR_SHOP_CHARA_DEL_FAIL_NOT_ACTIVE;
	}

	ITEM_INFO CharaItemInfo;
	m_MultiSlot.GetCharEquipment(i32DeleteSlotIdx, CHAR_EQUIPMENT_CHARA, &CharaItemInfo);

	// 3. 기본 캐릭터 인지 체크.
	if (CharaItemInfo.m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN)
	{
		return EVENT_ERROR_SHOP_CHARA_DEL_FAIL_DEFAULT;
	}

	return EVENT_ERROR_SUCCESS;

}
BOOL	CUserSession::_CheckBeretTitle(INT32 i32ItemID)
{
	BOOL bResult = TRUE;	

	switch (i32ItemID)
	{
	case BERET_ASSAULT_ITEMID:
		bResult = _CheckBeretMatchtitle(BERET_ASSAULT_TITLE);
		break;
	case BERET_RIFLE_ITEMID:
		bResult = _CheckBeretMatchtitle(BERET_RIFLE_TITLE);
		break;
	case BERET_SMG_ITEMID:
		bResult = _CheckBeretMatchtitle(BERET_SMG_TITLE);
		break;
	case BERET_HANDGUN_ITEMID:
		bResult = _CheckBeretMatchtitle(BERET_HANDGUN_TITLE);
		break;
	case BERET_SHOTGUN_ITEMID:
		bResult = _CheckBeretMatchtitle(BERET_SHOTGUN_TITLE);
		break;
	default:
		break;
	}

	return bResult;	
}
BOOL	CUserSession::_CheckBeretMatchtitle(UINT8 ui8Title)
{
	for (INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		if (m_aui8EquipUserTitle[i] == ui8Title) return TRUE;
	}
	return FALSE;
}


BOOL    CUserSession::CheckExistItemInvenByItemID(UINT32 ui32ItemID)
{
	return m_Inventory.CheckExistItemInvenByItemID(ui32ItemID);
}

void	CUserSession::InvenInsertPriority(INVEN_BUFFER *aInvenItem, INVEN_BUFFER_SET * InvenSet, UINT16 ui16ItemCount)
{
	_ExceptionEquip();		// 공짜아이템 추가
	_ExceptionSpecial();	// 스페샬 아이템 추가

	for (INT32 i = 0; i < ui16ItemCount; i++) //사용중인 아이템 먼저 넣기.
	{
		if (ITEM_ATTR_TYPE_AUTH == aInvenItem[i]._ui8ItemType ||
			ITEM_TYPE_CHARA		== GET_ITEM_TYPE(aInvenItem[i]._ui32ItemID))
		{
			OnInsertItem(&aInvenItem[i], InvenSet);
			aInvenItem[i]._ui32ItemID = 0;
			//i3mem::FillZero(&aInvenItem[i], sizeof(INVEN_BUFFER));
		}
	}

	UINT32 InvenCount = m_Inventory.GetInventoryCount();

	//남은 아이템 인벤에 넣기.
	for (INT32 i = 0; i < ui16ItemCount; i++)
	{
		if (aInvenItem[i]._ui32ItemID == 0) continue;
		if (InvenCount >= MAX_INVEN_COUNT) break;

		OnInsertItem(&aInvenItem[i], InvenSet);
		InvenCount++;
	}
	InvenCount = m_Inventory.GetInventoryCount();

	_SyncEquipIdx();
}

// 상품 구입시 인벤토리 체크 후 구입 성공 여부 확인.
INT32   CUserSession::CheckExistItemInven(BUY_BASKET* stBasket, UINT8 ui8GoodsCount)
{
	//상품 목록 가져오기
	SHOP_MATCHING_DATA*		pMatching = g_pContextMain->GetShop()->GetMatchingAllData();
	if (pMatching == NULL)		return EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_GOODS;
	UINT32 ui16InvenCount = m_Inventory.GetInventoryCount();

	if (ui16InvenCount < MAX_INVEN_COUNT - MAX_SHOP_GOODS_ITEM_COUNT * MAX_SHOP_GOODS_BUY_COUNT) return EVENT_ERROR_SUCCESS;
	//매칭 개수
	UINT32					ui32MatchingCount = g_pContextMain->GetShop()->GetMatchingCount();
	
	UINT16 ui16ItemCount = 0;
	UINT32 ui32ItemID = 0;

	for (UINT32 i = 0; i < ui8GoodsCount; i++)
	{
		INT32 ui32ItemType = GET_ITEM_TYPE(GET_ITEMID_FROM_GOODSID(stBasket[i].m_ui32GoodsID));

		if (ITEM_TYPE_SET == ui32ItemType)
		{
			ui16ItemCount += MAX_SHOP_GOODS_ITEM_COUNT;
			continue;
		}

		for (UINT32 j = 0; j < ui32MatchingCount; j++)
		{
			if (pMatching[j]._ui32GoodsID != stBasket[i].m_ui32GoodsID)	continue;
			ui32ItemID = pMatching[j]._ui32ItemID;

			if (FALSE == m_Inventory.CheckExistItemInvenByItemID(ui32ItemID)) ui16ItemCount += 1;
			break;
		}

	}
	if (MAX_INVEN_COUNT < ui16InvenCount + ui16ItemCount) return EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL_SHOP;

	return EVENT_ERROR_SUCCESS;
}

void	CUserSession::SendGiftNoteByUID(T_UID i64RecvUID, UINT8 ui8MessageSize, TTCHAR *strMessage)
{
	if (strMessage == NULL) return;

	g_pModuleMessenger->MessengerSendNoteUserGiftNotice( MODULE_TRANS_THREAD_IDX, this, i64RecvUID, ui8MessageSize, strMessage);
}

void	CUserSession::SendBannerURLList(void)
{
	i3NetworkPacket SendPacket;
	SendPacket.SetProtocol(PROTOCOL_BASE_URL_LIST_ACK);
	SendPacket.WriteData(&g_pContextMain->m_ui8URLVersion, sizeof(UINT8));
	SendPacket.WriteData(&g_pContextMain->m_ui8URLListCount, sizeof(UINT8));

	UINT16		ui16Len = 0;
	URL_LIST	*pulURLList = nullptr;

	for (INT32 i = 0; i < g_pContextMain->m_ui8URLListCount; i++)
	{
		pulURLList = &g_pContextMain->m_ulURLList[i];
		ui16Len = (UINT16)(i3String::Length(pulURLList->m_strURL));
		SendPacket.WriteData(&ui16Len, sizeof(UINT16));
		SendPacket.WriteData(&pulURLList->m_i32Type, sizeof(INT32));
		SendPacket.WriteData(&pulURLList->m_i32Data, sizeof(INT32));
		SendPacket.WriteData(pulURLList->m_strURL, sizeof(wchar_t) *ui16Len);
	}

	SendPacketMessage(&SendPacket);
}

void	CUserSession::_SendEquipedInfo(void)
{
	PACKET_SERVER_MESSAGE_CHANGE_INVENTORY Packet;

	BOOL bSendPacket = TRUE;	// 혹시나 Common 만 보낼 경우가 생길 수도 있으니..
	ITEM_INFO pCommonEquips[CHAR_EQUIPMENT_COMMON_COUNT];
	m_MultiSlot.GetCommonEquipmentAll(pCommonEquips);
	Packet.m_aCommonEquips.SetValue(pCommonEquips, 0, CHAR_EQUIPMENT_COMMON_COUNT);

	INT32 i32Count = 0;
	S2_CHAR_CHANGE_EQUIP_INFO pEquipInfo;
	for (INT32 i = 0; i < S2MO_MULTI_SLOT_COUNT; i++)
	{
		if (FALSE == m_MultiSlot.IsActiveChar(i, MULTI_SLOT_CHECK_USING))	continue;

		pEquipInfo.m_ui8SlotIdx = (UINT8)i;
		m_MultiSlot.GetCharEquipmentAll(i, pEquipInfo.m_aCharEquipment);

		Packet.m_ChangeEquipInfo[i32Count] = pEquipInfo;
		i32Count++;
		bSendPacket = TRUE;

		if (0 == i32Count % S2MO_MULTI_SLOT_PACKET_COUNT)
		{
			S2MOSendEvent(&Packet);

			Packet.m_ChangeEquipInfo.SetCount(0);
			i32Count = 0;
			bSendPacket = FALSE;
		}
	}
	if (bSendPacket)
	{
		S2MOSendEvent(&Packet);
	}
}