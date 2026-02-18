#include "pch.h"
#include "DataBase.h"
#include "ModuleTransSocket.h"
#include "ModuleTrans.h"
#include "UserManager.h"
#include "UserSession.h"
#include "ModuleContextNc.h"
#include "RoomManager.h"
#include "Room.h"

I3_CLASS_INSTANCE( CModuleTransSocket, i3NetworkClientSocket );

CModuleTransSocket::CModuleTransSocket(void)
{
	m_bConnect		= FALSE; 
}

CModuleTransSocket::~CModuleTransSocket(void)
{
	OnDestroy();
}

BOOL CModuleTransSocket::OnCreate(char * pAddress, UINT16 ui16PortNo)
{
	BOOL bRv = i3NetworkClientSocket::OnCreate(pAddress, ui16PortNo);
	m_ui32LastRecvTime	= g_pContextMain->GetServerTime();
	m_ui32LastSendTime	= g_pContextMain->GetServerTime();

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
	UINT32		UTemp = 0; 
	INT32		i32Temp;
	INT32		i32Rv;
	INT32		i32SessionIdx;
	TUID		i64UID;

	switch(ProtocolID)
	{
	case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS	:	//연결 되었음 //PROTOCOL_AUTH_SHOP_GET_SAILLIST_ACK에서 받는값과 동일한 값이 옵니다.		
		{
			g_pModuleLogFile->Write_M_Log(MODULE_TRANS_THREAD_IDX,NULL, "AUTH SERVER CONNECT MESSAGE RECV");			
			m_bConnect = TRUE;
			pPacket->ReadData( &g_pModuleTrans->m_ui8RSEnable,		sizeof(UINT8) );

			if( 1 == g_pModuleTrans->m_ui8RSEnable )
			{
				g_pModuleLogFile->Write_M_Log(MODULE_TRANS_THREAD_IDX,NULL, "[Trans Server Connect] RS ON ");			
			}

			// 접속 완료후 버전을 얻어온다.
			i3NetworkPacket SendPacket( PROTOCOL_AUTH_SHOP_VERSION_REQ );
			SendPacketMessage( &SendPacket );
		}
		break; 
	case PROTOCOL_BASE_LOGIN_ACK		:					//로그인 응답 
		{
			UINT32	ui32AuthKey;
			char	strCustom[NORTH_CUSTOM_MSG_SIZE] = "\0";	
			char	strUID[TURKEY_UID_SIZE] = "\0";	 
			char	strId[NET_ID_SIZE] = "\0";
			UINT8	ui8CustomSize;
			UINT8	ui8LinkThreadIdx;
			UINT32	ui32LoginDelayTime;
			UINT8	ui8UIDSize;
			UINT8	ui8IDSize;
			UINT8	ui8Popup;

			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32) );		//SessionIDX
			pPacket->ReadData( &i64UID,				sizeof(TUID) );			//DBIDX
			pPacket->ReadData( &i32Rv,				sizeof(INT32) );		//Result 
			pPacket->ReadData( &ui32AuthKey,		sizeof(UINT32) );		//Login AuthKey
			pPacket->ReadData( &ui8LinkThreadIdx,	sizeof(UINT8) );		//Link Thread Idx
			pPacket->ReadData( &ui32LoginDelayTime,	sizeof(UINT32) );		//Link Thread Idx
			pPacket->ReadData( &ui8Popup,			sizeof(UINT8) );		//Login After Popup
 
			// ID
			pPacket->ReadData( &ui8IDSize,	sizeof(UINT8) );			
			if( ui8IDSize > 0 )  pPacket->ReadData( &strId,	ui8IDSize );				

			//Custom Message	// 터키는 TempKey
			pPacket->ReadData( &ui8CustomSize,	sizeof(UINT8) );			
			if( ui8CustomSize > 0 )  pPacket->ReadData( &strCustom,	ui8CustomSize );				

			// 터키 UID
			pPacket->ReadData( &ui8UIDSize,		sizeof(UINT8) );
			if( ui8UIDSize > 0 )  pPacket->ReadData( &strUID,	ui8UIDSize );				

			g_pUserManager->LogInUser_A( i32SessionIdx, i64UID, i32Rv, ui32AuthKey, strId, strCustom, strUID, ui8LinkThreadIdx, ui32LoginDelayTime, ui8Popup );
 
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
	case PROTOCOL_BASE_GET_MYINFO_ACK	:
		{
			pPacket->ReadData( &i32SessionIdx,	sizeof(INT32) );				//SessionIDX
			pPacket->ReadData( &i64UID,			sizeof(TUID) );					//DBIDX
			pPacket->ReadData( &i32Rv,			sizeof(INT32) );				//Result 

			g_pUserManager->GetMyInfo_A( i32SessionIdx, i64UID, i32Rv, pPacket ); 
		}
		break;

	case PROTOCOL_BASE_USER_ENTER_ACK:
		{
			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32)); 
			pPacket->ReadData( &i64UID,					sizeof(TUID));
			pPacket->ReadData( &i32Rv,					sizeof(INT32));

			g_pUserManager->UserServerEnter_A( i32SessionIdx, i64UID, i32Rv, pPacket );
		}
		break;
	case PROTOCOL_BASE_USER_LEAVE_ACK:
		{
			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32)); 
			pPacket->ReadData( &i64UID,					sizeof(TUID));
			pPacket->ReadData( &i32Rv,					sizeof(INT32));

			g_pUserManager->UserServerLeave_A( i32SessionIdx, i64UID, i32Rv );
		}
		break;
	case PROTOCOL_LOBBY_CREATE_NICK_NAME_ACK:
		{
			UINT8 ui8NickSize;
			char strNickName[ NET_NICK_NAME_SIZE ];

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32)); 
			pPacket->ReadData( &i64UID,					sizeof(TUID));
			pPacket->ReadData( &i32Rv,					sizeof(INT32));
			if( EV_SUCCESSED( i32Rv ) )
			{
				pPacket->ReadData( &ui8NickSize,		sizeof(UINT8));	LENGTH_CHECK( ui8NickSize, NET_NICK_NAME_SIZE );
				pPacket->ReadData( strNickName,			ui8NickSize);
				strNickName[ NET_NICK_NAME_SIZE-1 ] = '\0';
			}

			g_pUserManager->UserCreateNickName_A( i32SessionIdx, i64UID, i32Rv, strNickName );
		}
		break;

	case PROTOCOL_GSM_ACCOUNT_KICK_REQ	:					//강제 퇴장.
		{
			UINT8 ui8Reason;
			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32) ); 
			pPacket->ReadData( &i64UID,					sizeof(TUID) ); 			
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
			pPacket->ReadData(&UTemp,	sizeof(UINT32)); 
			g_pUserManager->SendUserNowInfo_A(UTemp);
		}
		break; 
	case PROTOCOL_AUTH_ROOM_DESTROY		:			//방 폭파^^;	
		{
			pPacket->ReadData(&UTemp,	sizeof(UINT32));             
			g_pUserManager->DestroyRoom_A(UTemp); 
		}
		break;

	case PROTOCOL_AUTH_SHOP_VERSION_ACK:
		{
			UINT32 ui32Version;
			CShop* pShop = g_pContextMain->GetShop();
			pPacket->ReadData(&ui32Version,	sizeof(UINT32));
			if( pShop->GetVersion() != ui32Version )
			{	// 버전이 맞지 않으면 상품 리스트 요청
				i3NetworkPacket SendPacket( PROTOCOL_AUTH_SHOP_LIST_REQ );
				SendPacketMessage( &SendPacket );
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_GOODSLIST_ACK:
		{
			UINT32 ui32GoodsCount;
			UINT32 ui32CurGoodsCount;
			UINT32 ui32StartIdx;

			CShop* pShop = g_pContextMain->GetShop();
			SHOP_GOODS_BASE*		pGoods		= pShop->GetGoodsAllData();
			SHOP_GOODS_EXPANSION*	pGoodsEx	= pShop->GetGoodsAllDataEx();

			pPacket->ReadData(&ui32GoodsCount,		sizeof(UINT32));
			pPacket->ReadData(&ui32CurGoodsCount,	sizeof(UINT32));
			pPacket->ReadData(&ui32StartIdx,		sizeof(UINT32));
			UINT32 ui32EndIdx = ui32StartIdx + ui32CurGoodsCount;
			for( UINT32 i = ui32StartIdx ; i < ui32EndIdx ; i++ )
			{
				pPacket->ReadData(&pGoods[i],		sizeof(SHOP_GOODS_BASE));
				pPacket->ReadData(&pGoodsEx[i],		sizeof(SHOP_GOODS_EXPANSION));
			}
			pShop->SetGoodsCount( ui32GoodsCount );
		}
		break;
	case PROTOCOL_AUTH_SHOP_ITEMLIST_ACK:
		{
			UINT32 ui32ItemCount;
			UINT32 ui32CurItemCount;
			UINT32 ui32StartIdx;

			CShop* pShop = g_pContextMain->GetShop();
			SHOP_ITEM_BASE*		pItem		= pShop->GetItemAllData();
			SHOP_ITEM_DATA*		pItemEx		= pShop->GetItemAllDataEx();

			pPacket->ReadData(&ui32ItemCount,		sizeof(UINT32));
			pPacket->ReadData(&ui32CurItemCount,	sizeof(UINT32));
			pPacket->ReadData(&ui32StartIdx,		sizeof(UINT32));
			UINT32 ui32EndIdx = ui32StartIdx + ui32CurItemCount;
			for( UINT32 i = ui32StartIdx ; i < ui32EndIdx ; i++ )
			{
				pPacket->ReadData( &pItem[i],		sizeof(SHOP_ITEM_BASE));
				pPacket->ReadData( &pItemEx[i],		sizeof(SHOP_ITEM_EXPANSION));
			}
			pShop->SetItemCount( ui32ItemCount );
		}
		break;
	case PROTOCOL_AUTH_SHOP_REPAIRLIST_ACK:
		{
			UINT32 ui32RepairCount = 0;
			UINT32 ui32CurRepairCount;
			UINT32 ui32StartIdx;

			CShop* pShop = g_pContextMain->GetShop();
			SHOP_ITEM_REPAIR_DATA* pRepair = pShop->GetRepairAllData();

			pPacket->ReadData(&ui32RepairCount,		sizeof(UINT32));
			pPacket->ReadData(&ui32CurRepairCount,	sizeof(UINT32));
			pPacket->ReadData(&ui32StartIdx,		sizeof(UINT32));
			pPacket->ReadData(&pRepair[ui32StartIdx],	sizeof(SHOP_ITEM_REPAIR_DATA)*ui32CurRepairCount);
			pShop->SetRepairCount( ui32RepairCount );
		}
		break;
	case PROTOCOL_AUTH_SHOP_MATCHINGLIST_ACK:
		{
			UINT32 ui32MatchingCount = 0;
			UINT32 ui32CurMatchingCount = 0;
			UINT32 ui32StartIdx;
			UINT32 ui32Version;
			CShop* pShop = g_pContextMain->GetShop();
			SHOP_MATCHING_DATA* pMatching = pShop->GetMatchingAllData();

			pPacket->ReadData(&ui32MatchingCount,		sizeof(UINT32));
			pPacket->ReadData(&ui32CurMatchingCount,	sizeof(UINT32));
			pPacket->ReadData(&ui32StartIdx,			sizeof(UINT32));
			pPacket->ReadData(&pMatching[ui32StartIdx],	sizeof(SHOP_MATCHING_DATA)*ui32CurMatchingCount);
			pShop->SetMatchingCount( ui32MatchingCount );

			pPacket->ReadData(&ui32Version,			sizeof(UINT32));
			pShop->SetRepairList();
			pShop->SetVersion( ui32Version );

			if( ui32MatchingCount == (ui32StartIdx + ui32CurMatchingCount) )
			{
				char strLog[256];
				i3String::Format(strLog, 256, "Shop Update - Ver:%d, ItemCount:%d, Goods:%d, Matching:%d", pShop->GetVersion(), pShop->GetItemCount(), pShop->GetGoodsCount(), pShop->GetMatchingCount() );
				g_pModuleLogFile->Write_M_Log(MODULE_TRANS_THREAD_IDX,NULL, strLog);
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_SALECOUPONLIST_ACK:
		{
			// 할인쿠폰 목록을 받습니다.
			UINT32 ui32CouponCount;
			UINT32 ui32CurCouponCount;
			UINT32 ui32StartIdx;

			CShop* pShop = g_pContextMain->GetShop();
			SALE_COUPON*		pCoupon= pShop->GetCouponAllData();

			pPacket->ReadData(&ui32CouponCount,		sizeof(UINT32));
			pPacket->ReadData(&ui32CurCouponCount,	sizeof(UINT32));
			pPacket->ReadData(&ui32StartIdx,		sizeof(UINT32));
			UINT32 ui32EndIdx = ui32StartIdx + ui32CurCouponCount;
			for( UINT32 i = ui32StartIdx ; i < ui32EndIdx ; i++ )
			{
				pPacket->ReadData( &pCoupon[i],		sizeof(SALE_COUPON));
			}
			pShop->SetCouponCount( ui32CouponCount );
		}
		break;
	case PROTOCOL_FIELDSHOP_SEND_NEWVERSION_ACK:
		{
			pPacket->ReadData( g_pContextMain->GetFieldShop(),	sizeof(FieldShopInfo) );
		}
		break;
	case PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK:			//구입 
		{
			UINT32			ui32GoodsID;
			UINT8			ui8BuyType;
			UINT16			ui16ItemCount	= 0;
			INVEN_BUFFER	aInvenItem[ MAX_SHOP_GOODS_ITEM_COUNT ];
			UINT32			ui32Point		= 0;
			UINT32			ui32Cash		= 0;
			char			strRedirectUrl[MAX_REDIRECTURL] = {0,};

			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32)); 
			pPacket->ReadData( &i64UID,				sizeof(TUID));
			pPacket->ReadData( &i32Temp,			sizeof(INT32));
			pPacket->ReadData( &ui32GoodsID,		sizeof(UINT32));
			pPacket->ReadData( &ui8BuyType,			sizeof(UINT8));
			if( EV_SUCCESSED( i32Temp ))
			{
				pPacket->ReadData( &ui16ItemCount,	sizeof(UINT16));
				if( MAX_SHOP_GOODS_ITEM_COUNT < ui16ItemCount ) ui16ItemCount = MAX_SHOP_GOODS_ITEM_COUNT;
				pPacket->ReadData( aInvenItem,		sizeof(INVEN_BUFFER) * ui16ItemCount );
				pPacket->ReadData( &ui32Point,		sizeof(UINT32) );
				pPacket->ReadData( &ui32Cash,		sizeof(UINT32) );
			}

			g_pUserManager->OnGoodsBuy_A(i32SessionIdx, i64UID, i32Temp, ui32GoodsID, ui8BuyType, ui16ItemCount, aInvenItem, ui32Point, ui32Cash, strRedirectUrl );
		}
		break;
	case PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK:			// 선물하기
		{
			UINT32	ui32GoodsID;
			UINT32	ui32Point	= 0;
			UINT32	ui32Cash	= 0;

			pPacket->ReadData(&i32SessionIdx,			sizeof(INT32)); 
			pPacket->ReadData(&i64UID,					sizeof(TUID));
			pPacket->ReadData(&i32Temp,					sizeof(INT32));
			pPacket->ReadData(&ui32GoodsID,				sizeof(UINT32));
			if( EV_SUCCESSED( i32Temp ))
			{
				pPacket->ReadData(&ui32Point,			sizeof(UINT32));
				pPacket->ReadData(&ui32Cash,			sizeof(UINT32));
			}

			g_pUserManager->OnGoodsGift_A( i32SessionIdx, i64UID, i32Temp, ui32GoodsID, ui32Point, ui32Cash );
		}
		break;	
	case PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK:			// 선물 받기
		{	
			UINT16			ui16ItemCount	= 0;
			INVEN_BUFFER	aInvenItem[ MAX_SHOP_GOODS_ITEM_COUNT ];

			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(TUID));
			pPacket->ReadData(&i32Temp,			sizeof(INT32));
			if( EV_SUCCESSED( i32Temp ))
			{
				pPacket->ReadData( &ui16ItemCount,	sizeof(UINT16));
				if( MAX_SHOP_GOODS_ITEM_COUNT < ui16ItemCount ) ui16ItemCount = MAX_SHOP_GOODS_ITEM_COUNT;
				pPacket->ReadData( aInvenItem,		sizeof(INVEN_BUFFER) * ui16ItemCount );
			}

			g_pUserManager->OnGiftAuth_A(i32SessionIdx, i64UID, i32Temp, ui16ItemCount, aInvenItem );
		}
		break;
	case PROTOCOL_AUTH_SHOP_NOTIFY_GIFT_ACK:			// 선물 받음 알림
		{
			pPacket->ReadData(&i32SessionIdx,			sizeof(INT32));
			pPacket->ReadData(&i64UID,					sizeof(TUID));

			UINT8 ui8SenderNickSize = 0;
			UINT8 ui8MessageSize = 0;

			GIFT_BUFFER GiftBuffer;

			pPacket->ReadData( &GiftBuffer._i32GiftWareDBIdx ,	sizeof(TGiftDxIdx));
			pPacket->ReadData( &GiftBuffer._ui32GoodsID,		sizeof(UINT32));
			pPacket->ReadData( &GiftBuffer._eDeliver,			sizeof(ENUM_TYPE_GIFT_DELIVER));
			pPacket->ReadData( &GiftBuffer._ui32Expire,			sizeof(UINT32));

			pPacket->ReadData( &ui8SenderNickSize,				sizeof(UINT8));			MIN( ui8SenderNickSize,	NET_NICK_NAME_SIZE );
			pPacket->ReadData( GiftBuffer._strSenderNick,		ui8SenderNickSize);	

			pPacket->ReadData( &ui8MessageSize,					sizeof(UINT8));			MIN( ui8MessageSize,	MAX_GIFT_MESSAGE_SIZE );
			pPacket->ReadData( GiftBuffer._strMessage,			ui8MessageSize);	

			g_pUserManager->OnGiftNotify_A( i32SessionIdx, i64UID, GiftBuffer );
		}
		break;

	case PROTOCOL_AUTH_SHOP_INSERT_ITEM_ACK				:	//인벤토리에 밀어 넣기 
		{
			INVEN_BUFFER	InvenItem;
			INT8	i8RequestType;
			UINT32	ui32InsertArg;

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32));
			pPacket->ReadData( &i64UID,					sizeof(TUID));
			pPacket->ReadData( &i32Temp,				sizeof(INT32));
			if( EV_SUCCESSED( i32Temp ))
			{
				pPacket->ReadData( &i8RequestType,		sizeof(INT8));
				pPacket->ReadData( &ui32InsertArg,		sizeof(INT32));
				pPacket->ReadData( &InvenItem,			sizeof(INVEN_BUFFER));
			}

			g_pUserManager->OnInsertItem_A( i32SessionIdx, i64UID, i32Temp, i8RequestType, ui32InsertArg, &InvenItem );
		}
		break; 
	case PROTOCOL_AUTH_SHOP_DELETE_ITEM_ACK:
		{
			TWareDxIdx i64ItemWareIdx;

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32) );
			pPacket->ReadData( &i64UID,					sizeof(TUID) );
			pPacket->ReadData( &i32Temp,				sizeof(INT32) );
			if( EV_SUCCESSED( i32Temp ))
			{
				pPacket->ReadData( &i64ItemWareIdx,	sizeof(TWareDxIdx));
			}
			g_pUserManager->OnDeleteItem_A(i32SessionIdx, i64UID, i32Temp, i64ItemWareIdx );
		}
		break;
	case PROTOCOL_AUTH_SHOP_ITEM_REPAIR_ACK:
		{
			INVEN_BUFFER	InvenItem;
			UINT32			ui32RepairPoint	= 0;
			UINT32			ui32RepairCash	= 0;
			UINT32			ui32Cash		= 0;
			UINT32			ui32RepairCount = 0;

			pPacket->ReadData( &i32SessionIdx,				sizeof(INT32)); 
			pPacket->ReadData( &i64UID,						sizeof(TUID));
			pPacket->ReadData( &i32Temp,					sizeof(INT32));
			if( EV_SUCCESSED( i32Temp ))
			{
				pPacket->ReadData( &InvenItem,					sizeof(INVEN_BUFFER) );
				pPacket->ReadData( &ui32RepairPoint,			sizeof(UINT32) );
				pPacket->ReadData( &ui32RepairCash,				sizeof(UINT32) );
				pPacket->ReadData( &ui32Cash,					sizeof(UINT32) );
				pPacket->ReadData( &ui32RepairCount,			sizeof(UINT32) );

				CUserSession * pSession =g_pUserManager-> GetSession( i32SessionIdx, i64UID ); // 유저 정보를 받아옴
				if( pSession )
				{
					// 로그 보내기 (수리) // ZLog 서동권
					REPAIR_INFO stRepair; // 수리 내용이 들어갈 구조체

					// 아이템의 정보를 얻어옵니다.
					CShop* pShop	= g_pContextMain->GetShop();
					UINT32 ui32Idx	= pShop->GetFindItemIdx( InvenItem._ui32ItemID );
					SHOP_ITEM_DATA*	pItemEx		= pShop->GetItemEx( ui32Idx );

					// 얻어온 정보로 수리 내용을 기록.
					stRepair.m_ui32Point	= ui32RepairPoint; // 수리에 소모된 포인트
					stRepair.m_ui32Cash		= ui32RepairCash;
					stRepair.m_ui32ItemID	= InvenItem._ui32ItemID;
					stRepair.m_ui32Arg		= ui32RepairCount;

					// 수리 - 포인트 or 캐쉬 소진으로 빼야함
					g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_REPAIR, pSession, &stRepair );
				}
			}
			else
			{
				pPacket->ReadData( &InvenItem._i64ItemWareDBIdx,	sizeof(TUID) );
			}

			g_pUserManager->OnItemRepair_A(i32SessionIdx, i64UID, i32Temp, &InvenItem, ui32RepairPoint, ui32Cash );
		}
		break;
	case PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK	: 
		{
			INVEN_BUFFER InvenItem;

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32) );
			pPacket->ReadData( &i64UID,					sizeof(TUID) );
			pPacket->ReadData( &i32Temp,				sizeof(INT32) );
			if( EV_SUCCESSED( i32Temp ))
			{
				pPacket->ReadData( &InvenItem,			sizeof(INVEN_BUFFER));
			}
			g_pUserManager->OnItemAuth_A(i32SessionIdx, i64UID, i32Temp, &InvenItem );
		}
		break; 
	case PROTOCOL_AUTH_USED_WEAPON_ACK:
		{
			UINT16 ui16InvenCount;
			INVEN_BUFFER aInvenItem[ MAX_INVEN_COUNT ] = { 0, };

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32) );
			pPacket->ReadData( &i64UID,					sizeof(TUID) );
			pPacket->ReadData( &ui16InvenCount,			sizeof(UINT16) );
			if( MAX_INVEN_COUNT < ui16InvenCount ) ui16InvenCount = MAX_INVEN_COUNT;
			pPacket->ReadData( aInvenItem,				sizeof(INVEN_BUFFER)*ui16InvenCount);

			g_pUserManager->OnAuthUsedWeapon_A(i32SessionIdx, i64UID, ui16InvenCount, aInvenItem );
		}
		break;	
	case PROTOCOL_AUTH_SHOP_GET_GIFTLIST_ACK:
		{
			USER_GIFT_LIST UserGiftList;
			i3mem::FillZero( &UserGiftList,	sizeof( USER_GIFT_LIST ) );

			pPacket->ReadData(&i32SessionIdx,			sizeof(INT32));
			pPacket->ReadData(&i64UID,					sizeof(TUID));
			pPacket->ReadData(&i32Temp,					sizeof(INT32));		

			if (EV_SUCCESSED(i32Temp))
			{
				UINT8 ui8SenderNickSize = 0;
				UINT8 ui8MessageSize = 0;
				UINT8 ui8TotalGiftCount = 0;

				pPacket->ReadData( &UserGiftList.m_ui8TotalGiftCount,		sizeof(UINT8));		// 총 선물 갯수
				pPacket->ReadData( &UserGiftList.m_ui8SendGiftCount,		sizeof(UINT8));		// 현재 선물 갯수( 이번 패킷 )
				pPacket->ReadData( &UserGiftList.m_ui8StartIdx,				sizeof(UINT8));		// 배열 인덱스.

				UINT8 ui8SendGiftCount = UserGiftList.m_ui8SendGiftCount;
				UINT8 ui8StartIdx = UserGiftList.m_ui8StartIdx;

				for( INT32 i = 0 ; i < ui8SendGiftCount ; i++ )
				{
					// 혹시 모르니
					if( ui8StartIdx >= MAX_SHOP_USER_GIFT_COUNT ) break;

					pPacket->ReadData( &UserGiftList.m_Gift[ui8StartIdx]._i32GiftWareDBIdx,	sizeof(TGiftDxIdx));		// 선물 DB Idx.
					pPacket->ReadData( &UserGiftList.m_Gift[ui8StartIdx]._ui32GoodsID,		sizeof(UINT32));			// 선물 GoodsID.
					pPacket->ReadData( &UserGiftList.m_Gift[ui8StartIdx]._eDeliver,			sizeof(ENUM_TYPE_GIFT_DELIVER));	// 선물 지급 여부
					pPacket->ReadData( &UserGiftList.m_Gift[ui8StartIdx]._ui32Expire,		sizeof(UINT32));			// 선물 만료 날짜.
					
					pPacket->ReadData( &ui8SenderNickSize,				sizeof(UINT8));		// 선물 보낸 사람 닉네임 사이즈.
					pPacket->ReadData( UserGiftList.m_Gift[ui8StartIdx]._strSenderNick,		ui8SenderNickSize );		// 선물 보낸 사람 닉네임.
					pPacket->ReadData( &ui8MessageSize,					sizeof(UINT8));		// 선물 보낸 사람 닉네임 사이즈.
					pPacket->ReadData( UserGiftList.m_Gift[ui8StartIdx]._strMessage,			ui8MessageSize );		// 선물 메시지.

					ui8StartIdx++;
				}

			}
			g_pUserManager->OnGiftList_A( i32SessionIdx, i64UID, i32Temp, UserGiftList );
		}
		break;	
	case PROTOCOL_AUTH_SHOP_CAPSULE_ACK			:
		{
			UINT32	ui32ItemID;
			UINT8	ui8Capsule;
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID));
			pPacket->ReadData(&ui32ItemID,			sizeof(UINT32));
			pPacket->ReadData(&ui8Capsule,			sizeof(UINT8));

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);

			if (pSession != NULL)
			{
				i3NetworkPacket sendPacket(PROTOCOL_AUTH_SHOP_CAPSULE_ACK);
				sendPacket.WriteData(&ui32ItemID,	sizeof(UINT32));
				sendPacket.WriteData(&ui8Capsule,	sizeof(UINT8));
				pSession->SendPacketMessage(&sendPacket);

				// 캡슐 아이템 인증에 대한 ZLog 기록 남기기
				g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_INVEN_USE, pSession, &ui32ItemID, NULL, &ui8Capsule ); 
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_JACKPOT_ACK:
		{
			UINT8	ui8NickSize;
			char	strNick[ NET_NICK_NAME_SIZE ];
			UINT32	ui32ItemID;
			UINT8	ui8Capsule;
			pPacket->ReadData(&ui8NickSize,			sizeof(UINT8));
			pPacket->ReadData(strNick,				ui8NickSize);
			pPacket->ReadData(&ui32ItemID,			sizeof(UINT32));
			pPacket->ReadData(&ui8Capsule,			sizeof(UINT8));

			UINT8 ui8Lenght = (UINT8)i3String::Length( strNick )+1;

			i3NetworkPacket SendPacket(PROTOCOL_AUTH_SHOP_JACKPOT_ACK);
			SendPacket.WriteData(&ui8NickSize,	sizeof(UINT8)	);
			SendPacket.WriteData(strNick,		ui8NickSize		);
			SendPacket.WriteData(&ui32ItemID,	sizeof(UINT32)	);
			SendPacket.WriteData(&ui8Capsule,	sizeof(UINT8)	);

			//g_pUserManager->OnSendAllUser( &SendPacket );
			g_pUserManager->AnnounceMessagePush( MODULE_TRANS_THREAD_IDX, SendPacket.GetProtocolID(), SendPacket.GetDataFieldSize(), SendPacket.GetFieldBuffer() );
		}
		break;

	case PROTOCOL_AUTH_LOGOUT_ACK				:	break;	//로그아웃
	case PROTOCOL_BASE_HEART_BIT_ACK			:	break;	//심장 소리 
	case PROTOCOL_AUTH_GET_POINT_CASH_ACK		:
		{
			UINT32 ui32Point	= 0;
			UINT32 ui32Cash		= 0;
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(TUID));
			pPacket->ReadData(&i32Temp,			sizeof(INT32));
			if( EV_SUCCESSED( i32Temp ) )
			{
				pPacket->ReadData(&ui32Point,		sizeof(UINT32));
				pPacket->ReadData(&ui32Cash,		sizeof(UINT32));
			}

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if (pSession != NULL)
			{
				pSession->SendGetRemainMoney( i32Temp, ui32Point, ui32Cash );
			}
		}
		break;
	case PROTOCOL_AUTH_GET_POINT_CASH_NCSA_ACK		:
		{
			UINT32 ui32Point		= 0;
			UINT32 ui32NCCoin		= 0;
			UINT32 ui32HappyCoin	= 0;
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(TUID));
			pPacket->ReadData(&i32Temp,			sizeof(INT32));
			if( EV_SUCCESSED( i32Temp ) )
			{
				pPacket->ReadData(&ui32Point,		sizeof(UINT32));
				pPacket->ReadData(&ui32NCCoin,		sizeof(UINT32));
				pPacket->ReadData(&ui32HappyCoin,	sizeof(UINT32));
			}

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if (pSession != NULL)
			{
				pSession->SendGetRemainMoneyNCSA( i32Temp, ui32Point, ui32NCCoin, ui32HappyCoin );
			}
		}
		break;
	case PROTOCOL_AUTH_PLUS_POINT_ACK:
		{
			UINT32	ui32Point		= 0;
			BOOL	bIsRSJackPot	= FALSE;	// 뽑기 상점 잭팟 당첨 여부.
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(TUID));
			pPacket->ReadData(&ui32Point,		sizeof(UINT32));
			pPacket->ReadData(&bIsRSJackPot,	sizeof(BOOL));

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if (pSession != NULL)
			{
				pSession->SendGetPoint( ui32Point, bIsRSJackPot );
			}
		}
		break;
	case PROTOCOL_AUTH_COUPON_AWARD_ITEM_ACK:
		{
			UINT32			ui32ItemID = 0;
			UINT8			ui8Size;
			UINT16			ui16ItemCount	= 0;
			INVEN_BUFFER	aInvenItem[ MAX_SHOP_GOODS_ITEM_COUNT ];
			char			strCouponNum[MAX_ORDERID] = {0,};

			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32)); 
			pPacket->ReadData( &i64UID,				sizeof(TUID));
			pPacket->ReadData( &i32Temp,			sizeof(INT32));						
			pPacket->ReadData( &ui8Size,			sizeof(UINT8));
			pPacket->ReadData( strCouponNum,		ui8Size);
			strCouponNum[MAX_ORDERID-1] = '\0';

			if( EV_SUCCESSED( i32Temp ))
			{
				pPacket->ReadData( &ui16ItemCount,	sizeof(UINT16));
				if( MAX_SHOP_GOODS_ITEM_COUNT < ui16ItemCount ) ui16ItemCount = MAX_SHOP_GOODS_ITEM_COUNT;
				pPacket->ReadData( aInvenItem,		sizeof(INVEN_BUFFER) * ui16ItemCount );				
			}

			g_pUserManager->OnCouponAwardItem_A(i32SessionIdx, i64UID, i32Temp, ui32ItemID, strCouponNum, ui16ItemCount, aInvenItem );

		}
		break;
	case PROTOCOL_LOBBY_CHECK_NICK_NAME_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(TUID));
			pPacket->ReadData(&i32Rv,			sizeof(INT32));

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if (pSession != NULL)
			{
				pSession->SendCheckNickName( i32Rv );
			}
		}
		break;
	case PROTOCOL_AUTH_CHANGE_NICKNAME_ACK:
		{
			UINT8	ui8Size;
			char	strNickName[ NET_NICK_NAME_SIZE ];
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(TUID));
			pPacket->ReadData(&ui8Size,			sizeof(UINT8));	LENGTH_CHECK( ui8Size, NET_NICK_NAME_SIZE );
			pPacket->ReadData(strNickName,		ui8Size);
			strNickName[ NET_NICK_NAME_SIZE-1 ] = '\0';

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
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
			pPacket->ReadData(&i64UID,			sizeof(TUID));
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
			}
			else
			{
				i3mem::FillZero( &UserRecord, sizeof( USER_INFO_RECORD ) );
			}

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
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
			INT64	i64UID = 0;
			INT32	i32SessionIdx = 0;
			INT32	i32Rv = EVENT_ERROR_FAIL;

			pPacket->ReadData( &i32WebSessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,				sizeof(INT64) );
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32) );

			i3NetworkPacket SendPacket( PROTOCOL_ASC_WEB_USER_INFO_LOCATION_ACK );
			SendPacket.WriteData( &i32WebSessionIdx,	sizeof(INT32) );

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if( pSession != NULL )
			{
				INT8	i8ServerIdx		= g_pContextMain->m_i32ServerIdx;
				INT8	i8ChannelNum	= pSession->m_i32ChannelNum + 1;
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
	case PROTOCOL_BATTLE_SUPPLY_BOX_RESULT_ACK:
		{
			char strLog[256];

			INT32	i32ChannelNum;
			INT32	i32RoomIdx;
			UINT32	ui32RoomStateCheckTime;
			INT8	i8BattleMemberCount;
			USER_PRESENTED_INFO	stUserPresentedInfo[SLOT_MAX_COUNT];
			pPacket->ReadData( &i32ChannelNum,			sizeof(INT32) );
			pPacket->ReadData( &i32RoomIdx,				sizeof(INT32) );
			pPacket->ReadData( &ui32RoomStateCheckTime,	sizeof(UINT32) );
			pPacket->ReadData( &i8BattleMemberCount,	sizeof(INT8) );
			if( i8BattleMemberCount <= SLOT_MAX_COUNT )
				pPacket->ReadData( stUserPresentedInfo,		sizeof(USER_PRESENTED_INFO)*i8BattleMemberCount );

			if( GAME_CHANNEL_COUNT <= i32ChannelNum || SERVER_VALUE_UNKNOWN >= i32ChannelNum || 0 > i32RoomIdx || g_pContextMain->m_ui32ChannelSessionCount <= i32RoomIdx )
			{
				i3String::Format( strLog, 256, "SUPPLY_BOX_RESULT ACK - Unknown ChannelNum(%d) or RoomIdx(%d)", i32ChannelNum, i32RoomIdx);
				g_pModuleLogFile->Write_M_Log(MODULE_TRANS_THREAD_IDX,NULL, strLog);
				break;
			}
			else
			{
				CRoom * pRoom = g_pRoomManager->GetRoom( i32ChannelNum, i32RoomIdx );
				if( pRoom != NULL )
					pRoom->SetPresentSupplyBox( ui32RoomStateCheckTime, i8BattleMemberCount, stUserPresentedInfo );
			}
		}
		break;
	case PROTOCOL_BASE_SUPPLY_BOX_ANNOUNCE_ACK:
		{
			SUPPLY_BOX_ANNOUNCE_SETTING stSupplyAnnounce;

			pPacket->ReadData( &stSupplyAnnounce,		sizeof(SUPPLY_BOX_ANNOUNCE_SETTING) );

			if( g_eServerState == SERVER_STATE_START )
			{
				g_pUserManager->OnSendSupplyBoxAnnounceMessage( MODULE_TRANS_THREAD_IDX, &stSupplyAnnounce );
			}
		}
		break;
	case PROTOCOL_BASE_SUPPLY_BOX_PRESENT_MESSAGE_ACK:
		{
			UINT8	ui8PresentMessage;
			UINT32	ui32ServerIdx;
			UINT32	ui32ChannelNum;
			UINT32	ui32RoomIdx;
			UINT32	ui32ItemID;
			UINT8	ui8Size;
			char	strNick[NET_NICK_NAME_SIZE+1];

			pPacket->ReadData( &ui8PresentMessage,	sizeof(UINT8) );
			pPacket->ReadData( &ui32ServerIdx,		sizeof(UINT32) );
			pPacket->ReadData( &ui32ChannelNum,		sizeof(UINT32) );
			pPacket->ReadData( &ui32RoomIdx,		sizeof(UINT32) );
			pPacket->ReadData( &ui32ItemID,			sizeof(UINT32) );
			pPacket->ReadData( &ui8Size,			sizeof(UINT8) );
			if( ui8Size > NET_NICK_NAME_SIZE ) ui8Size = NET_NICK_NAME_SIZE;
			pPacket->ReadData( strNick,				ui8Size	);

			g_pUserManager->OnSendSupplyBoxPresentMessage( MODULE_TRANS_THREAD_IDX, ui8PresentMessage, ui32ServerIdx, ui32ChannelNum, ui32RoomIdx, ui32ItemID, ui8Size, strNick );
		}
		break;	
	case PROTOCOL_BASE_ATTENDANCE_INFO_ACK	: // 이벤트 정보 요청
		{
			INT64	i64UID;
			ATTENDANCE_USER	stAttendance;
			ATTENDANCE_INFO	aInfo[2];

			pPacket->ReadData( &i32SessionIdx,		sizeof( INT32 ) );
			pPacket->ReadData( &i64UID,				sizeof( INT64 ) );
			pPacket->ReadData( &stAttendance,		sizeof( ATTENDANCE_USER ) );
			pPacket->ReadData( aInfo,				sizeof( aInfo ) );

			g_pUserManager->GetAttendanceInfo( i32SessionIdx, i64UID, &stAttendance, aInfo ); 
		}
		break;
	case PROTOCOL_BASE_ATTENDANCE_ACK	:	// 출석 요청
		{
			INT64	i64UID;
			INT32	i32Rv;
			ATTENDANCE_USER	stAttendance;

			pPacket->ReadData( &i32SessionIdx,		sizeof( INT32 ) );
			pPacket->ReadData( &i64UID,				sizeof( INT64 ) );
			pPacket->ReadData( &i32Rv,				sizeof( INT32 ) );
			pPacket->ReadData( &stAttendance,		sizeof( ATTENDANCE_USER ) );

			g_pUserManager->GetAttendanceCheck( i32SessionIdx, i64UID, i32Rv, &stAttendance ); 
		}
		break;
	case PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_ACK	: // 아이템 보상 지급 요청
		{
			INT64	i64UID;
			INT32	i32Rv;

			// 출석체크 이벤트 완료 보상 요청결과
			pPacket->ReadData( &i32SessionIdx,	sizeof(INT32) );				//SessionIDX
			pPacket->ReadData( &i64UID,			sizeof(TUID) );					//DBIDX
			pPacket->ReadData( &i32Rv,			sizeof(INT32) );				//Result 

			g_pUserManager->GetAttendanceResult( i32SessionIdx, i64UID, i32Rv ); 
		}
		break;


	case PROTOCOL_AUTH_RS_RESULT_ACK:			//구입 
		{			
			UINT32			ui32TabGoodsID;
			UINT32			ui32PrizeGoodsID;
			UINT16			ui16ItemCount	= 0;
			INVEN_BUFFER	aInvenItem[ MAX_SHOP_GOODS_ITEM_COUNT ];
			UINT32			ui32Cash = 0;
			char			strRedirectUrl[MAX_REDIRECTURL] = {0,};
			RS_ITEM_GRADE	eItemGrade = RS_ITEM_GRADE_LOSE;
			UINT8			ui8RsIdx = 0; 
			UINT32			ui32RSLastBaseInfoTime = 0;
			RS_BASE_INFO	RsBaseInfo;
			RS_TAB			eTabIdx;
			RS_RESULT_TYPE	eRSResult;
			UINT8			ui8TabUseCount;

			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32)); 
			pPacket->ReadData( &i64UID,				sizeof(TUID));
			pPacket->ReadData( &i32Temp,			sizeof(INT32));
			if( EV_SUCCESSED( i32Temp ))
			{
				pPacket->ReadData( &eRSResult,		sizeof(RS_RESULT_TYPE));

				pPacket->ReadData( &eTabIdx,		sizeof(RS_TAB));
				pPacket->ReadData( &eItemGrade,		sizeof(RS_ITEM_GRADE));

				pPacket->ReadData( &ui32TabGoodsID,		sizeof(UINT32));
				pPacket->ReadData( &ui32PrizeGoodsID,	sizeof(UINT32));

				pPacket->ReadData( &ui8RsIdx,		sizeof(UINT8));
				pPacket->ReadData( &ui8TabUseCount,	sizeof(UINT8));
			
				pPacket->ReadData( &ui16ItemCount,	sizeof(UINT16));
				if( MAX_SHOP_GOODS_ITEM_COUNT < ui16ItemCount ) ui16ItemCount = MAX_SHOP_GOODS_ITEM_COUNT;
				pPacket->ReadData( aInvenItem,		sizeof(INVEN_BUFFER) * ui16ItemCount );
				pPacket->ReadData( &ui32Cash,		sizeof(UINT32) );
				pPacket->ReadData( &ui32RSLastBaseInfoTime,		sizeof(UINT32));
				pPacket->ReadData( &RsBaseInfo,		sizeof(RS_BASE_INFO));
			}

			g_pUserManager->OnRSGoodsBuy_A(i32SessionIdx, i64UID, i32Temp, eTabIdx, ui32TabGoodsID,  ui32PrizeGoodsID, ui16ItemCount, aInvenItem, ui32Cash, strRedirectUrl,
			eItemGrade, ui8RsIdx, ui8TabUseCount, ui32RSLastBaseInfoTime, &RsBaseInfo, eRSResult  );
		}
		break;

	case PROTOCOL_AUTH_RS_ENTER_ACK:			// 뽑기 상점 입장 응답. 
		{			
			INT32			i32SessionIdx;
			TUID			i64UID;
			UINT8			ui8TabUseCount1 = 0;
			UINT8			ui8TabUseCount2 = 0;
			UINT8			ui8TabUseCount3 = 0;
			RS_BASE_INFO	RsBaseInfo;
			
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32)); 
			pPacket->ReadData( &i64UID,				sizeof(TUID)); 
			pPacket->ReadData( &i32Temp,			sizeof(INT32));
			if( EV_SUCCESSED( i32Temp ))
			{
				pPacket->ReadData( &ui8TabUseCount1,	sizeof(UINT8));
				pPacket->ReadData( &ui8TabUseCount2,	sizeof(UINT8));
				pPacket->ReadData( &ui8TabUseCount3,	sizeof(UINT8));
				pPacket->ReadData( &RsBaseInfo,			sizeof(RS_BASE_INFO));
			} 

			g_pUserManager->OnRSEnter_A(i32SessionIdx, i64UID, i32Temp, ui8TabUseCount1, ui8TabUseCount2, ui8TabUseCount3, &RsBaseInfo );
		}
		break;
	case PROTOCOL_AUTH_RS_ITEM_INFO_ACK:
		{
			pPacket->ReadData( &g_pModuleTrans->m_stRSItemInfo,		sizeof(RS_ALL_ITEM_INFO));
		}
		break;

	case PROTOCOL_AUTH_RS_JACKPOT_ACK:
		{
			RS_JACKPOT_INFO stRSJackpotInfo;
			pPacket->ReadData(&stRSJackpotInfo,		sizeof(RS_JACKPOT_INFO));
	 
			i3NetworkPacket SendPacket(PROTOCOL_RS_JACKPOT_NOTIFY_ACK);
			SendPacket.WriteData(&stRSJackpotInfo,	sizeof(RS_JACKPOT_INFO)	);
		 
			g_pUserManager->AnnounceMessagePush( MODULE_TRANS_THREAD_IDX, SendPacket.GetProtocolID(), SendPacket.GetDataFieldSize(), SendPacket.GetFieldBuffer() );
		}
		break;

	case PROTOCOL_LOBBY_SET_BIRTHDAY_ACK:
		{
			UINT32 ui32BirthDay = 0;

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32)); 
			pPacket->ReadData( &i64UID,					sizeof(TUID));
			pPacket->ReadData( &i32Rv,					sizeof(INT32));
			if( EV_SUCCESSED( i32Rv ) )
			{
				pPacket->ReadData( &ui32BirthDay,		sizeof(UINT32));
			}

			g_pUserManager->UserSetBirthDay_A( i32SessionIdx, i64UID, i32Rv, ui32BirthDay );
		}
		break;

	case PROTOCOL_AUTH_SECURITYID_DUPLICATE_ACK:
		{
			pPacket->ReadData( &i32SessionIdx,	sizeof(INT32)); 
			pPacket->ReadData( &i64UID,			sizeof( TUID ) );
			pPacket->ReadData( &i32Rv,			sizeof( INT32 ) );

			g_pUserManager->SendRetCheckSID( i32SessionIdx, i64UID, i32Rv );
		}
		break;

	case PROTOCOL_AUTH_SECURITYID_CREATE_ACK:
		{
			pPacket->ReadData( &i32SessionIdx,	sizeof(INT32)); 
			pPacket->ReadData( &i64UID,			sizeof( TUID ) );
			pPacket->ReadData( &i32Rv,			sizeof( INT32 ) );

			g_pUserManager->SendRetCreateSID( i32SessionIdx, i64UID, i32Rv );
		}
		break;

	default: 
		g_pModuleLogFile->Write_M_Log(MODULE_TRANS_THREAD_IDX, NULL, "[CModuleTransSocket::PacketParsing]Unknown Message"); 
		break; 
	}

	m_ui32LastRecvTime	= g_pContextMain->GetServerTime();
	m_ui32LastSendTime	= g_pContextMain->GetServerTime();
	return; 
}

INT32	CModuleTransSocket::SendPacketMessage( i3NetworkPacket* packet )
{
#if defined( USE_FINISH_PACKET )
	UINT32 ui32FinishPacket = FINISH_PACKET;
	if( FALSE == packet->WriteData( &ui32FinishPacket, sizeof(UINT32) ) )
	{
		char strTemp[ MAX_PATH ];
		i3String::Format( strTemp, MAX_PATH, "[ERROR] Finish Packet Error, Protocol : %d, Size : %d", packet->GetProtocolID(), packet->GetPacketSize() );
		g_pModuleLogFile->Write_M_Log( MODULE_TRANS_THREAD_IDX, NULL, strTemp );
		return -1;
	}
#endif
	return i3NetworkClientSocket::SendPacketMessage( packet );
}
