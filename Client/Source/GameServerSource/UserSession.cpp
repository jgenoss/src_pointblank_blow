#include "pch.h"
#include "UserSession.h"
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
	m_bSendPacket		= FALSE; 
	m_i32MainTask		= GAME_TASK_NONE;
	m_ui32LoginKey		= 0;
	m_strWebKey[0]		= '\0';		//초기화 꼬옥 필요	웹연동이면 필요없음  	

	m_ui32FirstObserverTime = 0;	// 난입 후 첫 관전시간
	m_bFirstObserver	= FALSE;	

	m_ui8EventCountry	= 0;

	m_bRequestUserList	= FALSE;
	m_bHSCheck			= TRUE;

	m_ui8OtherCountry	= 0;

	m_bCompleteTS		= FALSE;
}

CUserSession::~CUserSession()
{
}

BOOL CUserSession::OnCreate(SOCKET Socket, struct sockaddr_in * pAddr)
{
	BOOL bRv = i3NetworkSession::OnCreate(Socket, pAddr); 

#if defined( USE_INDONESIA_PCBANG )
	i3mem::FillZero( m_strNesiaPCbangKey, MAX_INDONESIA_PCBANG_KEY );
#endif
	m_ui32RoomBattleCount	= 0;
	m_ui32PlayTime			= 0;
	m_bGetEventItem			= FALSE;

	//Set Idx & Number
	m_i64UID					= SERVER_VALUE_UNKNOWN;
	m_i32RoomIdx				= SERVER_VALUE_UNKNOWN;
	m_i32SlotIdx				= SERVER_VALUE_UNKNOWN;
	m_i32ChannelNum				= SERVER_VALUE_UNKNOWN;
	m_i32LogChannelNum			= SERVER_VALUE_UNKNOWN;

	m_bLogin					= FALSE;
	m_bMyinfo					= FALSE;

	//Position	
	m_i32SubTask				= GAME_TASK_SUB_NONE;

	m_ui32EncriptKey			= (m_SessionIdx % 7) + 1;					// 비밀 키값 설정
	m_bGMUser					= FALSE;									// GM 설정
	m_bMODUser					= FALSE;									// MOD 설정

	//Set Time 	
	m_ui64LoginTime				= 0;
	m_ui32LastLoginTime			= 0;
	m_ui32LogInST				= 0;
	m_ui32ATLastRecv			= g_pContextMain->GetServerTime();
	m_ui32ATIsKill				= 0;
	m_ui32ATSendKill			= 0;
	m_strHackNotice[0]			= '\0';
	m_ui32KillReason			= KILL_REASON_NO;
	m_ui32HackKillReason		= 0;
	m_bHackKill					= FALSE;
	m_strId[0]					= '\0';

	m_i32SendErrorCount			= 0;

	m_ui32ConnectServerTime		= g_pContextMain->GetServerTime();

	m_ui32ATPacketSecond		= g_pContextMain->GetServerTime(); 
	m_ui32PacketSecondCount		= 0; 

	//setGameGuard타임 
	m_ui32GameGuardSendST		= g_pContextMain->GetServerTime();
	m_ui32GameGuardRecvST		= m_ui32GameGuardSendST;

	m_ui32GameGuardCheckCount	= 0;

	//Set UDP NetInfo
	m_UdpNetInfo.m_nRealIP		= m_ConIp; 
	m_UdpNetInfo.m_nLocalPort	= g_pContextMain->m_ui16ClientPort;			//홀펀칭에서 사용함
	m_UdpNetInfo.m_nRealPort	= g_pContextMain->m_ui16ClientPort;			//홀펀칭에서 사용함

	//Set Inventory
	m_i32InvWeaponCount			= 0;
	m_i32InvCharaCount			= 0;
	m_i32InvItemCount			= 0;
	i3mem::FillZero( m_aInvWeapon,	sizeof( INVEN_BUFFER ) * MAX_INVEN_COUNT );
	i3mem::FillZero( m_aInvChara,	sizeof( INVEN_BUFFER ) * MAX_INVEN_COUNT );
	i3mem::FillZero( m_aInvItem,	sizeof( INVEN_BUFFER ) * MAX_INVEN_COUNT );

	//메달정보 입니다.
	m_ui32Miniature				= 0;
	m_ui32Insignia				= 0;
	m_ui32Order					= 0;
	m_ui32Master				= 0;
	m_ui64GainUserTitle			= 0;

	//Log땜빵용
	m_i32LogD_RespawnCount		= 0;

	m_ui8LoadingStart			= LOAD_START_NONE; 

	m_ui32QuestEventDate		= 0; 
	m_bMatchRoom				= FALSE;
	m_bHaveInfo					= FALSE; 
	m_ui8ChangeDBFlage			= 0x00;

	m_ui8LocaleCode				= LOCALE_NUMBER;

	m_ui8Ping					= 0;

	m_ui16BackProtocolID		= 0;

	m_bIsBattleComplete			= FALSE;

	m_i16PacketRandSeed			= (INT16)rand();

	m_ui8LoginLinkThreadIdx		= 0;
	m_ui32LoginWaitCount		= 0;
	m_ui32LoginWaitSendTime		= 0xFFFFFFFF;

	m_UserInfoBasic.Reset();
	m_UserInfoRecord.Reset();
	m_UserInfoEquipmentWeapon.Reset();
	m_UserInfoEquipmentChara.Reset();
	m_UserInfoInvItemData.Reset();

	i3mem::FillZero( m_abGetInfoToServer, sizeof(m_abGetInfoToServer) );
	
	m_bIsRankUp		= FALSE;
	m_ui32rankUpPoint	= 0;
	
	// 로그인 시간 체크
	m_ui32LoginST	= 0;
	m_ui32ATInfo	= 0;

	// 어뷰징 체크
	m_ui32HoldAccExp		= 0;
	m_ui32HoldAccPoint		= 0;
	m_ui8AbusingLevel		= ABUSING_LEVEL_NONE;
	m_ui32AbusingPopupTime	= 0;
	m_ui32BattleStartTime	= 0;
	m_ui32BattleStartInvenTime = 0;
	
	m_ui32BattleDeathTime	= 0;

	m_ui16PrevExpRank		= 1000;		// 최초 기록이 발생하기 전까지 최하위 순위를 가진다

	m_bCheckLogout			= TRUE;
 
	m_ui32GrenadeCount		= 0;

	m_ui32RSLastBaseInfoTime = 0;
	m_i16RSTab1UseCount		= -1;
	m_i16RSTab2UseCount		= -1;
	m_i16RSTab3UseCount		= -1;

	// 중복채팅 방지 시스템
	m_i32LimitTime			= CHAT_LIMIT_NORMAL;				// 제한 시간
	m_ui32ChatTime			= 0;
	m_ui8ChatCount			= 0;
	m_ui32LimitStartTime	= 0;

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
		#ifdef USE_AHNHS
			m_hClient = ANTICPX_INVALID_HANDLE_VALUE;
			// 해당 서버의 핸들을 이용해 접속한 유저의 핸들을 생성
			m_hClient = _AhnHS_CreateClientObject( g_hAhnhsServer );

			if ( NULL == m_hClient )
			{
				return FALSE;
			}
		#endif
		}
		break;
	case GAMEGUARD_XTRAP:
		{	// Xtrap
		#ifdef USE_XTRAP  
			UINT32 ui32Res = XTrap_S_SessionInit( XTRAP_TIMEOUT, CSFILE_NUM, g_pXTrapModule->m_ui8XTrapMap,  m_pXtrapSessionBuf ) ;
			if ( XTRAP_API_RETURN_OK != ui32Res ) 
			{
				I3TRACE( "XTrap_S_SessionInit()	Failed Error:%d\n", ui32Res ); 
				return FALSE;
			}
		#endif
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
				char szLog[ MAX_PATH ];
				i3String::Format( szLog,"[USER_CONNECTIONSUCCESS] GPK Loading TimeOut : %d, m_i32CodeIdx : %d ", dwNewTime, m_i32CodeIdx );
				g_pModuleLogFile->Write_M_Log( MAIN_THREAD_IDX, NULL, szLog ); 
			}
		#endif
		}
		break;
	}	

	//Connect Ok
	m_bSendPacket = TRUE; 

	// HeartBit2 초기화
	m_bIsRecvHeartBit2 = 0;

	// 패킷 처리 테스트코드
	m_dwParseTimeHigh = 0;
	m_ui16ParseTimeHighProtocol = 0;

	i3NetworkPacket	StartPacket( PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS );
	StartPacket.WriteData( &m_SessionIdx,						sizeof( INT32 ) );	
	StartPacket.WriteData( &m_ConIp,							sizeof( UINT32) );
	StartPacket.WriteData( &g_pContextMain->m_ui16ClientPort,	sizeof( UINT16) );
	StartPacket.WriteData( &m_i16PacketRandSeed,				sizeof( INT16) );

	StartPacket.WriteData( g_pContextMain->m_pui8ChannelType,	(UINT16)(sizeof( UINT8 ) * GAME_CHANNEL_COUNT) );

	StartPacket.WriteData( &g_pContextMain->m_ui8UseServerSelect,	sizeof(UINT8)); 
	StartPacket.WriteData( &g_pContextMain->m_ui32GameServerCount,	sizeof(UINT32)); 
	StartPacket.WriteData( g_pContextMain->m_aGameServerInfo,		(UINT16)(sizeof(GAME_SERVER_INFO) * g_pContextMain->m_ui32GameServerCount) ); 

#if defined( USE_SERVER_BONUS )
	// 2011.05.06 - 서동권 - 패킷에 서버 보너스 정보를 담습니다.

	StartPacket.WriteData( &g_pContextMain->m_ui16ServerBonusCount,	sizeof(UINT16));

	for( UINT16 i = 0; i < g_pContextMain->m_ui16ServerBonusCount; ++i )
	{
		StartPacket.WriteData( &g_pContextMain->m_stServerBouns[i]._ui16Type,	sizeof(UINT16));
		StartPacket.WriteData( &g_pContextMain->m_stServerBouns[i]._ui32Exp,	sizeof(UINT32));
		StartPacket.WriteData( &g_pContextMain->m_stServerBouns[i]._ui32Point,	sizeof(UINT32));
	}

	// 2011.05.06 - 서동권 - 패킷에 채널 보너스 정보를 담습니다.
	StartPacket.WriteData( &g_pContextMain->m_ui8ChannelBonusCount,	sizeof(UINT8));
	StartPacket.WriteData( &g_pContextMain->m_stChannelBonus,		sizeof(TYPE_CHANNEL_BONUS) * g_pContextMain->m_ui8ChannelBonusCount);

#endif

#ifdef USE_GPK
	StartPacket.WriteData( &i32Len,	sizeof(INT32)); 
	StartPacket.WriteData( pCode,	i32Len); 
#endif
	
	SendPacketMessage(&StartPacket);

	return bRv;
}

//
//
//워크, 메인에서 호출 가능 Auth는 사용하지 않음 
BOOL CUserSession::OnDestroy(BOOL bForceMainThread)
{	
	m_bSendPacket = FALSE; 

	INT32	i32UserState = GetPosition();
	INT32	i32ChannelNum = m_i32ChannelNum;
	INT32	i32RoomIdx = m_i32RoomIdx;

	INT32 i32WorkingThread = m_WorkThreadIdx; 
	if(bForceMainThread)i32WorkingThread = MAIN_THREAD_IDX; 

	// 죽일려고 하려는 시도가 있으면. 메인이 죽이도록 만든다.이 부분 아주 중요합니다. 만약 이부분이 없다면. 크래쉬 할 수 있습니다. 
	if( FALSE == bForceMainThread )
	{
		if( 0 == m_ui32ATIsKill )
		{
			// 로그아웃 패킷에 의해 정상적인 절차로 종료되지 않았다면 로그 남깁니다.
			if( FALSE == m_bCheckLogout )
			{
				_SendErrorMessage( i32WorkingThread, EVENT_ERROR_EVENT , 0, KILL_REASON_WRONG_LOGOUT, "Wrong Logout" );
				//m_iKillReason	= KILL_REASON_WRONG_LOGOUT;
			}
			else	_SendErrorMessage( i32WorkingThread, EVENT_ERROR_EVENT , 0, KILL_REASON_LOGOUT );// m_iKillReason	= KILL_REASON_LOGOUT;
		}
		return FALSE;
	}

	// Check Room 
	g_pRoomManager->OnLeaveRoom( this, m_i32ChannelNum, i32WorkingThread, ROOM_ENDTYPE_DISCONNECT );

	//꼭해야함 하지않으면 크래쉬
	SetPosition(GAME_TASK_NONE);

	// Check Channel
	if((m_i32ChannelNum > SERVER_VALUE_UNKNOWN) && ( m_i32ChannelNum < (INT32)(GAME_CHANNEL_COUNT) ))//슈퍼 유저땜시 뒤에것은 검사해야함.
	{
		g_pUserManager->OnLeaveChannel( this, m_i32ChannelNum );
		m_i32ChannelNum		= SERVER_VALUE_UNKNOWN;
		m_i32LogChannelNum	= SERVER_VALUE_UNKNOWN;
	}

	// DB 가 없다면 이동하는 유저입니다.
	if(m_i64UID > SERVER_VALUE_UNKNOWN && GAME_TASK_LOGIN <= i32UserState)
	{
		//Zepetto 다른 서버에 로그아웃을 알립니다.
		if( !g_pModuleTrans->AuthSendLogOut(i32WorkingThread, this ) )
		{
			char strTemp[MAX_PATH];
			i3String::Format( strTemp, MAX_PATH, "#### OnDestroy ChangeDBFlage :%d ", m_ui8ChangeDBFlage );
			g_pModuleLogFile->Write_M_Log(i32WorkingThread, this, strTemp ) ; 
		}
		g_pModuleClan->ClanSendLogOut(i32WorkingThread, this );			
		g_pModuleMessenger->MessengerLogout(i32WorkingThread, m_i64UID);
	
		if(0 != m_ui64LoginTime)
		{
			g_pModuleContextNC->LogSendMessage(i32WorkingThread, LOG_MESSAGE_U_LOGOUT, this, &i32ChannelNum, &i32RoomIdx);
		}

		m_i64UID = SERVER_VALUE_UNKNOWN; 
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
		#ifdef USE_AHNHS
			_AhnHS_CloseClientHandle( m_hClient );
		#endif
		}
		break;
	}
	return i3NetworkSession::OnDestroy(bForceMainThread);
}

void CUserSession::_SendErrorMessage(INT32 i32WorkIdx, UINT32 ui32ErrorCode, PROTOCOL ProtocolID, INT32 i32KillReason, char* pstrMessage )
{	// 함수 사용하시기 전에 쓰레드 다시한번 확인 바랍니다.
	if( 0 < m_ui32ATIsKill ) return;

	//Set Time
	UINT32	ui32KillTime = g_pContextMain->GetServerTime();
	UINT32	ui32CutTime = g_pContextMain->GetDateSecTime();
	BOOL	bHacking = FALSE;
	BOOL	bLog = TRUE;
	UINT32	ui32HackCode = 0;
	i3NetworkPacket Packet;

	switch( i32KillReason )
	{
	case KILL_REASON_HACK_CLIENT:
	case KILL_REASON_HACK_SERVER_D:
	case KILL_REASON_HACK_SERVER_G:
		{
			bHacking = TRUE; // 해킹 탐지로 인한 종료 ( 클라이언트 표시 팝업 분류를 위해 추가 )
			ui32HackCode += (( i32KillReason - 20 ) * 100 ) + m_ui32HackKillReason;
		}
		//break; 주석이 맞음
	case KILL_REASON_HACK_PACKET:
		{
			Packet.SetProtocol(PROTOCOL_SERVER_MESSAGE_DISCONNECTED_HACK);
			Packet.WriteData( &ui32CutTime,		sizeof(UINT32) );
			Packet.WriteData( &ui32ErrorCode,	sizeof(UINT32) );
			Packet.WriteData( &bHacking,		sizeof(BOOL) );
			
			if( bHacking )	Packet.WriteData( &ui32HackCode,	sizeof(UINT32) );

			SendPacketMessage( &Packet );
		}
		break;
	case KILL_REASON_SERVER_MOVE:
		{
			bLog = FALSE;
		}
		break;
	case KILL_REASON_LOGOUT:
		{
			bLog = FALSE;
		}
		//break; 주석이 맞음
	case KILL_REASON_WRONG_LOGOUT:
		{
			ui32KillTime	= 1;// 쓰레드에서 죽일려는 시도가 있으면 메인에서 바로 죽을수 있게 m_iIsKillST 을 변경합니다
			bLog = FALSE;
		}
		break;
	case KILL_REASON_USERKICK:
		{
			ui32KillTime	+= rand() % 50;
		}
		//break; 주석이 맞음
	default:
		{
			Packet.SetProtocol(PROTOCOL_SERVER_MESSAGE_ERROR);
			Packet.WriteData( &ui32ErrorCode,	sizeof(UINT32) );
			SendPacketMessage( &Packet );
		}
		break;
	}

	m_ui32ATIsKill		= ui32KillTime;
	m_ui32KillReason	= i32KillReason;

	if( bLog )
	{
		char strMsg[ MAX_PATH ];
		strMsg[ 0 ] ='\0';
		if( pstrMessage )
		{
			i3String::Copy( strMsg, pstrMessage, MAX_PATH );
		}

		char strTemp[ LOG_FILE_STRING_COUNT ];	
		i3String::Format( strTemp, LOG_FILE_STRING_COUNT, "[ERROR]SendErrorMessage(UID:%I64d, Protocol:%d, BackProtocol:%d,timestamp(%d), Error:%x, KillReason:%d, Message:%s)", m_i64UID, ProtocolID, m_ui16BackProtocolID, (m_ui32ATIsKill - m_ui32ATLastRecv), ui32ErrorCode, i32KillReason, strMsg );
		g_pModuleLogFile->Write_M_Log( i32WorkIdx, this, strTemp );
	}
	return; 
}

void CUserSession::_SendFieldShopInfo( UINT32 ui32FieldShopVersion )
{
	if( ui32FieldShopVersion == g_pContextMain->GetFieldShopVersion() )	return;

	// Send All FieldShop information
	i3NetworkPacket SendPacket(PROTOCOL_FIELDSHOP_GOODSLIST_ACK);
	SendPacket.WriteData( g_pContextMain->GetFieldShop(), sizeof( FieldShopInfo ) );
	SendPacketMessage( &SendPacket );
}

void CUserSession::_SendShopInfo(UINT32 ui32ShopVersion)
{
	CShop* pShop = g_pContextMain->GetShop();
	if( ui32ShopVersion == pShop->GetVersion() )	return;

	UINT32 ui32Version				= pShop->GetVersion();
	UINT32 ui32GoodsCount			= pShop->GetGoodsCount();
	UINT32 ui32ItemCount			= pShop->GetItemCount();
	UINT32 ui32MatchingCount		= pShop->GetMatchingCount();
	UINT32 ui32RepairCount			= pShop->GetRepairCount();
	UINT32 ui32SaleCouponCount		= pShop->GetCouponCount();

	SHOP_GOODS_BASE*		pGoodsData	= pShop->GetGoodsAllData();
	SHOP_ITEM_BASE*			pItemData	= pShop->GetItemAllData();
	SHOP_MATCHING_DATA*		pMatching	= pShop->GetMatchingAllData();
	SHOP_ITEM_REPAIR_DATA*	pRepairData	= pShop->GetRepairAllData();
	SALE_COUPON*			pCouponData = pShop->GetCouponAllData();

	// Send Ready
	INT32	i32PacketSize		= PACKETBUFFERSIZE - 20; //PACKETHEADERSIZE - (sizeof( UINT32 ) + sizeof( UINT32 ) + sizeof( UINT32 ));
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
	while( bSendContinue )
	{
		ui32StartIdx			+= ui32CurItemCount;
		if( ui32RemainCount > ui32MaxCount )	{	ui32CurItemCount	= ui32MaxCount;								}
		else									{	ui32CurItemCount	= ui32RemainCount; bSendContinue = false;	}
		ui32RemainCount -= ui32CurItemCount;

		i3NetworkPacket	ItemPacket( PROTOCOL_AUTH_SHOP_ITEMLIST_ACK );
		ItemPacket.WriteData( &ui32ItemCount,				sizeof( UINT32 ) );
		ItemPacket.WriteData( &ui32CurItemCount,			sizeof( UINT32 ) );
		ItemPacket.WriteData( &ui32StartIdx,				sizeof( UINT32 ) );
		ItemPacket.WriteData( &pItemData[ui32StartIdx],		(UINT16)(sizeof( SHOP_ITEM_BASE )*ui32CurItemCount) );
		ItemPacket.WriteData( &ui32Version,					sizeof( UINT32 ) );
		SendPacketMessage( &ItemPacket );
	}

	// GoodsList
	ui32RemainCount		= ui32GoodsCount;
	ui32CurItemCount	= 0;
	ui32StartIdx		= 0;

	bSendContinue		= true;			// 데이터가 0개라도 데이터가 0개임을 보내서 알린다.
	ui32MaxCount		= (i32PacketSize / sizeof( SHOP_GOODS_BASE ));
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
		GoodsPacket.WriteData( &pGoodsData[ui32StartIdx],	(UINT16)(sizeof( SHOP_GOODS_BASE )*ui32CurItemCount) );
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
		RepairPacket.WriteData( &ui32Version,				sizeof( UINT32 ) );
		SendPacketMessage( &RepairPacket );
	}

	// CouponList
	ui32RemainCount		= ui32SaleCouponCount;
	ui32CurItemCount	= 0;
	ui32StartIdx		= 0;

	bSendContinue		= true;			// 데이터가 0개라도 데이터가 0개임을 보내서 알린다.
	ui32MaxCount		= (i32PacketSize / sizeof( SALE_COUPON ));
	while( bSendContinue )
	{
		ui32StartIdx			+= ui32CurItemCount;
		if( ui32RemainCount > ui32MaxCount )	{	ui32CurItemCount	= ui32MaxCount;								}
		else									{	ui32CurItemCount	= ui32RemainCount; bSendContinue = false;	}
		ui32RemainCount -= ui32CurItemCount;

		i3NetworkPacket SaleCouponPacket( PROTOCOL_AUTH_SHOP_SALECOUPONLIST_ACK );
		SaleCouponPacket.WriteData( &ui32SaleCouponCount,	sizeof( UINT32 ) );
		SaleCouponPacket.WriteData( &ui32CurItemCount,		sizeof( UINT32 ) );
		SaleCouponPacket.WriteData( &ui32StartIdx,			sizeof( UINT32 ) );
		SaleCouponPacket.WriteData( &pCouponData[ui32StartIdx],	(UINT16)(sizeof( SALE_COUPON )*ui32CurItemCount) );
		SaleCouponPacket.WriteData( &ui32Version,			sizeof( UINT32 ) );
		SendPacketMessage( &SaleCouponPacket );
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

	return; 
}

void CUserSession::_SendEquipment(void)
{
	i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_EQUIPMENT_INFO_ACK );
	SendPacket.WriteData(&m_UserInfoEquipmentChara,		sizeof(USER_INFO_EQUIPMENT_CHARA));
	SendPacket.WriteData(&m_UserInfoEquipmentWeapon,	sizeof(USER_INFO_EQUIPMENT_WEAPON));
	SendPacketMessage(&SendPacket);
	return; 
}

