#include "pch.h"
#include "DataBase.h"
#include "ModuleTransSocket.h"
#include "ModuleTrans.h"
#include "UserManager.h"
#include "UserSession.h"
// #include "ModuleLogFile.h"
#include "ModuleContextNc.h"
#include "RoomManager.h"
#include "Room.h"
#include "ModuleMessenger.h"

I3_CLASS_INSTANCE( CModuleTransSocket, i3NetworkClientSocket );

CModuleTransSocket::CModuleTransSocket(void)
{
	m_bConnect		= FALSE; 
}

CModuleTransSocket::~CModuleTransSocket(void)
{
	OnDestroy();
}

BOOL CModuleTransSocket::OnCreate( UINT32 ui32IP, UINT16 ui16PortNo)
{
	BOOL bRv = i3NetworkClientSocket::OnCreate( ui32IP, ui16PortNo);
	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
	m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();
	m_ui32ShopVersionCheckTime = i3ThreadTimer::GetServerTime();
	return bRv;
}

void CModuleTransSocket::OnDestroy(void)
{
	DisConnect();
}

void CModuleTransSocket::DisConnect()
{
	m_bConnect = FALSE; 
	i3NetworkClientSocket::DisConnect();
}

void CModuleTransSocket::PacketParsing(i3NetworkPacket  * pPacket)
{
	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 
	UINT32		ui32UTemp = 0; 
	INT32		i32Temp;
	INT32		i32Rv;
	INT32		i32SessionIdx;
	T_UID		i64UID = 0;

	CUserSession * pSession;
	//char		Id[NET_ID_SIZE];

#ifdef _DEBUG
	g_PrintProtocolName( ProtocolID, "Trans" );
#endif

	switch(ProtocolID)
	{
	case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS	:	//연결 되었음 //PROTOCOL_AUTH_SHOP_GET_SAILLIST_ACK에서 받는값과 동일한 값이 옵니다.		
		{
			g_pLog->WriteLog( L"[SUCC] AUTH SERVER CONNECT MESSAGE RECV");			
			m_bConnect = TRUE;

			// 접속 완료후 버전을 얻어온다.
			i3NetworkPacket SendPacket( PROTOCOL_AUTH_SHOP_VERSION_REQ );
			SendPacketMessage( &SendPacket );

			if( g_pContextMain->IsContentsEnabled(CONTENTS_GACHA) )
			{
				i3NetworkPacket stGachaNotice(PROTOCOL_SS_GACHA_NOTICE_REQ);
				SendPacketMessage(&stGachaNotice);
			} 
		}
		break; 
	case PROTOCOL_LOGIN_ACK		:					//로그인 응답 
		{
			UINT32		ui32AuthKey;
			UINT32		ui32BlockDate;
			LOGIN_INFO	LoginInfo;

			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32) );			//SessionIDX
			pPacket->ReadData( &ui32AuthKey,		sizeof(UINT32) );			//Login AuthKey
			pPacket->ReadData( &i32Rv,				sizeof(INT32) );			//Result
			pPacket->ReadData( &ui32BlockDate,		sizeof(UINT32) );
			if( EV_SUCCESSED( i32Rv ) )
			{
				pPacket->ReadData( &LoginInfo,			sizeof(LOGIN_INFO) );
				LoginInfo.m_strID[ NET_ID_SIZE-1 ]					= '\0';
				LoginInfo.m_strcustom[ NORTH_CUSTOM_MSG_SIZE-1 ]	= '\0';
				LoginInfo.m_strBillingID[ BILLING_ID_SIZE-1 ]		= '\0';
				LoginInfo.m_strExtUID[ EXT_UID_SIZE - 1 ]			= '\0';
				LoginInfo.m_strWebLoginKey[ WEB_LOGINKEY_SIZE - 1 ]	= '\0';
			}			
			else if (EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT == i32Rv)
			{
				UINT8 ui8Size = 0;
				pPacket->ReadData(&ui8Size, sizeof(UINT8));
				pPacket->ReadData(LoginInfo.m_strUserNoticeComment, sizeof(TTCHAR) * ui8Size);
				LoginInfo.m_strUserNoticeComment[ui8Size] = '\0';
				pPacket->ReadData(&LoginInfo.m_i32PenaltyAmount, sizeof(INT32));
			}

			g_pUserManager->LogInUser_A( i32SessionIdx, i32Rv, ui32AuthKey, ui32BlockDate, &LoginInfo );
		}
		break;
	case PROTOCOL_BASE_LOGIN_WAIT_ACK:
		{
			UINT32	ui32AuthKey;
			INT32	i32WaitCount;
			UINT32	ui32LoginTimeArg;
			UINT8	ui8LinkThreadIdx;
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32) );			//SessionIDX
			pPacket->ReadData( &ui32AuthKey,		sizeof(UINT32) );			//Login AuthKey
			pPacket->ReadData( &i32WaitCount,		sizeof(INT32) );			//Login AuthKey
			pPacket->ReadData( &ui32LoginTimeArg,	sizeof(UINT32) );			//Login AuthKey
			pPacket->ReadData( &ui8LinkThreadIdx,	sizeof(UINT8) );			//Login AuthKey

			g_pUserManager->LogInWaitUser_A( i32SessionIdx, ui32AuthKey, i32WaitCount, ui32LoginTimeArg, ui8LinkThreadIdx );
		}
		break;

	case PROTOCOL_BASE_GET_INVEN_INFO_ACK	:
		{
			pPacket->ReadData( &i32SessionIdx,	sizeof(INT32) );				//SessionIDX
			pPacket->ReadData( &i64UID,			sizeof(T_UID) );				//DBIDX
			
			pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID );
			if( NULL == pSession )	break;;

			UINT16				ui16ItemCount = 0;
			INVEN_BUFFER		pInvenItem[ MAX_INVEN_COUNT ];
			INVEN_BUFFER_SET	InvenSet;

			pPacket->ReadData( &ui16ItemCount,								sizeof(INT16) );		LENGTH_CHECK( ui16ItemCount, MAX_INVEN_COUNT );
			pPacket->ReadData( pInvenItem,									sizeof(INVEN_BUFFER)*ui16ItemCount );	
			pPacket->ReadData( &pSession->m_UserInfoBasic.m_ui8PCCafe,		sizeof(UINT8));
			pPacket->ReadData( &pSession->m_ui32QuestEventDate,				sizeof(UINT32) );
			pPacket->ReadData( pSession->m_aaui8QuestCtxActive,				sizeof(UINT8)*MAX_QUEST_ALL_USE_NEW );
			pPacket->ReadData( &pSession->m_QuestIngInfo,					sizeof(QUESTING_INFO) );
			pPacket->ReadData( &pSession->m_ui32Miniature,					sizeof(UINT32) );
			pPacket->ReadData( &pSession->m_ui32Insignia,					sizeof(UINT32) );
			pPacket->ReadData( &pSession->m_ui32Order,						sizeof(UINT32) );
			pPacket->ReadData( &pSession->m_ui32Master,						sizeof(UINT32) );
			pPacket->ReadData( &pSession->m_ui64GainUserTitle,				sizeof(UINT64) );
			pPacket->ReadData(pSession->m_aui8EquipUserTitle,				sizeof(UINT8)*MAX_EQUIP_USER_TITLE );
			pPacket->ReadData( &pSession->m_UserInfoBasic.m_ui32Rank,		sizeof(UINT32));

			pSession->InvenInsertPriority(pInvenItem, &InvenSet, ui16ItemCount);
	
		}
		break;

	case PROTOCOL_BASE_GET_CHARA_INFO_ACK	:
		{
			pPacket->ReadData( &i32SessionIdx,	sizeof(INT32) );				//SessionIDX
			pPacket->ReadData( &i64UID,			sizeof(T_UID) );				//DBIDX

			pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID );
			if( NULL == pSession )	break;;

			UINT8					ui8SlotIdx;
			UINT8					ui8SlotState;
			S2MO_CHAR_BASE_INFO		CharBaseInfo;
			S2MO_CHAR_SKILL_INFO	CharSkillInfo;
			UINT8					ui8CharNickSize;
			ITEM_INFO				aCharEquips[ CHAR_EQUIPMENT_COUNT ];								// 장착 정보
			ITEM_INFO				aCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];								// 장착 정보

			INT32				i32Count = 0;

			pPacket->ReadData( &i32Count,									sizeof(INT32)) ;

			for( INT32 i = 0 ; i < i32Count ; ++i )
			{
				pPacket->ReadData( &ui8SlotIdx,								sizeof(UINT8) );
				pPacket->ReadData( &ui8SlotState,							sizeof(UINT8) );
				pPacket->ReadData( &CharBaseInfo.m_i32WeaponDBIdx,			sizeof(INT32) );
				pPacket->ReadData( &CharBaseInfo.m_ui32CreateDate,			sizeof(UINT32) );
				pPacket->ReadData( &CharBaseInfo.m_ui32BattleTime,			sizeof(UINT32) );
				pPacket->ReadData( &CharBaseInfo.m_ui32SkillRelTime,		sizeof(UINT32) );
				pPacket->ReadData( &ui8CharNickSize,						sizeof(UINT8) );
				pPacket->ReadData( CharBaseInfo.m_strCharName,				sizeof(TTCHAR) * ui8CharNickSize );
				pPacket->ReadData( &CharSkillInfo,							sizeof(S2MO_CHAR_SKILL_INFO) );
				pPacket->ReadData( aCharEquips,								sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COUNT );
				CharBaseInfo.m_strCharName[ ui8CharNickSize ] = '\0';

				if( pSession->m_MultiSlot.InsertCharInfoAll( ui8SlotIdx, (MULTI_SLOT_STATE)ui8SlotState, &CharBaseInfo ) )
				{
					pSession->m_MultiSlot.SetCharEquipmentAll( ui8SlotIdx, aCharEquips );
					pSession->m_MultiSlot.SetCharSkillInfo( ui8SlotIdx, &CharSkillInfo );
				}
			}

		}
		break;

	case PROTOCOL_BASE_GET_USER_INFO_ACK	:
		{
			pPacket->ReadData( &i32SessionIdx,	sizeof(INT32) );				//SessionIDX
			pPacket->ReadData( &i64UID,			sizeof(T_UID) );				//DBIDX
			pPacket->ReadData( &i32Rv,			sizeof(INT32) );				//Result 

			pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID );
			if( NULL == pSession )	break;

			pSession->GetMyInfo_A( MODULE_TRANS_THREAD_IDX, i32Rv, pPacket );
		}
		break;

	case PROTOCOL_BASE_USER_ENTER_ACK:
		{
			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32)); 
			pPacket->ReadData( &i64UID,					sizeof(T_UID));
			pPacket->ReadData( &i32Rv,					sizeof(INT32));

			g_pUserManager->UserServerEnter_A( i32SessionIdx, i64UID, i32Rv, pPacket );
		}
		break;
	case PROTOCOL_BASE_USER_LEAVE_ACK:
		{
			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32)); 
			pPacket->ReadData( &i64UID,					sizeof(T_UID));
			pPacket->ReadData( &i32Rv,					sizeof(INT32));

			g_pUserManager->UserServerLeave_A( i32SessionIdx, i64UID, i32Rv );
		}
		break;
	case PROTOCOL_BASE_CREATE_NICK_ACK:
		{
			UINT32 ui32NewPoint = 0;
			UINT8 ui8NickSize;
			TTCHAR strNickName[ NET_NICK_NAME_SIZE ];

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32)); 
			pPacket->ReadData( &i64UID,					sizeof(T_UID));
			pPacket->ReadData( &i32Rv,					sizeof(INT32));
			if( EV_SUCCESSED( i32Rv ) )
			{
				pPacket->ReadData( &ui32NewPoint,		sizeof(UINT32));
				pPacket->ReadData( &ui8NickSize,		sizeof(UINT8));	LENGTH_CHECK( ui8NickSize, NET_NICK_NAME_SIZE );
				pPacket->ReadData( strNickName,			sizeof(TTCHAR) * ui8NickSize);
				strNickName[ NET_NICK_NAME_SIZE-1 ] = '\0';
			}

			pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID );
			if( NULL == pSession )	break;

			pSession->CreateNickName_A( i32Rv, ui32NewPoint, strNickName );
		}
		break;
	case PROTOCOL_CHAR_CREATE_CHARA_ACK:
		{
			T_GoodsID				TGoodsID;
			UINT32					ui32CharaPoint = 0;
			UINT32					ui32CharaCash = 0;
			INVEN_BUFFER			CharaInven;					
			
			PACKET_CHAR_CREATE_CHARA_ACK	Packet;
			
			pPacket->ReadData( &i32SessionIdx,							sizeof(INT32)); 
			pPacket->ReadData( &i64UID,									sizeof(T_UID));

			pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID );
			if( NULL == pSession )	break;

			pPacket->ReadData( &TGoodsID,								sizeof(T_GoodsID));
			SHOP_GOODS_EXPANSION* pGoods = g_pContextMain->GetShop()->GetGoodsData( TGoodsID );
			if( pGoods )
			{
				ui32CharaPoint	= pGoods->GetPrice(GOODS_BUY_POINT);
				ui32CharaCash	= pGoods->GetPrice(GOODS_BUY_CASH); 
			}

			pPacket->ReadData( &Packet.m_ui32Cash,						sizeof(UINT32));
			pPacket->ReadData( &Packet.m_TResult,						sizeof(INT32));
			if( EV_SUCCESSED( Packet .m_TResult ) )
			{
				pPacket->ReadData( &CharaInven,							sizeof(INVEN_BUFFER)); 
				pPacket->ReadData(&Packet.m_ui8SlotState,				sizeof(UINT8));
				pPacket->ReadData(&Packet.m_ui8CreateType,				sizeof(UINT8)); 
				pPacket->ReadData(&Packet.m_bAddCharaSlot,				sizeof(bool));
				if( Packet.m_bAddCharaSlot )
				{
					
					
					pPacket->ReadData( &Packet.m_ui8SlotIdx,					sizeof(UINT8));
					pPacket->ReadData( &((&Packet.m_SkillInfo)->m_ui8Class),	sizeof(UINT8)); 
					pPacket->ReadData( &Packet.m_BaseInfo,						sizeof(S2MO_CHAR_BASE_INFO));
					pPacket->ReadData( &Packet.m_SkillInfo,						sizeof(S2MO_CHAR_SKILL_INFO));				 

					// 병과 선택 
					g_pModuleContextNC->LogSendMessage(MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_CLASS_SETTING, pSession, &Packet.m_ui8SlotIdx, &((&Packet.m_SkillInfo))->m_ui8Class  );
				}
			}
			else
			{
				// 구매 실패시 포인트 롤백합니다.
				if( GOODS_BUY_POINT == Packet.m_ui8CreateType )
				{
					pSession->m_UserInfoBasic.m_ui32Point	+= ui32CharaPoint;
					pSession->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );
				}
			}

			pSession->m_UserInfoBasic.m_ui32Cash = Packet.m_ui32Cash;
			Packet.m_ui32Point		= pSession->m_UserInfoBasic.m_ui32Point; 
			pSession->CreateChar_A( &Packet, TGoodsID, &CharaInven );
		}
		break; 
	case PROTOCOL_GSM_ACCOUNT_KICK_REQ	:					//강제 퇴장.
		{
			UINT8 ui8Reason;
			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32) ); 
			pPacket->ReadData( &i64UID,					sizeof(T_UID) ); 			
			pPacket->ReadData( &ui8Reason,				sizeof(UINT8) );
			g_pUserManager->KickUser_A(i32SessionIdx, i64UID, ui8Reason); 
		}
		break; 
	case PROTOCOL_AUTH_ANNOUNCE			:					//전체 공지 
		{
			UINT32 ui32Len; 
			pPacket->ReadData(&ui32Len, sizeof(UINT32)); 
			if(ui32Len > NOTICE_STRING_MAX_COUNT - 1)return;
			pPacket->ReadData(g_pContextMain->m_strEmergencyNotice, ui32Len);
			g_pContextMain->m_strEmergencyNotice[ui32Len] = 0; 
			g_pUserManager->OnSendNoticeMessage( MODULE_TRANS_THREAD_IDX, NOTICE_TYPE_EMERGENCY, g_pContextMain->m_strEmergencyNotice, ui32Len); 
		}
		break; 
	case PROTOCOL_AUTH_ANNOUNCE_ROOM	:		//방 공지
		{
			UINT32	ui32Len; 			
			pPacket->ReadData(&i32SessionIdx,	sizeof(UINT32)); 
			pPacket->ReadData(&ui32Len,			sizeof(UINT32)); 
			if(ui32Len > NOTICE_STRING_MAX_COUNT - 1)return;
			pPacket->ReadData(m_strRoomNotice, ui32Len);
			m_strRoomNotice[ui32Len] = 0;
			g_pUserManager->SendRoomNoticeMessage_A(i32SessionIdx, m_strRoomNotice, ui32Len); 
		}
		break; 
	case PROTOCOL_AUTH_USER_NOW_INFO_REQ	:		//
		{
			pPacket->ReadData(&ui32UTemp,	sizeof(UINT32)); 
			g_pUserManager->SendUserNowInfo_A(ui32UTemp);
		}
		break; 
	case PROTOCOL_AUTH_ROOM_DESTROY		:			//방 폭파^^;	
		{
			pPacket->ReadData(&ui32UTemp,	sizeof(UINT32));
			g_pUserManager->DestroyRoom_A(ui32UTemp); 
		}
		break;

	case PROTOCOL_AUTH_SHOP_VERSION_ACK:
		{
			UINT32 ui32TransShopVersion;
			CShop* pShop = g_pContextMain->GetShop();
			pPacket->ReadData(&ui32TransShopVersion,	sizeof(UINT32));

			UINT32 ui32CurShopVer = pShop->GetVersion();

			// 현재 없는 버전이면 상점 정보 요청.
			if( ui32TransShopVersion != ui32CurShopVer )
			{	
				g_pLog->WriteLog( L"[PROTOCOL_AUTH_SHOP_VERSION_ACK]  Shop Ver / Old : %d / New : %d ", 
					ui32CurShopVer, ui32TransShopVersion );

				i3NetworkPacket SendPacket( PROTOCOL_AUTH_SHOP_LIST_REQ );
				SendPacketMessage( &SendPacket );
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_ITEMLIST_ACK:
		{
			UINT32 ui32ItemCount;
			UINT32 ui32CurItemCount;
			UINT32 ui32StartIdx;

			CShop* pShop = g_pContextMain->GetShop();
			SHOP_ITEM_BASE*		pItem;	 
			SHOP_ITEM_DATA*		pItemEx;	 

			pPacket->ReadData(&ui32ItemCount,		sizeof(UINT32));
			pPacket->ReadData(&ui32CurItemCount,	sizeof(UINT32));
			pPacket->ReadData(&ui32StartIdx,		sizeof(UINT32));
			UINT32 ui32EndIdx = ui32StartIdx + ui32CurItemCount;

			// 중요합니다. Trans서버에서 가장 먼저 오는패킷에만 있어야 합니다.
			// 첫 패킷이라면 데이터 초기화.
			if( 0 == ui32StartIdx )
			{
				pShop->InitShopAllData();
			}

			pItem	= pShop->GetItemAllDataNotUse();
			pItemEx = pShop->GetItemAllDataExNotUse();
			
			for( UINT32 i = ui32StartIdx ; i < ui32EndIdx ; i++ )
			{																	
				pPacket->ReadData( &pItem[i],		sizeof(SHOP_ITEM_BASE));
				pPacket->ReadData( &pItemEx[i],		sizeof(SHOP_ITEM_EXPANSION));
			}			

			pShop->SetItemCount( ui32ItemCount );

		}
		break;
	case PROTOCOL_AUTH_SHOP_GOODSLIST_ACK:
		{
			UINT32 ui32GoodsCount;
			UINT32 ui32CurGoodsCount;
			UINT32 ui32StartIdx;

			CShop* pShop = g_pContextMain->GetShop();
			SHOP_GOODS_EXPANSION*	pGoods; 
			

			pPacket->ReadData(&ui32GoodsCount,		sizeof(UINT32));
			pPacket->ReadData(&ui32CurGoodsCount,	sizeof(UINT32));
			pPacket->ReadData(&ui32StartIdx,		sizeof(UINT32));
			
			UINT32 ui32EndIdx = ui32StartIdx + ui32CurGoodsCount;

			
			pGoods		= pShop->GetGoodsAllDataNotUse();
			
			for( UINT32 i = ui32StartIdx ; i < ui32EndIdx ; i++ )
			{
				pPacket->ReadData(&pGoods[i],		sizeof(SHOP_GOODS_EXPANSION)); 
			}

			pShop->SetGoodsCount( ui32GoodsCount );
		}
		break;
	
	case PROTOCOL_AUTH_SHOP_REPAIRLIST_ACK:
		{
			UINT32 ui32RepairCount = 0;
			UINT32 ui32CurRepairCount;
			UINT32 ui32StartIdx;

			CShop* pShop = g_pContextMain->GetShop();
			SHOP_ITEM_REPAIR_DATA* pRepair;// = pShop->GetRepairAllData();

			pPacket->ReadData(&ui32RepairCount,		sizeof(UINT32));
			pPacket->ReadData(&ui32CurRepairCount,	sizeof(UINT32));
			pPacket->ReadData(&ui32StartIdx,		sizeof(UINT32));

			pRepair = pShop->GetRepairAllDataNotUse();

			pPacket->ReadData(&pRepair[ui32StartIdx],	sizeof(SHOP_ITEM_REPAIR_DATA)*ui32CurRepairCount);

			pShop->SetRepairCount( ui32RepairCount );
		}
		break;
	case PROTOCOL_AUTH_SHOP_SALECOUPONLIST_ACK:
		{
			
		}
		break;
	case PROTOCOL_AUTH_SHOP_MATCHINGLIST_ACK:
		{
			UINT32 ui32MatchingCount = 0;
			UINT32 ui32CurMatchingCount = 0;
			UINT32 ui32StartIdx;
			UINT32 ui32Version;
			CShop* pShop = g_pContextMain->GetShop();
			SHOP_MATCHING_DATA* pMatching; 

			pPacket->ReadData(&ui32MatchingCount,		sizeof(UINT32));
			pPacket->ReadData(&ui32CurMatchingCount,	sizeof(UINT32));
			pPacket->ReadData(&ui32StartIdx,			sizeof(UINT32));

			pMatching = pShop->GetMatchingAllDataNotUse();

			pPacket->ReadData(&pMatching[ui32StartIdx],	sizeof(SHOP_MATCHING_DATA)*ui32CurMatchingCount);
			pShop->SetMatchingCount( ui32MatchingCount );

			pPacket->ReadData(&ui32Version,			sizeof(UINT32));

			char strMD5Key[NET_MD5_KEY_SIZE+1];
			pPacket->ReadData( strMD5Key,	sizeof(char) * NET_MD5_KEY_SIZE);
			strMD5Key[NET_MD5_KEY_SIZE] = 0;

			// 마지막 패킷이라면 버전 셋팅.
			if( ui32MatchingCount == (ui32StartIdx + ui32CurMatchingCount) )
			{
				// Item에 Repair를 담는다.
				pShop->SetRepairList();

				// 버전 변경.
				pShop->ChangeShopVersion( ui32Version );
				pShop->SetShopMD5Key(strMD5Key);

				g_pLog->WriteLog( L"[PROTOCOL_AUTH_SHOP_MATCHINGLIST_ACK] Shop Update - Ver:%d, ItemCount:%d, Goods:%d, Matching:%d", 
					 pShop->GetVersion(), pShop->GetItemCount(), pShop->GetGoodsCount(), pShop->GetMatchingCount() );
			}
		}
		break;

	case PROTOCOL_FIELDSHOP_SEND_NEWVERSION_ACK:
		{
			pPacket->ReadData( g_pContextMain->GetFieldShop(),	sizeof(FieldShopInfo) );
		}
		break;
	case PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK:			//구입 
		{			
			UINT32			ui32Point		= 0;
			UINT32			ui32Cash		= 0;
			UINT8			ui8GoodsCount	= 0;
			UINT8			ui8Action;
			DB_GOODS		stDBGoods[MAX_SHOP_GOODS_BUY_COUNT];
			
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32)); 
			pPacket->ReadData( &i64UID,				sizeof(T_UID));
			pPacket->ReadData( &ui32Point,			sizeof(UINT32));
			pPacket->ReadData( &ui32Cash,			sizeof(UINT32));
			pPacket->ReadData( &ui8Action,			sizeof(UINT8));
			pPacket->ReadData( &ui8GoodsCount,		sizeof(UINT8));
			pPacket->ReadData( stDBGoods,			sizeof(DB_GOODS) * ui8GoodsCount);

			g_pUserManager->OnGoodsBuy_A(i32SessionIdx, i64UID, ui32Point, ui32Cash, ui8GoodsCount, stDBGoods, ui8Action );
		}
		break;
	case PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK:			// 선물하기
		{
			PACKET_AUTH_SHOP_GOODS_GIFT_ACK	Packet;

			UINT8 ui8Count			= 0;
			UINT8 ui8MessageSize	= 0;
			T_UID i64RecvUID		= 0;
			TTCHAR strMessage[MAX_GIFT_MESSAGE_SIZE];
			
			pPacket->ReadData(&i32SessionIdx,			sizeof(INT32)); 
			pPacket->ReadData(&i64UID,					sizeof(T_UID));  
			pPacket->ReadData(&Packet.m_ui32Point,		sizeof(UINT32));
			pPacket->ReadData(&Packet.m_ui32Cash,		sizeof(UINT32));
			pPacket->ReadData(&ui8Count,				sizeof(UINT8));
			pPacket->ReadData(&Packet.m_aResult,		sizeof(S2MO_SHOP_BUY_RESULT)*ui8Count );
			Packet.m_aResult.SetCount( ui8Count );
			
			pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID );
			if (pSession == NULL)	break;

			if (!g_pContextMain->IsContentsEnabled(CONTENTS_GIFT))
			{
				BOOL bResult = TRUE;
				if (ui8Count > MAX_SHOP_GOODS_BUY_COUNT) ui8Count = MAX_SHOP_GOODS_BUY_COUNT;
				for (INT32 i = 0; i < ui8Count; i++)
				{
					if (EV_SUCCESSED(Packet.m_aResult[i].m_i32Rv)) continue;
					bResult = FALSE;
				}
				pPacket->ReadData(&i64RecvUID, sizeof(T_UID));
				pPacket->ReadData(&ui8MessageSize, sizeof(UINT8));
				pPacket->ReadData(strMessage, sizeof(TTCHAR) * ui8MessageSize);

				if (0 < i64RecvUID  && 0 < ui8MessageSize && bResult == TRUE)
				{
					strMessage[ui8MessageSize] = 0;
					pSession->SendGiftNoteByUID(i64RecvUID, ui8MessageSize, strMessage);
				}
			}

			g_pUserManager->OnGoodsGift_A( pSession, &Packet );

		}
		break;	
	case PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK:			// 선물 인증
		{
			UINT8				ui8Type;
			UINT8				ui8Count;
			AUTH_GIFT_INVEN		aAuthGift[MAX_SHOP_GOODS_BUY_COUNT];

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32));
			pPacket->ReadData( &i64UID,					sizeof(T_UID));
			pPacket->ReadData( &ui8Type,				sizeof(UINT8));
			pPacket->ReadData( &ui8Count,				sizeof(UINT8));
			pPacket->ReadData( aAuthGift,				sizeof(AUTH_GIFT_INVEN)*ui8Count );
			
			pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID );
			if (pSession == NULL)	break;
			
			g_pUserManager->OnGiftAuth_A( pSession, ui8Type, ui8Count, aAuthGift );
		}
		break;
	case PROTOCOL_AUTH_SHOP_NOTIFY_GIFT_ACK:			// 선물 받음 알림
		{
			PACKET_AUTH_SHOP_RECV_GIFT_ACK Packet;

			pPacket->ReadData(&i32SessionIdx,			sizeof(INT32));
			pPacket->ReadData(&i64UID,					sizeof(T_UID));

			pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if(pSession == NULL)	break;

			Packet.m_ui32InvenTime = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();

			pPacket->ReadData( &Packet.m_ui32Expire,			sizeof(UINT32));

			UINT8	ui8NickSize;
			wchar_t	wstrSenderNick[ NET_NICK_NAME_SIZE ];
			pPacket->ReadData( &ui8NickSize,					sizeof(UINT8));
			pPacket->ReadData( wstrSenderNick,					sizeof(TTCHAR) * ui8NickSize );
			wstrSenderNick[ ui8NickSize ] = '\0'; 

			Packet.m_strSenderNick = wstrSenderNick;

			UINT8	ui8MessageSize;
			wchar_t wstrMessage[ MAX_GIFT_MESSAGE_SIZE ];
			pPacket->ReadData( &ui8MessageSize,					sizeof(UINT8));
			pPacket->ReadData( wstrMessage,						sizeof(TTCHAR) * ui8MessageSize );
			wstrMessage[ ui8MessageSize ] = '\0';

			Packet.m_strMessage = wstrMessage;

			UINT8 ui8GiftCount;
			pPacket->ReadData( &ui8GiftCount,					sizeof(UINT8));
			pPacket->ReadData( &Packet.m_aGift,					sizeof( S2MO_RECV_GIFT ) * ui8GiftCount );
			Packet.m_aGift.SetCount( ui8GiftCount );

			pSession->S2MOSendEvent( &Packet );	
		}
		break;

	case PROTOCOL_AUTH_SHOP_INSERT_ITEM_ACK				:	//인벤토리에 밀어 넣기 
		{
			UINT8 ui8ItemCount = 0;
			INVEN_BUFFER_DB aItem[MAX_SHOP_DB_ITEM_COUNT];
			INT8	i8RequestType; 
			UINT32	ui32CapsuleItemID = 0;

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32));
			pPacket->ReadData( &i64UID,					sizeof(T_UID));
			pPacket->ReadData( &i8RequestType,			sizeof(INT8)); 
			pPacket->ReadData( &i32Temp,				sizeof(INT32));
			if( EV_SUCCESSED( i32Temp ))
			{
				pPacket->ReadData( &ui8ItemCount,		sizeof(UINT8));
				pPacket->ReadData( aItem,				sizeof(INVEN_BUFFER_DB)*ui8ItemCount);
				pPacket->ReadData(&ui32CapsuleItemID,	sizeof(UINT32));
			}

			g_pUserManager->OnInsertItem_A( i32SessionIdx, i64UID, i32Temp, i8RequestType, ui8ItemCount, aItem, ui32CapsuleItemID);
		}
		break; 
	case PROTOCOL_AUTH_SHOP_DELETE_ITEM_ACK:
		{
			T_ItemDBIdx TItemWareIdx = 0;

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32) );
			pPacket->ReadData( &i64UID,					sizeof(T_UID) );
			pPacket->ReadData( &i32Temp,				sizeof(INT32) );
			if( EV_SUCCESSED( i32Temp ))
			{
				pPacket->ReadData( &TItemWareIdx,	sizeof(T_ItemDBIdx));
			}
			g_pUserManager->OnDeleteItem_A(i32SessionIdx, i64UID, i32Temp, TItemWareIdx );
		}
		break;
	case PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK	: 
		{
			INVEN_BUFFER InvenItem;

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32) );
			pPacket->ReadData( &i64UID,					sizeof(T_UID) );
			pPacket->ReadData( &i32Temp,				sizeof(INT32) );
			if( EV_SUCCESSED( i32Temp ))
			{
				pPacket->ReadData( &InvenItem,			sizeof(INVEN_BUFFER));
			}
			g_pUserManager->OnItemAuth_A(i32SessionIdx, i64UID, i32Temp, &InvenItem );
		}
		break; 
	case PROTOCOL_CHEAT_ITEM_AUTH_ACK :
		{
			INVEN_BUFFER InvenItem;

			pPacket->ReadData(&i32SessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(T_UID));
			pPacket->ReadData(&i32Temp, sizeof(INT32));
			if (EV_SUCCESSED(i32Temp))
			{
				pPacket->ReadData(&InvenItem, sizeof(INVEN_BUFFER));
			}
			g_pUserManager->OnItemCheatAuth_A(i32SessionIdx, i64UID, i32Temp, &InvenItem);
		}
		break;
	case PROTOCOL_AUTH_USED_WEAPON_ACK:
		{
			UINT16 ui16InvenCount;
			INVEN_BUFFER aInvenItem[ MAX_INVEN_COUNT ] = { 0, };

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32) );
			pPacket->ReadData( &i64UID,					sizeof(T_UID) );
			pPacket->ReadData( &ui16InvenCount,			sizeof(UINT16) );
			if( MAX_INVEN_COUNT < ui16InvenCount ) ui16InvenCount = MAX_INVEN_COUNT;
			pPacket->ReadData( aInvenItem,				sizeof(INVEN_BUFFER)*ui16InvenCount);

			g_pUserManager->OnAuthUsedWeapon_A(i32SessionIdx, i64UID, ui16InvenCount, aInvenItem );
		}
		break;	
	case PROTOCOL_AUTH_SHOP_GET_GIFTLIST_ACK:
		{
			PACKET_AUTH_SHOP_GET_GIFTLIST_ACK Packet;

			pPacket->ReadData(&i32SessionIdx,			sizeof(INT32));
			pPacket->ReadData(&i64UID,					sizeof(T_UID));
			pPacket->ReadData(&Packet.m_TResult,		sizeof(T_RESULT));		

			pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if(pSession == NULL)	break;
			 
			UINT8 ui8RealGiftCount;
			if (EV_SUCCESSED(Packet.m_TResult))
			{
				pPacket->ReadData( &Packet.m_ui8TotalGiftCount,		sizeof(UINT8));		// 총 선물 갯수
				pPacket->ReadData( &ui8RealGiftCount,				sizeof(UINT8));		// 이번 패킷에서 선물 갯수
				pPacket->ReadData( &Packet.m_ui8StartIdx,			sizeof(UINT8));		// 배열 인덱스. 
				ui8RealGiftCount = MIN ( ui8RealGiftCount, MAX_SHOP_USER_GIFT_COUNT );

				for( UINT8 i = 0 ; i < ui8RealGiftCount ; ++i )
				{
					pPacket->ReadData( &Packet.m_aGift[i].m_TDBIdx,						sizeof(T_GiftDBIdx));		// 선물 DB Idx.
					pPacket->ReadData( &Packet.m_aGift[i].m_TGoodsID,					sizeof(T_GoodsID));			// 선물 GoodsID.
					pPacket->ReadData( &Packet.m_aGift[i].m_ui8Deliver,					sizeof(UINT8));				// 선물 전달 상태
					pPacket->ReadData( &Packet.m_aGift[i].m_ui32Expire,					sizeof(UINT32));			// 선물 만료 날짜.
					
					pPacket->ReadData( &Packet.m_aGift[i].m_ui8SenderNickSize,			sizeof(UINT8));				// 선물 보낸 사람 닉네임 사이즈. 
					pPacket->ReadData( Packet.m_aGift[i].m_strSenderNick,				sizeof(TTCHAR) * Packet.m_aGift[i].m_ui8SenderNickSize );	// 선물 보낸 사람 닉네임.
					Packet.m_aGift[i].m_strSenderNick[Packet.m_aGift[i].m_ui8SenderNickSize-1] = 0;

					pPacket->ReadData( &Packet.m_aGift[i].m_ui8MessageSize,				sizeof(UINT8));				// 선물 메시지 사이즈.
					pPacket->ReadData( Packet.m_aGift[i].m_strMessage,					sizeof(TTCHAR) * Packet.m_aGift[i].m_ui8MessageSize );	// 선물 메시지.
					Packet.m_aGift[i].m_strMessage[Packet.m_aGift[i].m_ui8MessageSize-1] = 0;					 
				}

			} 
			g_pUserManager->OnGetGift_A( pSession, Packet.m_TResult, &Packet );
		}
		break;	
	case PROTOCOL_AUTH_SHOP_CAPSULE_ACK			:
		{
			UINT32		ui32ItemID;
			UINT8		ui8Capsule;
			UINT8		ui8RewardCount;
			ST_ITEM		stItem[ CAPSULE_REWARD_ITEM_COUNT ];
			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32));
			pPacket->ReadData( &i64UID,					sizeof(T_UID));
			pPacket->ReadData( &ui32ItemID,				sizeof(UINT32));
			pPacket->ReadData( &ui8Capsule,				sizeof(UINT8));
			pPacket->ReadData( &ui8RewardCount,			sizeof(UINT8));
			pPacket->ReadData( stItem,					sizeof(ST_ITEM)*ui8RewardCount );

			pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);

			if (pSession != NULL)
			{
				PACKET_AUTH_SHOP_CAPSULE_ACK Packet;
				Packet.m_ui32ItemID			= ui32ItemID;
				Packet.m_ui8Capsule			= ui8Capsule;
				Packet.m_stItem.SetValue( stItem, 0, ui8RewardCount );
				pSession->S2MOSendEvent( &Packet );

				// 캡슐 아이템 인증에 대한 ZLog 기록 남기기
				g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_INVEN_USE, pSession, &ui32ItemID, NULL, &ui8Capsule ); 
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_JACKPOT_ACK:
		{
			UINT8	ui8NickSize;
			TTCHAR	strNick[ NET_NICK_NAME_SIZE ];
			UINT32	ui32ItemID;
			UINT8	ui8Capsule;
			pPacket->ReadData(&ui8NickSize,			sizeof(UINT8));
			pPacket->ReadData(strNick,				sizeof(TTCHAR) * ui8NickSize);
			pPacket->ReadData(&ui32ItemID,			sizeof(UINT32));
			pPacket->ReadData(&ui8Capsule,			sizeof(UINT8));

			PACKET_AUTH_SHOP_JACKPOT_ACK SendPacket;
			SendPacket.m_strNickName	= strNick;
			SendPacket.m_ui32ItemID		= ui32ItemID;
			SendPacket.m_ui8Capsule		= ui8Capsule;

			g_pUserManager->AnnounceMessagePush( MODULE_TRANS_THREAD_IDX, SendPacket.GetProtocol(), SendPacket );
		}
		break;

	case PROTOCOL_AUTH_LOGOUT_ACK				:	break;	//로그아웃
	case PROTOCOL_BASE_HEART_BIT_ACK			:	break;	//심장 소리 
	case PROTOCOL_AUTH_GET_POINT_CASH_ACK		:
		{
			UINT32 ui32Point	= 0;
			UINT32 ui32Cash		= 0;
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID));
			pPacket->ReadData(&i32Temp,			sizeof(INT32));
			if( EV_SUCCESSED( i32Temp ) )
			{
				pPacket->ReadData(&ui32Point,		sizeof(UINT32));
				pPacket->ReadData(&ui32Cash,		sizeof(UINT32));
			}

			pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if (pSession != NULL)
			{
				pSession->SendGetRemainMoney( i32Temp, ui32Point, ui32Cash );
			}
		}
		break;
	case PROTOCOL_SHOP_PLUS_POINT_ACK:
		{
			UINT32	ui32Point		= 0;
			UINT8	ui8GetRoute;
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID));
			pPacket->ReadData(&ui32Point,		sizeof(UINT32));
			pPacket->ReadData(&ui8GetRoute,		sizeof(UINT8));

			pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if (pSession != NULL)
			{
				pSession->SendGetPoint( MODULE_TRANS_THREAD_IDX, ui32Point, ui8GetRoute );
			}
		}
		break;
	case PROTOCOL_BASE_CHECK_NICK_NAME_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID));
			pPacket->ReadData(&i32Rv,			sizeof(INT32));

			pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if (pSession != NULL)
			{
				pSession->SendCheckNickName( i32Rv );
			}
		}
		break;
	case PROTOCOL_AUTH_CHANGE_NICKNAME_ACK:
		{
			UINT8	ui8Size;
			TTCHAR	strNickName[ NET_NICK_NAME_SIZE ];
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID));
			pPacket->ReadData(&ui8Size,			sizeof(UINT8));	LENGTH_CHECK( ui8Size, NET_NICK_NAME_SIZE );
			pPacket->ReadData(strNickName,		sizeof(TTCHAR) * ui8Size);
			strNickName[ NET_NICK_NAME_SIZE-1 ] = '\0';

			pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if (pSession != NULL)
			{
				pSession->SendNickName( MODULE_TRANS_THREAD_IDX, strNickName );
			}
		}
		break;
	case PROTOCOL_BASE_GET_RECORD_INFO_DB_ACK:
		{
			USER_INFO_RECORD UserRecord;
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID));
			pPacket->ReadData(&i32Rv,			sizeof(INT32));

			if( EV_SUCCESSED( i32Rv ) )
			{
				pPacket->ReadData(&UserRecord,	sizeof(USER_INFO_RECORD));

				UserRecord._match		+= UserRecord._smatch;
				UserRecord._win			+= UserRecord._swin;
				UserRecord._lose		+= UserRecord._slose;
				UserRecord._draw		+= UserRecord._sdraw;
				UserRecord._killcount	+= UserRecord._skillcount;
				UserRecord._headshot	+= UserRecord._sheadshot;
				UserRecord._death		+= UserRecord._sdeath;
				UserRecord._dmatch		+= UserRecord._sdmatch;
				UserRecord._dkillcount	+= UserRecord._sdkillcount;
				UserRecord._dis			+= UserRecord._sdis;
				UserRecord._AssistCount += UserRecord._sAssistCount;
			}
			else
			{
				i3mem::FillZero( &UserRecord, sizeof( USER_INFO_RECORD ) );
			}

			pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if (pSession != NULL)
			{
				pSession->SendUserRecord( &UserRecord );
			}
		}
		break;
