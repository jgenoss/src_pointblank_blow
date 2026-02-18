#include "pch.h"
#include "UserSession.h"

#include "ModuleTrans.h"
#include "ModuleMessenger.h"
#include "ModuleClan.h"
 

BOOL CUserSession::_PacketParsingGacha(i3NetworkPacket* pRecvPacket, i3NetworkPacket* pSendPacket, BOOL& bIsKilled)
{
	if( !g_pContextMain->IsContentsEnabled(CONTENTS_GACHA) )	return FALSE;

	CGachaShop* pGachaShop = g_pContextMain->GetGachaShop();

	PROTOCOL ProtocolID = pRecvPacket->GetProtocolID();
	switch( ProtocolID )
	{
	case PROTOCOL_GACHA_ENTER_REQ:
		{
			INT32 i32ErrorCode = ERROR_SUCCESS;
			if( m_i32MainTask != GAME_TASK_LOBBY || m_i32SubTask != GAME_TASK_SUB_NONE )
				i32ErrorCode = ERROR_GACHA_INVALID_ENTER_POSION;
			
			if( i32ErrorCode == ERROR_SUCCESS ) 
			{
				m_i32SubTask = GAME_TASK_SUB_GACHA;
				g_pModuleMessenger->MessengerSendChangeState( m_WorkThreadIdx, this, FRIEND_STATE_CANT_INVITE); 
				g_pModuleClan->ClanSendChangeState(  m_WorkThreadIdx, this, FRIEND_STATE_CANT_INVITE );
			}
 

			PACKET_GACHA_ENTER_ACK stGachaEnterAck;
			stGachaEnterAck.m_i32ErrorCode = i32ErrorCode; 

			stGachaEnterAck.m_ui32LuckyGoodsID	= pGachaShop->GetLuckyGoodsID();
			stGachaEnterAck.m_strLuckyUserNick	= pGachaShop->GetLuckyUser();
			stGachaEnterAck.m_ui32LuckyTime		= pGachaShop->GetLuckyTime();

			S2MOSendEvent(&stGachaEnterAck);
			 
		}

		break;

	case PROTOCOL_GACHA_LEAVE_REQ:
		{
			CHECKVALID_LOBBY;

			if( m_i32SubTask != GAME_TASK_SUB_GACHA ) break;

			m_i32SubTask = GAME_TASK_SUB_NONE;
			g_pModuleMessenger->MessengerSendChangeState( m_WorkThreadIdx, this, FRIEND_STATE_ONLINE); 
			g_pModuleClan->ClanSendChangeState(  m_WorkThreadIdx, this, FRIEND_STATE_ONLINE );
 
			i3NetworkPacket stPacket(PROTOCOL_GACHA_LEAVE_ACK);
			SendPacketMessage(&stPacket);
		}
		break;

	case PROTOCOL_GACHA_PURCHASE_REQ:
		{
			CHECKVALID_LOBBY;

			if( m_i32SubTask != GAME_TASK_SUB_GACHA ) break;

			PACKET_GACHA_SS_RESULT_HEADER Temp;
			Temp.m_ui32Cash = m_UserInfoBasic.m_ui32Cash;

			PACKET_GACHA_PURCHASE_REQ Recv;
			S2MORecvPacket( &Recv, pRecvPacket->GetPacketBuffer() );

			// 요청 데이터 유효성 검사
			if( !pGachaShop->IsValidRequest( Recv.m_TGoodsID, Recv.m_i32ItemIdx ) )
			{
				Temp.m_eErrorCode = GACHA_ERROR_INVALID_REQUEST;
				SendGachaPurchaseAck( &Temp, NULL );
				break;
			}
			 
			// 가격체크
			SHOP_GOODS_EXPANSION* pGoods	= g_pContextMain->GetShop()->GetGoodsData( Recv.m_TGoodsID );
			if( !pGoods )
			{
				Temp.m_eErrorCode = GACHA_ERROR_INVALID_REQUEST; 
				SendGachaPurchaseAck( &Temp, NULL );
				break;
			}

			INT32 i32UserMoney = 0; 
			INT32 i32GachaPrice = 0;

			switch( Recv.m_ui8BuyType )
			{
				case GOODS_BUY_POINT :	
					{
						i32UserMoney = m_UserInfoBasic.m_ui32Point;
						i32GachaPrice = pGoods->GetPoint(); 
					}
					break;
				case GOODS_BUY_CASH :	
					{
						i32UserMoney = m_UserInfoBasic.m_ui32Cash;
						i32GachaPrice = pGoods->GetCash(); 
					}
					break;
				case GOODS_BUY_COIN :
					{
						i32UserMoney = m_UserInfoBasic.m_ui32Coin;
						i32GachaPrice = pGoods->GetCoin(); 
					}
					break;
				default :
					i32UserMoney = -1;
					break;
			}

			if( 0 == i32GachaPrice || i32UserMoney < i32GachaPrice )
			{ 
				Temp.m_eErrorCode = GACHA_ERROR_CHARGE;
				SendGachaPurchaseAck( &Temp, NULL );
				break;
			}

			if( !g_pModuleTrans->SendGachaPurchase(m_WorkThreadIdx, m_i64UID, Recv.m_ui8BuyType, Recv.m_TGoodsID, Recv.m_i32ItemIdx ) )
			{
				Temp.m_eErrorCode = GACHA_ERROR_INTERNAL;
				SendGachaPurchaseAck( &Temp, NULL );
				break;
			}

			// Point, Coin 복사 방지.
			switch( Recv.m_ui8BuyType )
			{
			case GOODS_BUY_POINT :	
				{
					m_UserInfoBasic.m_ui32Point	-= i32GachaPrice;
					ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );
				}
				break; 
			case GOODS_BUY_COIN :
				{
					m_UserInfoBasic.m_ui32Coin	-= i32GachaPrice;
					ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );
				}
				break; 
			}			
		}
		break;
	case PROTOCOL_GACHA_ITEM_INFO_REQ:
		{	// 로그인 후 최초 Gacha Shop 입장시 한번만 받습니다.
			// 1. Send Gacha Notice
			UINT8 ui8NoticeCount = (UINT8)pGachaShop->GetNoticeCount();

			i3NetworkPacket NoticePacket(PROTOCOL_GACHA_NOTICE_ACK);
			NoticePacket.WriteData(&ui8NoticeCount, sizeof(UINT8));

			for( INT32 i = 0; i < ui8NoticeCount; ++i )
			{
				PACKET_GACHA_NOTICE stNotice;
				pGachaShop->MakePacketGachaNotice(i, &stNotice); 
				UINT8 ui8Size = (UINT8)i3String::Length( stNotice.m_szNotice );
				NoticePacket.WriteData(&ui8Size,			sizeof(UINT8)); 
				NoticePacket.WriteData(stNotice.m_szNotice, sizeof(TTCHAR) * ui8Size ); 
			}

			SendPacketMessage(&NoticePacket);

			// 2. Send Gacha ItemList
			// 클라이언트와 서버의 버전이 같다면 Pass 합니다.
			char strMD5[NET_MD5_KEY_SIZE+1];
			pRecvPacket->ReadData(strMD5,	sizeof(char) * NET_MD5_KEY_SIZE ); 
			strMD5[NET_MD5_KEY_SIZE]	= 0;

			bool bLoadGachaShopFile = true;  // GachaShop.dat 파일에서 로드할지 여부 
			if( 0 == i3String::Compare(strMD5, pGachaShop->GetGachaShopMD5Key() ) 	)
			{
				i3NetworkPacket ItemPacket(PROTOCOL_GACHA_ITEM_INFO_ACK);
				ItemPacket.WriteData(&bLoadGachaShopFile,								sizeof(bool));
				SendPacketMessage(&ItemPacket);
				break;
			}

			bLoadGachaShopFile = false;

			for( UINT8 i = 0; i < GACHA_GROUP_COUNT; ++i )
			{
				GACHA_GROUP eGroup = (GACHA_GROUP)i;

				GACHA_GROUP_INFO stGachaGroup;
				pGachaShop->MakePacketGachaInfo(eGroup, &stGachaGroup);

				bool bActive = true; 
				if( GACHA_GROUP_NONE == stGachaGroup.m_eGachaGroup )
				{
					bActive = false;
				}

				i3NetworkPacket ItemPacket(PROTOCOL_GACHA_ITEM_INFO_ACK);
				ItemPacket.WriteData(&bLoadGachaShopFile,					sizeof(bool));
				ItemPacket.WriteData(&eGroup,								sizeof(GACHA_GROUP));
				ItemPacket.WriteData(&bActive,								sizeof(bool));
				if( bActive )
				{
//					ItemPacket.WriteData(&stGachaGroup.m_i32GloryCount,			sizeof(INT32));
					ItemPacket.WriteData(&stGachaGroup.m_ui32LuckyGoodsID,		sizeof(UINT32));

					ItemPacket.WriteData(&stGachaGroup.m_i32WinItemCount,		sizeof(INT32));
					ItemPacket.WriteData(stGachaGroup.m_arWinItems,				(UINT16)(sizeof(GACHA_ITEM_INFO)*stGachaGroup.m_i32WinItemCount));

					ItemPacket.WriteData(&stGachaGroup.m_i32RandomItemCnt,		sizeof(INT32));
					ItemPacket.WriteData(stGachaGroup.m_arRandomGoodsIDs,		(UINT16)(sizeof(UINT32)*stGachaGroup.m_i32RandomItemCnt));
				}
				SendPacketMessage(&ItemPacket);
			}
		}
		break;

	case PROTOCOL_GACHA_SHOP_STATE_REQ:
		{	// 일정 시간 간격으로 받습니다.
			CHECKVALID_LOBBY;

			if( m_i32SubTask != GAME_TASK_SUB_GACHA ) break;
			
			for( INT32 i = 0; i < GACHA_GROUP_COUNT ; ++i )
			{
				GACHA_GROUP eGroup = (GACHA_GROUP)i;
				GACHA_GROUP eActiveGroup =  pGachaShop->GetGachaGroupType(eGroup);
				if( GACHA_GROUP_NONE == eActiveGroup ) continue;

				i3NetworkPacket stPacket(PROTOCOL_GACHA_SHOP_STATE_ACK);
				stPacket.WriteData(&eActiveGroup,									sizeof(GACHA_GROUP));

				pGachaShop->MakeGachaStatePacketForClient( eActiveGroup, &stPacket);
				
				SendPacketMessage(&stPacket);
			}
		}
		break;

	case PROTOCOL_GACHA_GET_PURCHASE_COUNT_REQ:
		{	// 로그인 후 최초 Gacha Shop 입장시 한번만 받습니다.
			CHECKVALID_LOBBY;

			if( m_i32SubTask != GAME_TASK_SUB_GACHA ) break;
			
			for( INT32 i = 0; i < GACHA_GROUP_COUNT ; ++i )
			{
				GACHA_GROUP eGroup = (GACHA_GROUP)i;
				GACHA_GROUP eActiveGroup =  pGachaShop->GetGachaGroupType(eGroup);
				if( GACHA_GROUP_NONE == eActiveGroup ) continue;

				i3NetworkPacket stPacket(PROTOCOL_GACHA_GET_PURCHASE_COUNT_ACK);
				stPacket.WriteData(&eActiveGroup,									sizeof(GACHA_GROUP));

				GACHA_GROUP_INFO stGachaGroup;
				pGachaShop->MakePacketGachaInfo(eGroup, &stGachaGroup);
				m_GachaPurchase.MakeGachaPurchaseCount( &stGachaGroup, &stPacket);
				
				SendPacketMessage(&stPacket);
			}
		}
		break;

	default:
		{
			// Log
			return FALSE;
		}
		break;
	}

	return FALSE;
}