// -----------------------------------------------------------
// Name : _GetMyInfoPacket
// Desc : Trans 서버로 부터 받는 나의 정보에 대한 패킷을 파싱합니다.
// -----------------------------------------------------------
UINT8 CUserSession::_GetMyInfoPacket( INT32 i32ThreadIdx, i3NetworkPacket* pPacket, BOOL bClan )
{
	UINT8				ui8RecvInventory = 0;
	UINT16				ui16ItemCount;
	INVEN_BUFFER		aInvenItem[ MAX_INVEN_COUNT ];
	INVEN_BUFFER_SET	InvenSet;

	pPacket->ReadData( &m_ui32BirthDay,						sizeof(UINT32) );
	pPacket->ReadData( &m_UserInfoBasic._cash,				sizeof(UINT32) );
	pPacket->ReadData( &m_ui8Gender,						sizeof(UINT8) );
	pPacket->ReadData( &m_UserInfoBasic._pcCafe,			sizeof(UINT8) );
	pPacket->ReadData( &m_UserInfoBasic._eSportID,			sizeof(UINT8) );

	pPacket->ReadData( m_UserInfoBasic._nick,				NET_NICK_NAME_SIZE );
	pPacket->ReadData( &m_UserInfoBasic._rank,				sizeof(UINT32) );
	pPacket->ReadData( &m_UserInfoBasic._point,				sizeof(UINT32) );
	pPacket->ReadData( &m_UserInfoBasic._exp,				sizeof(UINT32) );
	pPacket->ReadData( &m_UserInfoBasic._keep_rank,			sizeof(UINT32) );	
 	pPacket->ReadData( &m_UserInfoBasic._ui32AbusingCount,	sizeof(UINT32) );
	pPacket->ReadData( &m_UserInfoBasic._nickcolor,			sizeof(UINT8) );
	pPacket->ReadData( &m_UserInfoBasic._ui8TutorialIng,	sizeof(UINT8)  );
	pPacket->ReadData( &m_ui32LoginBattleTime,				sizeof(UINT32)  );
	pPacket->ReadData( &m_UserInfoBasic._ui32TotalBattleTime,sizeof(UINT32)  );
	pPacket->ReadData( &m_UserInfoBasic._ui32LastBuyCash,	sizeof(UINT32)  );
 
	if( bClan )
	{
		pPacket->ReadData( &m_UserInfoBasic._clanidx,		sizeof(UINT32) );
		pPacket->ReadData( &m_UserInfoBasic._clanstate,		sizeof(UINT32) );

		pPacket->ReadData( &m_UserInfoBasic._clanExpRank,	sizeof(UINT8) );
		pPacket->ReadData( &m_UserInfoBasic._clanPerRank,	sizeof(UINT8) );
		pPacket->ReadData( &m_UserInfoBasic._clanMark,		sizeof(UINT32) );
		pPacket->ReadData( &m_UserInfoBasic._clanName,		NET_CLAN_NAME_SIZE );
	}	

	pPacket->ReadData( &m_UserInfoEquipmentWeapon,		sizeof(USER_INFO_EQUIPMENT_WEAPON) );
	pPacket->ReadData( &m_UserInfoEquipmentChara,		sizeof(USER_INFO_EQUIPMENT_CHARA) );
	pPacket->ReadData( &m_UserInfoRecord,				sizeof(USER_INFO_RECORD) );
	pPacket->ReadData( &m_UserInfoInvItemData,			sizeof(USER_INFO_INVITEM_DATA) );

	pPacket->ReadData( &m_ui32QuestEventDate,			sizeof(UINT32) );
	pPacket->ReadData( m_aaui8QuestCtxActive,			sizeof(m_aaui8QuestCtxActive) );
	pPacket->ReadData( &m_QuestIngInfo,					sizeof(QUESTING_INFO) );
	pPacket->ReadData( &m_ui32Miniature,				sizeof(UINT32) );
	pPacket->ReadData( &m_ui32Insignia,					sizeof(UINT32) );
	pPacket->ReadData( &m_ui32Order,					sizeof(UINT32) );
	pPacket->ReadData( &m_ui32Master,					sizeof(UINT32) );
	pPacket->ReadData( &m_ui64GainUserTitle,			sizeof(UINT64) );
	pPacket->ReadData( m_aui8EquipUserTitle,			sizeof(m_aui8EquipUserTitle) );

	pPacket->ReadData( &ui8RecvInventory,				sizeof(UINT8) );
	if( ui8RecvInventory )
	{
		ui8RecvInventory = 1;

		pPacket->ReadData( &ui16ItemCount,				sizeof(INT16) );			LENGTH_CHECK( ui16ItemCount, MAX_INVEN_COUNT );
		pPacket->ReadData( aInvenItem,					sizeof(INVEN_BUFFER)*ui16ItemCount );

		for( INT32 i = 0 ; i < ui16ItemCount ; i++ )
		{
			OnInsertItem( &aInvenItem[i], &InvenSet );
		}
	}

	//
	pPacket->ReadData( &m_ui64LoginTime,				sizeof(UINT64) );
	pPacket->ReadData( &m_ui32AccConnectTime,			sizeof(UINT32) );
	pPacket->ReadData( &m_ui32AccConnectCount,			sizeof(UINT32) );
	pPacket->ReadData( &m_ui32LastLoginTime,			sizeof(UINT32) );
	
	// 당일 전적 정보 
	pPacket->ReadData( & m_UserInfoDaily,				sizeof(USER_INFO_DAILY) );
	pPacket->ReadData( &m_bGetEventItem,				sizeof(BOOL) );

	pPacket->ReadData( &m_ui8ReportCount,				sizeof(UINT8) );

	pPacket->ReadData( &m_ui8EventCountry,				sizeof(UINT8));
	pPacket->ReadData( &m_ui8OtherCountry,				sizeof(UINT8));

	pPacket->ReadData( &m_bHasSID,						sizeof(bool));

	// 게임서버에서 Trans서버로부터 받은 값들을 가지고 기본적으로 설정하는 값입니다.
	_SetUserRecord();						// 유저 전적 세팅
	_SetCalcLimitTitleCount();				// 호칭슬롯 해제
	_SetAge();								// 나이설정
	_SetGMUser();							// GM 확인
	_ExceptionWeapon( i32ThreadIdx );		// 무기 추가
	//_ExceptionCharacter( i32ThreadIdx );	// 케릭터 추가
	_ExceptionSpecial( i32ThreadIdx );		// 스페샬 아이템 추가
	_CheckEquipment();						// 장착된 아이템 확인
	_SetQuestEvent();						// 새로운 이벤트 임무카드를 확인합니다.

	m_bHSCheck      = TRUE;
	m_bCompleteTS	= FALSE;

	return ui8RecvInventory;
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
	UINT32 ui32Temp;
	// 트랜스 서버에서 나이를 사용하지 않는나라의 경우 생년월일을 1980년으로 임의 설정 하고 있음
	if( m_ui32BirthDay == 0 )
	{
		m_ui8Age = 0;
	}
	else
	{
		ui32Temp	= m_ui32BirthDay / 10000;
		m_ui8Age	= (UINT8)(g_pContextMain->m_i32Year - ui32Temp); 
		ui32Temp	= m_ui32BirthDay % 10000;
		if( ui32Temp < g_pContextMain->m_ui32Date ) m_ui8Age++;

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

void CUserSession::_SetGMUser()
{
	if( 'G' == m_UserInfoBasic._nick[0] && 'M' == m_UserInfoBasic._nick[1] )	m_bGMUser = TRUE;

	// GM 은 강제로 계급을 RANK_GAMEMASTER(53) 로 맞춰줍니다.
	if( m_bGMUser )					m_UserInfoBasic._rank = RANK_GAMEMASTER;

switch (m_ui8LocaleCode)
	{
	case LOCALE_CODE_TURKEY:	// 터키 클라이언트일 경우 MOD권한 체크
		{
			// 터키에서는
			// 임의로 특정 UID에 대해 Nick이 GM이 아니라도 GM으로 동작할 수 있도록 설정해준다.
			switch( m_i64UID )
			{
			case 7:				// ristto@gmail.com
			case 8:				// effa.gr@gmail.com
			case 55089:			// zepetto01
			case 338985:		// tavuklutost@mynet.com
			case 482555:		// pointblanktester@hotmail.com
				m_bGMUser = TRUE;
				break;
			}

			// MOD보다 GM이 더 높으므로 GM이 아니면서 계급이 RANK_MOD일 경우 MOD 플래그 On - 2012-09-06 김용수
			if ( (m_bGMUser != TRUE) && (m_UserInfoBasic._rank == RANK_MOD) )		m_bMODUser = TRUE;
		}
		break;

	// 클라팀 요청으로 일단 국가코드 없이 세팅되도록 수정합니다.
	// 2월빌드로 머지할 때는 지울 예정입니다.
	default:
		{
			// MOD보다 GM이 더 높으므로 GM이 아니면서 계급이 RANK_MOD일 경우 MOD 플래그 On - 2012-09-06 김용수
			if ( (m_bGMUser != TRUE) && (m_UserInfoBasic._rank == RANK_MOD) )		m_bMODUser = TRUE;
		}
		break;
	}


#if defined ( I3_DEBUG )
	switch( m_i64UID )
	{
	case 10000002228:			// servertest1
		m_bGMUser = TRUE;
		break;
	}
#endif
}

BOOL CUserSession::_ExceptionInsert( T_ItemID TItemID, UINT8 ui8ItemType )
{
	switch( GET_ITEM_FLAG_CLASS( TItemID ) )
	{
	case ITEM_CLASS_ASSAULT:
	case ITEM_CLASS_SMG:
	case ITEM_CLASS_SNIPER:
	case ITEM_CLASS_SHOTGUN:
	case ITEM_CLASS_HANDGUN:
	case ITEM_CLASS_KNIFE:
	case ITEM_CLASS_GRENADE:
	case ITEM_CLASS_THROWING:
	case ITEM_CLASS_MACHINE:
		{
			return _ExceptionWeaponInsert( TItemID, ui8ItemType );
		}
		break;
	case ITEM_CLASS_CHARA:
	case ITEM_CLASS_HEAD:
		{
			return _ExceptionCharaInsert( TItemID, ui8ItemType );
		}
		break;
	case ITEM_CLASS_MAINTENANCE:
	case ITEM_CLASS_EXPENDABLE:
	case ITEM_CLASS_POINT:		
	case ITEM_CLASS_TEMP:	
	case ITEM_CLASS_COUPON:
	//case ITEM_CLASS_POINT:
	case ITEM_CLASS_SALECOUPON:
		{
			return _ExceptionItemInsert( TItemID, ui8ItemType );
		}
		break;
	}
	return FALSE;
}

BOOL CUserSession::_ExceptionWeaponInsert( UINT32 ui32ItemID, UINT8 ui8ItemType )
{
	if( m_i32InvWeaponCount >= MAX_INVEN_COUNT )	return FALSE;
	_ExceptionInsert( &m_aInvWeapon[m_i32InvWeaponCount], ui32ItemID, ui8ItemType );
	m_i32InvWeaponCount++;
	return TRUE;
}

BOOL CUserSession::_ExceptionCharaInsert( UINT32 ui32ItemID, UINT8 ui8ItemType )
{
	if( m_i32InvCharaCount >= MAX_INVEN_COUNT )	return FALSE;
	_ExceptionInsert( &m_aInvChara[m_i32InvCharaCount], ui32ItemID, ui8ItemType );
	m_i32InvCharaCount++; 
	return TRUE;
}

BOOL CUserSession::_ExceptionItemInsert( UINT32 ui32ItemID, UINT8 ui8ItemType )
{
	if( m_i32InvItemCount >= MAX_INVEN_COUNT )	return FALSE;
	_ExceptionInsert( &m_aInvItem[m_i32InvItemCount], ui32ItemID, ui8ItemType );
	m_i32InvItemCount++; 
	return TRUE;
}

void CUserSession::_ExceptionInsert( INVEN_BUFFER* pBuffer, UINT32 ui32ItemID, UINT8 ui8ItemType )
{
	pBuffer->_i64ItemWareDBIdx		= 0;
	pBuffer->_ui32ItemID			= ui32ItemID;
	pBuffer->_ui32ItemArg			= 1;
	pBuffer->_ui8ItemType			= ui8ItemType; 
}

INT32 CUserSession::_SelectChannel( UINT32 ui32Channel )
{	
	//채널 검사 
	if( ui32Channel >= GAME_CHANNEL_COUNT )	return EVENT_ERROR_SERVER;
	if( (FALSE == m_bGMUser) && (FALSE == m_bMODUser) )	// GM이거나 MOD 일경우 무조건 성공
	{
		switch(g_pContextMain->m_pui8ChannelType[ ui32Channel ])
		{
		case CHANNEL_TYPE_BEGIN1	:
			{
				//KillDeath 확인 
				REAL32  Killcount	= (REAL32)m_UserInfoRecord._killcount; 
				REAL32	Deathcount	= (REAL32)m_UserInfoRecord._death; 			


				switch( m_ui8LocaleCode )
				{
				case LOCALE_CODE_TAIWAN		:	// 대만은 킬뎃 50퍼 미만.
					{
						if((m_UserInfoRecord._killcount + m_UserInfoRecord._death) != 0)
						{
							if((Killcount / (Killcount + Deathcount)) >= 0.50f )
							{
								return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
							}
						}
					}
					break;
				default : // 타국가는 킬뎃 40퍼 미만
					{
						if((m_UserInfoRecord._killcount + m_UserInfoRecord._death) != 0)
						{
							if((Killcount / (Killcount + Deathcount)) > 0.40f )
							{
								return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
							}
						}
					}
					break;
				}
			}
			break; 
		case CHANNEL_TYPE_BEGIN2	: 
			{
				if( LOCALE_CODE_INDONESIA == m_ui8LocaleCode )
				{
					if (m_UserInfoBasic._ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME) break;	// 10시간미만이면 계급상관없이 입장시킵니다.
				}

				UINT32 wRank = m_UserInfoBasic._rank;
				if( -1 != GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_RANK, CASHITEM_DAY_NO) ) )
				{
					wRank = m_UserInfoInvItemData._wRank;
				}
				switch( m_ui8LocaleCode )
				{
				case LOCALE_CODE_TURKEY			:
					{
						if( RANK_05 < wRank )	// 하사 1호봉
						{
							return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
						}
					}
					break;
				// 대만도 상병으로
				//case LOCALE_CODE_TAIWAN			:	
				//	{
				//		if( RANK_07 < wRank )	// 하사 3호봉
				//		{
				//			return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
				//		}
				//	}
				//	break;
				default:
					{
						if( RANK_03 < wRank )	// 상병
						{
							return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
						}
					}
					break;				
				}
			}
			break; 
		case CHANNEL_TYPE_PCCAFE_CLAN:
			if( m_ui8LocaleCode == LOCALE_CODE_INDONESIA )
			{
				if (m_UserInfoBasic._pcCafe != PC_INDO_SILVER && m_UserInfoBasic._pcCafe != PC_INDO_GOLD)	return EVENT_ERROR_CHANNEL_ENTER_CONDITION_GWARNET;
			}
			break;
		case CHANNEL_TYPE_CLAN		: 
			if( LOCALE_CODE_INDONESIA == m_ui8LocaleCode )
			{
				if (m_UserInfoBasic._ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME) return EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME;
			}
			if( (m_UserInfoBasic._clanidx == 0) || (m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN) )
				return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
			break; 
		case CHANNEL_TYPE_EXPERT	: 
			{
				if( LOCALE_CODE_INDONESIA == m_ui8LocaleCode )
				{
					if (m_UserInfoBasic._ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME) return EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME;
				}
				UINT32 ui32Rank = m_UserInfoBasic._rank;
				if( -1 != GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_RANK, CASHITEM_DAY_NO) ) )
				{
					ui32Rank = m_UserInfoInvItemData._wRank;
				}
				switch( m_ui8LocaleCode )
				{
				case LOCALE_CODE_INDONESIA		:
					{
						if( RANK_31 > ui32Rank )			// 소령 1호봉 이상
						{
							return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
						}
					}
					break;
				case LOCALE_CODE_RUSSIA			:
					{
						if( RANK_26 > ui32Rank )			// 대위 1호봉 이상
						{
							return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
						}
					}
					break;
				case LOCALE_CODE_TURKEY			:
					{
						if( RANK_21 > ui32Rank )			// 중위 1호봉 이상
						{
							return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
						}
					}
					break;
				case LOCALE_CODE_PHILIPPINES:
					{
						if( RANK_15 > ui32Rank )			// 상사 4호봉 이상
						{
							return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
						}
					}
					break;
				default:
					{
						if( RANK_04 > ui32Rank )			// 병장이상
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
					if (m_UserInfoBasic._ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME) return EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME;
				}
				UINT32 ui32Rank = m_UserInfoBasic._rank;
				if( -1 != GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_RANK, CASHITEM_DAY_NO) ) )
				{
					ui32Rank = m_UserInfoInvItemData._wRank;
				}

				switch( m_ui8LocaleCode )
				{
				case LOCALE_CODE_PHILIPPINES:
					{
						if( RANK_30 > ui32Rank )	// 대위 5호봉 이상
						{
							return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
						}
					}
					break;


				default:
					{
						if( RANK_12 > ui32Rank )	// 상사 1호봉이상
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
					if (m_UserInfoBasic._ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME) return EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME;
				}
				UINT32 ui32Rank = m_UserInfoBasic._rank;
				if( -1 != GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_RANK, CASHITEM_DAY_NO) ) )
				{
					ui32Rank = m_UserInfoInvItemData._wRank;
				}

				switch( m_ui8LocaleCode )
				{
				case LOCALE_CODE_LATIN_AMERICA		:
					{
						// 하사 1호봉 ~ 상사 5호봉 까지
						if( RANK_04 > ui32Rank )
						{
							return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
						}
						if( RANK_11 < ui32Rank )
						{
							return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
						}
					}
					break;
				case LOCALE_CODE_TURKEY				:
					{
						// 하사 2호봉 ~ 원수
						if( RANK_06 > ui32Rank )
						{
							return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
						}
						if( RANK_50 < ui32Rank )
						{
							return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
						}
					}
					break;
				default:
					{
						// 하사 1호봉 ~ 상사 5호봉 까지
						if( RANK_05 > ui32Rank )
						{
							return EVENT_ERROR_CHANNEL_ENTER_CONDITION;
						}
						if( RANK_16 < ui32Rank )
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
				if (m_UserInfoBasic._ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME) return EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME;
				if (m_UserInfoBasic._pcCafe != PC_INDO_SILVER && m_UserInfoBasic._pcCafe != PC_INDO_GOLD)	return EVENT_ERROR_CHANNEL_ENTER_CONDITION_GWARNET;
			}
			break; 
		case CHANNEL_TYPE_PCCAFE_GOLD	: 
			if( m_ui8LocaleCode == LOCALE_CODE_INDONESIA )
			{
				if (m_UserInfoBasic._ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME) return EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME;
				if (m_UserInfoBasic._pcCafe != PC_INDO_GOLD)	return EVENT_ERROR_CHANNEL_ENTER_CONDITION_GWARNET;
			}
			break; 
		case CHANNEL_TYPE_AZERBAIJAN:
			if( 0 != i3String::Compare(g_astrGeoIP2_country_code[m_ui8OtherCountry], "AZ"))	return EVENT_ERROR_CHANNEL_ENTER_CONDITION_AZERBAIJAN;
			break;
		default:
			if( LOCALE_CODE_INDONESIA == m_ui8LocaleCode )
			{
				if (m_UserInfoBasic._ui32TotalBattleTime < BEGIN3_CHANNEL_CONDITION_TIME) return EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME;
			}
			break; 
		}
	}

	m_i32ChannelNum		= SERVER_VALUE_UNKNOWN;
	m_i32LogChannelNum	= SERVER_VALUE_UNKNOWN;

	return g_pUserManager->OnEnterChannel( this, ui32Channel );
}

INT32 CUserSession::_CreateRoom( ROOM_INFO_CREATE* pInfoCreate )
{
	pInfoCreate->nChannelNum			= m_i32ChannelNum;

	INT32 i32RoomIdx = g_pRoomManager->OnCreateRoom( this, pInfoCreate );

	if( SERVER_VALUE_UNKNOWN < i32RoomIdx )
	{
		SetPosition(GAME_TASK_READY_ROOM); 
		m_pRoom				= g_pRoomManager->GetRoom( m_i32ChannelNum, i32RoomIdx ); 
		m_i32RoomIdx		= i32RoomIdx; 
		m_i32SlotIdx		= 0; 	
		m_bForceRoomLeave	= FALSE; 
				
		g_pModuleMessenger->MessengerSendFriendRoomEnter( m_WorkThreadIdx, this, (UINT8)m_i32ChannelNum, (UINT16)m_i32RoomIdx ); 
		g_pContextMain->SetCreateRoomTime_NC_LOG(); 
	}
	else
	{
		m_i32RoomIdx = SERVER_VALUE_UNKNOWN; 				
		m_i32SlotIdx = SERVER_VALUE_UNKNOWN; 
	}

	i3NetworkPacket SendPacket( PROTOCOL_LOBBY_CREATE_ROOM_ACK );
	SendPacket.WriteData( &i32RoomIdx,				sizeof(INT32) );
	if( SERVER_VALUE_UNKNOWN < i32RoomIdx )
	{
		SendPacket.WriteData(&m_pRoom->m_Info._InfoBasic,	sizeof(ROOM_INFO_BASIC) );
		SendPacket.WriteData(&m_pRoom->m_Info._InfoAdd,	sizeof(ROOM_INFO_ADD) );

		// 클랜 서버에 알려줌
		g_pModuleClan->ClanSendRoomEnter(m_WorkThreadIdx, this, m_i32RoomIdx );
	}
	SendPacketMessage( &SendPacket );

	return i32RoomIdx;
}

INT32 CUserSession::_BuyGoods( UINT32 ui32GoodsID, UINT8 ui8BuyType, UINT32 ui32CouponID, INT64 i64CouponIdx )
{
	CShop* pShop = g_pContextMain->GetShop();
	SHOP_GOODS_BASE* pGoods = pShop->GetGoodsData( ui32GoodsID );

	if( NULL == pGoods )																	return EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_GOODS;	// 상품이 없다.
	if( GOODS_USE_DISABLE == pGoods->_ui8Enable )											return EVENT_ERROR_FAIL;							// 판매하지 않는 상품
	
	// 구매 유형
	switch( ui8BuyType )
	{
	case GOODS_BUY_POINT:
		{
			if( 1 > pGoods->_ui32Point )								return EVENT_ERROR_FAIL;							// Point 로 판매 안되는 상품
			if( pGoods->_ui32Point	> m_UserInfoBasic._point )			return EVENT_ERROR_EVENT_AUTH_START_PRICE;			// 돈이 모자람
		}
		break;
	case GOODS_BUY_CASH:
		{
			if( 1 > pGoods->_ui32Cash )									return EVENT_ERROR_FAIL;							// Cash 로 판매 안되는 상품
			
			if( ui32CouponID != 0 )
			{
				SALE_COUPON *pCoupon		= g_pContextMain->GetShop()->GetCouponData( ui32CouponID );

				if( NULL == pCoupon )											return EVENT_ERROR_INVALID_COUPON; // 해당 할인쿠폰이 존재하지 않음
				if( FALSE == CheckSaleCouponDate( i64CouponIdx, pCoupon ) )		return EVENT_ERROR_EXPIRY_DATE_COUPON; // 사용 기간이 아닌 쿠폰

				UINT32	ui32Cash = 0;
				if( FALSE == CheckUseCoupon( pCoupon, pGoods, ui32Cash )  )		return EVENT_ERROR_COUPON_DATA;
				if( ui32Cash > m_UserInfoBasic._cash )							return EVENT_ERROR_EVENT_AUTH_START_PRICE;			// 돈이 모자람
			}
			else 
				if( pGoods->_ui32Cash	> m_UserInfoBasic._cash )				return EVENT_ERROR_EVENT_AUTH_START_PRICE;			// 돈이 모자람
		}
		break;
	default:
		{
			return EVENT_ERROR_FAIL;
		}
		break;
	}

	if( FALSE == _CheckBuyUserTitle( ui32GoodsID ) )					return EVENT_ERROR_EVENT_BUY_GOODS_USER_TITLE_NOT_ALLOWED;		// 호칭이 없어서 구매 불가능

	if( FALSE == g_pModuleTrans->AuthSendBuyGoods( m_WorkThreadIdx, this, ui32GoodsID, ui8BuyType, ui32CouponID, i64CouponIdx ))	return EVENT_ERROR_EVENT_BUY_GOODS_OVERLOAD;

	// 돈 차감
	switch( ui8BuyType )
	{
	case GOODS_BUY_POINT:
		m_UserInfoBasic._point	-= pGoods->_ui32Point;
		ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );
		break;
	case GOODS_BUY_CASH:
		break;
	}

	return EVENT_ERROR_SUCCESS;
}

INT32 CUserSession::_BuyGoodsBasket( BUY_BASKET *pBasket , UINT8 ui8GoodsCount )
{
// 장바구니
	UINT32	ui32Point = 0;
	UINT32	ui32Cash = 0;
	SALE_COUPON*	pCoupon = NULL;

	for( UINT8 i = 0; i < ui8GoodsCount; ++i )
	{
		SHOP_GOODS_BASE* pGoods = g_pContextMain->GetShop()->GetGoodsData( pBasket[i].m_ui32GoodsID );

		if( NULL == pGoods )																	return EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED;		// 상품이 없다.
		if( GOODS_USE_DISABLE == pGoods->_ui8Enable )											return EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED;		// 판매하지 않는 상품

		if( 0 != pBasket[i].m_ui32CouponID && pGoods->_ui8SaleType == GOODS_BUY_CASH ) // 할인쿠폰을 사용할 경우 할인쿠폰에 대한 검사 실시 ( SIA 서버에서 처리시 확인 또 함 )
		{
			pCoupon		= g_pContextMain->GetShop()->GetCouponData( pBasket[i].m_ui32CouponID );

			if( NULL == pCoupon ) return EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED; // 해당 할인쿠폰이 존재하지 않음

			if( FALSE == CheckSaleCouponDate( pBasket[i].m_i64CouponIdx, pCoupon ) )	return EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED; // 사용 기간이 아닌 쿠폰
			if( FALSE == CheckUseCoupon( pCoupon, pGoods, ui32Cash ) )					return EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED; // 할인쿠폰 사용 조건이 맞지 않음
		}
		else
			ui32Cash	+= pGoods->_ui32Cash; // 할인 쿠폰을 사용하지 않았다면

		// 구매 유형
		switch( pBasket[i].m_ui8BuyType )
		{
		case GOODS_BUY_POINT:
			{
				if( 1 > pGoods->_ui32Point )								return EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED;		// Point 로 판매 안되는 상품

				ui32Point	+= pGoods->_ui32Point;
				if( ui32Point	> m_UserInfoBasic._point )					return EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED;		// 돈이 모자람
			}
			break;
		case GOODS_BUY_CASH:
			{
				if( 1 > pGoods->_ui32Cash )									return EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED;		// Cash 로 판매 안되는 상품
				if( ui32Cash	> m_UserInfoBasic._cash )					return EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED;		// 돈이 모자람
			}
			break;
		default:
			{
				return EVENT_ERROR_FAIL;
			}
			break;
		}

		if( FALSE == _CheckBuyUserTitle( pBasket[i].m_ui32GoodsID ) )					return EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED;		// 호칭이 없어서 구매 불가능
	}
	
	if( FALSE == g_pModuleTrans->AuthSendBuyGoodsBasket( m_WorkThreadIdx, this, pBasket, ui8GoodsCount ))	return EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED;

	// 돈 차감
	m_UserInfoBasic._point	-= ui32Point;
	ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );

	return EVENT_ERROR_SUCCESS;
}

BOOL CUserSession::_CheckBuyUserTitle( UINT32 ui32GoodsID )
{

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
	if( 0 == ui32ItemID ) return TRUE; // 매칭되는 ItemID가 없다면 강제 성공.

	// ui32ItemID에 호칭 Number 값을 검색한다.
	UINT8 ui8Restriction = 0;
	for ( UINT32 i = 0 ; i < ui32ItemCount ; i ++ )
	{
		if( pItemData[i]._ui32ItemID == ui32ItemID )
		{
			ui8Restriction	= pItemData[i]._ui8Restriction ;
			break;
		}
	}
	if( 0 == ui8Restriction ) return TRUE; // 매칭되는 호칭 Number가 없다면 강제 성공.

	// 검색한 호칭 Number의 호칭을 유저가 획득했다면 성공.
	if( GET_USERTITLE( m_ui64GainUserTitle, ui8Restriction ) )	return TRUE;
	return FALSE;

 
	/* 
	// OLD VERSION - 일단 백업버전들 확인을 위해 남겨둡니다. 2011.02.23 노준영
	switch( (ui32GoodsID / 100) )
	{
	case 100007:		if( GET_USERTITLE( m_ui64GainUserTitle, 8 ) )	return TRUE;		break;		// SG 550 S
	
	case 100026:		if( GET_USERTITLE( m_ui64GainUserTitle, 14 ) )	return TRUE;		break;		// Dragunov G.
	case 200004:		if( GET_USERTITLE( m_ui64GainUserTitle, 30 ) )	return TRUE;		break;		// P99&HAK
	case 100016:		if( GET_USERTITLE( m_ui64GainUserTitle, 20 ) )	return TRUE;		break;		// MP5K G.
	case 100018:		if( GET_USERTITLE( m_ui64GainUserTitle, 20 ) )	return TRUE;		break;		// Spectre W.
	case 400001:		if( GET_USERTITLE( m_ui64GainUserTitle, 40 ) )	return TRUE;		break;		// C-5
	case 100033:		if( GET_USERTITLE( m_ui64GainUserTitle, 35 ) )	return TRUE;		break;		// 870MCS W.
	case 200010:		if( GET_USERTITLE( m_ui64GainUserTitle, 31 ) )	return TRUE;		break;		// Colt Python G. + 10
	case 100008:		if( GET_USERTITLE( m_ui64GainUserTitle, 10 ) )	return TRUE;		break;		// G36C Ext.
	case 300002:		if( GET_USERTITLE( m_ui64GainUserTitle, 28 ) )	return TRUE;		break;		// Amok Kukri
	case 100027:		if( GET_USERTITLE( m_ui64GainUserTitle, 16 ) ) 	return TRUE;		break;		// PSG1 S.
	case 200005:		if( GET_USERTITLE( m_ui64GainUserTitle, 32 ) ) 	return TRUE;		break;		// Dual HandGun
	case 100019:		if( GET_USERTITLE( m_ui64GainUserTitle, 22 ) ) 	return TRUE;		break;		// P90 Ext.
	
	case 500002:		if( GET_USERTITLE( m_ui64GainUserTitle, 42 ) ) 	return TRUE;		break;		// WP Smoke
	case 100034:		if( GET_USERTITLE( m_ui64GainUserTitle, 37 ) ) 	return TRUE;		break;		// SPAS-15
	case 100009:		if( GET_USERTITLE( m_ui64GainUserTitle, 12 ) ) 	return TRUE;		break;		// AK SOPMOD
	case 100062:		if( GET_USERTITLE( m_ui64GainUserTitle, 12 ) ) 	return TRUE;		break;		// AUG A3
	case 300003:		if( GET_USERTITLE( m_ui64GainUserTitle, 29 ) ) 	return TRUE;		break;		// Mini Axe
	case 100021:		if( GET_USERTITLE( m_ui64GainUserTitle, 18 ) ) 	return TRUE;		break;		// L115A1
	case 200006:		if( GET_USERTITLE( m_ui64GainUserTitle, 33 ) ) 	return TRUE;		break;		// Dual D-Eagle
	case 100020:		if( GET_USERTITLE( m_ui64GainUserTitle, 24 ) ) 	return TRUE;		break;		// Kriss S.V

#if !defined( LOCALE_RUSSIA)
	// 러시아는 아래 무기가 호칭무기가 아님.
	case 300001:		if( GET_USERTITLE( m_ui64GainUserTitle, 26 ) )	return TRUE;		break;		// Dual Knife
	case 100079:		if( GET_USERTITLE( m_ui64GainUserTitle, 22 ) ) 	return TRUE;		break;		// AKS74U Ext.
#endif
																																																																																											
#if !defined( LOCALE_INDONESIA)
	// 인도네시아는 아래 무기가 호칭무기가 아님.
	case 100080:		if( GET_USERTITLE( m_ui64GainUserTitle, 38 ) ) 	return TRUE;		break;		// Win. M1887 S.
#endif

	// 호칭무기 아님
	//case 200011:		if( GET_USERTITLE( m_ui64GainUserTitle, 33 ) ) 	return TRUE;		break;		// Glock18
	//case 300007:		if( GET_USERTITLE( m_ui64GainUserTitle, 28 ) )	return TRUE;		break;		// GH5007 ( Combat Machete )
	//case 400003:		if( GET_USERTITLE( m_ui64GainUserTitle, 42 ) ) 	return TRUE;		break;		// Dual K-413
	//case 300008:		if( GET_USERTITLE( m_ui64GainUserTitle, 42 ) ) 	return TRUE;		break;		// Brass Knuckle

	default:																				return TRUE;
	}
	return FALSE;
	
#endif
	*/

}