#ifdef USE_TRANS_FIND
	case PROTOCOL_LOBBY_FIND_NICK_NAME_ACK:	// 유저찾기 (닉네임으로)
		{
			INT32 sessionIdx;
			UINT32 dbIdx;

			pPacket->ReadData(&sessionIdx, sizeof(INT32));
			pPacket->ReadData(&dbIdx, sizeof(UINT32));

			CUserSession* session = g_pUserManager->GetSession(sessionIdx, dbIdx);

			if (session != NULL)
			{
				INT32 result;
				UINT32 userDbIdx		= 0;
				UINT32 userConnectState = 0;

				pPacket->ReadData(&result, sizeof(INT32));

				if (EV_SUCCESSED(result))
				{
					pPacket->ReadData(&userDbIdx, sizeof(UINT32));
					pPacket->ReadData(&userConnectState, sizeof(UINT32));					
				}

				session->SendFindUser(result, userDbIdx, userConnectState);
			}
			else
			{
				//유저없음 -명일-
				//SendUnknownUser_A(dbIdx); 
			}
		}
		break;
#endif	
	
	case PROTOCOL_ASC_WEB_USER_INFO_LOCATION_REQ:
		{
			INT32	i32WebSessionIdx = -1;
			i32Rv = EVENT_ERROR_FAIL;

			pPacket->ReadData( &i32WebSessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,				sizeof(T_UID) );
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32) );

			i3NetworkPacket SendPacket( PROTOCOL_ASC_WEB_USER_INFO_LOCATION_ACK );
			SendPacket.WriteData( &i32WebSessionIdx,	sizeof(INT32) );

			pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if( pSession != NULL )
			{
				INT8	i8ServerIdx		= (INT8)g_pContextMain->m_i32ServerIdx;
				INT8	i8ChannelNum	= (INT8)(pSession->m_i32ChannelNum + 1);
				INT32	i32RoomIdx		= pSession->m_i32RoomIdx + 1;
				i32Rv = EVENT_ERROR_SUCCESS;
				SendPacket.WriteData( &i32Rv,			sizeof(INT32) );
				SendPacket.WriteData( &i8ServerIdx,		sizeof(INT8) );
				SendPacket.WriteData( &i8ChannelNum,	sizeof(INT8) );
				SendPacket.WriteData( &i32RoomIdx,		sizeof(INT32) );
			}
			else
			{
				SendPacket.WriteData( &i32Rv,			sizeof(INT32) );
			}

			SendPacketMessage( &SendPacket );
		}
		break;
	case PROTOCOL_BASE_ATTENDANCE_ACK	:	// 출석 요청
		{
			ATTENDANCE_USER_SVR	stAttendance;

			pPacket->ReadData( &i32SessionIdx,		sizeof( INT32 ) );
			pPacket->ReadData( &i64UID,				sizeof( T_UID ) );
			pPacket->ReadData( &i32Rv,				sizeof( INT32 ) );
			pPacket->ReadData( &stAttendance,		sizeof( ATTENDANCE_USER_SVR ) );

			g_pUserManager->GetAttendanceCheck( i32SessionIdx, i64UID, i32Rv, &stAttendance ); 
		}
		break;
	case PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_ACK	: // 아이템 보상 지급 요청
		{
			// 출석체크 이벤트 완료 보상 요청결과
			pPacket->ReadData( &i32SessionIdx,	sizeof(INT32) );				//SessionIDX
			pPacket->ReadData( &i64UID,			sizeof(T_UID) );					//DBIDX
			pPacket->ReadData( &i32Rv,			sizeof(INT32) );				//Result 

			g_pUserManager->GetAttendanceResult( i32SessionIdx, i64UID, i32Rv ); 
		}
		break;
	case PROTOCOL_LOBBY_SET_BIRTHDAY_ACK:
		{
			UINT32 ui32BirthDay = 0;

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32)); 
			pPacket->ReadData( &i64UID,					sizeof(T_UID));
			pPacket->ReadData( &i32Rv,					sizeof(INT32));
			if( EV_SUCCESSED( i32Rv ) )
			{
				pPacket->ReadData( &ui32BirthDay,		sizeof(UINT32));
			}

			g_pUserManager->UserSetBirthDay_A( i32SessionIdx, i64UID, i32Rv, ui32BirthDay );
		}
		break;

	case PROTOCOL_CHAR_DELETE_CHARA_ACK:			// 캐릭터 삭제.
		{
			UINT8			ui8DeleteSlotIdx = 0; 

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32)); 
			pPacket->ReadData( &i64UID,					sizeof(T_UID));

			pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID );
			if( NULL == pSession )	break;

			pPacket->ReadData( &i32Rv,					sizeof(INT32));

			if( EV_SUCCESSED( i32Rv ))
			{
				pPacket->ReadData( &ui8DeleteSlotIdx,	sizeof(UINT8));		
				
				g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_CHARA_DELETE, pSession, &ui8DeleteSlotIdx ); 
			}

			pSession->DeleteChar_A(MODULE_TRANS_THREAD_IDX, i32Rv, ui8DeleteSlotIdx );
		}
		break;

	case PROTOCOL_AUTH_SPECIALITY_CLASS_SETTING_ACK:
		{
			 // 사용안함.
		}
		break;
	
	case PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK:
		{
			T_UID				TUID;
			T_RESULT			TResult;
			S2_USER_DETAIL_INFO	UserInfo;	

			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32)); 
			pPacket->ReadData( &TUID,				sizeof(T_UID));
			pPacket->ReadData( &TResult,			sizeof(T_RESULT) );
			pPacket->ReadData( &UserInfo,			sizeof(S2_USER_DETAIL_INFO) );

			pSession = g_pUserManager->GetSession( i32SessionIdx, TUID );
			if( NULL != pSession )
			{
				pSession->SendFindDetailUserInfo( TResult, &UserInfo );
			}
		}
		break;

	case PROTOCOL_SHOP_REPAIR_ACK:			// 전체수리
		{
			UINT32	 ui32UserCash;
			S2MO_REPAIR_INFO  stRepair;
			
			pPacket->ReadData( &i32SessionIdx,					sizeof(INT32)); 
			pPacket->ReadData( &i64UID,							sizeof(T_UID));
			pPacket->ReadData( &ui32UserCash,					sizeof(UINT32));
			pPacket->ReadData( &stRepair.m_ui8ItemCount,		sizeof(UINT8 ));
			pPacket->ReadData( stRepair.m_stRepairItem,			sizeof(S2MO_REPAIR_ITEM)*stRepair.m_ui8ItemCount );	

			pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID );
			if( NULL == pSession )	break;

			pSession->m_UserInfoBasic.m_ui32Cash = ui32UserCash;

			S2MO_SHOP_REPAIR_RESULT		aRepairResult[MAX_SHOP_GOODS_BUY_COUNT];
			for( UINT8 i = 0 ; i < stRepair.m_ui8ItemCount ; i++ )
			{ 
				S2MO_REPAIR_ITEM*pItem = &stRepair.m_stRepairItem[i];

				aRepairResult[i].m_i32Rv = pItem->m_i32Rv;
				if( EV_FAILED( pItem->m_i32Rv ) && GOODS_BUY_POINT == pItem->m_ui8BuyType)
				{	// 실패시 포인트 복구.
					pSession->m_UserInfoBasic.m_ui32Point	+= pItem->m_ui32RepairPoint;
					continue;
				}
				aRepairResult[i].m_TItemID = pItem->m_TItemID;

				// ZLOG 기록
				REPAIR_INFO stLogRepair; 
				stLogRepair.m_ui32Point		= pItem->m_ui32RepairPoint; // 수리에 소모된 포인트
				stLogRepair.m_ui32Cash		= pItem->m_ui32RepairCash;
				stLogRepair.m_ui32Arg		= pItem->m_ui32AuthArg;
				stLogRepair.m_ui32ItemID	= pItem->m_TItemID;
				g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_REPAIR, pSession, &stLogRepair );

				
			}

			// 인벤토리 갱신 여부를 클라이언트에 알려줍니다.
			pSession->OnRepairItem( &stRepair );

			pSession->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );
		
			pSession->SendRepairResult( stRepair.m_ui8ItemCount, aRepairResult );
		}
		break;

	case PROTOCOL_AUTH_USE_GIFTCOUPON_ACK:
		{
			INT32	i32Ret;
			char	strCouponCode[GIFTCOUPON_LENGTH];
			INT8	i8GoodsCount;
			INT32	ai32GoodsID[MAX_GIFTCOUPON_COUNT];

			i3mem::FillZero( ai32GoodsID, sizeof( ai32GoodsID ) );

			pPacket->ReadData( &i32SessionIdx,	sizeof( INT32 ) );
			pPacket->ReadData( &i64UID,			sizeof( T_UID ) );
			pPacket->ReadData( &i32Ret,			sizeof( INT32 ) );
			pPacket->ReadData( strCouponCode,	GIFTCOUPON_LENGTH );
			if ( EV_SUCCESSED( i32Ret ) )
			{
				pPacket->ReadData( &i8GoodsCount,	sizeof( INT8 ) );
				pPacket->ReadData( ai32GoodsID,		sizeof( INT32 )*i8GoodsCount );
			}

			pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID );
			if( NULL == pSession )	break;

			// 유저에게 성공/실패&실패사유를 알린다.
			PACKET_AUTH_SHOP_USE_COUPON_ACK Packet;
			Packet.m_TResult = i32Ret;
			pSession->S2MOSendEvent( &Packet );

			// 쿠폰 사용이 성공했다면, ZLog에 기록을 남긴다. ( UID, 쿠폰번호, 지급받은 GoodsID )
			if ( EV_SUCCESSED( i32Ret ) )
			{
				g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_USED_COUPON, pSession, strCouponCode, ai32GoodsID );
			}
		}
		break;

	case PROTOCOL_COMMUNITY_USER_INVITED_ACK:
		{// 배틀룸에 친구 초대가 실패하면 에러 코드를 전송한다.(EVENT_ERROR_EVENT_NOT_LOCATION_LOBBY_USER)
			PACKET_TRANS_COMMUNITY_USER_INVITED_ACK	PacketAck;

			pPacket->ReadData( &PacketAck, sizeof(PACKET_TRANS_COMMUNITY_USER_INVITED_ACK) );

			pSession = g_pUserManager->GetSession(PacketAck.m_i32SessionIdx, PacketAck.m_i64UID); 
			if(pSession != NULL)
			{
				pSession->SendFriendInvitedError(PacketAck.m_i32Rv);
			}
		}
		break;

	case PROTOCOL_COMMUNITY_USER_INVITED_REQUEST_ACK:
		{// 배틀룸에 친구초대가 성공한 경우 친구에게 초대 메시지를 전송한다.
			PACKET_TRANS_COMMUNITY_USER_INVITED_REQUEST_ACK	PacketAck;
			pPacket->ReadData( &PacketAck, sizeof(PACKET_TRANS_COMMUNITY_USER_INVITED_REQUEST_ACK) );
				
			g_pUserManager->OnInviteUserServer(&PacketAck);
		}
		break;
		
	case PROTOCOL_SS_GACHA_NOTICE_ACK:
		{
			PACKET_GACHA_NOTICE stNotice[GACHA_NOTICE_COUNT];

			CGachaShop* pGachaShop = g_pContextMain->GetGachaShop(); 

			pPacket->ReadData(pGachaShop->GetGachaShopMD5Key(), sizeof(char) * NET_MD5_KEY_SIZE);

			UINT8 ui8NoticeCount;
			pPacket->ReadData(&ui8NoticeCount, sizeof(UINT8));

			ui8NoticeCount = MIN( ui8NoticeCount, GACHA_NOTICE_COUNT );

			pPacket->ReadData(stNotice, sizeof(PACKET_GACHA_NOTICE) * ui8NoticeCount );

			for( UINT8 i = 0 ; i < ui8NoticeCount ; ++i )
			{
				pGachaShop->InsertNotice(stNotice[i].m_szNotice);
			}
			 
			GACHA_GROUP eGachaGroup = GACHA_GROUP_POOR;
			i3NetworkPacket stGachaReq(PROTOCOL_SS_GACHA_ITEM_INFO_REQ);
			stGachaReq.WriteData( &eGachaGroup,	sizeof(GACHA_GROUP) );
			SendPacketMessage(&stGachaReq);
		}
		break;


	case PROTOCOL_SS_GACHA_ITEM_INFO_ACK:
		{
			SS_GACHA_GROUP_INFO stGachaGroup;

			CGachaShop* pGachaShop = g_pContextMain->GetGachaShop(); 

			bool	bActive = false;

			pPacket->ReadData(&stGachaGroup.m_eGachaGroup,			sizeof(GACHA_GROUP));
			pPacket->ReadData(&bActive,								sizeof(bool));

			if( bActive )
			{
				//pPacket->ReadData(&stGachaGroup.m_ui32PaymentGoodsID,	sizeof(UINT32));
				//pPacket->ReadData(&stGachaGroup.m_i32RequireTicket,		sizeof(INT32));
				pPacket->ReadData(&stGachaGroup.m_ui32LuckyGoodsID,		sizeof(UINT32));
				pPacket->ReadData(&stGachaGroup.m_i32LuckyProbability,	sizeof(INT32));
//				pPacket->ReadData(&stGachaGroup.m_i32GloryCount,		sizeof(INT32));

				pPacket->ReadData(&stGachaGroup.m_i32WinItemCount,		sizeof(INT32));
				pPacket->ReadData(stGachaGroup.m_arWinItems,			sizeof(SS_GACHA_ITEM_INFO)*stGachaGroup.m_i32WinItemCount);

				pPacket->ReadData(&stGachaGroup.m_i32RandomItemCnt,		sizeof(INT32));
				pPacket->ReadData(stGachaGroup.m_arRandomGoodsIDs,		sizeof(UINT32)*stGachaGroup.m_i32RandomItemCnt);


				pGachaShop->SetGachaGroupInfo(&stGachaGroup);
				pGachaShop->Initialize(stGachaGroup.m_eGachaGroup);
			}

			INT32 i32GroupIdx = (INT32)stGachaGroup.m_eGachaGroup;
			++i32GroupIdx;
			GACHA_GROUP eGachaGroup = (GACHA_GROUP)i32GroupIdx;

			if( eGachaGroup >= GACHA_GROUP_COUNT ) 
			{
				break;
			}
			
			i3NetworkPacket stGachaReq(PROTOCOL_SS_GACHA_ITEM_INFO_REQ);
			stGachaReq.WriteData( &eGachaGroup,	sizeof(GACHA_GROUP) );
			SendPacketMessage(&stGachaReq);
		}
		break;

	case PROTOCOL_SS_GACHA_WINNING_USER_ACK:
		{
			PACKET_GACHA_WINNING_USER stLuckyUser;
			pPacket->ReadData(&stLuckyUser, sizeof(PACKET_GACHA_WINNING_USER));

			if( stLuckyUser.m_eGachaResult == GACHA_RESULT_LUCKY )
			{
				CGachaShop* pGachaShop = g_pContextMain->GetGachaShop();
				pGachaShop->SetLuckyUser(stLuckyUser.m_szLuckyUser, stLuckyUser.m_ui32LuckyGoodsID);
				pGachaShop->SetLuckyTime(); 
				stLuckyUser.m_ui32LuckyTime = pGachaShop->GetLuckyTime();
			}

			PACKET_GACHA_WINNING_USER_ACK SendPacket;

			SendPacket.m_eGachaResult		= stLuckyUser.m_eGachaResult;
			SendPacket.m_ui32LuckyGoodsID	= stLuckyUser.m_ui32LuckyGoodsID;
			SendPacket.m_strNickName		= stLuckyUser.m_szLuckyUser;
			SendPacket.m_ui32LuckyTime		= stLuckyUser.m_ui32LuckyTime;
			
			g_pUserManager->AnnounceMessagePush( MODULE_TRANS_THREAD_IDX, SendPacket.GetProtocol(), SendPacket );
		}
		break;

	case PROTOCOL_GACHA_SHOP_STATE_ACK:
		{
			PACKET_GACHA_SHOP_STATE stGachaShopState;
			pPacket->ReadData(&stGachaShopState, sizeof(stGachaShopState));

			CGachaShop* pGachaShop = g_pContextMain->GetGachaShop();
			pGachaShop->UpdateGachaState(&stGachaShopState); 
		}
		break;

	case PROTOCOL_SS_GACHA_PURCHASE_ACK:
		{
			PACKET_GACHA_SS_RESULT_HEADER stResult;
			PACKET_GACHA_RESULT_DATA stDatas[MAX_GACHA_GAME_COUNT];
			UINT8 ui8ItemCount = 0;
			INVEN_BUFFER_DB aItem[MAX_SHOP_DB_ITEM_COUNT];
			
			pPacket->ReadData(&stResult,	sizeof(PACKET_GACHA_SS_RESULT_HEADER));
			
			if( GACHA_ERROR_SUCCESS == stResult.m_eErrorCode )
			{
				pPacket->ReadData(stDatas,			sizeof(PACKET_GACHA_RESULT_DATA) * stResult.m_i32ResultCount );
				pPacket->ReadData(&ui8ItemCount,	sizeof(UINT8) );
				pPacket->ReadData(aItem,			sizeof(INVEN_BUFFER_DB) * ui8ItemCount );
			}

			CUserSession* pUserSession = g_pUserManager->GetSession(stResult.m_TUID);
			if( pUserSession == NULL )	break;

			pUserSession->SendGachaPurchaseAck( &stResult, stDatas, ui8ItemCount, aItem );
		}
		break;
	case PROTOCOL_GET_MEDAL_REWARD_ACK				:	// 메달 보상 지급 결과
		{
			UINT8 ui8Count = 0;
			INVEN_BUFFER_DB				aItem[MAX_SHOP_DB_ITEM_COUNT]; 
			MEDAL_REWARD_DETAIL_INFO	aRewardDetail[MAX_SHOP_DB_ITEM_COUNT];	// 보상 상세 정보
			

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32));
			pPacket->ReadData( &i64UID,					sizeof(T_UID));
			pPacket->ReadData( &i32Temp,				sizeof(INT32));
			pPacket->ReadData( &ui8Count,				sizeof(UINT8));
			if( EV_SUCCESSED( i32Temp ))
			{
				pPacket->ReadData( aItem,				sizeof(INVEN_BUFFER_DB)*ui8Count);
			}
			pPacket->ReadData( aRewardDetail,			sizeof(MEDAL_REWARD_DETAIL_INFO)*ui8Count);

			g_pUserManager->OnMedalReward_A( i32SessionIdx, i64UID, i32Temp,  ui8Count, aItem, aRewardDetail );
		}
		break;

	case PROTOCOL_GMCHAT_APPLY_PENALTY_ACK			:	//유저 제재 적용 결과
		{
			INT8	i8PenaltyType = 0;
			INT32	i32PenaltyAmount = 0;

			pPacket->ReadData( &i32Rv,				sizeof(INT32));
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32));

			if( EV_SUCCESSED( i32Rv ))
			{
				pPacket->ReadData( &i64UID,				sizeof(T_UID));
				pPacket->ReadData( &i8PenaltyType,		sizeof(INT8));
				pPacket->ReadData( &i32PenaltyAmount,	sizeof(INT32));
			}

			//적용결과 GM계정에 전달
			CUserSession* pUserSession = g_pUserManager->GetSession(i32SessionIdx);

			if( NULL != pUserSession )
			{
				PACKET_GMCHAT_APPLY_PENALTY_ACK	Send;
				Send.m_TResult = i32Rv;
				pUserSession->S2MOSendEvent( &Send );
			}
			
			//적용결과 유저에게 통보
			if( 0 != i64UID )
			{
				CUserSession* pNotiUserSession = g_pUserManager->GetSession(i64UID);

				if( NULL != pNotiUserSession )
				{
					
					UINT32 ui32CurrentTime = i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI();
					
					switch(i8PenaltyType)
					{
					case GMCHAT_PENALTY_BLOCK:
						{
							pNotiUserSession->SendGMChatUserNoti( i8PenaltyType, i32PenaltyAmount );
							g_pUserManager->KickUser_A( pNotiUserSession->m_SessionIdx, i64UID, KILL_REASON_GMCHAT_BLOCK );						
						}
						break;

					case GMCHAT_PENALTY_CHAT:
						{	//채팅 금지인 경우 채팅금지 시간 갱신			

							//적용할 패널티가 없는 경우 유저에게 NOTI를 하지 않습니다.
							if( 0 == i32PenaltyAmount && pNotiUserSession->GetChatBlockDate() < ui32CurrentTime )  break;
							
							//채팅금지가 진행중인경우
							if(	pNotiUserSession->GetChatBlockDate() > ui32CurrentTime )
							{
								pNotiUserSession->SetChatBlockDate( g_GetAddAuthTime( pNotiUserSession->GetChatBlockDate(), i32PenaltyAmount) );
							}
							else
							{
								pNotiUserSession->SetChatBlockDate( g_GetAddAuthTime( ui32CurrentTime, i32PenaltyAmount) );
							}

							if( 0 == i32PenaltyAmount )
								pNotiUserSession->SetChatBlockDate(1001010000);

							pNotiUserSession->SendGMChatUserNoti( i8PenaltyType, i32PenaltyAmount );
						}
						break;
					}
				}
			}
			
		}
		break;

	case PROTOCOL_BASE_CHANGE_PCCAFE_STATUS_ACK		:	// PC방 효과 변경
		{
			UINT8 ui8PCCafe;

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32));
			pPacket->ReadData( &i64UID,					sizeof(T_UID));
			pPacket->ReadData( &ui8PCCafe,				sizeof(UINT8));

			pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID );

			pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID );
			if ( pSession )
			{
				pSession->ChangePCCafeBonus_A( ui8PCCafe );
			}
		}
		break;

	case PROTOCOL_BASE_MEGAPHONE_ACK	: //확성기 내용 전달
		{
			UINT8	ui8NickLength;
			UINT8	ui8Message;

			TTCHAR	strNickName[NET_NICK_NAME_SIZE];
			TTCHAR	strMessage[MAX_CHATTING_COUNT];

			pPacket->ReadData( &ui8NickLength,			sizeof(UINT8) );
			pPacket->ReadData( &strNickName,			sizeof(TTCHAR) * NET_NICK_NAME_SIZE );
			pPacket->ReadData( &ui8Message,				sizeof(UINT8));
			pPacket->ReadData( &strMessage,				ui8Message );

			PACKET_BASE_MEGAPHONE_ACK	SendPacket;

			SendPacket.m_strNickName	= strNickName;
			SendPacket.m_strChatMessage	= strMessage;

			g_pUserManager->AnnounceMessagePush( MODULE_TRANS_THREAD_IDX, SendPacket.GetProtocol(), SendPacket );

		}
		break;

	case PROTOCOL_BASE_GENERALRANK_CHANGE_ACK:
		{
			GENERAL_RANK_RESULT stGeneralRankResult;

			pPacket->ReadData( &i32SessionIdx,	sizeof(INT32)); 
			pPacket->ReadData( &stGeneralRankResult, sizeof(GENERAL_RANK_RESULT) );

			pSession = g_pUserManager->GetSession(i32SessionIdx, stGeneralRankResult.m_TUID);
			if( pSession != NULL )
			{
				pSession->m_ui32ChangedRank = stGeneralRankResult.m_ui32ChangeRank;
			}
		}
		break;

	case PROTOCOL_CHAR_CHANGE_STATE_ACK:
		{
			UINT8			ui8SlotIdx		= 0; 
			UINT8			ui8SlotState	= 0; 

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32)); 
			pPacket->ReadData( &i64UID,					sizeof(T_UID));

			pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID );
			if( NULL == pSession )	break;

			pPacket->ReadData( &i32Rv,					sizeof(INT32));

			if( EV_SUCCESSED( i32Rv ))
			{
				pPacket->ReadData( &ui8SlotIdx,		sizeof(UINT8));		
				pPacket->ReadData( &ui8SlotState,	sizeof(UINT8));		 
			}

			pSession->ChangeStateChar_A( i32Rv, ui8SlotIdx, ui8SlotState );
		}
		break;
	case PROTOCOL_AUTH_GIFT_INSERT_ACK:
		{
			UINT8 ui8NickSize		= 0;
			UINT8 ui8MessageSize	= 0;
			UINT8 ui8InsertType = 0;

			TTCHAR	strNickName[NET_NICK_NAME_SIZE];
			TTCHAR	strMessage[MAX_CHATTING_COUNT]; 

			pPacket->ReadData(&i32Rv, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(T_UID));
			pPacket->ReadData(&ui8InsertType, sizeof(UINT8));
			pPacket->ReadData(&ui8NickSize, sizeof(UINT8));
			pPacket->ReadData(&ui8MessageSize, sizeof(UINT8));
			pPacket->ReadData(&strNickName, sizeof(TTCHAR) * ui8NickSize);
			pPacket->ReadData(&strMessage, sizeof(TTCHAR) * ui8MessageSize);

			strNickName[ui8NickSize] = '\0';
			strMessage[ui8MessageSize] = '\0';

			g_pModuleMessenger->MessengerSendNoteGiftNotice(MODULE_TRANS_THREAD_IDX, i64UID, ui8MessageSize, strMessage, ui8NickSize, strNickName);

		}
		break;
	default: 
		g_pLog->WriteLog( L"[CModuleTransSocket::PacketParsing]Unknown Message" ); 
		break; 
	}

	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
	m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();
	return; 
}

INT32	CModuleTransSocket::SendPacketMessage( i3NetworkPacket* packet )
{
//#if defined( USE_FINISH_PACKET )
//	UINT32 ui32FinishPacket = FINISH_PACKET;
//	if( FALSE == packet->WriteData( &ui32FinishPacket, sizeof(UINT32) ) )
//	{
//		g_pLog->WriteLog(g_i32TotalLogIdx, "[ERROR] Finish Packet Error, Protocol : %d, Size : %d", packet->GetProtocolID(), packet->GetPacketSize() );
//		return -1;
//	}
//#endif
	return i3NetworkClientSocket::SendPacketMessage( packet );
}