void CUserSession::_EquipmentInBattle( UINT32 ui32ItemID )
{
	if( (SERVER_VALUE_UNKNOWN == m_i32RoomIdx) || (GAME_TASK_BATTLE != m_i32MainTask) )
	{
		// 배틀중이 아닐때 이 메시지를 받으면 자릅니다.
		_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, PROTOCOL_BATTLE_SLOT_EQUIPMENT_REQ, KILL_REASON_HACK_PACKET, "_EquipmentInBattle - No Battle" );
		return;
	}

	UINT16	ui16ChangeFlag = 0;
	switch( GET_ITEM_FLAG_CLASS( ui32ItemID ) )
	{
		case ITEM_CLASS_ASSAULT:		case ITEM_CLASS_SMG:			case ITEM_CLASS_SNIPER:
		case ITEM_CLASS_SHOTGUN:		case ITEM_CLASS_MACHINE:
			m_aui32UseWeaponList[ WEAPON_USAGE_PRIMARY ]		= ui32ItemID;
			ui16ChangeFlag |= EQUIPMENT_IN_BATTLE_PRIMARY;
			//m_UserInfoEquipmentWeapon._noprim	= ui32ItemID;
			break;
		case ITEM_CLASS_HANDGUN:
			m_aui32UseWeaponList[ WEAPON_USAGE_SECONDARY ]	= ui32ItemID;
			ui16ChangeFlag |= EQUIPMENT_IN_BATTLE_SECONDARY;
			//m_UserInfoEquipmentWeapon._nosub	= ui32ItemID;
			break;
		case ITEM_CLASS_KNIFE:
			m_aui32UseWeaponList[ WEAPON_USAGE_MELEE ]		= ui32ItemID;
			ui16ChangeFlag |= EQUIPMENT_IN_BATTLE_MELEE;
			//m_UserInfoEquipmentWeapon._nomelee	= ui32ItemID;
			break;
		case ITEM_CLASS_GRENADE:
			m_aui32UseWeaponList[ WEAPON_USAGE_THROWING ]	= ui32ItemID;
			ui16ChangeFlag |= EQUIPMENT_IN_BATTLE_GRENADE;
			//m_UserInfoEquipmentWeapon._nothrow	= ui32ItemID;
			break;
		case ITEM_CLASS_THROWING:
			m_aui32UseWeaponList[ WEAPON_USAGE_ITEM ]		= ui32ItemID;
			ui16ChangeFlag |= EQUIPMENT_IN_BATTLE_THROWING;
			//m_UserInfoEquipmentWeapon._noitem	= ui32ItemID;
			break;
		case ITEM_CLASS_CHARA:
			ui16ChangeFlag |= EQUIPMENT_IN_BATTLE_CHAR;
			break;
		case ITEM_CLASS_HEAD:
			//m_UserInfoEquipmentChara.SetHairNum( ui32ItemID );
			ui16ChangeFlag |= EQUIPMENT_IN_BATTLE_HEAD;
			break;
		case ITEM_CLASS_MAINTENANCE:
		case ITEM_CLASS_EXPENDABLE:
			m_pRoom->InsertSlotItem( this, m_pRoom->GetSlotInfo(m_i32SlotIdx) );
			ui16ChangeFlag |= EQUIPMENT_IN_BATTLE_ITEM;
			break;
	}

	bool bError = false;
	switch( GET_ITEM_FLAG_CLASS( ui32ItemID ) )
	{
		case ITEM_CLASS_ASSAULT:		case ITEM_CLASS_SMG:			case ITEM_CLASS_SNIPER:
		case ITEM_CLASS_SHOTGUN:		case ITEM_CLASS_MACHINE:		case ITEM_CLASS_HANDGUN:
		case ITEM_CLASS_KNIFE:			case ITEM_CLASS_GRENADE:		case ITEM_CLASS_THROWING:
			if( FALSE == _CheckWeaponEquip( TRUE, m_pRoom->GetInfo()->_WeaponFlag, m_aui32UseWeaponList, g_pContextMain->GetInvenTime() ) )
			{
				bError = true;
			}
			break;
		case ITEM_CLASS_CHARA:			case ITEM_CLASS_HEAD:
			if( EV_FAILED( _CheckCharEquip( TRUE, (m_i32SlotIdx % TEAM_COUNT), &m_UseCharaList, g_pContextMain->GetInvenTime() ) ) )
			{
				bError = true;
			}
			break;
		case ITEM_CLASS_MAINTENANCE:
			break;
	}
	if( bError )
	{	// 이상한 아이템으로 장착을 시도 했습니다.
		_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, PROTOCOL_BATTLE_SLOT_EQUIPMENT_REQ, KILL_REASON_HACK_PACKET, "_EquipmentInBattle - Not Use Item" );
		return;
	}
	// 무기일 경우 내구도 처리
	m_pRoom->SendSlotEquipmentInfo( m_WorkThreadIdx, m_i32SlotIdx, ui16ChangeFlag );
}

BOOL CUserSession::_CheckWeaponEquip( BOOL bUsedItem, UINT8 ui8WeaponFlag, UINT32* pui32WeaponList, UINT32 ui32InvenTime )
{
	CShop* pShop = g_pContextMain->GetShop();

	// 무기 내구도
	m_aui8UseWeaponEndurance[ WEAPON_USAGE_PRIMARY ]	= 0;
	m_aui8UseWeaponEndurance[ WEAPON_USAGE_SECONDARY ]	= 0;
	m_aui8UseWeaponEndurance[ WEAPON_USAGE_MELEE ]		= 0;
	m_aui8UseWeaponEndurance[ WEAPON_USAGE_THROWING ]	= 1;
	m_aui8UseWeaponEndurance[ WEAPON_USAGE_ITEM ]		= 1;
	m_aui8UseWeaponEndurance[ WEAPON_USAGE_MISSION ]	= 1;

	INT32 i32WeaponStart	= WEAPON_USAGE_PRIMARY;
	INT8  bCheckSkipFlag	= 0;

	// 무기별 게임 모드(돌격전 모드, 저격전 모드, 샷건전 모드, RPG전 모드) 실행시 주무기 장착이 안된 경우, 클라이언트에서 강제로 입력해주는 무기가 올바른지 검사.
	CRoom * pRoom = g_pRoomManager->GetRoom( m_i32ChannelNum, m_i32RoomIdx );
	if( NULL == pRoom )	return FALSE;

	bool bCheckPrimary = false;
	switch( pui32WeaponList[ WEAPON_USAGE_PRIMARY ] )
	{
#ifdef DEF_WEAPON_NOT_CHECK_USAGE_PRIMARY
	case 0:
#endif
	case 100003047:				//MAKE_ITEM_FLAG(ITEM_CLASS_ASSERT,WEAPON_USAGE_PRIMARY,WEAPON_CLASS_ASSAULT,WEAPON_NUM_ASSAULT_K2_IN_MODE):	
	case 300005025:				//MAKE_ITEM_FLAG(ITEM_CLASS_SNIPER,WEAPON_USAGE_PRIMARY,WEAPON_CLASS_SNIPER,WEAPON_NUM_SNIPER_SSG69_IN_MODE):
	case 400006007:				//MAKE_ITEM_FLAG(ITEM_CLASS_SHOTGUN,WEAPON_USAGE_PRIMARY,WEAPON_CLASS_SHOTGUN,WEAPON_NUM_SHOTGUN_870MCS_IN_MODE):
		{
			bCheckPrimary = true;
		}
		break;
	case 1600016002:			//MAKE_ITEM_FLAG(ITEM_CLASS_TEMP, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_ROCKET_LAUNCHER, WEAPON_NUM_ROCKET_LAUNCHER_RPG7):
		if( 0 != GET_ROOM_INFO_WEAPON_RPG7( pRoom->m_Info._InfoBasic._WeaponFlag ))
		{
			bCheckPrimary = true;
		}
		break;
	case 100003045:				// TUTORIAL (M4 SR-16 F.C)
	case 100003058:				// TUTORIAL (AK-47 F.C)
		if( GET_STAGETYPE( pRoom->m_Info._InfoBasic._StageID ) == STAGE_TYPE_TUTORIAL )
		{
			bCheckPrimary = true;
		}
		break;
	default:
		break;
	}

	if( GET_ROOM_INFO_WEAPON_KNUCKLE( pRoom->m_Info._InfoBasic._WeaponFlag ) > 0 )
	{
		bCheckPrimary = true;	// 너클전에서는 주무기 사용하지 않으므로 체크 스킵
		switch( pui32WeaponList[ WEAPON_USAGE_MELEE ] )
		{
		case 702023001:				// Fist
			bCheckSkipFlag |= 0x01 << WEAPON_USAGE_MELEE;
			m_aEnduranceUsedWeapon[ WEAPON_USAGE_MELEE ].UseStop();
			m_aui8UseWeaponEndurance[ WEAPON_USAGE_MELEE ] = 100;
			break;
		default:
			break;
		}
	}

	if( bCheckPrimary )
	{
		bCheckSkipFlag |= 0x01 << WEAPON_USAGE_PRIMARY;
		m_aEnduranceUsedWeapon[ WEAPON_USAGE_PRIMARY ].UseStop();
		m_aui8UseWeaponEndurance[ WEAPON_USAGE_PRIMARY ] = 100;
	}

	INT32 i32WeaponUsageCount = WEAPON_USAGE_MISSION;	//WEAPON_USAGE_COUNT-1;	// WEAPON_USAGE_MISSION 는 미션용 아이템이기 때문에 빼고 계산합니다.
	
	for( INT32 i = i32WeaponStart ; i < i32WeaponUsageCount ; i++ )
	{
		// 이미 위에서 계산된 무기는 더이상 계산하지 않습니다.
		if( bCheckSkipFlag & (0x01<<i) )			continue;

		// 인벤에 없는 무기입니다.
		INVEN_BUFFER* pInvenItem = GetItemByItemID( pui32WeaponList[ i ] );
		if( NULL == pInvenItem )
		{
#ifdef I3_DEBUG
			char strTemp[ MAX_PATH ];
			i3String::Format( strTemp, MAX_PATH, "Error _CheckWeaponEquip() - Don't have this Item in inventory, ItemID : %d", pui32WeaponList[ i ] );
			g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp );
#endif
			return FALSE;
		}
		// 슬롯에 맞지 않는 무기를 장착하고 있습니다.
		if( FALSE == _CheckItemEquipment( pInvenItem->_ui32ItemID, (EQMIPEMENT_SLOT)i ) )
		{
#ifdef I3_DEBUG
			char strTemp[ MAX_PATH ];
			i3String::Format( strTemp, MAX_PATH, "Error _CheckWeaponEquip() - Wrong slot, ItemID : %d, Slot : %d", pInvenItem->_ui32ItemID, i );
			g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp );
#endif
			return FALSE;
		}

		bCheckSkipFlag						|= 0x01 << i;
		m_aEnduranceUsedWeapon[i].UseStop();

		if( ITEM_ATTR_TYPE_AUTH < pInvenItem->_ui8ItemType )
		{	// 밀어넣기 아이템일 경우 무사통과
			m_aui8UseWeaponEndurance[ i ] = 100;
		}
		else
		{
			SHOP_ITEM_BASE*	pItem = pShop->GetItemData( pInvenItem->_ui32ItemID );
			// 오류 체크를 위해 추가
			if( pItem == NULL )
			{
				char strLog[256];
				i3String::Format( strLog, 256, "User use Item (%d) but Item is not in Shop...... ", pInvenItem->_ui32ItemID );
				g_pModuleLogFile->Write_M_Log(m_WorkThreadIdx, this, strLog ) ; 
				return FALSE;
			}

			switch( pItem->_ui8AuthType )
			{
			case ITEM_TYPE_COUNT:
				{
					if( 0 == pInvenItem->_ui32ItemArg )	break;
					if( ( ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType ) && g_pContextMain->m_bIsNewRepair )
					{	// 내구도 시스템 이면서 새로운 수리 시스템이면 내구도 % 를 포함 해 줍니다.
						SHOP_ITEM_DATA*	pItemData = pShop->GetItemDataEx( pInvenItem->_ui32ItemID );
						if( 0 < pItemData->_ui32Durability )
						{
							m_aui8UseWeaponEndurance[ i ] = (pInvenItem->_ui32ItemArg*100) / pItemData->_ui32Durability;
							if(0 == m_aui8UseWeaponEndurance[ i ])	{ m_aui8UseWeaponEndurance[ i ] = 1; }
						}
						else
						{
							m_aui8UseWeaponEndurance[ i ] = 0;
						}						
					}
					else
					{
						m_aui8UseWeaponEndurance[ i ] = 100;
					}
					if( FALSE == bUsedItem )			break;

					// 공룡이 사용되는 모드일경우 공룡이면 무기를 소모하지 않음
					if( m_pRoom->m_Info._InfoBasic._State == ROOM_STATE_BATTLE &&
						( m_pRoom->m_Info._i32StageType == STAGE_TYPE_ESCAPE ||	m_pRoom->m_Info._i32StageType == STAGE_TYPE_CROSSCOUNT ) )
					{
						if( m_pRoom->m_Info._MiDinoInfo.IsDinoIdx(m_i32SlotIdx) == true )
						{
							continue;
						}
					}

					// ui8WeaponFlag 는 방에서 활성화 된 무기입니다. ( 0x01111111, 0, 샷건, 저격, RPG7, 주, 보조, 칼, 투척 )
					UINT8 ui8ItemFlag = 0;
					switch( i )
					{
					case WEAPON_USAGE_ITEM:								// 주, 보조, 칼, 투척 순이기때문에 칼 다음에 수류탄이 없습니다. 예외처리입니다.
						ui8ItemFlag = WEAPON_USAGE_ITEM;
						break;
					default:
						ui8ItemFlag = WEAPON_USAGE_THROWING;
						break;
					}

					// pSession->m_ui32UsedWeaponTimeList[i]	= g_pContextMain->GetServerTime();
					if( ui8WeaponFlag & (0x1 << (ui8ItemFlag-i)) )
					{
						UINT8 ui8Type = USED_ITEM_TYPE_NONE;
						switch( pItem->_ui8AuthExpireType )
						{
						case ITEM_EXPIRE_LEAVE:
							{
								if( g_pContextMain->m_bIsNewRepair )
								{
									ui8Type		= USED_ITEM_TYPE_ENDURANCE;
								}
								else
								{
									ui8Type		= USED_ITEM_TYPE_ONE;
								}
							}
							break;
						case ITEM_EXPIRE_DELETE:
							{
								ui8Type			= USED_ITEM_TYPE_ONE;
							}
							break;
						}
						m_aEnduranceUsedWeapon[i].UseStart(ui8Type, pInvenItem->_i64ItemWareDBIdx, g_pContextMain->GetServerTime());
						//_SetUsedItem( pInvenItem->_i64ItemWareDBIdx, pInvenItem->_ui32ItemID );
					}
				}
				break;
			case ITEM_TYPE_TIME:
				{
					if( ITEM_ATTR_TYPE_AUTH != pInvenItem->_ui8ItemType )
					{
#ifdef I3_DEBUG
						char strTemp[ MAX_PATH ];
						i3String::Format( strTemp, MAX_PATH, "Error _CheckWeaponEquip() - It's not auth item, ItemID : %d, AuthType : %d",
							pInvenItem->_ui32ItemID, pInvenItem->_ui8ItemType );
						g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp );
#endif
						return FALSE;
					}

					// 인증 시간 확인(인증 기준 시간이 0이면 현재시간으로 확인)
					if( ui32InvenTime > 0 )
					{
						if( ui32InvenTime > pInvenItem->_ui32ItemArg ) break;
					}
					else
					{
						if( g_pContextMain->GetInvenTime() > pInvenItem->_ui32ItemArg ) break;
					}
					m_aui8UseWeaponEndurance[ i ] = 100;
				}
				break;
			}
		}
	}

	if( bCheckSkipFlag != 0x1F )
	{
#ifdef I3_DEBUG
		g_pModuleLogFile->Write_M_Log(m_WorkThreadIdx, this,  "Error 1");
#endif
		return FALSE;
	}

	return TRUE;
}

INT32 CUserSession::_CheckCharEquip( BOOL bUsedItem, UINT32 ui32Team, USER_INFO_EQUIPMENT_CHARA* pChara, UINT32 ui32InvenTime )
{
	CShop* pShop = g_pContextMain->GetShop();
	// 무기 내구도

	UINT32 ui32EqChara = pChara->GetCharaID( (TEAM_TYPE)ui32Team );
	EQMIPEMENT_SLOT	eEquipmentSlot;
	
	INVEN_BUFFER* pInvenItem = NULL;;
	INT32 i32Count = 0;
	// Item 은 없을 수도 있습니다.
	if( 0 == pChara->GetItemNum() ) i32Count= 1;
	
	for( INT32 i = 0 ; i < m_i32InvCharaCount; i++ )  
	{
		pInvenItem = NULL;
		if( m_aInvChara[i]._ui32ItemArg == 0 ) continue;

		if( m_aInvChara[i]._ui32ItemID == ui32EqChara )
		{
			switch( ui32Team )
			{
			case TEAM_RED:
				m_aEnduranceUsedChara[EQMIPEMENT_SLOT_CHAR_RED - (WEAPON_USAGE_COUNT-1)].UseStop();
				eEquipmentSlot	= EQMIPEMENT_SLOT_CHAR_RED;
				break;
			case TEAM_BLUE:
				m_aEnduranceUsedChara[EQMIPEMENT_SLOT_CHAR_BLUE - (WEAPON_USAGE_COUNT-1)].UseStop();
				eEquipmentSlot	= EQMIPEMENT_SLOT_CHAR_BLUE;
				break;
			}

			pInvenItem = &m_aInvChara[i];
		}
		else if( m_aInvChara[i]._ui32ItemID == pChara->GetHairNum() )
		{
			m_aEnduranceUsedChara[EQMIPEMENT_SLOT_CHAR_HEAD - (WEAPON_USAGE_COUNT-1)].UseStop();
			pInvenItem = &m_aInvChara[i];
			eEquipmentSlot	= EQMIPEMENT_SLOT_CHAR_HEAD;
		}
		else if( m_aInvChara[i]._ui32ItemID == pChara->GetItemNum() )
		{
			m_aEnduranceUsedChara[EQMIPEMENT_SLOT_CHAR_ITEM - (WEAPON_USAGE_COUNT-1)].UseStop();
			pInvenItem = &m_aInvChara[i];
			eEquipmentSlot	= EQMIPEMENT_SLOT_CHAR_ITEM;
		}
		else if( m_aInvChara[i]._ui32ItemID == pChara->GetDinoNum() )
		{
			m_aEnduranceUsedChara[EQMIPEMENT_SLOT_CHAR_DINO - (WEAPON_USAGE_COUNT-1)].UseStop();

			if( m_pRoom->m_Info._i32StageType == STAGE_TYPE_CROSSCOUNT )
			{
				char szLog[MAX_STRING_COUNT];
				switch( pChara->GetDinoNum() )
				{
				// 기존 공룡들은
				case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,		CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_Raptor):
				case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,		CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_STING):
				case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,		CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_TANK):
				case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,		CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_ACID):
				case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,		CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_ELITE):
					
					/*
					i3String::Format(szLog, "[ERROR][CUserSession::_CheckCharEquip] Can't use in CC Mode. (ItemID: %d)", pInvenItem->_ui32ItemID);
					g_pModuleLogFile->Write_M_Log(m_WorkThreadIdx, this,  szLog);
					*/

					return -1;
				}
			}

			pInvenItem = &m_aInvChara[i];
			eEquipmentSlot	= EQMIPEMENT_SLOT_CHAR_DINO;
		}

		if( NULL == pInvenItem ) continue;

		// 슬롯에 맞지 않는 무기를 장착하고 있습니다.
		if( FALSE == _CheckItemEquipment( pInvenItem->_ui32ItemID, eEquipmentSlot ))
		{
			return -2;
		}
		
		// 공룡 사용되는 모드일 경우 공룡일때와 아닐때 체크 예외 대상 있음
		if( m_pRoom->m_Info._InfoBasic._State == ROOM_STATE_BATTLE &&
			( m_pRoom->m_Info._i32StageType == STAGE_TYPE_ESCAPE || m_pRoom->m_Info._i32StageType == STAGE_TYPE_CROSSCOUNT ) )
		{
			if( m_pRoom->m_Info._MiDinoInfo.IsDinoIdx(m_i32SlotIdx) == true )
			{
				// 공룡만 체크
				if( eEquipmentSlot != EQMIPEMENT_SLOT_CHAR_DINO )
				{
					i32Count++;
					continue;
				}
			}
			else
			{
				// 공룡은 제외
				if( eEquipmentSlot == EQMIPEMENT_SLOT_CHAR_DINO )
				{
					i32Count++;
					continue;
				}
			}
		}
		else if( m_pRoom->m_Info._InfoBasic._State == ROOM_STATE_BATTLE )
		{
			// 공룡은 제외
			if( eEquipmentSlot == EQMIPEMENT_SLOT_CHAR_DINO )
			{
				i32Count++;
				continue;
			}
		}

		if( ITEM_ATTR_TYPE_AUTH < pInvenItem->_ui8ItemType )
		{
			// 따로 체크할게 없습니다. 무기는 내구도를 체크합니다. 혹시 나중에 뭔가를 체크할지 몰라 비워둡니다. // 20100910 김현우
		}
		else
		{
			// 밀어넣기 아이템일 경우 무사통과
			SHOP_ITEM_BASE*	pItem = pShop->GetItemData( pInvenItem->_ui32ItemID );

			// 오류 체크를 위해 추가
			if( pItem == NULL )
			{
				continue;
			}

			switch( pItem->_ui8AuthType )
			{
			case ITEM_TYPE_COUNT:
				{
					if( FALSE == bUsedItem )			break;

					UINT8 ui8Type = USED_ITEM_TYPE_NONE;
					INT8 i8UsedCharaSlot = eEquipmentSlot - (WEAPON_USAGE_COUNT-1);
					switch( pItem->_ui8AuthExpireType )
					{
					case ITEM_EXPIRE_LEAVE:
						{
							if( g_pContextMain->m_bIsNewRepair )
							{
								ui8Type		= USED_ITEM_TYPE_ENDURANCE;
							}
							else
							{
								ui8Type		= USED_ITEM_TYPE_ONE;
							}
						}
						break;
					case ITEM_EXPIRE_DELETE:
						{
							ui8Type			= USED_ITEM_TYPE_ONE;
						}
						break;
					}
					m_aEnduranceUsedChara[i8UsedCharaSlot].UseStart(ui8Type, pInvenItem->_i64ItemWareDBIdx, g_pContextMain->GetServerTime());
					//_SetUsedItem( pInvenItem->_i64ItemWareDBIdx, pInvenItem->_ui32ItemID, USED_ITEM_TYPE_ONE, 1 );
				}
				break;
			case ITEM_TYPE_TIME:
				{
					if( ITEM_ATTR_TYPE_AUTH != pInvenItem->_ui8ItemType )
					{

						return -3;
					}
				}
				break;
			}
		}
		i32Count++;
		
		// 모든 아이템을 체크했는지 확인
		if( CHECK_CHARA_EQUIP == i32Count )	return EVENT_ERROR_SUCCESS;
	}

	// 공룡또는 인간 캐릭터 한가지만 확인하고 루프를 끝까지 안도는 경우가 있기 때문에 루프 밖에서 체크한 아이템 개수를 다시 한번 확인
	if( CHECK_CHARA_EQUIP == i32Count )	return EVENT_ERROR_SUCCESS;

#ifdef I3_DEBUG
	char strTemp[ MAX_PATH ];
	i3String::Format( strTemp, MAX_PATH, "[_CheckCharEquip] bUsedItem:%s, ui32Team:%u, Red:%u, Blue:%u, Dino:%u, Hair:%u, Item:%u, ui32InvenTime:%u",
		bUsedItem ? "true" : "false",
		ui32Team,
		pChara->GetRedNum(),
		pChara->GetBlueNum(),
		pChara->GetDinoNum(),
		pChara->GetHairNum(),
		pChara->GetItemNum(),
		ui32InvenTime
	);
	g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp );


	for( INT32 i = 0 ; i < m_i32InvCharaCount; i++ )  
	{
		i3String::Format( strTemp, MAX_PATH, "[_CheckCharEquip] {%02d} ItemID:%u, ItemArg:%u, ItemType:%u, DBIdx:%I64d",
			i,
			m_aInvChara[i]._ui32ItemID,
			m_aInvChara[i]._ui32ItemArg,
			m_aInvChara[i]._ui8ItemType,
			m_aInvChara[i]._i64ItemWareDBIdx
		);
		g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp );
	}
#endif

	return -4;
}

void CUserSession::_SetUsedItem( TUID i64ItemWareDBIdx, UINT32 ui32ItemID, UINT8 ui8Type, UINT32 ui32UsedDurablity, UINT8 ui8CheckSlotInfo )
{
	BOOL bUsed = FALSE;
	UINT16 ui16Idx = m_ui16UsedItemCount;
	for( UINT16 i = 0 ; i < m_ui16UsedItemCount ; i++ ) 
	{
		if( m_aUsedItemList[ i ].m_i64UsedItemID == i64ItemWareDBIdx )
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
				if( GET_USED_CHECK_SLOT(ui8CheckSlotInfo) >= WEAPON_USAGE_MISSION ) break;
				m_aui8WeaponUseCount[ GET_USED_CHECK_SLOT(ui8CheckSlotInfo) ]++;
				break;
			case USED_CHECK_CHARA:
				m_aui8CharaUseCount[ GET_USED_CHECK_SLOT(ui8CheckSlotInfo) - (WEAPON_USAGE_COUNT-1) ]++;
				break;
			}
		}
		else
		{
			// 더 이상 추가 할 수 없습니다. 로그 박고 리턴
			return;
		}
	}
	
	m_aUsedItemList[ ui16Idx ].m_i64UsedItemID			= i64ItemWareDBIdx;
	m_aUsedItemList[ ui16Idx ].m_ui32ItemID				= ui32ItemID;
	m_aUsedItemList[ ui16Idx ].m_ui8Type				= ui8Type;
	m_aUsedItemList[ ui16Idx ].m_ui32UsedDurablity		+= ui32UsedDurablity;
	m_aUsedItemList[ ui16Idx ].m_ui8CheckSlotInfo		= ui8CheckSlotInfo;
}

//장착된 총기, 캐릭터가 유효한지를 검사합니다. 
//0x00000010	//Equip Character
//0x00000020	//Equip Weapon
INT32 CUserSession::_CheckEquipment(void)
{
	INT32	i32Rv = 0; 
	UINT32	ui32CheckList; 
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//장착된 캐릭터를 검사합니다.
	if( FALSE == _CheckEquipmentChara( &m_UserInfoEquipmentChara, &ui32CheckList ) )
	{	// 바뀐 아이템이 있다면.
		if( GENDER_FEMALE == m_ui8Gender )
		{
			if((ui32CheckList & 0x01) != 0x01)	m_UserInfoEquipmentChara.SetRedNum( g_pContextMain->m_ui32BasicEquipCharaRed_F );
			if((ui32CheckList & 0x02) != 0x02)	m_UserInfoEquipmentChara.SetBlueNum( g_pContextMain->m_ui32BasicEquipCharaBlue_F );
		}
		else
		{
			if((ui32CheckList & 0x01) != 0x01)	m_UserInfoEquipmentChara.SetRedNum( g_pContextMain->m_ui32BasicEquipCharaRed );
			if((ui32CheckList & 0x02) != 0x02)	m_UserInfoEquipmentChara.SetBlueNum( g_pContextMain->m_ui32BasicEquipCharaBlue );
		}

		if((ui32CheckList & 0x04) != 0x04)		m_UserInfoEquipmentChara.SetHairNum( g_pContextMain->m_ui32BasicEquipCharaHelmel );
		if((ui32CheckList & 0x08) != 0x08)		m_UserInfoEquipmentChara.SetItemNum( 0 );

		if((ui32CheckList & 0x10) != 0x10)		m_UserInfoEquipmentChara.SetDinoNum( g_pContextMain->m_ui32BasicEquipCharaDino );
		
		ChangeDataBaseInfo( SAVE_DATABASE_EQUIPMENT );

		i32Rv |= INVENTORY_FLAG_CHARACTER;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//장착된 무기들을 검사합니다.
	 if( FALSE == _CheckEquipmentWeapon( &m_UserInfoEquipmentWeapon, &ui32CheckList ) )
	 {
		if((ui32CheckList & 0x01) != 0x01)		m_UserInfoEquipmentWeapon._noprim	= g_pContextMain->m_ui32BasicEquipWeaponPrim;
		if((ui32CheckList & 0x02) != 0x02)		m_UserInfoEquipmentWeapon._nosub	= g_pContextMain->m_ui32BasicEquipWeaponSub; 
		if((ui32CheckList & 0x04) != 0x04)		m_UserInfoEquipmentWeapon._nothrow	= g_pContextMain->m_ui32BasicEquipWeaponThrow; 
		if((ui32CheckList & 0x08) != 0x08)		m_UserInfoEquipmentWeapon._nomelee	= g_pContextMain->m_ui32BasicEquipWeaponMelee;
		if((ui32CheckList & 0x10) != 0x10)		m_UserInfoEquipmentWeapon._noitem	= g_pContextMain->m_ui32BasicEquipWeaponItem; 

		ChangeDataBaseInfo( SAVE_DATABASE_EQUIPMENT );

		i32Rv |= INVENTORY_FLAG_WEAPON; 
	 }

	return i32Rv; 
}

BOOL CUserSession::_CheckEquipmentChara( USER_INFO_EQUIPMENT_CHARA * pEquipment, UINT32* pui32Check )
{
	*pui32Check = 0;

	if( 0 == pEquipment->GetItemNum() )				*pui32Check |= 0x08;	// Item 은 착용 안할 수도 있습니다.
//	if( 0 == pEquipment->_noDino )				*pui32Check != 0x10;	// Dino 는 착용 안할 수도 있습니다.
	// 장착아이템과 인벤토리의 아이템을 검사
	for(INT32 i = 0; i < m_i32InvCharaCount; i++)
	{
		if( pEquipment->GetRedNum()  == m_aInvChara[i]._ui32ItemID )
		{
			if( _CheckItemAuth( &m_aInvChara[i], FALSE ) && _CheckItemEquipment( m_aInvChara[i]._ui32ItemID, EQMIPEMENT_SLOT_CHAR_RED ) )		*pui32Check |= 0x01;
		}
		if( pEquipment->GetBlueNum() == m_aInvChara[i]._ui32ItemID )
		{
			if( _CheckItemAuth( &m_aInvChara[i], FALSE ) && _CheckItemEquipment( m_aInvChara[i]._ui32ItemID, EQMIPEMENT_SLOT_CHAR_BLUE ) )		*pui32Check |= 0x02;
		}
		if( pEquipment->GetHairNum() == m_aInvChara[i]._ui32ItemID )
		{
			if( _CheckItemAuth( &m_aInvChara[i], FALSE ) && _CheckItemEquipment( m_aInvChara[i]._ui32ItemID, EQMIPEMENT_SLOT_CHAR_HEAD ) )		*pui32Check |= 0x04;
		}
		if( pEquipment->GetItemNum() == m_aInvChara[i]._ui32ItemID )
		{
			if( _CheckItemAuth( &m_aInvChara[i], FALSE ) && _CheckItemEquipment( m_aInvChara[i]._ui32ItemID, EQMIPEMENT_SLOT_CHAR_ITEM ) )		*pui32Check |= 0x08;
		}
		if( pEquipment->GetDinoNum() == m_aInvChara[i]._ui32ItemID )
		{
			if( _CheckItemAuth( &m_aInvChara[i], FALSE ) && _CheckItemEquipment( m_aInvChara[i]._ui32ItemID, EQMIPEMENT_SLOT_CHAR_DINO ) )		*pui32Check |= 0x10;
		}
	}
	if( 0x1F == *pui32Check )	return TRUE;

	return FALSE;
}

BOOL CUserSession::_CheckEquipmentWeapon( USER_INFO_EQUIPMENT_WEAPON * pEquipment, UINT32* pui32Check ) 
{
	*pui32Check = 0;

#ifdef DEF_WEAPON_NOT_CHECK_USAGE_PRIMARY
	// 주무기 없이 가는 버전, 주무기가 0일때 주무기 세팅 없어도 True 입니다.
	if( 0 == pEquipment->_noprim )				*pui32Check |= 0x01;
#endif
	// 장착아이템과 인벤토리의 아이템을 검사
	for(INT32 i = 0; i < m_i32InvWeaponCount; i++)
	{
		if(pEquipment->_noprim	== m_aInvWeapon[i]._ui32ItemID)
		{			
			if( _CheckItemAuth( &m_aInvWeapon[i] ) && _CheckItemEquipment( m_aInvWeapon[i]._ui32ItemID, EQMIPEMENT_SLOT_PRIM )  )		*pui32Check |= 0x01;
		}
		if(pEquipment->_nosub	== m_aInvWeapon[i]._ui32ItemID)
		{
			if( _CheckItemAuth( &m_aInvWeapon[i] ) && _CheckItemEquipment( m_aInvWeapon[i]._ui32ItemID, EQMIPEMENT_SLOT_SUB ) )			*pui32Check |= 0x02;
		}
		if(pEquipment->_nothrow	== m_aInvWeapon[i]._ui32ItemID)
		{
			if( _CheckItemAuth( &m_aInvWeapon[i] ) && _CheckItemEquipment( m_aInvWeapon[i]._ui32ItemID, EQMIPEMENT_SLOT_THROWING ) )	*pui32Check |= 0x04;
		}
		if(pEquipment->_nomelee	== m_aInvWeapon[i]._ui32ItemID)
		{
			if( _CheckItemAuth( &m_aInvWeapon[i] ) && _CheckItemEquipment( m_aInvWeapon[i]._ui32ItemID, EQMIPEMENT_SLOT_MELEE ) )		*pui32Check |= 0x08;
		}
		if(pEquipment->_noitem	== m_aInvWeapon[i]._ui32ItemID)
		{
			if( _CheckItemAuth( &m_aInvWeapon[i] ) && _CheckItemEquipment( m_aInvWeapon[i]._ui32ItemID, EQMIPEMENT_SLOT_ITEM ) )		*pui32Check |= 0x10;
		}
	}
	if( 0x1F == *pui32Check )	return TRUE;
	return FALSE;
}

BOOL CUserSession::_CheckItemAuth( INVEN_BUFFER* pInvenItem, BOOL bEnduranceItemCheckPass )
{
	if( ITEM_ATTR_TYPE_BASIC <= pInvenItem->_ui8ItemType )					return TRUE;

	CShop* pShop = g_pContextMain->GetShop();

	SHOP_ITEM_BASE*	pItem = pShop->GetItemData( pInvenItem->_ui32ItemID );  
	if( NULL == pItem )														return FALSE;

	if( bEnduranceItemCheckPass == TRUE && ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType )					return TRUE;

	switch( pItem->_ui8AuthType )
	{
	case ITEM_TYPE_COUNT:
		{
			if( 0 == pInvenItem->_ui32ItemArg )								return FALSE;
		}
		break;
	case ITEM_TYPE_TIME:
		{
			if( ITEM_ATTR_TYPE_AUTH != pInvenItem->_ui8ItemType )			return FALSE;
			if( g_pContextMain->GetInvenTime() > pInvenItem->_ui32ItemArg ) return FALSE;
		}
		break;
	}

	return TRUE;
}

BOOL CUserSession::_CheckItemEquipment( UINT32 ui32Item, EQMIPEMENT_SLOT eSlot )
{
	if( 0 == ui32Item ) return FALSE;

	switch( eSlot )
	{
	case EQMIPEMENT_SLOT_PRIM:
		{
			// CLASS Check
			switch( GET_ITEM_FLAG_CLASS( ui32Item ) )
			{
			case ITEM_CLASS_ASSAULT:		case ITEM_CLASS_SMG:			case ITEM_CLASS_SNIPER:
			case ITEM_CLASS_SHOTGUN:		case ITEM_CLASS_MACHINE:
				break;				// 정상
			default:
				return FALSE;		// 비정상
			}
			// Usage Check
			switch( GET_ITEM_FLAG_USAGE( ui32Item ) )
			{
			case WEAPON_USAGE_PRIMARY:
				break;
			default:
				return FALSE;
			}
		}
		break;
	case EQMIPEMENT_SLOT_SUB:
		{
			// CLASS Check
			switch( GET_ITEM_FLAG_CLASS( ui32Item ) )
			{
			case ITEM_CLASS_HANDGUN:
				break;				// 정상
			default:
				return FALSE;		// 비정상
			}
			// Usage Check
			switch( GET_ITEM_FLAG_USAGE( ui32Item ) )
			{
			case WEAPON_USAGE_SECONDARY:
				break;
			default:
				return FALSE;
			}
		}
		break;
	case EQMIPEMENT_SLOT_MELEE:
		{
			// CLASS Check
			switch( GET_ITEM_FLAG_CLASS( ui32Item ) )
			{
			case ITEM_CLASS_KNIFE:
				break;				// 정상
			default:
				return FALSE;		// 비정상
			}
			// Usage Check
			switch( GET_ITEM_FLAG_USAGE( ui32Item ) )
			{
			case WEAPON_USAGE_MELEE:
				break;
			default:
				return FALSE;
			}
		}
		break;
	case EQMIPEMENT_SLOT_THROWING:
		{
			// CLASS Check
			switch( GET_ITEM_FLAG_CLASS( ui32Item ) )
			{
			case ITEM_CLASS_GRENADE:
				break;				// 정상
			default:
				return FALSE;		// 비정상
			}
			// Usage Check
			switch( GET_ITEM_FLAG_USAGE( ui32Item ) )
			{
			case WEAPON_USAGE_THROWING:
				break;
			default:
				return FALSE;
			}
		}
		break;
	case EQMIPEMENT_SLOT_ITEM:
		{
			// CLASS Check
			switch( GET_ITEM_FLAG_CLASS( ui32Item ) )
			{
			case ITEM_CLASS_THROWING:
				break;				// 정상
			default:
				return FALSE;		// 비정상
			}
			// Usage Check
			switch( GET_ITEM_FLAG_USAGE( ui32Item ) )
			{
			case WEAPON_USAGE_ITEM:
				break;
			default:
				return FALSE;
			}
		}
		break;
	case EQMIPEMENT_SLOT_CHAR_RED:
		{
			// CLASS Check
			switch( GET_ITEM_FLAG_CLASS( ui32Item ) )
			{
			case ITEM_CLASS_CHARA:
				break;				// 정상
			default:
				return FALSE;		// 비정상
			}
			// Usage Check
			switch( GET_ITEM_FLAG_USAGE( ui32Item ) )
			{
			case CHARACTER_CLASS_CHARA_TYPE:
				break;
			default:
				return FALSE;
			}
			// Type Check
			if( 0 == (CHARACTER_TEAM_RED & GET_ITEM_FLAG_TYPE( ui32Item ) ) )
			{
				return FALSE;
			}
		}
		break;
	case EQMIPEMENT_SLOT_CHAR_BLUE:
		{
			// CLASS Check
			switch( GET_ITEM_FLAG_CLASS( ui32Item ) )
			{
			case ITEM_CLASS_CHARA:
				break;				// 정상
			default:
				return FALSE;		// 비정상
			}
			// Usage Check
			switch( GET_ITEM_FLAG_USAGE( ui32Item ) )
			{
			case CHARACTER_CLASS_CHARA_TYPE:
				break;
			default:
				return FALSE;
			}
			// Type Check
			if( 0 == (CHARACTER_TEAM_BLUE & GET_ITEM_FLAG_TYPE( ui32Item ) ) )
			{
				return FALSE;
			}
		}
		break;
	case EQMIPEMENT_SLOT_CHAR_HEAD:
		{
			// CLASS Check
			switch( GET_ITEM_FLAG_CLASS( ui32Item ) )
			{
			case ITEM_CLASS_HEAD:
				break;				// 정상
			default:
				return FALSE;		// 비정상
			}
			// Usage Check
			switch( GET_ITEM_FLAG_USAGE( ui32Item ) )
			{
			case CHARACTER_CLASS_HEAD:
			case CHARACTER_CLASS_MASK:
			case CHARACTER_CLASS_EVENT:
				break;
			default:
				return FALSE;
			}
			// Type Check
			if( 0 == (CHARACTER_TEAM_RB & GET_ITEM_FLAG_TYPE( ui32Item ) ) )
			{
				return FALSE;
			}
		}
		break;
	case EQMIPEMENT_SLOT_CHAR_ITEM:
		{
			// CLASS Check
			switch( GET_ITEM_FLAG_CLASS( ui32Item ) )
			{
			case ITEM_CLASS_HEAD:
				break;				// 정상
			default:
				return FALSE;		// 비정상
			}
			// Usage Check
			switch( GET_ITEM_FLAG_USAGE( ui32Item ) )
			{
			case CHARACTER_CLASS_ITEM:
				break;
			default:
				return FALSE;
			}
			// Type Check
			if( 0 == (CHARACTER_TEAM_RB & GET_ITEM_FLAG_TYPE( ui32Item ) ) )
			{
				return FALSE;
			}
		}
		break;
	case EQMIPEMENT_SLOT_CHAR_DINO:
		{
			// CLASS Check
			switch( GET_ITEM_FLAG_CLASS( ui32Item ) )
			{
			case ITEM_CLASS_CHARA:
				break;				// 정상
			default:
				return FALSE;		// 비정상
			}
			// Usage Check
			switch( GET_ITEM_FLAG_USAGE( ui32Item ) )
			{
			case CHARACTER_CLASS_CHARA_DINO:
				break;
			default:
				return FALSE;
			}
			// Type Check
			if( 0 == (CHARACTER_TEAM_RB & GET_ITEM_FLAG_TYPE( ui32Item ) ) )
			{
				return FALSE;
			}
		}
		break;
	}

	return TRUE;
}

BOOL CUserSession::_ChangeUserTitle( UINT8 ui8UserTitleId, LOG_GET_TITLE* pstLogGetTitle )
{
	// 교환 조건 테이블
	const USER_TITLE_REQUIREMENT* pRequirement = &g_UserTitleRequirement[ui8UserTitleId];

	// 계급 낮으면 불가
	UINT32 ui32Rank = m_UserInfoBasic._rank;

	if( -1 != GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_RANK, CASHITEM_DAY_NO) ) )
	{
		ui32Rank = m_UserInfoInvItemData._wRank;
	}
	if( ui32Rank < pRequirement->_rank )
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
	for( INT32 i = 0 ; i < WEAPON_USAGE_COUNT ; i++ )
	{
		if( false == m_aEnduranceUsedWeapon[i].m_bUse )	continue;

		_SetUsedItem( m_aEnduranceUsedWeapon[i].m_TWareDBIdx, m_aui32UseWeaponList[i], m_aEnduranceUsedWeapon[i].m_ui8Type, g_pContextMain->GetServerTime() - m_aEnduranceUsedWeapon[i].m_ui32UsedTime, MAKE_USED_CHECK_SLOTINFO(USED_CHECK_WEAPON, i) );
		m_aEnduranceUsedWeapon[i].m_ui32UsedTime = g_pContextMain->GetServerTime();
	}

	for( INT32 i = 0; i < EQMIPEMENT_SLOT_COUNT - (WEAPON_USAGE_COUNT-1); i++ )
	{
		if( false == m_aEnduranceUsedChara[i].m_bUse )	continue;

		UINT32 ui32ItemID = 0;
		INT8 i8EquipSlot = i+(WEAPON_USAGE_COUNT-1);
		switch( i8EquipSlot )
		{
		case EQMIPEMENT_SLOT_CHAR_RED:
			ui32ItemID = m_UseCharaList.GetRedNum();	break;
		case EQMIPEMENT_SLOT_CHAR_BLUE:
			ui32ItemID = m_UseCharaList.GetBlueNum();	break;
		case EQMIPEMENT_SLOT_CHAR_HEAD:
			ui32ItemID = m_UseCharaList.GetHairNum();	break;
		case EQMIPEMENT_SLOT_CHAR_ITEM:
			ui32ItemID = m_UseCharaList.GetItemNum();	break;
		case EQMIPEMENT_SLOT_CHAR_DINO:
			ui32ItemID = m_UseCharaList.GetDinoNum();	break;
		default:
			continue;
		}

		_SetUsedItem( m_aEnduranceUsedChara[i].m_TWareDBIdx, ui32ItemID, m_aEnduranceUsedChara[i].m_ui8Type, g_pContextMain->GetServerTime() - m_aEnduranceUsedChara[i].m_ui32UsedTime, MAKE_USED_CHECK_SLOTINFO(USED_CHECK_CHARA, i + (WEAPON_USAGE_COUNT-1)) );
		m_aEnduranceUsedChara[i].m_ui32UsedTime = g_pContextMain->GetServerTime();
	}
}

void CUserSession::CharUseAllStop()
{
	CheckEndurance();

	for( INT32 i = 0; i < EQMIPEMENT_SLOT_COUNT - (WEAPON_USAGE_COUNT-1); i++ )
	{
		if( false == m_aEnduranceUsedChara[i].m_bUse )	continue;

		m_aEnduranceUsedChara[i].UseStop();
	}
}

float	CUserSession::_GetRepairPremium( UINT32 ui32Endurance )
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
		pQuest		= &g_pContextMain->m_aaQuestEvent[ m_ui8LocaleCode ][ ui8CardSet*MAX_QUEST_PER_CARD_NEW ];
		pItemInfo	= &g_pContextMain->m_aQuestEvent_ItemInfo[ m_ui8LocaleCode ].m_CardReward[ui8CardSet];
		pLastItemInfo = &g_pContextMain->m_aQuestEvent_ItemInfo[ m_ui8LocaleCode ].m_CompleteReward; // 최종 보상 정보 추가
		break;
	default:
		pQuest		= &g_pContextMain->m_aaQuest[ ui8CardType ][ ui8CardSet*MAX_QUEST_PER_CARD_NEW ];
		pItemInfo	= &g_pContextMain->m_aQuest_ItemInfo[ ui8CardType ].m_CardReward[ui8CardSet];
		pLastItemInfo = &g_pContextMain->m_aQuest_ItemInfo[ ui8CardType ].m_CompleteReward;
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
		UINT8 ui8Active = 0;
		switch( pQuest[i].m_ID )
		{
		case 1:
		default:
			{	// 현재 완료한 카운트
				if( pQuest[i].m_Arg[2] > (pQuestCtxActive[i] + m_aui8QuestCtxActiveNow[i]) )
				{
					if( m_aui8QuestCtxActiveNow[i] > 0 )
					{
						// 누적 되지 않는 퀘스트이므로 
						// 이번 배틀에서 임무 수행했던 것은 배틀 시작 전 수행 회수를 클라이언트에 다시 보내 초기화 한다.
						UINT8 ui8QuestCtxActiveIdx	= i;
						UINT8 ui8QuestCtxActive = pQuestCtxActive[i];

						i3NetworkPacket SendPacket( PROTOCOL_BASE_QUEST_CHANGE_ACK );
						SendPacket.WriteData( &ui8QuestCtxActiveIdx, sizeof(UINT8) );		// Finish : 0xF0, Index : 0x0F
						SendPacket.WriteData( &ui8QuestCtxActive, sizeof(UINT8) );			// Count
						SendPacketMessage( &SendPacket );
					}

					continue;
				}
				pQuestCtxActive[i] = pQuest[i].m_Arg[2];
				ChangeDataBaseInfo(SAVE_DATABASE_MEDAL);
			}
			break;
		case 2:
			{	// 누적 카운트 계산
				if( m_aui8QuestCtxActiveNow[i] )
				{
					pQuestCtxActive[i]		+= m_aui8QuestCtxActiveNow[i];
					ChangeDataBaseInfo(SAVE_DATABASE_MEDAL);
				}

				if( pQuest[i].m_Arg[2] > pQuestCtxActive[i] )								continue;
				pQuestCtxActive[i] = pQuest[i].m_Arg[2];
			}
			break;
		}
		
		ui16NewQuestCtx |= (0xF << (i*4));
	}

	if( m_QuestIngInfo.m_pActiveIndexCard[ui8Use][ui8CardSet] != ui16NewQuestCtx )
	{
		m_QuestIngInfo.m_pActiveIndexCard[ui8Use][ui8CardSet] = m_QuestIngInfo.m_pActiveIndexCard[ui8Use][ui8CardSet] | ui16NewQuestCtx;
		ChangeDataBaseInfo(SAVE_DATABASE_MEDAL);
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
		SendPacket.WriteData( &m_UserInfoBasic._exp,		sizeof(UINT32)); 
		SendPacket.WriteData( &m_UserInfoBasic._point,		sizeof(UINT32)); 
		SendPacket.WriteData( &m_ui32Miniature,				sizeof(UINT32));
		SendPacket.WriteData( &m_ui32Insignia,				sizeof(UINT32));
		SendPacket.WriteData( &m_ui32Order,					sizeof(UINT32));
		SendPacket.WriteData( &m_ui32Master,				sizeof(UINT32));
		SendPacket.WriteData( &m_UserInfoBasic._rank,		sizeof(UINT32));					//뒤에 추가함 .. 0410
	}
	SendPacketMessage( &SendPacket );

	// 진급 포인트 시스템 추가 - 발송후 초기화
	if( m_bIsRankUp )
	{
		UINT32 ui32Rank = m_UserInfoBasic._rank;
		// 임시 계급을 사용중이면 임시 계급 변경
		if( -1 != GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_RANK, CASHITEM_DAY_NO) ) )
		{
			ui32Rank = m_UserInfoInvItemData._wRank;
		}
		i3NetworkPacket packet(PROTOCOL_BASE_RANK_UP_ACK);
		packet.WriteData(&ui32Rank,										sizeof(UINT32));
		packet.WriteData(&(GetUserInfoBasic()->_keep_rank),				sizeof(UINT32));
		packet.WriteData(&m_ui32rankUpPoint,								sizeof(UINT32));
		SendPacketMessage(&packet);

		m_bIsRankUp		= FALSE;
		m_ui32rankUpPoint	= 0;
	}

	ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO | SAVE_DATABASE_MEDAL); 
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
	for( int i = 0; i < MAX_QUEST_ITEM_CARD; ++i )
	{
		if( pItemInfo->m_Item[i].m_ui32ItemID != 0 )
		{
			g_pModuleTrans->AuthSendInsertItem( MAIN_THREAD_IDX, this, ITEM_INSERT_REQUEST_MISSION_CARD, pItemInfo->m_Item[i].m_ui32ItemID, pItemInfo->m_Item[i].m_ui32Arg );
		}
	}

	// 카드 10개 전부 완료시 최종 보상 //
	if( bLastGet )
	{//마지막 보상을 준다. 		
		i32Rv = i32Rv | GET_QUEST_FINISH_LAST_CARD;

		for( int i = 0; i < MAX_QUEST_ITEM_CARD; ++i ) // 최종 보상으로 주어지는 아이템 //
		{
			if( pLastItemInfo->m_Item[i].m_ui32ItemID != 0 )
			{
				g_pModuleTrans->AuthSendInsertItem( MAIN_THREAD_IDX, this, ITEM_INSERT_REQUEST_MISSION_CARD, pLastItemInfo->m_Item[i].m_ui32ItemID, pLastItemInfo->m_Item[i].m_ui32Arg);
				i32Rv = i32Rv | GET_QUEST_WEAPON;
			}
		}
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
		m_UserInfoBasic._exp		+= ui32Exp;
		m_UserInfoBasic._point		+= ui32Point;

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

	for( int i = 0; i < MAX_QUEST_ITEM_CARD; ++i ) // 보상 아이템 정보
	{
		stMission._aui32GetItem[i]		= pItemInfo->m_Item[i].m_ui32ItemID;
		stMission._aui32LastGetItem[i]	= pLastItemInfo->m_Item[i].m_ui32ItemID;
	}

	g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_U_QUEST_FINISH, this, &stMission );

	if( stMission._ui32Point > 0 || stMission._ui32LastPoint > 0 )
	{
		UINT32 ui32Point	= stMission._ui32Point + stMission._ui32LastPoint;
		INT32 i32GetRoute	= 2;
		g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_U_GET_POINT, this, &ui32Point, &i32GetRoute ); 
	}

	return i32Rv; 
}

// -----------------------------------------------------------
// Desc : 카드를 삭제합니다. 
// -----------------------------------------------------------
void CUserSession::_DeleteQuestCardSet(UINT8 iCardSet)
{
	m_QuestIngInfo.m_ActiveIndexUse						= 0;
	m_QuestIngInfo.m_pActiveIndexCardSet[iCardSet]		= 0;
	m_QuestIngInfo.m_pCardSetType[iCardSet]				= QUEST_CARDSET_TYPE_NONE;

	for(INT32 i = 0; i < MAX_CARD_PER_CARDSET; i++)
	{
		m_QuestIngInfo.m_pActiveIndexCard[iCardSet][i]	= 0;
	}
	// 등록되어있는 퀘스트를 삭제합니다.
	i3mem::FillZero( m_aaui8QuestCtxActive[iCardSet],	sizeof( UINT8 ) * MAX_QUEST_PER_CARDSET_NEW );

	ChangeDataBaseInfo( SAVE_DATABASE_MEDAL ); 
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

		ChangeDataBaseInfo( SAVE_DATABASE_MEDAL );
	}
}

void CUserSession::_SendQuestInfoPacket( INT8 i8CardSolt )
{
	//쿼스트 정보 보내기
	i3NetworkPacket SendPacket( PROTOCOL_BASE_QUEST_GET_INFO_ACK );
	SendPacket.WriteData( &i8CardSolt,			sizeof( INT8 ) );
	SendPacket.WriteData( &m_QuestIngInfo,		sizeof(QUESTING_INFO) );
	SendPacketMessage( &SendPacket );
	return;
}

INT32 CUserSession::_SetItemAbilityAuth( INVEN_BUFFER* pBuffer )
{
	if( ITEM_CLASS_EXPENDABLE != GET_ITEM_FLAG_CLASS( pBuffer->_ui32ItemID ) )	return EVENT_ERROR_SUCCESS;

	switch( GET_ITEM_FLAG_TYPE( pBuffer->_ui32ItemID ) )
	{
	case CASHITEM_GROUP_CLR_HISTORY:
		{	// 전적 초기화
			_ResetRecord_MAU();
			// 일단 초기화 정보 업데이트
			i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_MYINFO_RECORD_ACK );
			SendPacket.WriteData( &m_UserInfoRecord, sizeof(USER_INFO_RECORD) );
			SendPacketMessage( &SendPacket );
		}
		break;
	case CASHITEM_GROUP_CLR_ESCAPE:
		{	// 이탈률 초기화
			_ResetDisconnect_MAU();

			i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_MYINFO_RECORD_ACK );
			SendPacket.WriteData( &m_UserInfoRecord, sizeof(USER_INFO_RECORD) );
			SendPacketMessage( &SendPacket );
		}
		break;
	case CASHITEM_GROUP_CLR_SCORE:
		{	// 킬/데스 초기화
			_ResetKillDeath_MAU();

			// 일단 초기화 정보 업데이트
			i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_MYINFO_RECORD_ACK );
			SendPacket.WriteData( &m_UserInfoRecord, sizeof(USER_INFO_RECORD) );
			SendPacketMessage( &SendPacket );
		}
		break;
	case CASHITEM_GROUP_CLAN_PERSON_PLUS_50:
		{	// 클랜원 증가
			if(m_UserInfoBasic._clanidx	  == 0)										return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if( m_UserInfoBasic._clanstate != CLAN_MEMBER_LEVEL_MASTER )		return EVENT_ERROR_CLAN_LEVEL_MASTER;
			
			g_pModuleClan->ClanSendPersonMaxRequest(m_WorkThreadIdx, this );
		}
		break;
	case CASHITEM_GROUP_CLR_CLAN_HISTORY:
		{	// 클랜전적 초기화
			if(m_UserInfoBasic._clanidx	  == 0)										return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if( m_UserInfoBasic._clanstate != CLAN_MEMBER_LEVEL_MASTER )		return EVENT_ERROR_CLAN_LEVEL_MASTER;

			g_pModuleClan->ClanSendRecordResetRequest(m_WorkThreadIdx, this );
		}
		break;
	case CASHITEM_GROUP_CLAN_POINT_RESET:
		{	// 클랜 포인트 초기화
			if(m_UserInfoBasic._clanidx	  == 0)										return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if( m_UserInfoBasic._clanstate != CLAN_MEMBER_LEVEL_MASTER )		return EVENT_ERROR_CLAN_LEVEL_MASTER;
				
            g_pModuleClan->ClanSendPointResetRequest(m_WorkThreadIdx, this );
		}
		break;
	}
	return EVENT_ERROR_SUCCESS;
}

INT32 CUserSession::_SetItemAbilityAuth( INVEN_BUFFER* pBuffer, void* pData )
{
	if( ITEM_CLASS_EXPENDABLE != GET_ITEM_FLAG_CLASS( pBuffer->_ui32ItemID ) )	return EVENT_ERROR_SUCCESS;

	switch( GET_ITEM_FLAG_TYPE( pBuffer->_ui32ItemID ) ) 
	{
	case CASHITEM_GROUP_CHANGE_NICK:
		{	// 닉네임 변경권
			// 위장 닉네임 사용시 닉네임 변경 실패			
 			if( -1 == GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_EXPENDABLE, CASHITEM_USAGE_EXPENDABLES, CASHITEM_GROUP_CHANGE_NICK, CASHITEM_DAY_NO)))	return EVENT_ERROR_FAIL;
			// 자신의 닉네임을 다시 변경 요청시 변경 실패처리
			if( i3String::Compare((char*)pData, m_UserInfoBasic._nick) == 0)	return EVENT_ERROR_FAIL;
		}
		break;
	case CASHITEM_GROUP_CHANGE_CLAN_NAME:
		{	// 클랜 이름 변경권			
			if(m_UserInfoBasic._clanidx	  == 0)										return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if( m_UserInfoBasic._clanstate != CLAN_MEMBER_LEVEL_MASTER )		return EVENT_ERROR_CLAN_LEVEL_MASTER;
			
			char szClanName[NET_CLAN_NAME_SIZE]; 
			i3mem::Copy( szClanName, pData, NET_CLAN_NAME_SIZE );
			szClanName[NET_CLAN_NAME_SIZE - 1] = 0; 

			// 클랜 이름 변경
			if(FALSE == g_pModuleClan->ClanSendNameRequest(m_WorkThreadIdx, this, szClanName ))
			{
				return EVENT_ERROR_FAIL;
			}
		}
		break;
	case CASHITEM_GROUP_CHANGE_CLAN_MARK:
		{	// 클랜 마크 변경권
			if(m_UserInfoBasic._clanidx	  == 0)										return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if( m_UserInfoBasic._clanstate != CLAN_MEMBER_LEVEL_MASTER )		return EVENT_ERROR_CLAN_LEVEL_MASTER;
				
			UINT32 ui32Mark;
			i3mem::Copy( &ui32Mark, pData, sizeof(UINT32) );

			// 클랜 마크 변경
			if( FALSE == g_pModuleClan->ClanSendMarkRequest(m_WorkThreadIdx, this, ui32Mark ))
			{
				return EVENT_ERROR_FAIL;
			}
		}
		break;
	case CASHITEM_GROUP_COLOR_CLAN:
		{	// 클랜 컬러 네임
			if(m_UserInfoBasic._clanidx	  == 0)										return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if( m_UserInfoBasic._clanstate != CLAN_MEMBER_LEVEL_MASTER )		return EVENT_ERROR_CLAN_LEVEL_MASTER;

			UINT8 ui8Color;
			i3mem::Copy( &ui8Color, pData, sizeof(UINT8) );

			// 클랜 서버에 알려준다.
			if( FALSE == g_pModuleClan->ClanSendClanColorRequest(m_WorkThreadIdx, this, MAKE_ITEM_FLAG(ITEM_CLASS_EXPENDABLE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_COLOR_CLAN, CASHITEM_DAY_NO), 0, ui8Color ) )
			{
				return EVENT_ERROR_FAIL;
			}
		}
		break;
	case CASHITEM_GROUP_COLOR_NICK:
		{	// 컬러 닉네임
			UINT8 ui8Color;
			i3mem::Copy( &ui8Color, pData, sizeof(UINT8) );
			m_UserInfoBasic._nickcolor = ui8Color;
		}
		break;
	case CASHITEM_GROUP_DISGUISE_NICK:
		{	// 위장 닉네임
			char strNickname[NET_NICK_NAME_SIZE];
			i3mem::Copy( strNickname, pData, NET_NICK_NAME_SIZE );
			strNickname[NET_NICK_NAME_SIZE - 1] = '\0';

			// 아이디 중복 체크
			INT32 i32Result = 0;
			if( EV_FAILED( i32Result ))												return i32Result;

			// 임시로 저장을 해놓는다.
			i3String::Copy( m_UserInfoInvItemData._cNick, strNickname, NET_NICK_NAME_SIZE );

			m_UserInfoInvItemData._cNick[ NET_NICK_NAME_SIZE - 1 ] = '\0';
		}
		break;
	case CASHITEM_GROUP_DISGUISE_RANK:
		{	// 위장 계급
			UINT8 ui8Rank;
			i3mem::Copy( &ui8Rank, pData, sizeof(UINT8) );
			
			// 임시로 저장을 해놓는다.
			m_UserInfoInvItemData._wRank = (UINT32)ui8Rank;
		}
		break;
	case CASHITEM_GROUP_COLOR_CHATTING:
		{	// 컬러 체팅
			UINT8 ui8Color;
			i3mem::Copy( &ui8Color, pData, sizeof(UINT8) );

			// 임시로 저장을 해놓는다.
			m_UserInfoInvItemData._cColorChatting = ui8Color;
		}
		break;
	case CASHITEM_GROUP_COLOR_CROSSHAIR:
		{	// 컬러 크로스헤어
			UINT8 ui8Color;
			i3mem::Copy( &ui8Color, pData, sizeof(UINT8) );
			
			// 임시로 저장을 해놓는다.
			m_UserInfoInvItemData._cColorCrossHair = ui8Color;
		}
		break;
	case CASHITEM_GROUP_VIEW_OTHER_USER_ITEM_INFO:
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
	}
	return EVENT_ERROR_SUCCESS;
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

	ChangeDataBaseInfo( SAVE_DATABASE_RECORD );
}
//----------------------------------------------------------------------------------------------------------------
// public
void CUserSession::CheckRankUp( INT32 i32WorkThread )
{
	UINT32* pui32Rank = &m_UserInfoBasic._rank;

	// 임시 계급을 사용중이면 임시 계급 변경
	if( -1 != GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_RANK, CASHITEM_DAY_NO) ) )
	{
		pui32Rank = &m_UserInfoInvItemData._wRank;
	}

	// (준장 이상은 자동 진급 불가) -> pwGainRank : 대령 5호봉이하
	// 준장 이상이면 포인트 미지급
	// 준장 이하로 if문 진입 불가
	if( g_pContextMain->SetUserRank( m_UserInfoBasic._exp, pui32Rank ) )
	{		
		// 계급 상승 & 진급 포인트 지급
		// 진급 포인트 시스템		
		UINT32 ui32Rank = 0;

		if(RANK_51 == *pui32Rank)		ui32Rank = RANK_51;
		else						ui32Rank = MIN( *pui32Rank, RANK_46 );	

		m_ui32rankUpPoint			= 0;
		m_bIsRankUp				= TRUE;


		// KeepRank도 업데이트
		if(m_UserInfoBasic._keep_rank < ui32Rank)
		{
			SHOP_MATCHING_DATA*	  pMatching = g_pContextMain->GetShop()->GetMatchingAllData();

			m_UserInfoBasic._keep_rank = ui32Rank;

			INT32 i32GetRoute = 5;
			g_pModuleContextNC->LogSendMessage( i32WorkThread, LOG_MESSAGE_U_GET_POINT, this, &m_ui32rankUpPoint, &i32GetRoute );

			if(g_pContextMain->m_aRankUpItem[ui32Rank - 1].m_ui8Rank == m_UserInfoBasic._keep_rank)
			{
				for(int i = 0; i < MAX_RANKUP_ITEM_COUNT; i++)
				{
					if(g_pContextMain->m_aRankUpItem[ui32Rank - 1].m_ui32GoodsID[i] != 0)
					{
						UINT32 ui32ItemID = 0;
						UINT32 ui32ItemArg = 0;

						UINT32	ui32MatchingCount	= g_pContextMain->GetShop()->GetMatchingCount();
						for( UINT32 j = 0; j < ui32MatchingCount; ++j )
						{
							if( pMatching[j]._ui32GoodsID != g_pContextMain->m_aRankUpItem[ui32Rank - 1].m_ui32GoodsID[i]) continue;

							ui32ItemID	= pMatching[j]._ui32ItemID;
							ui32ItemArg = pMatching[j]._ui32ItemArg;
							break;
						}	
						if( ui32ItemID > 0 && ui32ItemArg > 0 )
						{
							g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP, ui32ItemID, ui32ItemArg);
						}
					}
				}
			}

			if( LOCALE_CODE_LATIN_AMERICA == m_ui8LocaleCode ) // 중남미 진급시 무기 지급 이벤트 하자하자 이벤트
			{
				// 이벤트 기간 무제한
				// 퍼블리셔로부터(미란님통해) 기간설정 없이 상시로 진행하자는 요청 받음. 하자 보상은 3차(6.15~8.31)과 동일. 2011.09.01 안국정
				switch( ui32Rank )
				{	
				case RANK_00: 
					break;
				case RANK_01:	// 이병
					g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP,   100003021, 1 * 86400); // M4A1 Sl 1D
					g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP,   200004014, 1 * 86400); // MP5K Sl 1D
					g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP,   300005009, 1 * 86400); // PSG1 Sl 1D
					break;
				case RANK_02:	// 일병
					g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP,  1200064000, 3 * 86400); // Short Respawn 50% 3D
					break;
				case RANK_03:	// 상병
					g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP,   100003017, 1 * 86400); // AK-47 Sl 1D
					g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP,   200004020, 1 * 86400); // Spectre Sl 1D
					g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP,   300005008, 1 * 86400); // SSG-69 Sl 1D
					break;
				case RANK_05:	// 하사1호봉
					g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP,   100003019, 3 * 86400); // SG 550 Sl 3D
					g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP,   200004010, 3 * 86400); // P90 M.C. 3D
					g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP,   300005010, 3 * 86400); // Dragunov Sl 3D
					break;
				case RANK_09:	// 중사2호봉 
					g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP,   100003023, 3 * 86400); // M4A1 G 3D
					g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP,   200004022, 3 * 86400 ); // MP7 SL 3D
					g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP,   300005024, 3 * 86400); // PSG1 G 3D
					break;
				default:	// 기타 레벨 진급시는 MiniBomb 1개 지급
					g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP,  1301046000, 1 );
					break;
				}
			}
		}
		// 로그 전송
		g_pModuleContextNC->LogSendMessage( i32WorkThread, LOG_MESSAGE_U_LEVEL_UP, this );
	}
	else
	{
		m_ui32rankUpPoint = 0;
	}
}

void CUserSession::CheckGeneralRankUp( INT32 i32WorkThread )
{
	UINT32	ui32Loop = 0;

	UINT32* pui32Rank = &m_UserInfoBasic._rank;

	// 임시 계급을 사용중이면 임시 계급 변경
	if( -1 != GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_RANK, CASHITEM_DAY_NO) ) )
	{
		pui32Rank = &m_UserInfoInvItemData._wRank;
	}

	// 영웅이상, 준장 이하는 검사 대상에서 제외
	if(RANK_51 <= *pui32Rank || RANK_47 > *pui32Rank) return;

	UINT32 ui32Rank = MIN( *pui32Rank, RANK_50 );

	// (준장 이상은 자동 진급 불가)
	// 준장 이상이면 일회만 보상 포인트 지급
	// 준장 이하로 올수는 없음	
	// 최초 로그인시에만 적용됨.
	if(ui32Rank > m_UserInfoBasic._keep_rank)
	{
		m_ui32rankUpPoint = 0;
		// 계급 상승 & 진급 포인트 지급
		// 진급 포인트 시스템		
		for(ui32Loop = m_UserInfoBasic._keep_rank; ui32Loop < ui32Rank; ui32Loop++)
		{
			m_UserInfoBasic._point	+= g_ai32RankUpBonus[ui32Loop];
			m_ui32rankUpPoint			+= g_ai32RankUpBonus[ui32Loop];
		}

		SHOP_MATCHING_DATA*	  pMatching = g_pContextMain->GetShop()->GetMatchingAllData();

		if(g_pContextMain->m_aRankUpItem[ui32Rank - 1].m_ui8Rank == m_UserInfoBasic._keep_rank)
		{
			for(int i = 0; i < MAX_RANKUP_ITEM_COUNT; i++)
			{
				if(g_pContextMain->m_aRankUpItem[ui32Rank - 1].m_ui32GoodsID[i] != 0)
				{
					UINT32 ui32ItemID = 0;
					UINT32 ui32ItemArg = 0;

					UINT32	ui32MatchingCount	= g_pContextMain->GetShop()->GetMatchingCount();
					for( UINT32 j = 0; j < ui32MatchingCount; ++j )
					{
						if( pMatching[j]._ui32GoodsID != g_pContextMain->m_aRankUpItem[ui32Rank - 1].m_ui32GoodsID[i]) continue;

						ui32ItemID	= pMatching[j]._ui32ItemID;
						ui32ItemArg = pMatching[j]._ui32ItemArg;
						break;
					}	
					if( ui32ItemID > 0 && ui32ItemArg > 0 )
					{
						g_pModuleTrans->AuthSendInsertItem(i32WorkThread, this, ITEM_INSERT_REQUEST_RANKUP, ui32ItemID, ui32ItemArg);
					}
				}
			}
		}

		// 로그 전송
		g_pModuleContextNC->LogSendMessage( i32WorkThread, LOG_MESSAGE_U_LEVEL_UP, this );
	}
	else
	{
		m_ui32rankUpPoint = 0;
	}
}

// -----------------------------------------------------------
// Name : GeneralRankUp
// Desc : 장성급 유저의 랭킹업시 1회 진급 보상을 주는 함수
// -----------------------------------------------------------
void CUserSession::GeneralRankUp(INT32 i32WorkThread)
{
	// 진급 포인트 시스템 추가 - 발송후 초기화
	if (0 < m_ui32rankUpPoint)
	{
		UINT32 ui32Rank = m_UserInfoBasic._rank;
		// 임시 계급을 사용중이면 임시 계급 변경
		if( -1 != GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_RANK, CASHITEM_DAY_NO) ) )
		{
			ui32Rank = m_UserInfoInvItemData._wRank;
		}
		i3NetworkPacket SendPacket(PROTOCOL_BASE_RANK_UP_ACK);
		SendPacket.WriteData(&ui32Rank,							sizeof(UINT32));
		SendPacket.WriteData(&(GetUserInfoBasic()->_keep_rank),	sizeof(UINT32));
		SendPacket.WriteData(&m_ui32rankUpPoint,				sizeof(UINT32));
		SendPacketMessage(&SendPacket);

		m_ui32rankUpPoint = 0;
		GetUserInfoBasic()->_keep_rank = ui32Rank;

		ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO);

		// 클랜 목록의 랭킹 변경
		g_pModuleClan->ClanSendClanInfoRankUpdate( i32WorkThread, this );
	}			
}

INVEN_BUFFER* CUserSession::GetItemByWareDBIdx( TWareDxIdx i64WareDBIdx )
{
	for( INT32 i = 0 ; i < m_i32InvWeaponCount ; i++ )
	{
		if( i64WareDBIdx == m_aInvWeapon[i]._i64ItemWareDBIdx )
		{
			return &m_aInvWeapon[i];
		}
	}
	for( INT32 i = 0 ; i < m_i32InvCharaCount ; i++ )
	{
		if( i64WareDBIdx == m_aInvChara[i]._i64ItemWareDBIdx )
		{
			return &m_aInvChara[i];
		}
	}
	for( INT32 i = 0 ; i < m_i32InvItemCount ; i++ )
	{
		if( i64WareDBIdx == m_aInvItem[i]._i64ItemWareDBIdx )
		{
			return &m_aInvItem[i];
		}
	}
	
	return NULL;
}

INVEN_BUFFER* CUserSession::GetItemByItemID( UINT32 ui32ItemID )
{
	for( INT32 i = 0 ; i < m_i32InvWeaponCount ; i++ )
	{
		if( ui32ItemID == m_aInvWeapon[i]._ui32ItemID )
		{
			return &m_aInvWeapon[i];
		}
	}
	for( INT32 i = 0 ; i < m_i32InvCharaCount ; i++ )
	{
		if( ui32ItemID == m_aInvChara[i]._ui32ItemID )
		{
			return &m_aInvChara[i];
		}
	}
	for( INT32 i = 0 ; i < m_i32InvItemCount ; i++ )
	{
		if( ui32ItemID == m_aInvItem[i]._ui32ItemID )
		{
			return &m_aInvItem[i];
		}
	}
	
	return NULL;
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

BOOL CUserSession::_CheckClanName( char* pClanName )
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

void CUserSession::SendWhisper(char* pstrsenderNick, char* pstrmessage, UINT16 ui16messageSize)
{
	i3NetworkPacket packet(PROTOCOL_AUTH_RECV_WHISPER_ACK);
	
	packet.WriteData(pstrsenderNick,	NET_NICK_NAME_SIZE);
	packet.WriteData(&ui16messageSize,	sizeof(UINT16));
	packet.WriteData(pstrmessage,		ui16messageSize);

	SendPacketMessage(&packet);
}

void CUserSession::SendWhisperError(INT32 i32result, char* pstrreceiverNick)
{
	i3NetworkPacket packet(PROTOCOL_AUTH_SEND_WHISPER_ACK);

	INT32	i32State = CHAT_LIMIT_NORMAL;
	packet.WriteData( &i32State,		sizeof(INT32) );
	packet.WriteData( &i32result,		sizeof(INT32));
	packet.WriteData( pstrreceiverNick,	NET_NICK_NAME_SIZE);

	SendPacketMessage(&packet);
}

#ifdef USE_UNDECIDED_DESIGN_TEST
void CUserSession::SendChannelEnterResult(INT32 result)
{
	i3NetworkPacket packet(PROTOCOL_MESSENGER_RECV_CHANNEL_NUM_ACK);
	packet.WriteData(&result, sizeof(INT32));	
	SendPacketMessage(&packet);
}
#endif

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
		SendPacket.WriteData( &pFriendInfo[i]._ui8NickSize,		sizeof(UINT8)); 
		SendPacket.WriteData( pFriendInfo[i]._strNickName,		pFriendInfo[i]._ui8NickSize ); 
		SendPacket.WriteData( &pFriendInfo[i]._BasicInfo,	sizeof(FRIEND_INFO));
	}
	SendPacketMessage(&SendPacket); 
	return; 
}

void CUserSession::SetFriendChangeInfo_A(UINT8 ui8FriendIdx, UINT8 ui8State, UINT8 ui8NickNameSize, char  * pstrNickName, FRIEND_INFO * pInfo)
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
		break; 
	case FRIEND_CHANGE_STATE_ACCEPT	:
	case FRIEND_CHANGE_STATE_DELETE	:
	case FRIEND_CHANGE_STATE_CHANGE	:
		break; 
	default : I3TRACE("Friend State Error"); return; break; 
	}
 
	//2. Send Message 
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_FRIEND_INFO_CHANGE_ACK );
	SendPacket.WriteData( &ui8State,		sizeof(UINT8) );		//상태
	SendPacket.WriteData( &ui8FriendIdx,	sizeof(UINT8) );		//친구인덱스
	SendPacket.WriteData( &ui8NickNameSize,	sizeof(UINT8)); 
	SendPacket.WriteData( pstrNickName,		ui8NickNameSize); 
	SendPacket.WriteData( pInfo,			sizeof(FRIEND_INFO));	//친구정보

	SendPacketMessage(&SendPacket); 
	return; 
}

void CUserSession::SendFriendAcceptError(INT32	i32ErrorCode)
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_FRIEND_ACCEPT_ACK );
	SendPacket.WriteData( &i32ErrorCode,		sizeof(INT32) );
	SendPacketMessage(&SendPacket); 
	return; 
}

void CUserSession::SendFriendInsertError(INT32 i32ErrorCode)
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_FRIEND_INSERT_ACK );
	SendPacket.WriteData( &i32ErrorCode,		sizeof(INT32) );
	SendPacketMessage(&SendPacket); 
	return; 
}

void CUserSession::SendFriendDeleteError(INT32 i32ErrorCode)
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_FRIEND_DELETE_ACK );
	SendPacket.WriteData( &i32ErrorCode,		sizeof(INT32) );
	SendPacketMessage(&SendPacket); 
	return; 
}

void CUserSession::SendFriendInvitedError(INT32 i32ErrorCode)
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_FRIEND_INVITED_ACK );
	SendPacket.WriteData( &i32ErrorCode, sizeof(INT32) );
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
	INVEN_BUFFER*	pInven = NULL;
	INT32*			pi32InvenCount = NULL;
	
	switch( GET_ITEM_FLAG_CLASS(pInvenItem->_ui32ItemID) )
	{
	case ITEM_CLASS_ASSAULT:
	case ITEM_CLASS_SMG:
	case ITEM_CLASS_SNIPER:
	case ITEM_CLASS_SHOTGUN:
	case ITEM_CLASS_HANDGUN:
	case ITEM_CLASS_KNIFE:
	case ITEM_CLASS_GRENADE:
	case ITEM_CLASS_THROWING:
	case ITEM_CLASS_MACHINE:
		pInven = m_aInvWeapon;
		pi32InvenCount = &m_i32InvWeaponCount;

		// Send 를 위해 버퍼에 담아준다
		i3mem::Copy( &pInvenSet->aWeaponBuffer[ pInvenSet->ui32WeaponCount ], pInvenItem, sizeof(INVEN_BUFFER)  );
		pInvenSet->ui32WeaponCount++;
		break;
	case ITEM_CLASS_CHARA:
	case ITEM_CLASS_HEAD:
		pInven = m_aInvChara;
		pi32InvenCount = &m_i32InvCharaCount;

		// Send 를 위해 버퍼에 담아준다
		i3mem::Copy( &pInvenSet->aCharaBuffer[ pInvenSet->ui32CharaCount ], pInvenItem, sizeof(INVEN_BUFFER)  );
		pInvenSet->ui32CharaCount++;
		break;
	case ITEM_CLASS_MAINTENANCE:
	case ITEM_CLASS_EXPENDABLE:
	case ITEM_CLASS_POINT:
		pInven = m_aInvItem;
		pi32InvenCount = &m_i32InvItemCount;

		// Send 를 위해 버퍼에 담아준다
		i3mem::Copy( &pInvenSet->aItemBuffer[ pInvenSet->ui32ItemCount ], pInvenItem, sizeof(INVEN_BUFFER)  );
		pInvenSet->ui32ItemCount++;
		break;
	case ITEM_CLASS_TEMP:	
	case ITEM_CLASS_COUPON:
	//case ITEM_CLASS_POINT:
	case ITEM_CLASS_SALECOUPON:
		pInven = m_aInvItem;
		pi32InvenCount = &m_i32InvItemCount;

		// Send 를 위해 버퍼에 담아준다
		i3mem::Copy( &pInvenSet->aItemBuffer[ pInvenSet->ui32ItemCount ], pInvenItem, sizeof(INVEN_BUFFER)  );
		pInvenSet->ui32ItemCount++;
		break;
	case ITEM_CLASS_UNKNOWN:
		break;
	}
	if( NULL == pInven )	return;

	
	BOOL bNew = TRUE;
	INT32 i32BlankIdx = -1;
	for( INT32 i = 0 ; i < MAX_INVEN_COUNT ; i++ ) 
	{
		if( ITEM_ATTR_TYPE_AUTH < pInven[i]._ui8ItemType)	continue;

		if( 0 == pInven[ i ]._ui32ItemID )
		{
			if( -1 == i32BlankIdx )
			{
				i32BlankIdx = i;
			}
		}
		else if( pInven[ i ]._i64ItemWareDBIdx == pInvenItem->_i64ItemWareDBIdx )
		{
			i32BlankIdx = i;
			bNew = FALSE;
			break;
		}
	}
	// 아이템 복사
	if( -1 != i32BlankIdx )
	{
		i3mem::Copy( &pInven[ i32BlankIdx ], pInvenItem, sizeof(INVEN_BUFFER));
		if( bNew )
		{
			(*pi32InvenCount)++;
		}
	}
}

UINT32 CUserSession::OnDeleteItem( TUID i64ItemWareDBIdx, INVEN_BUFFER_SET* pInvenSet )
{
	UINT32 ui32DeleteItemID = 0;
	for( INT32 i = 0 ; i < MAX_INVEN_COUNT ; i++ )
	{
		if( i64ItemWareDBIdx == m_aInvWeapon[ i ]._i64ItemWareDBIdx )
		{
			ui32DeleteItemID = m_aInvWeapon[ i ]._ui32ItemID;
			i3mem::Copy( &pInvenSet->aWeaponBuffer[ pInvenSet->ui32WeaponCount ], &m_aInvWeapon[ i ], sizeof(INVEN_BUFFER)  );
			pInvenSet->aWeaponBuffer[ pInvenSet->ui32WeaponCount ]._ui32ItemID = 0;
			pInvenSet->ui32WeaponCount++;

			m_i32InvWeaponCount--;

			// 바로 FillZero 시키지 않고 뒤에 있던 메모리를 복사해온다.
			i3mem::Copy( &m_aInvWeapon[ i ], &m_aInvWeapon[ m_i32InvWeaponCount ], sizeof( INVEN_BUFFER ) );
			i3mem::FillZero( &m_aInvWeapon[ m_i32InvWeaponCount ], sizeof( INVEN_BUFFER ) );
			return ui32DeleteItemID;
		}
		else if( i64ItemWareDBIdx == m_aInvChara[ i ]._i64ItemWareDBIdx )
		{
			ui32DeleteItemID = m_aInvChara[ i ]._ui32ItemID;
			i3mem::Copy( &pInvenSet->aCharaBuffer[ pInvenSet->ui32CharaCount ], &m_aInvChara[ i ], sizeof(INVEN_BUFFER)  );
			pInvenSet->aCharaBuffer[ pInvenSet->ui32CharaCount ]._ui32ItemID = 0;
			pInvenSet->ui32CharaCount++;

			m_i32InvCharaCount--;

			// 바로 FillZero 시키지 않고 뒤에 있던 메모리를 복사해온다.
			i3mem::Copy( &m_aInvChara[ i ], &m_aInvChara[ m_i32InvCharaCount ], sizeof( INVEN_BUFFER ) );
			i3mem::FillZero( &m_aInvChara[ m_i32InvCharaCount ], sizeof( INVEN_BUFFER ) );
			return ui32DeleteItemID;
		}
		else if( i64ItemWareDBIdx == m_aInvItem[ i ]._i64ItemWareDBIdx )
		{
			ui32DeleteItemID = m_aInvItem[ i ]._ui32ItemID;
			i3mem::Copy( &pInvenSet->aItemBuffer[ pInvenSet->ui32ItemCount ], &m_aInvItem[ i ], sizeof(INVEN_BUFFER)  );
			pInvenSet->aItemBuffer[ pInvenSet->ui32ItemCount ]._ui32ItemID = 0;
			pInvenSet->ui32ItemCount++;

			m_i32InvItemCount--;

			i3mem::Copy( &m_aInvItem[ i ], &m_aInvItem[ m_i32InvItemCount ], sizeof( INVEN_BUFFER ) );
			i3mem::FillZero( &m_aInvItem[ m_i32InvItemCount ], sizeof( INVEN_BUFFER ) );
			return ui32DeleteItemID;
		}
	}
	return ui32DeleteItemID;
}

void CUserSession::SendChangeInven( INVEN_BUFFER_SET* pInvenSet )
{
	i3NetworkPacket SendPacket( PROTOCOL_INVENTORY_GET_INFO_ACK );
	SendPacket.WriteData( &pInvenSet->ui8ChangeType,			sizeof(UINT8));
	SendPacket.WriteData( &pInvenSet->ui32CharaCount,			sizeof(UINT32));
	SendPacket.WriteData( &pInvenSet->ui32WeaponCount,			sizeof(UINT32));
	SendPacket.WriteData( &pInvenSet->ui32ItemCount,			sizeof(UINT32));

	if( 0 < pInvenSet->ui32CharaCount )
		SendPacket.WriteData( pInvenSet->aCharaBuffer,			(UINT16)(sizeof(INVEN_BUFFER)*pInvenSet->ui32CharaCount) );
	if( 0 < pInvenSet->ui32WeaponCount )
		SendPacket.WriteData( pInvenSet->aWeaponBuffer,			(UINT16)(sizeof(INVEN_BUFFER)*pInvenSet->ui32WeaponCount) );		
	if( 0 < pInvenSet->ui32ItemCount )
		SendPacket.WriteData( pInvenSet->aItemBuffer,			(UINT16)(sizeof(INVEN_BUFFER)*pInvenSet->ui32ItemCount) );

	SendPacketMessage( &SendPacket );
}

/*------------------------------------------------------
리턴값 0x00000001		//Chara 변경 됨 
리턴값 0x00000002		//Weapon 변경 됨
리턴값 0x00000004		//Item 변경 됨
리턴값 0x00000010		//Chara Equip 변경 됨 
리턴값 0x00000020		//Weapon Equip 변경 됨 
리턴값 0x00000040		//Item Equip 변경 됨 - Item은 장비할 수 없음으로 사용하지 않는다.
------------------------------------------------------*/
void CUserSession::CheckInventory_MAU( )
{
	INT32 i32Rv = 0; 

	INVEN_BUFFER_SET InvenSet;
	InvenSet.ui8ChangeType = ITEM_CHANGE_TYPE_USE;

	CShop* pShop = g_pContextMain->GetShop();

	BOOL bDelete = FALSE;
	for( INT32 i = 0 ; i < m_i32InvCharaCount ; i++ )
	{
		if( 0 == m_aInvChara[i]._ui32ItemID )									continue;
		if( ITEM_ATTR_TYPE_AUTH != m_aInvChara[i]._ui8ItemType )				continue;
		if( g_pContextMain->GetInvenTime() < m_aInvChara[i]._ui32ItemArg )		continue;
			
		SHOP_ITEM_BASE*	pItem = pShop->GetItemData( m_aInvChara[i]._ui32ItemID );
		if( NULL == pItem )														continue;
		if( ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType )					continue;
		
		OnDeleteItem( m_aInvChara[i]._i64ItemWareDBIdx, &InvenSet );
		bDelete = TRUE;
	}

	for( INT32 i = 0 ; i < m_i32InvWeaponCount ; i++ )
	{
		if( 0 == m_aInvWeapon[i]._ui32ItemID )									continue;
		if( ITEM_ATTR_TYPE_AUTH != m_aInvWeapon[i]._ui8ItemType )				continue;
		if( g_pContextMain->GetInvenTime() < m_aInvWeapon[i]._ui32ItemArg )		continue;

		SHOP_ITEM_BASE*	pItem = pShop->GetItemData( m_aInvWeapon[i]._ui32ItemID );
		if( NULL == pItem )														continue;
		if( ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType )					continue;
			
		OnDeleteItem( m_aInvWeapon[i]._i64ItemWareDBIdx, &InvenSet );
		bDelete = TRUE;
	}

	for( INT32 i = 0 ; i < m_i32InvItemCount ; i++ )
	{
		if( 0 == m_aInvItem[i]._ui32ItemID )									continue;
		if( ITEM_ATTR_TYPE_AUTH != m_aInvItem[i]._ui8ItemType )					continue;
		if( g_pContextMain->GetInvenTime() < m_aInvItem[i]._ui32ItemArg )		continue;

		SHOP_ITEM_BASE*	pItem = pShop->GetItemData( m_aInvItem[i]._ui32ItemID );
		if( NULL == pItem )														continue;
		if( ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType )					continue;

		OnDeleteItem( m_aInvItem[i]._i64ItemWareDBIdx, &InvenSet );
		bDelete = TRUE;
	}

	if( bDelete )
	{
		SendChangeInven( &InvenSet );
		CheckEquipment(  );
	}
}

void CUserSession::CheckEquipment()
{
	//장착 확인하기 
	INT32 i32Rv = _CheckEquipment();
	if( 0 == i32Rv )	return;
	
	// 아이템 장착 실패 시에 기본 세팅으로 변경하여 내려줍니다.

	// Equip Item - 아이템은 장착하지 않는다.
	i3NetworkPacket SendPacket(PROTOCOL_SERVER_MESSAGE_CHANGE_INVENTORY); 
	SendPacket.WriteData(&i32Rv,				sizeof(INT32));

	if(i32Rv & INVENTORY_FLAG_CHARACTER)
	{
		SendPacket.WriteData(&m_UserInfoEquipmentChara,		sizeof(USER_INFO_EQUIPMENT_CHARA)); 
	}

	if(i32Rv & INVENTORY_FLAG_WEAPON)
	{
		SendPacket.WriteData(&m_UserInfoEquipmentWeapon,	sizeof(USER_INFO_EQUIPMENT_WEAPON));
	}
	SendPacketMessage(&SendPacket);
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
				m_ui32ATIsKill	= g_pContextMain->GetServerTime();
				m_ui32KillReason	= KILL_REASON_SENDPACKET_ERROR;
			}
			return FALSE;
		}
	}

	return TRUE; 
}

void CUserSession::KickUser_Module(TUID i64UID, UINT8 ui8Reason)
{
	if( FALSE == GetIsActive() )	return;
	if( i64UID != m_i64UID )		return;
		
	i3NetworkPacket SendPacket(PROTOCOL_AUTH_ACCOUNT_KICK_ACK); 
	SendPacket.WriteData(&ui8Reason, sizeof(UINT8));        
	SendPacketMessage( &SendPacket ); 

	if( 0 == m_ui32ATIsKill )
	{
		// ModuleTransSocket 에서 호출
		_SendErrorMessage( MODULE_TRANS_THREAD_IDX, EVENT_ERROR_EVENT , PROTOCOL_GSM_ACCOUNT_KICK_REQ, KILL_REASON_USERKICK, "KickUser_Module" );
	}

	return; 
}

void CUserSession::SetChangeNick( INT32 i32WorkThreadIdx, char* pstrOrgNick, char* pstrNewNick )
{
	// 변수의 값을 바꿔준다.
	char strTemp[ NET_NICK_NAME_SIZE ];

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

void CUserSession::SetChangeRank( INT32 i32WorkThreadIdx, UINT32 &ui32OldRank, UINT32 &ui32NewRank )
{
	UINT32 ui32Temp;

	ui32Temp = ui32OldRank;
	ui32OldRank = ui32NewRank;
	ui32NewRank = ui32Temp;

	// 중계서버에도 계급 변경 통보
	
	if (0 < m_ui8FriendCount)
	{
		g_pModuleTrans->AuthSendChangeRank(i32WorkThreadIdx, this, ui32NewRank);
	}
	
	// 클랜서버에도 계급 변경 통보
	if (0 < m_UserInfoBasic._clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < m_UserInfoBasic._clanstate)
	{
		g_pModuleClan->ClanSendChangeRank(i32WorkThreadIdx, this, (UINT8)ui32NewRank);
	}

	// 메신저서버에도 계급 변경 통보
	g_pModuleMessenger->MessengerChangeRank(i32WorkThreadIdx, this);

	// 룸에도 계급 변경 통보
	if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
	{
		m_pRoom->SendChangeRank(m_i32SlotIdx);
	}

	// 계급 변경 전달
	i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_MYINFO_BASIC_ACK ); 
	SendPacket.WriteData(&m_UserInfoBasic, sizeof(USER_INFO_BASIC));
	SendPacketMessage(&SendPacket);
}

void CUserSession::SetRankUp(INT32 i32WorkThreadIdx)
{
	UINT32 ui32Rank = m_UserInfoBasic._rank;

	// 임시 계급을 사용 중이 아닐때에만 진급처리
	if( -1 == GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_RANK, CASHITEM_DAY_NO) ) )
	{
		// 클랜서버에도 계급 변경 통보
		if (0 < m_UserInfoBasic._clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < m_UserInfoBasic._clanstate)
		{
			g_pModuleClan->ClanSendChangeRank(i32WorkThreadIdx, this, (UINT8)ui32Rank);
		}

		// 메신저서버에도 계급 변경 통보
		g_pModuleMessenger->MessengerChangeRank(i32WorkThreadIdx, this);
	}	
}

void CUserSession::SetChangeColorNick( INT32 i32WorkThreadIdx, UINT8 ui8Color )
{
	if (0 < m_ui8FriendCount)
	{
		g_pModuleTrans->AuthSendChangeColorNick(i32WorkThreadIdx, this, ui8Color);
	}
	
	// 클랜서버에도 계급 변경 통보
	if (0 < m_UserInfoBasic._clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < m_UserInfoBasic._clanstate)
	{
		g_pModuleClan->ClanSendChangeColorNick(i32WorkThreadIdx, this, ui8Color );
	}

	// 메신저서버에도 계급 변경 통보
	g_pModuleMessenger->MessengerChangeColorNick(i32WorkThreadIdx, this);

	// 룸에도 계급 변경 통보
	if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
	{
		m_pRoom->SendChangeColorNick(m_i32SlotIdx);
	}

	// 계급 변경 전달
	i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_MYINFO_BASIC_ACK ); 
	SendPacket.WriteData(&m_UserInfoBasic, sizeof(USER_INFO_BASIC));
	SendPacketMessage(&SendPacket);
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
		switch( m_ui8LocaleCode )
		{
		case LOCALE_CODE_NONE			:		break;
		case LOCALE_CODE_KOREA			:		break;
		case LOCALE_CODE_THAILAND		:
			{
				// [howon.park |2013-06-05] : gametest31 , gametest32 는 핵실드 체크 안함
				if( (89375500 == m_i64UID) || (89375584 == m_i64UID) )	return TRUE;
			}
			break;
		case LOCALE_CODE_INDONESIA		:
			{
				// gametest01, gametest02 는 핵실드 체크 안함 
				if( (104218 == m_i64UID) || (104219 == m_i64UID) )		return TRUE;
			}
			break;
		case LOCALE_CODE_RUSSIA			:		
			{
				// Gametest01, Gametest02 는 핵실드 체크 안함 
				if( (4352083 == m_i64UID) || (4352084 == m_i64UID) )	return TRUE;	
			}
			break;
		case LOCALE_CODE_TURKEY			:		
			{
				// zepetto01, zepetto02 는 핵실드 체크 안함 
				if( (55089 == m_i64UID) || (217828 == m_i64UID) )		return TRUE;	
			}
			break;
		case LOCALE_CODE_CHINA			:		break;
		case LOCALE_CODE_TAIWAN			:		
			{
				// dgitPB000001, dgitPB000002 는 핵실드 체크 안함
				if( (30070	 == m_i64UID) || (6815	 == m_i64UID) )		return TRUE;	
			}
			break;
		case LOCALE_CODE_JAPAN			:		break;
		case LOCALE_CODE_BRAZIL			:	
			{
				// Gameest01, Gametest02는 핵실드 체크 안함
				if( m_i64UID == 2177408 || m_i64UID == 2177438 )
					return TRUE;
			}
			break;
		case LOCALE_CODE_LATIN_AMERICA	:
			{
				// FHL-991, FHL-992 는 핵실드 체크 안함 
				if( (328 == m_i64UID) || (504173 == m_i64UID) )			return TRUE;
			}
			break;
		case LOCALE_CODE_NORTHAMERICA	:		
			{
				// PBDEVTEST01, PBDEVTEST02  는 핵실드 체크 안함 
				if( (59 == m_i64UID) || (64 == m_i64UID) )				return TRUE;	
			}
			break;
		case LOCALE_CODE_ITALY			:	
			{
				// zepetto1000, zepetto1001  는 핵실드 체크 안함 
				if( (1019897 == m_i64UID) || (1019900 == m_i64UID) )	return TRUE;	
			}
			break;
		case LOCALE_CODE_MIDDLE_EAST	:		
			{
				//pbtestuser001, pbtestuser002 는 핵실드 체크 안함.
				if( (1 == m_i64UID) || (4 == m_i64UID) )				return TRUE;	
			}
			break;
		case LOCALE_CODE_PHILIPPINES	:		
			{
				// pbtest1, pbtest2 는 핵실드 체크 안함.
				if( (54572 == m_i64UID) || (54573 == m_i64UID) )		return TRUE;	
			}	
			break;
		case LOCALE_CODE_MATCH			:		break;
		}

		if( m_ui32GameGuardSendST > m_ui32GameGuardRecvST )
		{
			if( m_ui32GameGuardRecvST < ui32GameGuardTimeOut )
			{
#if defined( USE_NPROTECT ) || defined(USE_XTRAP )
#else
				_SendErrorMessage( MAIN_THREAD_IDX, EVENT_ERROR_GAMEGUARD_ERROR, 0, KILL_REASON_GAME_GUARD );
				return FALSE;
#endif
			}
		}
#if defined( USE_NPROTECT )
		 if( m_ui32GameGuardSendST < ui32GameGuardSendTime )
#else
		else if( m_ui32GameGuardSendST < ui32GameGuardSendTime )
#endif
		{
			SendGameGuardPacket_MU( MAIN_THREAD_IDX );
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
			#if defined( USE_AHNHS )
				// 해당 유저의 핸들을 이용해 요청 메세지 작성
				// 각 유저의 요청 메세지는 해당 유저의 핸들을 이용해서 작성
				unsigned long ulRet = 0;

				ulRet = _AhnHS_MakeRequest( m_hClient, &m_stRequestBuf );

				if ( ERROR_SUCCESS != ulRet )
				{
					char szTemp[ MAX_PATH ];
					i3String::Format( szTemp, MAX_PATH, "[ERROR]_AhnHS_MakeRequest ulRet:%x", ulRet );
					g_pModuleLogFile->Write_M_Log( i32WorkThread, this, szTemp );
				}
				else
				{
					// 3 요청 메시지를 클라이언트로 전송합니다.
					i3NetworkPacket GameGuardPack( PROTOCOL_BASE_GAMEGUARD_REQ ); 
					GameGuardPack.WriteData( &m_stRequestBuf.nLength, sizeof( UINT16 ) );
					GameGuardPack.WriteData( m_stRequestBuf.byBuffer, m_stRequestBuf.nLength );
					SendPacketMessage( &GameGuardPack );
				}
			#endif
			}
			break;

		case GAMEGUARD_XTRAP:
			{
			#if defined( USE_XTRAP )
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
						char strTemp[ MAX_PATH ];
						i3String::Format( strTemp, MAX_PATH, "[ERROR]XTrap_CS_Step1 / res : %x", ui32Res );
						g_pModuleLogFile->Write_M_Log( i32WorkThread, this, strTemp );
						_SendErrorMessage( i32WorkThread, ui32Res, 0, KILL_REASON_GAME_GUARD );
					}
					break;
				}
			#endif
			}
			break;
		case GAMEGUARD_GPK:
			{
			#if defined( USE_GPK )
				UINT8			ui8Len;
				unsigned char	strData[ GPK_SETINFO_PACKETBUFF_SIZE ];
				//ZeroMemory(strData,128);
				if(FALSE == g_pSvrDynCode->Encode( strData, GPK_SETINFO_PACKETBUFF_SIZE, m_i32CodeIdx))
				{
					strData[ 127 ] = '\0';

					char strTemp[ MAX_PATH ];
					sprintf_s(strTemp, MAX_PATH, "[GPK][Encode Error] pData[4](%c), cord(%s)",strData[4], strData ); 
					g_pModuleLogFile->Write_M_Log( i32WorkThread, this, strTemp );
				}
				else
				{
					/*char strTemp[ MAX_PATH ];
					sprintf_s(strTemp, MAX_PATH, "[GPK][Encode] cord(%s)",strData); 
					g_pModuleLogFile->Write_M_Log( i32WorkThread, this, strTemp );		*/			

					i3NetworkPacket GameGuardPack( PROTOCOL_BASE_GAMEGUARD_REQ ); 
					GameGuardPack.WriteData( strData, GPK_SETINFO_PACKETBUFF_SIZE );
					SendPacketMessage(&GameGuardPack); 
				}
			#endif
			}
			break;
	}

	m_ui32GameGuardSendST = g_pContextMain->GetServerTime();
	m_ui32GameGuardRecvST = m_ui32GameGuardSendST-1;

	return; 
}

void CUserSession::SendClanMemberInfo( UINT8 ui8TotalCount, CLANMEMBER_INFO_STRUCT* pClanMember )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MEMBER_INFO_ACK );
	SendPacket.WriteData( &ui8TotalCount,		sizeof(UINT8) );

	for( UINT8 i = 0 ; i < ui8TotalCount ; i++ )
	{
		SendPacket.WriteData( &pClanMember[i]._ui8NickSize,	sizeof(UINT8) );
		SendPacket.WriteData( pClanMember[i]._strNickName,	pClanMember[i]._ui8NickSize );
		SendPacket.WriteData( &pClanMember[i]._BasicInfo,	sizeof(CLANMEMBER_INFO) );
	}

	SendPacketMessage(&SendPacket);
}

void CUserSession::SendClanMemberInfoChange( TUID i64UID, UINT64 ui64State )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MEMBER_INFO_CHANGE_ACK );
	SendPacket.WriteData( &i64UID,		sizeof(TUID) );
	SendPacket.WriteData( &ui64State,	sizeof(UINT64) );
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendClanMemberInfoInsert( CLANMEMBER_INFO_STRUCT* pClanMember )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MEMBER_INFO_INSERT_ACK );
	SendPacket.WriteData( &pClanMember->_ui8NickSize,		sizeof(UINT8) );
	SendPacket.WriteData( pClanMember->_strNickName,		pClanMember->_ui8NickSize );
	SendPacket.WriteData( &pClanMember->_BasicInfo,		sizeof(CLANMEMBER_INFO) );
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendClanMemberInfoDelete( TUID i64UID )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MEMBER_INFO_DELETE_ACK );
	SendPacket.WriteData( &i64UID,		sizeof(TUID) );
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendClanCreate_C(INT32 i32Result, CLAN_DETAIL_INFO * pDetailInfo)
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_CREATE_CLAN_ACK );
	SendPacket.WriteData( &i32Result,		sizeof(INT32) );

	if( i32Result > SERVER_VALUE_UNKNOWN )
	{//클랜 만들기 성공
		//DataBase에는 넣지 않음
		m_UserInfoBasic._clanidx		= pDetailInfo->_idx;
		m_UserInfoBasic._clanstate		= CLAN_MEMBER_LEVEL_MASTER;
		m_UserInfoBasic._clanExpRank	= pDetailInfo->_expRank;
		m_UserInfoBasic._clanPerRank	= pDetailInfo->_personRank;
		m_UserInfoBasic._clanMark		= pDetailInfo->_mark;
		m_UserInfoBasic._clanNamecolor	= pDetailInfo->_color;

		i3String::Copy( m_UserInfoBasic._clanName, pDetailInfo->_name, NET_CLAN_NAME_SIZE );

		INT32 i32SlotIdx = SERVER_VALUE_UNKNOWN;
		if( m_i32SlotIdx != SERVER_VALUE_UNKNOWN )		i32SlotIdx = m_i32SlotIdx;

		if( m_pRoom && m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
		{
			SLOT_INFO*	pSlotInfo =	m_pRoom->GetSlotInfo();
			if( pSlotInfo && i32SlotIdx != SERVER_VALUE_UNKNOWN )
			{
				pSlotInfo[ i32SlotIdx ]._clanidx		= pDetailInfo->_idx;
				pSlotInfo[ i32SlotIdx ]._clanstate		= CLAN_MEMBER_LEVEL_MASTER;
				pSlotInfo[ i32SlotIdx ]._clanExpRank	= pDetailInfo->_expRank;
				pSlotInfo[ i32SlotIdx ]._clanMark		= pDetailInfo->_mark;
			}
		}

		//포인트 줄이기 
		if(m_UserInfoBasic._point > (UINT32)g_pContextMain->m_i32ClanCreatePoint)
			m_UserInfoBasic._point -= g_pContextMain->m_i32ClanCreatePoint; 
		else 
			m_UserInfoBasic._point = 0; 		
		
		SendPacket.WriteData( pDetailInfo,	sizeof(CLAN_DETAIL_INFO) );
		SendPacket.WriteData( &m_UserInfoBasic._point ,	sizeof(UINT32) );

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
		m_UserInfoBasic._clanidx	= i32ClanIdx;							//가입되었음
		m_UserInfoBasic._clanstate	= CLAN_MEMBER_LEVEL_UNKNOWN;			//가입신청 대기중
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
		m_UserInfoBasic._clanidx	= 0;
		m_UserInfoBasic._clanstate	= CLAN_MEMBER_LEVEL_UNKNOWN;
		m_UserInfoBasic._clanMark	= 0xFFFFFFFF;

		ChangeDataBaseInfo( SAVE_DATABASE_CLAN );
	}
	SendPacketMessage(&SendPacket); 
	return; 
}

/*-----------------------------------------------------------
Name : SendClanDetailInfo
Desc : 클랜 상세정보_클랜정보
-----------------------------------------------------------*/
void CUserSession::SendClanDetailInfo(INT32 i32Result, CLAN_DETAIL_INFO * pDetailInfo)
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_DETAIL_INFO_ACK );
	SendPacket.WriteData(&i32Result, sizeof(INT32));
	if( EV_SUCCESSED( i32Result ) )
	{
		SendPacket.WriteData(pDetailInfo, sizeof(CLAN_DETAIL_INFO));
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
	SendPacket.WriteData(&i32Result,		sizeof(INT32));
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
void CUserSession::SendClanChatingRequest( UINT8 ui8NickSize, char* pstrNick, UINT8 ui8ChatSize, char* pstrChating )
{
	INT32	i32State = CHAT_LIMIT_NORMAL;
	i3NetworkPacket SendPacket( PROTOCOL_CS_CHATTING_ACK );
	SendPacket.WriteData(&i32State,		sizeof(INT32) );
	SendPacket.WriteData(&ui8NickSize,	sizeof(UINT8) );
	SendPacket.WriteData(pstrNick,		ui8NickSize );	
	SendPacket.WriteData(&ui8ChatSize,	sizeof(UINT8) );
	SendPacket.WriteData(pstrChating,	ui8ChatSize );

	SendPacketMessage(&SendPacket);

	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSession::SendClanPageChatingRequest( UINT8 ui8NickSize, char* pstrNick, UINT8 ui8ChatSize, char* pstrChating )
{
	INT32	i32State = CHAT_LIMIT_NORMAL;
	i3NetworkPacket SendPacket( PROTOCOL_CS_PAGE_CHATTING_ACK );
	SendPacket.WriteData(&i32State,		sizeof(INT32) );
	SendPacket.WriteData(&ui8NickSize,	sizeof(UINT8) );
	SendPacket.WriteData(pstrNick,		ui8NickSize );	
	SendPacket.WriteData(&ui8ChatSize,	sizeof(UINT8) );
	SendPacket.WriteData(pstrChating,	ui8ChatSize );

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
		m_UserInfoBasic._clanidx		= 0;
		m_UserInfoBasic._clanstate		= CLAN_MEMBER_LEVEL_UNKNOWN;
		m_UserInfoBasic._clanMark		= CLAN_MARK_DEFAULT;
		m_UserInfoBasic._clanName[0]	= '\0';

		INT32 i32SlotIdx = SERVER_VALUE_UNKNOWN;
		if( m_i32SlotIdx != SERVER_VALUE_UNKNOWN )		i32SlotIdx = m_i32SlotIdx;

		if( m_pRoom && m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
		{
			SLOT_INFO*	pSlotInfo =	m_pRoom->GetSlotInfo();
			if( pSlotInfo && i32SlotIdx != SERVER_VALUE_UNKNOWN )
			{
				pSlotInfo[ i32SlotIdx ]._clanidx		= 0;
				pSlotInfo[ i32SlotIdx ]._clanstate	= CLAN_MEMBER_LEVEL_UNKNOWN;
				pSlotInfo[ i32SlotIdx ]._clanMark		= CLAN_MARK_DEFAULT;
				pSlotInfo[ i32SlotIdx ]._ClanName[0]	= '\0';
			}
		}
		ChangeDataBaseInfo( SAVE_DATABASE_CLAN );
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
		m_UserInfoBasic._clanidx	= 0;
		m_UserInfoBasic._clanstate	= CLAN_MEMBER_LEVEL_UNKNOWN;
		m_UserInfoBasic._clanMark	= 0xFFFFFFFF;

		INT32 i32SlotIdx = SERVER_VALUE_UNKNOWN;
		if( m_i32SlotIdx != SERVER_VALUE_UNKNOWN )		i32SlotIdx = m_i32SlotIdx;

		if( m_pRoom && m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
		{
			SLOT_INFO*	pSlotInfo =	m_pRoom->GetSlotInfo();
			if( pSlotInfo && i32SlotIdx != SERVER_VALUE_UNKNOWN )
			{
				pSlotInfo[ i32SlotIdx ]._clanidx		= 0;
				pSlotInfo[ i32SlotIdx ]._clanstate	= CLAN_MEMBER_LEVEL_UNKNOWN;
				pSlotInfo[ i32SlotIdx ]._clanMark		= 0xFFFFFFFF;
				pSlotInfo[ i32SlotIdx ]._ClanName[0]	= '\0';
			}
		}
		ChangeDataBaseInfo( SAVE_DATABASE_CLAN );
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
		m_UserInfoBasic._clanstate	= CLAN_MEMBER_LEVEL_STAFF;
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

	m_UserInfoBasic._clanExpRank = ui8Rank;
	
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

	m_UserInfoBasic._clanPerRank = ui8Rank;
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

	m_UserInfoBasic._clanMark = ui32Result;

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
void CUserSession::SendClanReplaceNameResultRequest( char* pstrName )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_NAME_RESULT_ACK );
	SendPacket.WriteData(pstrName, NET_CLAN_NAME_SIZE);
	SendPacketMessage(&SendPacket);

	i3String::Copy( m_UserInfoBasic._clanName, pstrName, NET_CLAN_NAME_SIZE );

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
void CUserSession::SendClanReplacePersonMaxRequest( UINT8 ui8ClanMaxCount )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_REPLACE_PERSONMAX_ACK );
	SendPacket.WriteData(&ui8ClanMaxCount, sizeof(UINT8));
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

	m_UserInfoBasic._clanidx		= pDetailInfo->_idx;
	m_UserInfoBasic._clanstate		= CLAN_MEMBER_LEVEL_REGULAR;
	m_UserInfoBasic._clanExpRank	= pDetailInfo->_expRank;
	m_UserInfoBasic._clanPerRank	= pDetailInfo->_personRank;
	m_UserInfoBasic._clanMark		= pDetailInfo->_mark;
	m_UserInfoBasic._clanNamecolor	= pDetailInfo->_color;

	i3String::Copy( m_UserInfoBasic._clanName, pDetailInfo->_name, NET_CLAN_NAME_SIZE );


	INT32 i32SlotIdx = SERVER_VALUE_UNKNOWN;
	if( m_i32SlotIdx != SERVER_VALUE_UNKNOWN )		i32SlotIdx = m_i32SlotIdx;

	//대기방에 있는경우 슬롯정보를 갱신합니다.
	if( m_pRoom && m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
	{
		SLOT_INFO*	pSlotInfo =	m_pRoom->GetSlotInfo();
		if( pSlotInfo && i32SlotIdx != SERVER_VALUE_UNKNOWN )
		{
			pSlotInfo[ i32SlotIdx ]._clanidx		= pDetailInfo->_idx;
			pSlotInfo[ i32SlotIdx ]._clanstate		= CLAN_MEMBER_LEVEL_REGULAR;
			pSlotInfo[ i32SlotIdx ]._clanExpRank	= pDetailInfo->_expRank;
			pSlotInfo[ i32SlotIdx ]._clanMark		= pDetailInfo->_mark;

			//대기방의 모든유저에게 가입한 클랜정보를 뿌린다.
			i3NetworkPacket Packet(PROTOCOL_ROOM_GET_SLOTONEINFO_ACK);
			Packet.WriteData( &i32SlotIdx,								sizeof(INT32) );
			Packet.WriteData( &m_pRoom->m_Info._SlotInfo[i32SlotIdx],	sizeof(SLOT_INFO) );
			Packet.WriteData( m_UserInfoBasic._nick,					NET_NICK_NAME_SIZE );
			Packet.WriteData( &m_UserInfoBasic._nickcolor,				sizeof(UINT8) );
			m_pRoom->SendAnnounceMessage( &Packet );
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

	m_UserInfoBasic._clanidx	= 0;
	m_UserInfoBasic._clanstate	= CLAN_MEMBER_LEVEL_UNKNOWN;

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

	m_UserInfoBasic._clanidx	= 0;
	m_UserInfoBasic._clanstate	= CLAN_MEMBER_LEVEL_UNKNOWN;
	m_UserInfoBasic._clanMark	= 0xFFFFFFFF;

	INT32 i32SlotIdx = SERVER_VALUE_UNKNOWN;
	if( m_i32SlotIdx != SERVER_VALUE_UNKNOWN )		i32SlotIdx = m_i32SlotIdx;

	if( m_pRoom && m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
	{
		SLOT_INFO*	pSlotInfo =	m_pRoom->GetSlotInfo();
		if( pSlotInfo && i32SlotIdx != SERVER_VALUE_UNKNOWN )
		{
			pSlotInfo[ i32SlotIdx ]._clanidx		= 0;
			pSlotInfo[ i32SlotIdx ]._clanstate		= CLAN_MEMBER_LEVEL_UNKNOWN;
			pSlotInfo[ i32SlotIdx ]._clanMark		= 0xFFFFFFFF;
			pSlotInfo[ i32SlotIdx ]._ClanName[0]	= '\0';
		}
	}
	ChangeDataBaseInfo( SAVE_DATABASE_CLAN );
	
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
	m_UserInfoBasic._clanstate	= CLAN_MEMBER_LEVEL_MASTER;	
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
	m_UserInfoBasic._clanstate	= CLAN_MEMBER_LEVEL_STAFF;
	
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
	m_UserInfoBasic._clanstate	= CLAN_MEMBER_LEVEL_REGULAR;

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
void CUserSession::SendClanRoomInvitedResultRequest( TUID TSenderUID )	
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_ROOM_INVITED_RESULT_ACK );
	SendPacket.WriteData(&TSenderUID, sizeof(TUID));
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

	m_UserInfoBasic._clanNamecolor = ui8Color;

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
// Name : SendClanMatchTeamListRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchTeamListRequest( UINT8 ui8TotalCount, UINT8 ui8SectionCount, UINT8 ui8Section, UINT8 ui8Count, CLAN_BATTLE_TEAM_LIST* pBattleTeamList )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_TEAM_LIST_ACK );
	SendPacket.WriteData(&ui8TotalCount,		sizeof(UINT8));
	if( 0 < ui8TotalCount )
	{
		SendPacket.WriteData(&ui8SectionCount,	sizeof(UINT8));
		SendPacket.WriteData(&ui8Section,		sizeof(UINT8));
		SendPacket.WriteData(&ui8Count,			sizeof(UINT8));
		if( 0 < ui8Count )
		{
			SendPacket.WriteData(pBattleTeamList,	sizeof(CLAN_BATTLE_TEAM_LIST) * ui8Count );
		}
	}
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchAllTeamListRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchAllTeamListRequest( UINT16 ui16TotalCount, UINT16 ui16SectionCount, UINT16 ui16Section, UINT8 ui8Count, CLAN_BATTLE_TEAM_INFO* pBattleTeamInfo )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK );
	SendPacket.WriteData(&ui16TotalCount,		sizeof(UINT16));
	SendPacket.WriteData(&ui16SectionCount,		sizeof(UINT16));
	SendPacket.WriteData(&ui16Section,			sizeof(UINT16));
	SendPacket.WriteData(&ui8Count,				sizeof(UINT8));
	SendPacket.WriteData(pBattleTeamInfo,		sizeof(CLAN_BATTLE_TEAM_INFO) * ui8Count );
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
	SendPacket.WriteData(&i32Success,		sizeof(INT32));
	SendPacket.WriteData(&ui32Idx,			sizeof(UINT32));
	SendPacket.WriteData(pTeam,				sizeof(CLAN_BATTLE_TEAM));
	SendPacketMessage(&SendPacket);

	if( EV_SUCCESSED( i32Success ) )
	{
		m_bMatchRoom = TRUE;
	}
}

// -----------------------------------------------------------
// Name : SendClanMatchTeamJoinRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchTeamJoinRequest( INT32 i32Success, CLAN_BATTLE_TEAM_INFO* pTeamInfo, CLAN_BATTLE_MEMBER* pMemberInfo )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_TEAM_JOIN_ACK );
	SendPacket.WriteData(&i32Success,	sizeof(INT32));
	SendPacket.WriteData(pTeamInfo,		sizeof( CLAN_BATTLE_TEAM_INFO ));
	SendPacket.WriteData(pMemberInfo,	sizeof(CLAN_BATTLE_MEMBER) * pTeamInfo->_Team._PerMax );
	SendPacketMessage(&SendPacket);

	if( EV_SUCCESSED( i32Success ) )
	{
		m_bMatchRoom = TRUE;
	}
}

// -----------------------------------------------------------
// Name : SendClanMatchTeamLeaveRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchTeamLeaveRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_TEAM_LEAVE_ACK );
	SendPacket.WriteData(&i32Result,		sizeof(INT32));
	SendPacketMessage(&SendPacket);

	if( EV_SUCCESSED( i32Result ) )
	{
		m_bMatchRoom = FALSE;;
	}
}

// -----------------------------------------------------------
// Name : SendClanMatchTeamChangePerRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchTeamChangePerRequest( INT32 i32Result, UINT8 ui8PerMax )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK );
	SendPacket.WriteData(&i32Result,		sizeof(INT32));
	SendPacket.WriteData(&ui8PerMax,		sizeof(UINT8));
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchTeamChangePerResultRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchTeamChangePerResultRequest( UINT8 ui8PerMax )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_RESULT_ACK );
	SendPacket.WriteData(&ui8PerMax,		sizeof(UINT8));
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchTeamInfoRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchTeamInfoRequest( CLAN_BATTLE_TEAM* pTeam, CLAN_BATTLE_MEMBER* pMemberInfo )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_TEAM_INFO_ACK );
	SendPacket.WriteData(pTeam,			sizeof(CLAN_BATTLE_TEAM));
	SendPacket.WriteData(pMemberInfo,	sizeof(CLAN_BATTLE_MEMBER) * pTeam->_PerMax);
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchFightRequestRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchFightRequestRequest( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK );
	SendPacket.WriteData(&i32Result,		sizeof(INT32));
	SendPacketMessage(&SendPacket);
}

// -----------------------------------------------------------
// Name : SendClanMatchChatingRequest
// Desc : 
// -----------------------------------------------------------
void CUserSession::SendClanMatchChatingRequest(UINT8 ui8NickSize, char* pstrNick, UINT8 ui8ChatSize, char* pstrChating )
{
	INT32	i32State = CHAT_LIMIT_NORMAL;
	i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_CHATING_ACK );
	SendPacket.WriteData(&i32State,			sizeof(INT32));
	SendPacket.WriteData(&ui8NickSize,		sizeof(UINT8));
	SendPacket.WriteData(pstrNick,			ui8NickSize);
	SendPacket.WriteData(&ui8ChatSize,		sizeof(UINT8));	
	SendPacket.WriteData(pstrChating,		ui8ChatSize);
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
	SendPacket.WriteData( &ui32RoomIdx,			sizeof(UINT32) );
	SendPacket.WriteData( &ui32MatchTeam,		sizeof(UINT32) );
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
		NOTE_BODY* pbody = &pnoteBodyList[i];
		
		SendPacket.WriteData(&pbody->_senderSize, sizeof(UINT8));
		SendPacket.WriteData(&pbody->_messageSize, sizeof(UINT8));
		SendPacket.WriteData(pbody->_sender, pbody->_senderSize);
		if( 0 < pbody->_messageSize )
		{
            SendPacket.WriteData(pbody->_message, pbody->_messageSize);
		}
		else
		{
			SendPacket.WriteData(&pbody->_message[0], sizeof(UINT8));
			SendPacket.WriteData(&pbody->_message[1], pbody->_message[0]);
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

void CUserSession::SendMessengerReceiveNote(NOTE_HEADER noteHeader, UINT8 ui8senderSize, UINT8 ui8messageSize, char * psender, char * pstrmessage)
{
	i3NetworkPacket SendPacket(PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK);
	SendPacket.WriteData(&noteHeader,		sizeof(NOTE_HEADER));
	SendPacket.WriteData(&ui8senderSize,	sizeof(UINT8));
	SendPacket.WriteData(&ui8messageSize,	sizeof(UINT8));
	SendPacket.WriteData(psender,			ui8senderSize);
	if( 0 < ui8messageSize )
	{
		SendPacket.WriteData(pstrmessage, ui8messageSize);
	}
	else
	{
		SendPacket.WriteData(&pstrmessage[0], sizeof(UINT8));
		SendPacket.WriteData(&pstrmessage[1], pstrmessage[0]);
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
		m_UserInfoBasic._cash	= ui32Cash;

		packet.WriteData(&m_UserInfoBasic._point,	sizeof(UINT32));
		packet.WriteData(&m_UserInfoBasic._cash,	sizeof(UINT32));
	}

	SendPacketMessage(&packet);
}

void CUserSession::SendGetRemainMoneyNCSA( INT32 i32Result, UINT32 ui32Point, UINT32 ui32NCCoin, UINT32 ui32HappyCoin )
{	
	i3NetworkPacket packet(PROTOCOL_AUTH_GET_POINT_CASH_NCSA_ACK);

	packet.WriteData(&i32Result,	sizeof(INT32));

	if (EV_SUCCESSED(i32Result))
	{
		// Point 는 게임서버에서 관리하기 때문에 더해줄 필요가 없습니다.
		m_UserInfoBasic._cash		= ui32NCCoin + ui32HappyCoin;
		m_UserInfoBasic._NCCoin		= ui32NCCoin;
		m_UserInfoBasic._HappyCoin	= ui32HappyCoin;

		packet.WriteData(&m_UserInfoBasic._point,	sizeof(UINT32));
		packet.WriteData(&ui32NCCoin,				sizeof(UINT32));
		packet.WriteData(&ui32HappyCoin,			sizeof(UINT32));
	}

	SendPacketMessage(&packet);
}

void CUserSession::SendGetPoint( UINT32 ui32Point, 	BOOL	bIsRSJackPot )
{	
	if( 0 == ui32Point )	return; 

	m_UserInfoBasic._point	+= ui32Point;
	ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO);

	INT32 i32GetRoute	= 3;
	g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_GET_POINT, this, &ui32Point, &i32GetRoute );
	
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_PLUS_POINT_ACK );
	SendPacket.WriteData(&ui32Point,				sizeof(UINT32));
	SendPacket.WriteData(&m_UserInfoBasic._point,	sizeof(UINT32));
	SendPacket.WriteData(&bIsRSJackPot,				sizeof(BOOL));
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendCheckNickName( INT32 i32Rv )
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_ACK );
	SendPacket.WriteData(&i32Rv,				sizeof(UINT32));
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendNickName( INT32 i32WorkIdx, char* pstrNickName )
{	
	if( '\0' == *pstrNickName )	return;

	char strNickTemp[ NET_NICK_NAME_SIZE ];

	i3String::Copy( strNickTemp, m_UserInfoBasic._nick, NET_NICK_NAME_SIZE );
	i3String::Copy( m_UserInfoBasic._nick, pstrNickName, NET_NICK_NAME_SIZE );

	ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO);
	
	// 클랜서버에 닉네임 변경 통보
	g_pModuleClan->ClanSendChangeNickname( i32WorkIdx, this, m_UserInfoBasic._nick );
	// 메신저서버에 닉네임 변경 통보
	g_pModuleMessenger->MessengerChangeNickname( i32WorkIdx, this );

	// 룸에도 닉네임 변경 통보
	if( SERVER_VALUE_UNKNOWN < m_i32RoomIdx )
	{
		m_pRoom->SendChangeNickname(m_i32SlotIdx);
	}

	g_pModuleContextNC->LogSendMessage( i32WorkIdx, LOG_MESSAGE_U_CHANGE_NICK, this, strNickTemp ); 

	// 클라이언트에 전달
	UINT8 ui8Size = (UINT8)i3String::Length( m_UserInfoBasic._nick ) + 1;
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_CHANGE_NICKNAME_ACK );
	SendPacket.WriteData(&ui8Size,				sizeof(UINT8));
	SendPacket.WriteData(m_UserInfoBasic._nick,	ui8Size);
	SendPacketMessage(&SendPacket);
}

void CUserSession::SendUserRecord( USER_INFO_RECORD* pUserRecord )
{
	i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_RECORD_INFO_DB_ACK );
	SendPacket.WriteData(pUserRecord,		sizeof(USER_INFO_RECORD));
	SendPacketMessage(&SendPacket);
}

INT32	CUserSession::GetCheckCashitem( UINT32 ui32ItemNum )
{
	for(INT32 itemIdx = 0; itemIdx < m_i32InvItemCount; itemIdx++)
	{
		if (m_aInvItem[itemIdx]._ui32ItemID == ui32ItemNum ) return itemIdx;
	}
	//찾을 수 없다.
	return -1;
}

// -----------------------------------------------------------
// Name : LogIn_MA
// Desc : 로그인 결과값입니다.
// -----------------------------------------------------------
void CUserSession::LogIn_MAU( INT32 i32WorkThread, INT32 i32Rv, TUID i64UID, char* pstrId, char* pstrCustom, char* pstrUID, UINT8 ui8LinkThreadIdx, UINT32 ui32LoginDelayTime, UINT8 ui8Popup )
{
	//1. 로그인을 요청했는지
	if( 0 == m_ui32LogInST )
	{	// 해킹으로 간주하고 끊어버립니다.
		_SendErrorMessage( i32WorkThread, EVENT_ERROR_LOGIN, 0, KILL_REASON_HACK_PACKET );
	}
	else
	{
		if( EV_SUCCESSED( i32Rv ) )
		{		
			m_i64UID	= i64UID;	//정상 로그인
			i3mem::Copy( m_strId, pstrId, NET_ID_SIZE );

			// Trans 로 부터 Login ACK 를 받았을때만 메신저와 Clan 에 로그인 성공 패킷을 보냅니다.
			// 쓰레드 주의해 주세요.	
			// ClanServer LogIn - 위치 변경하지 말아야 합니다.
			if( FALSE == g_pModuleClan->ClanSendLogIn( i32WorkThread, this ) )
			{
				m_abGetInfoToServer[ GET_INFO_TO_SERVER_CLAN ] = true;
			}

			m_bCheckLogout = FALSE;	
			SetPosition(GAME_TASK_LOGIN);

			if( DEF_TRANS_LINK_COUNT_MAX > ui8LinkThreadIdx )
			{
				g_pUserManager->m_aui32LinkLoginCount[ ui8LinkThreadIdx ]++;
				g_pUserManager->m_aui32LinkLoginTimeArg[ ui8LinkThreadIdx ] += ui32LoginDelayTime;
				g_pUserManager->m_aui32LinkLoginTimeArg[ ui8LinkThreadIdx ] /= 2;
			}

			m_ui32ATLastRecv			= g_pContextMain->GetServerTime();
			m_ui32GameGuardSendST			= g_pContextMain->GetServerTime();
			m_ui32GameGuardRecvST			= m_ui32GameGuardSendST;
		}

		i3NetworkPacket SendPacket( PROTOCOL_BASE_LOGIN_ACK );
		SendPacket.WriteData(&i32Rv,	sizeof(INT32));
		SendPacket.WriteData(&ui8Popup,	sizeof(UINT8));
		SendPacket.WriteData(&i64UID,	sizeof(TUID));
	
		UINT8	ui8CustomSize;
		UINT8	ui8UIDSize;
		UINT8	ui8IDSize;

		if( NULL == pstrId ) ui8IDSize = 0;
		else ui8IDSize = i3String::Length( pstrId );

		if( NULL == pstrCustom ) ui8CustomSize = 0;
		else ui8CustomSize = i3String::Length( pstrCustom );

		if( NULL == pstrUID ) ui8UIDSize = 0;
		else ui8UIDSize = i3String::Length( pstrUID );
		
		SendPacket.WriteData(&ui8IDSize,	sizeof(UINT8) );
		if( ui8IDSize > 0 )
			SendPacket.WriteData(pstrId,		ui8IDSize );

		SendPacket.WriteData(&ui8CustomSize,	sizeof(UINT8) );
		if( ui8CustomSize > 0 )
			SendPacket.WriteData(pstrCustom,	ui8CustomSize );

		SendPacket.WriteData(&ui8UIDSize,	sizeof(UINT8) );
		if( ui8UIDSize > 0 )
			SendPacket.WriteData(pstrUID,		ui8UIDSize );

		SendPacketMessage( &SendPacket );
	}

	m_ui32LogInST		= 0;
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
	m_abGetInfoToServer[ GET_INFO_TO_SERVER_BASIC ] = true;
	SendMyInfo( i32WorkThread, i32Rv );
}

void CUserSession::SendMyInfo( INT32 i32WorkThread, INT32 i32Result )
{
	if( EV_FAILED( i32Result ) )
	{
		m_ui32ATInfo = 0;

		i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_MYINFO_ACK );	
		SendPacket.WriteData( &i32Result,			sizeof(INT32) );
		SendPacketMessage( &SendPacket );
		return;
	}

	for( INT32 i = 0 ; i < GET_INFO_TO_SERVER_MAX ; i++ )
	{
		if( false == m_abGetInfoToServer[ i ] )	return;
	}

	m_bMyinfo		= TRUE;
	m_ui32ATInfo	= 0;

	UINT8 ui8RecvInventory = 1;
// 각 서버에 로그인 패킷을 보냅니다.
// NC Login Message
	g_pModuleContextNC->LogSendMessage(i32WorkThread, LOG_MESSAGE_U_LOGIN, this );		//LOG_D
	// MessengerServer Login
	g_pModuleMessenger->MessengerLogin( i32WorkThread , this );

	CheckGeneralRankUp(i32WorkThread);

	// Set Packet
	INT32 i32Rv = EVENT_ERROR_SUCCESS;
	i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_MYINFO_ACK );	
	SendPacket.WriteData( &i32Rv,										sizeof(INT32) );
	if( EV_SUCCESSED( i32Rv ) )
	{
		// 원래 PROTOCOL_BASE_LOGIN_ACK 입니다.
		SendPacket.WriteData( &m_ui8Age,								sizeof(UINT8) );									// 나이

		// 원래 PROTOCOL_BASE_GET_MYINFO_ACK 입니다.
		SendPacket.WriteData( &m_UserInfoBasic,							sizeof(USER_INFO_BASIC)	);							// User 기본 정보
		SendPacket.WriteData( &m_UserInfoRecord,						sizeof(USER_INFO_RECORD) );							// User 전적 정보
		SendPacket.WriteData( &m_UserInfoEquipmentChara,				sizeof(USER_INFO_EQUIPMENT_CHARA) );				// User 장비 정보 Chara
		SendPacket.WriteData( &m_UserInfoEquipmentWeapon,				sizeof(USER_INFO_EQUIPMENT_WEAPON) );				// User 장비 정보 Weapon
		SendPacket.WriteData( &m_UserInfoInvItemData,					sizeof(USER_INFO_INVITEM_DATA) );					// 상용화 아이템 정보

		SendPacket.WriteData( &m_ui8OtherCountry,						sizeof(UINT8) );									// 접속자 국가코드 (GeoIP2기준)

		SendPacket.WriteData( &ui8RecvInventory,						sizeof(UINT8) );									// Inventory  받았는지 체크
		if( ui8RecvInventory )
		{
			SendPacket.WriteData( &m_i32InvCharaCount,					sizeof(INT32) );									// Inventory Char Count
			SendPacket.WriteData( &m_i32InvWeaponCount,					sizeof(INT32) );									// Inventory Weapon Count

			INVEN_BUFFER	aInvItem[MAX_INVEN_COUPON_COUNT];
			INVEN_BUFFER	aInvCoupon[MAX_COUPON_INVEN_COUNT];
			INT32			i32ItemCount = 0;
			INT32			i32CouponCount = 0;
			char			szTemp[ MAX_PATH ];

			for( INT32 i = 0; i < m_i32InvItemCount; ++i )
			{
				if( GET_ITEM_FLAG_CLASS( m_aInvItem[i]._ui32ItemID ) == ITEM_CLASS_SALECOUPON )
				{
					if( i32CouponCount >= MAX_COUPON_INVEN_COUNT )
					{
						i3String::Format( szTemp, MAX_PATH, "[PROTOCOL_BASE_GET_MYINFO_ACK] SaleCounpon Count Over UID : %I64d", m_i64UID );
						g_pModuleLogFile->Write_M_Log(i32WorkThread, this, szTemp ) ; 	
						continue;	// 최대 개수를 초과한 경우 넘어갑니다.
					}
					aInvCoupon[ i32CouponCount ]._i64ItemWareDBIdx	= m_aInvItem[i]._i64ItemWareDBIdx;
					aInvCoupon[ i32CouponCount ]._ui32ItemArg		= m_aInvItem[i]._ui32ItemArg;
					aInvCoupon[ i32CouponCount ]._ui32ItemID		= m_aInvItem[i]._ui32ItemID;
					aInvCoupon[ i32CouponCount ]._ui8ItemType		= m_aInvItem[i]._ui8ItemType;
					++i32CouponCount;
				}
				else
				{
					if( i32ItemCount >= MAX_INVEN_COUNT )
					{
						i3String::Format( szTemp, MAX_PATH, "[ PROTOCOL_BASE_GET_MYINFO_ACK ] Error InvenItem Count Over UID : %I64d", m_i64UID );
						g_pModuleLogFile->Write_M_Log(i32WorkThread, this, szTemp ) ; 	
						continue;	// 최대 개수를 초과한 경우 넘어갑니다.
					}
					aInvItem[ i32ItemCount ]._i64ItemWareDBIdx	= m_aInvItem[i]._i64ItemWareDBIdx;
					aInvItem[ i32ItemCount ]._ui32ItemArg		= m_aInvItem[i]._ui32ItemArg;
					aInvItem[ i32ItemCount ]._ui32ItemID		= m_aInvItem[i]._ui32ItemID;
					aInvItem[ i32ItemCount ]._ui8ItemType		= m_aInvItem[i]._ui8ItemType;
					++i32ItemCount;
				}
			}

			if( i32CouponCount > MAX_COUPON_INVEN_COUNT )	i32CouponCount = MAX_COUPON_INVEN_COUNT;	// 최대 개수를 초과한 경우 재설정.
			if( i32ItemCount > MAX_INVEN_COUNT )			i32ItemCount = MAX_INVEN_COUNT;			// 최대 개수를 초과한 경우 재설정.

			SendPacket.WriteData( &i32ItemCount,						sizeof(INT32) );									// Inventory Item Count
			SendPacket.WriteData( &i32CouponCount,						sizeof(INT32) );									// Inventory Item Count

			SendPacket.WriteData( m_aInvChara,							(UINT16)(sizeof(INVEN_BUFFER)*m_i32InvCharaCount) );	// Inventory Char
			SendPacket.WriteData( m_aInvWeapon,							(UINT16)(sizeof(INVEN_BUFFER)*m_i32InvWeaponCount) );	// Inventory Weapon
			SendPacket.WriteData( aInvItem,								(UINT16)(sizeof(INVEN_BUFFER)*i32ItemCount) );			// Inventory Item
			SendPacket.WriteData( aInvCoupon,							(UINT16)(sizeof(INVEN_BUFFER)*i32CouponCount) );		// Inventory Coupon
		}


		SendPacket.WriteData( &g_pModuleTrans->m_ui8RSEnable,			sizeof(UINT8) );

		// 원래 PROTOCOL_BASE_MEDAL_GET_INFO_ACK 입니다.
		SendPacket.WriteData( &m_ui32Miniature,							sizeof(UINT32) );									// 약장
		SendPacket.WriteData( &m_ui32Insignia,							sizeof(UINT32) );									// 휘장
		SendPacket.WriteData( &m_ui32Order,								sizeof(UINT32) );									// 훈장
		SendPacket.WriteData( &m_ui32Master,							sizeof(UINT32) );									// 마스터훈장

		// 원래 PROTOCOL_BASE_QUEST_GET_INFO_ACK 입니다.
		SendPacket.WriteData( &m_QuestIngInfo,							sizeof(QUESTING_INFO) );							// 퀘스트 진행 상태
		SendPacket.WriteData( m_aaui8QuestCtxActive,					sizeof( m_aaui8QuestCtxActive ) );					// 퀘스트 완료 상태

		// 원래 PROTOCOL_BASE_USER_TITLE_INFO_ACK 입니다.
		SendPacket.WriteData( &m_ui64GainUserTitle,						sizeof(UINT64) );									// 획득한 호칭
		SendPacket.WriteData( m_aui8EquipUserTitle,						sizeof(m_aui8EquipUserTitle) );						// 장착한 호칭
		SendPacket.WriteData( &m_i32LimitUserTitleCount,				sizeof(INT32) );									// 사용가능 호칭 장비 슬롯 갯수


		SendPacket.WriteData( g_pContextMain->m_aStageDefaultActive,	sizeof(STAGE_ORDINAL_ID)*STAGE_TYPE_MAX);						// 기본 표시 스테이지
		SendPacket.WriteData( &g_pContextMain->m_ui8StageTotalCount,	sizeof(INT8) );													// DB 에서 로드된 총 스테이지 개수
		SendPacket.WriteData( &g_pContextMain->m_ui8StageBitCount,		sizeof(INT8) );													// 열릴 스테이지 개수
		SendPacket.WriteData( g_pContextMain->m_aui32StageBitActive,	sizeof(INT32)*g_pContextMain->m_ui8StageBitCount );				//열릴 스테이지
		SendPacket.WriteData( g_pContextMain->m_aTStageActiveOption,	sizeof(V_STAGEOPTION)*g_pContextMain->m_ui8StageTotalCount );	//열릴 스테이지 옵션
		SendPacket.WriteData( g_pContextMain->m_aui8StageMark,			sizeof(UINT8)*g_pContextMain->m_ui8StageTotalCount );			// 뉴핫 마크 적용 여부

		SendPacket.WriteData( &g_pContextMain->m_ui8ActiveQuestCount,	sizeof(UINT8));		
		SendPacket.WriteData( g_pContextMain->m_pui32ActiveQuest,		sizeof(UINT32)*g_pContextMain->m_ui8ActiveQuestCount);

		// Total PC Cafe Bonus
		REAL32 r32Exp, r32Point;
		_GetPCCafeBonusValue( m_UserInfoBasic._pcCafe, &r32Exp, &r32Point );
		SendPacket.WriteData( &r32Exp,									sizeof(REAL32) );									// PCCafe Bouns
		SendPacket.WriteData( &r32Point,								sizeof(REAL32) );
		
		// 중국 실명인증 값
		bool bTmp = false;
		INT16 i16Tmp = 20;
		char strTmp[20];
		SendPacket.WriteData( &bTmp,									sizeof(bool) );									// PCCafe Bouns
		SendPacket.WriteData( &i16Tmp,									sizeof(INT16) );
		SendPacket.WriteData( strTmp,									i16Tmp );
		
		SendPacket.WriteData( &m_bGMUser,								sizeof(BOOL)  );									// GM인지 확인
		SendPacket.WriteData( &m_bGetEventItem,							sizeof(BOOL)  );									// 이벤트 아이템 지급 여부
		SendPacket.WriteData( &g_pContextMain->m_bIsNewRepair,			sizeof(bool)  );									// 신규 수리 공식 적용 여부

		// 신고하기 - 스크린샷 기능 사용 - 서동권
		UINT32 ui32InvenTime = g_pContextMain->GetInvenTime();
		SendPacket.WriteData( &ui32InvenTime,							sizeof(UINT32) );
		SendPacket.WriteData( g_pContextMain->m_strReportFtpAddress,	MAX_STRING_COUNT );	// 서동권 - 신고하기 기능 스크린샷 전송 HTTP 주소
		SendPacket.WriteData( &g_pContextMain->m_ui32ReportFtpPort,		sizeof(UINT32) );
		SendPacket.WriteData( &g_pContextMain->m_bGiftEnable,			sizeof(bool) );

		// 이태리 빌링연동 Cash 충전 URL - 임준현
		SendPacket.WriteData( &g_pContextMain->m_ui16BillingURLLen,		sizeof(UINT16));
		SendPacket.WriteData( g_pContextMain->m_strBillingURL,			g_pContextMain->m_ui16BillingURLLen );

		// 북미 2차계정 소유여부 - 안국정
		SendPacket.WriteData( &m_bHasSID,								sizeof(bool));

		// 중남미 국기표시 기능 사용여부 - 안국정
		SendPacket.WriteData( &g_pContextMain->m_bNationalFlag,			sizeof(bool));
	}

	GeneralRankUp(i32WorkThread);
 

	// 자신에게 기본 정보를 전달
	SendPacketMessage( &SendPacket );
}

void CUserSession::_GetPCCafeBonusValue( UINT8 ui8PCCafe, REAL32* pr32Exp, REAL32* pr32Point )
{
	//PC방 추가 경험치, 포인트
	*pr32Exp	= g_pContextMain->m_stRoomBouns[ EVENT_ROOM_BONUS_CAFE + ui8PCCafe ]._r32Exp;
	*pr32Point	= g_pContextMain->m_stRoomBouns[ EVENT_ROOM_BONUS_CAFE + ui8PCCafe ]._r32Point;
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

// -----------------------------------------------------------
// Name : ServerEnter_A
// Desc : 새로운 서버에 접속합니다.
// -----------------------------------------------------------
void CUserSession::ServerEnter_A( INT32 i32Rv, i3NetworkPacket* pPacket )
{
	if( EV_SUCCESSED( i32Rv ) )
	{
		SetPosition( GAME_TASK_CHANNEL_LIST );

		pPacket->ReadData( &m_ui8LocaleCode, sizeof(UINT8) );
		pPacket->ReadData( m_strId,				NET_ID_SIZE );
		_GetMyInfoPacket( MODULE_TRANS_THREAD_IDX, pPacket, TRUE );

		// 메신저, 클랜서버에 서버이동알린다.
		g_pModuleMessenger->MessengerSendServerEnter(	MODULE_TRANS_THREAD_IDX, this );
		g_pModuleClan->ClanSendServerEnter( MODULE_TRANS_THREAD_IDX, this );

		m_bCheckLogout	= FALSE;
		m_bMyinfo		= TRUE; // 서버 이동으로 인해 추가합니다.
	}

	i3NetworkPacket SendPacket( PROTOCOL_BASE_USER_ENTER_ACK );	
	SendPacket.WriteData( &i32Rv,							sizeof(INT32));	

	SendPacketMessage( &SendPacket );

	I3TRACE( "ENTER USER ACK\n" );
}

// -----------------------------------------------------------
// Name : ServerLeave_A
// Desc : 서버에서 나갑니다.
// -----------------------------------------------------------
void CUserSession::ServerLeave_A( INT32 i32Rv )
{
	i3NetworkPacket SendPacket( PROTOCOL_BASE_USER_LEAVE_ACK );	
	SendPacket.WriteData( &i32Rv,	sizeof(INT32) );
	SendPacketMessage( &SendPacket );

	// m_iIsKillST 때문에 Send 다음에 위치해야 합니다.
	if( EV_SUCCESSED( i32Rv ) )
	{
		//서버를 이동한다는 메시지를 각 상위 서버에 보냄  -- 다시 생각해 봅시다. 꼭 보낼 필요가 있을지. 한가지 걸리는건 귓말이나, 찾아가기 초대하기 정도? -- 현우 일해라 --
		g_pModuleClan->ClanSendServerLeave( MODULE_TRANS_THREAD_IDX, this );
		g_pModuleMessenger->MessengerLeaveUser( MODULE_TRANS_THREAD_IDX, this );

		_SendErrorMessage( MODULE_TRANS_THREAD_IDX, EVENT_ERROR_EVENT , 0, KILL_REASON_SERVER_MOVE );
		m_i64UID		= SERVER_VALUE_UNKNOWN;											// DB 를 날림으로 유저가 이동할 것이라고 알려줍니다.
	}
}

// -----------------------------------------------------------
// Name : CreateNickName_A
// Desc : 닉네임이 생성 되었습니다.
// -----------------------------------------------------------
void CUserSession::CreateNickName_A( INT32 i32Rv, char* pstrNickName )
{
	i3NetworkPacket SendPacket( PROTOCOL_LOBBY_CREATE_NICK_NAME_ACK );
	SendPacket.WriteData( &i32Rv,	sizeof(INT32) );
	SendPacketMessage( &SendPacket );

	if( EV_SUCCESSED( i32Rv ) )
	{	// 신규유저 만들기 성공..		
		// 신규 유저 지금 무기
		i3String::Copy( m_UserInfoBasic._nick, pstrNickName, NET_NICK_NAME_SIZE );
		
		// 2011.04.14 - 국가별 튜토리얼 파일을 생성하여 읽어드리는 것으로 변동되어 유저의 국가에 맞는 튜토리얼 퀘스트 파일을 배포해야 합니다. - 서동권

		//// 튜토리얼 임무카드 
		g_pContextMain->GetQuest( QUEST_CARDSET_TYPE_TUTORIAL, TUTORIAL_CARDSET_SLOT, &m_QuestIngInfo, m_aaui8QuestCtxActive[ TUTORIAL_CARDSET_SLOT ], m_ui8LocaleCode );
		_SendQuestInfoPacket( TUTORIAL_CARDSET_SLOT );

		//SendGetPoint( NEW_ENTER_USER_POINT );
		INT32 i32GetRoute = 4;
		g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_GET_POINT, this, &m_UserInfoBasic._point, &i32GetRoute );
		g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_CREATE_NICK, this );

		//Zepetto Module에 통보 합니다.				
		g_pModuleMessenger->MessengerChangeNickname( MODULE_TRANS_THREAD_IDX, this );
		// MessengerServer Login
		g_pModuleMessenger->MessengerLogin(MODULE_TRANS_THREAD_IDX, this);
		// 친구 목록 초기화
		g_pModuleMessenger->MessengerSendResetFriendInfo( MODULE_TRANS_THREAD_IDX, m_i64UID );

		ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO | SAVE_DATABASE_MEDAL );
	}
	else
	{	// 닉네임을 다시 초기화 해줍니다.
		m_UserInfoBasic._nick[0] = '\0';
	}
}

// -----------------------------------------------------------
// Name : SetClan_C
// Desc : 클랜 정보를 받습니다.
// -----------------------------------------------------------
void CUserSession::SetClan_C( INT32 i32Result, UINT32 ui32CID, UINT8 ui8ClanState, UINT8 ui8ExpRank, UINT8 ui8PerRank, UINT32 ui32Mark, char* pstrClanName )
{
	if( EV_SUCCESSED( i32Result ) )
	{
		m_UserInfoBasic._clanidx		= ui32CID;
		m_UserInfoBasic._clanstate		= ui8ClanState;	
		if( CLAN_MEMBER_LEVEL_UNKNOWN < ui8ClanState )
		{
			m_UserInfoBasic._clanExpRank	= ui8ExpRank;
			m_UserInfoBasic._clanPerRank	= ui8PerRank;
			m_UserInfoBasic._clanMark		= ui32Mark;
			m_UserInfoBasic._clanNamecolor	= 0;
			i3mem::Copy( m_UserInfoBasic._clanName, pstrClanName, NET_CLAN_NAME_SIZE );
			m_UserInfoBasic._clanName[NET_CLAN_NAME_SIZE-1] = '\0';
		}
		ChangeDataBaseInfo( SAVE_DATABASE_CLAN );
	}
	m_abGetInfoToServer[ GET_INFO_TO_SERVER_CLAN ] = true;
	// 클랜 정보가 이상해도 로그인은 시킵니다.
	SendMyInfo( MODULE_CLAN_THREAD_IDX, EVENT_ERROR_SUCCESS );
}

void CUserSession::SendBuyQuestCardResult( INT32 i32Rv, UINT32 ui32GoodsID, INT32 i32ThreadIdx, TWareDxIdx i64ItemWareDBIdx, UINT8 ui8BuyType )
{
	INT32 i32Result = EVENT_ERROR_SUCCESS;
	INT8 i8QuestSlotIdx = 0;
	if(EV_FAILED( i32Rv ))	i32Result = EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL; // 클라이언트 작업 안되있어서 막습니다 i32Result = EVENT_ERROR_EVENT_BUY_CARD_NETWORK;
	else
	{
		UINT8 ui8SpendType	= 2;
		//상점 로그 보내기 (구입)
		g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_SHOP_BUY, this, &ui32GoodsID, &ui8BuyType, &ui8SpendType );
		// GoodsID로 부터 임무 카드 종류를 알아낸다.
		UINT8 ui8CardType = GET_GOODS_FLAG_NUMBER( ui32GoodsID ) ;

		i8QuestSlotIdx = _FindInsertQuestSlot( ui8CardType );

		// 계급 제한이 있는 카드의 경우 확인하여 맞지 않을 경우 실패 처리한다. ( 클라이언트 해킹 ) -- 서동권
		switch( ui8CardType )
		{
			// 사관
		case QUEST_CARDSET_TYPE_COMMISSIONED_O:
			{
				if( RANK_05 > m_UserInfoBasic._rank )
					i8QuestSlotIdx = -1;
			}
			break;
			// 위관
		case QUEST_CARDSET_TYPE_COMPANY_O:
			{
				if( RANK_17 > m_UserInfoBasic._rank )
					i8QuestSlotIdx = -1;
			}
			break;
			// 영관
		case QUEST_CARDSET_TYPE_FIELD_O:
			{
				if( RANK_31 > m_UserInfoBasic._rank )
					i8QuestSlotIdx = -1;
			}
			break;
		}

		if( EV_SUCCESSED( i8QuestSlotIdx ) )
		{
			g_pContextMain->GetQuest( ui8CardType, i8QuestSlotIdx, &m_QuestIngInfo, m_aaui8QuestCtxActive[ i8QuestSlotIdx ], m_ui8LocaleCode );

			ChangeDataBaseInfo(SAVE_DATABASE_MEDAL | SAVE_DATABASE_BASEINFO );

			//// 아이템 인증
			g_pModuleTrans->AuthSendAuthItem(i32ThreadIdx, this, i64ItemWareDBIdx, 1 );
		}
		else	// 여기서 실패하면 반드시 로그를 남깁니다. 임무카드 금액이 차감 되지만 복구는 안되기 때문
		{	
			i32Result = EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL; 

			char szTemp[ MAX_PATH ];
			i3String::Format( szTemp, MAX_PATH, "[SendBuyQuestCardResult] Quest Card Buy Error or Client hacking ( Rank ) / UID : %I64d / CardType : %d", m_i64UID, ui8CardType );

			g_pModuleLogFile->Write_M_Log(i32ThreadIdx, this, szTemp ) ; 
		}
	}
	  	

	i3NetworkPacket pPacket( PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK );
	pPacket.WriteData(&i32Result,								sizeof(INT32)); 
	if(EV_SUCCESSED( i32Result ))
	{
		pPacket.WriteData(&m_UserInfoBasic._point,				sizeof(UINT32)); 
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
		SendPacket.WriteData( &pBlockInfo[i]._ui8NickSize,		sizeof(UINT8)); 
		SendPacket.WriteData( pBlockInfo[i]._strNickName,		pBlockInfo[i]._ui8NickSize ); 
		SendPacket.WriteData( &pBlockInfo[i]._ui8CommentSize,	sizeof(UINT8)); 
		SendPacket.WriteData( pBlockInfo[i]._strComment,		pBlockInfo[i]._ui8CommentSize ); 
		SendPacket.WriteData( &pBlockInfo[i]._BasicInfo,	sizeof(BLOCK_INFO));
	}
	SendPacketMessage(&SendPacket); 
	return; 
}

void CUserSession::SendBlockInsertResult(INT32 i32Result, INT32 i32Idx, UINT8 ui8Rank, UINT8 ui8NickSize, char *pstrNick, UINT8 ui8CommentSize, char *pstrComment)
{
	i3NetworkPacket SendPacket( PROTOCOL_AUTH_BLOCK_INSERT_ACK );
	SendPacket.WriteData( &i32Result,		sizeof(INT32) );
	if( EV_SUCCESSED( i32Result ) )
	{
		SendPacket.WriteData( &i32Idx,			sizeof(INT32)); 
		SendPacket.WriteData( &ui8Rank,			sizeof(UINT8)); 
		SendPacket.WriteData( &ui8NickSize,		sizeof(UINT8)); 
		SendPacket.WriteData( pstrNick,			sizeof(char) * ui8NickSize ); 
		SendPacket.WriteData( &ui8CommentSize,	sizeof(UINT8)); 
		SendPacket.WriteData( pstrComment,		sizeof(char) * ui8CommentSize ); 
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
	if ( NULL != pSession && 0 != m_UserInfoBasic._clanidx )
	{
		if( m_UserInfoBasic._clanidx == pSession->GetClanIdx() ) bResult = TRUE;
	}

	return bResult;
}

void CUserSession::SendMyItemInfo( CUserSession * pSession, UINT8 ui8Type  )
{
	i3NetworkPacket SendPacket;
	SendPacket.Clear();

	switch( ui8Type )
	{
	case VIEW_USER_ITEM_INFO_TYPE_LOBBY :	SendPacket.SetProtocol(PROTOCOL_LOBBY_VIEW_USER_ITEM_ACK);	break;	// 로비
	case VIEW_USER_ITEM_INFO_TYPE_ROOM :	SendPacket.SetProtocol(PROTOCOL_ROOM_GET_USER_ITEM_ACK);	break; // 룸
	default:  
		break;
	}

	INT32 i32ItemCount = 0;
	UINT32 aui32ItemID[ MAX_INVEN_COUNT ];
	for( INT32 i = 0 ; i < m_i32InvItemCount ; i++ )
	{
		if( ITEM_CLASS_MAINTENANCE != GET_ITEM_FLAG_CLASS( m_aInvItem[i]._ui32ItemID ) ) continue;
		aui32ItemID[i32ItemCount] = m_aInvItem[i]._ui32ItemID;
		i32ItemCount++;
	}

	SendPacket.WriteData( &m_UserInfoEquipmentWeapon,	sizeof(USER_INFO_EQUIPMENT_WEAPON));
	SendPacket.WriteData( &m_UserInfoEquipmentChara,	sizeof(USER_INFO_EQUIPMENT_CHARA));
	SendPacket.WriteData( &i32ItemCount,				sizeof(INT32));
	SendPacket.WriteData( aui32ItemID,					(UINT16)(sizeof(UINT32)*i32ItemCount));	
	pSession->SendPacketMessage(&SendPacket);	// 나의 정보를 상대방에게 보낸다.

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
void CUserSession::SendAttendanceCheckResult( INT32 i32Rv, ATTENDANCE_USER* pstAttendance )
{
	i3NetworkPacket packet( PROTOCOL_BASE_ATTENDANCE_ACK );
	packet.WriteData( &i32Rv,	sizeof(INT32) );
	if( EVENT_ERROR_ATTENDANCE_SUCCESS == i32Rv )
	{
		packet.WriteData( pstAttendance,	sizeof(ATTENDANCE_USER) );
	}
	SendPacketMessage( &packet );	
}

void CUserSession::SendAttendanceInfoResult( ATTENDANCE_USER* pstAttendance, ATTENDANCE_INFO* pstInfo )
{
	i3NetworkPacket packet( PROTOCOL_BASE_ATTENDANCE_INFO_ACK );
	packet.WriteData( pstAttendance,	sizeof(ATTENDANCE_USER) );
	packet.WriteData( pstInfo,			sizeof(ATTENDANCE_INFO) * 2 );
	SendPacketMessage( &packet );	
}

BOOL CUserSession::CheckSaleCouponDate( INT64 i64CouponIdx, SALE_COUPON * pCoupon )
{
	if( pCoupon->m_ui8CouponType == DAY_TYPE_DATE )
	{
		if( pCoupon->m_ui32StartDate > g_pContextMain->GetInvenTime() || pCoupon->m_ui32EndDate < g_pContextMain->GetInvenTime() )
			return FALSE; // 정해진 기간이 아님

		return TRUE;
	}

	for( INT32 i = 0 ; i < m_i32InvItemCount ; i++ )
	{
		if( i64CouponIdx == m_aInvItem[i]._i64ItemWareDBIdx )
		{
			if( g_pContextMain->GetInvenTime() < m_aInvItem[i]._ui32ItemArg )
				return TRUE;

			return FALSE; // 이미 유효기간이 지남
		}
	}

	return FALSE;
}

BOOL CUserSession::CheckUseCoupon( SALE_COUPON *pCoupon, SHOP_GOODS_BASE *pGoods, UINT32 &ui32Cash )
{
	switch( pCoupon->m_ui8LimitType ) // 할인쿠폰 사용 제한 금액
	{
	case LIMIT_TYPE_NONE:
		break;
	case LIMIT_TYPE_UP: // 얼마 이상
		if( pGoods->_ui32Cash < pCoupon->m_ui32Limit )	return FALSE;
		break;
	case LIMIT_TYPE_DOWN: // 얼마 이하
		if( pGoods->_ui32Cash > pCoupon->m_ui32Limit )	return FALSE;
		break;
	default : // 잘못된 타입
		return FALSE;
	}

	// 매칭정보를 얻어와서 할인쿠폰을 사용 가능한 아이템 클래스인지 확인
	SHOP_MATCHING_DATA*		pMatching = g_pContextMain->GetShop()->GetMatchingAllData();
	UINT32	ui32MatchingCount	= g_pContextMain->GetShop()->GetMatchingCount();
	for( UINT32 i = 0; i < ui32MatchingCount; ++i )
	{
		if( pMatching[i]._ui32GoodsID != pGoods->_ui32GoodsID ) continue;

		UINT8 ui8ItemType = GET_ITEM_FLAG_CLASS( pMatching[i]._ui32ItemID );
		if( pCoupon->m_ui8ItemClass != ITEM_CLASS_UNKNOWN && ui8ItemType != pCoupon->m_ui8ItemClass ) return FALSE; // 쿠폰 사용 가능한 아이템 타입이 아님
	}

	switch( pCoupon->m_ui8SaleType )
	{
	case SALE_TYPE_NONE :
		ui32Cash	+= pGoods->_ui32Cash;
		break;
	case SALE_TYPE_PER :
		if( pCoupon->m_ui32DisCount > 50 ) pCoupon->m_ui32DisCount = 50; // 할인 퍼센트가 50퍼센트 이상일 경우 50으로 수정
		ui32Cash	+= (( pGoods->_ui32Cash * ( 100 - pCoupon->m_ui32DisCount )) / 100) ;
		break;
	case SALE_TYPE_CASH :
		if( pCoupon->m_ui32DisCount >= pGoods->_ui32Cash )	break; // 할인금액이 물건값과 같거나 더 크다면 총 금액에 포함시키지 않습니다.
		ui32Cash	+= ( pGoods->_ui32Cash - pCoupon->m_ui32DisCount );
		break;
	default	:
		return FALSE; // 잘못된 할인 타입
	}

	return TRUE;
}

BOOL CUserSession::_CheckSID_ID( char* strSecurityID, UINT8 ui8IDSize )
{
	INT32 i32WhelkPos = -1;
	INT32 i32WhelkCnt = 0;
	INT32 i32DotPos = -1;
	INT32 i32DotCnt = 0;

	for( INT32 i = 0; i < ui8IDSize; i++)
	{
		switch( strSecurityID[i] )
		{
		case '@':
			if ( i == 0 )				return FALSE;	// 맨앞글자가 골뱅이(@)면 에러
			i32WhelkPos = i;
			i32WhelkCnt++;
			break;
		case '.':
			i32DotPos = i;
			i32DotCnt++;
			break;
		}
	}

	if ( i32WhelkCnt != 1 )				return FALSE;	// 골뱅이(@)가 1개가 아니면 에러
	if ( i32DotCnt == 0 )				return FALSE;	// 컴마(.)가 1개도 없으면 에러
	if ( i32WhelkPos >= i32DotPos )		return FALSE;	// 맨마지막컴마(.)위치이후에 골뱅이(@)가 있으면 에러
	if ( i32WhelkPos + 1 == ui8IDSize )	return FALSE;	// 맨마지막글자가 골뱅이(@)면 에러

	return TRUE;
}



// 중복채팅을 시도했을 경우 카운트를 세주거나 제한 시작
// 인자 : 채팅내용, 채팅한 위치
// 반환 : 제한시간 (0, 60, 3600)
UINT32 CUserSession::_CheckDuplicateChat( char* pstrChat, INT32 i32ChatType, PROTOCOL ui16Protocol )
{
#ifdef USE_GM_PAUSE
	if ( m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
	{
		if ( m_pRoom->IsGMPause() )
		{
			if ( !IsGMUser() ) return m_i32LimitTime;
		}
	}
#endif

	INT32	i32PrevLimitTime	= m_i32LimitTime;
	UINT32	ui32PrevChatTime	= m_ui32ChatTime;					// 이전에 저장된 시간은 이전 시간으로
	m_ui32ChatTime				= g_pContextMain->GetServerTime();	// 방금 받은 시간 저장


	// 정상적인 채팅을 시도했을 경우
	if (m_ui32ChatTime > ui32PrevChatTime)
	{
		if ( m_ui32ChatTime > (m_ui32LimitStartTime + m_i32LimitTime) )
		{
			m_i32LimitTime			= CHAT_LIMIT_NORMAL;
			m_ui32LimitStartTime	= 0;

			return m_i32LimitTime;
		}
		else
		{
			return m_i32LimitTime;
		}
	}
	else
	{
		// 1초 이내에 중복입력을 시도한 횟수 카운트
		m_ui8ChatCount++;
	}


	// 제한 횟수 이상 중복입력을 시도했다면
	if (m_ui8ChatCount >= CHAT_HACK_COUNT)
	{
		switch (m_i32LimitTime)
		{
		case CHAT_LIMIT_MIN:
			{
				m_i32LimitTime			= CHAT_LIMIT_HOUR;
				m_ui32LimitStartTime	= m_ui32ChatTime;

				// ZLog에 남긴다 (도배한 곳, 채팅내용)
				g_pModuleContextNC->LogSendMessage(m_WorkThreadIdx, LOG_MESSAGE_U_CHATTING_HACK, this, &i32ChatType, pstrChat);
			}
			break;

		case CHAT_LIMIT_HOUR:
			{
				m_i32LimitTime			= CHAT_LIMIT_HOUR;
				m_ui32LimitStartTime	= m_ui32ChatTime;
			}
			break;

		case CHAT_LIMIT_NORMAL:
			{
				m_i32LimitTime			= CHAT_LIMIT_MIN;
				m_ui32LimitStartTime	= m_ui32ChatTime;
			}
			break;
		}

		m_ui8ChatCount	= 0;
	}


	// 채팅 제한 시간을 클라에게 보냄
	if ( (m_i32LimitTime != CHAT_LIMIT_NORMAL) && (m_i32LimitTime != i32PrevLimitTime) )
	{
		i3NetworkPacket	SendChattingPacket( ui16Protocol );
		SendChattingPacket.WriteData( &m_i32LimitTime,	sizeof(INT32) );
		SendPacketMessage(&SendChattingPacket);
	}


	return m_i32LimitTime;
}