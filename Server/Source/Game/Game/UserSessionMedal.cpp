#include "pch.h"
#include "UserSession.h"
#include "ModuleContextNc.h"
#include "ModuleTrans.h"

BOOL	CUserSession::_PacketParsingMedal( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &isKilled )
{	
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
	case PROTOCOL_GET_MEDAL_INFO_REQ:	// 메달 정보 요청. Client -> Game.
		{
			CHECKVALID_DBINDEX;

			SendMedalInfo();
		}
		break;

	case PROTOCOL_GET_CUR_MEDAL_SET_INFO_REQ:	// 진행중인 메달 셋트 정보 요청. Client -> Game.
		{
			CHECKVALID_DBINDEX;
			
			SendCurMedalSetInfo();
		}
		break;

	case PROTOCOL_GET_COM_MEDAL_SET_INFO_REQ:	// 완료된 메달 셋트 정보 요청. Client -> Game.
		{
			CHECKVALID_DBINDEX;
			
			SendComMedalSetInfo();
		}
		break;

	case PROTOCOL_SET_NOTIFY_MEDAL_REQ:
		{
			CHECKVALID_DBINDEX;

			PACKET_SET_NOTIFY_MEDAL_ACK	PacketAck;
			PACKET_SET_NOTIFY_MEDAL_REQ	PacketReq;

			pPacket->ReadData( &PacketReq,	sizeof(PACKET_SET_NOTIFY_MEDAL_REQ) );

			// 클라이언트가 셋팅 요청하는 값이 올바른지 체크
			UINT8 ui8MedalType = 0;
			UINT16 ui16MedalIdx = 0;
			
			i3mem::Copy( PacketAck.m_aNotifyMedal, PacketReq.m_aNotifyMedal, sizeof( NOTIFY_MEDAL ) * MAX_NOTIFY_MADAL_COUNT );

			for( UINT8 i = 0 ; i < MAX_NOTIFY_MADAL_COUNT; i++ )
			{
				ui16MedalIdx = PacketReq.m_aNotifyMedal[i].m_ui16Idx;
				if( 0 == ui16MedalIdx )		continue;		// 알리미 등록하지 않음.
				
				ui8MedalType = PacketReq.m_aNotifyMedal[i].m_ui8MedalType;

				// 해킹
				if( ui8MedalType >= MEDAL_TYPE_MAX )
				{
					SendFailMedalNotifySetting();
					return FALSE;
				}
				
				switch ( ui8MedalType ) 
				{
				case MEDAL_TYPE_NORMAL :
					{
						MEDAL_GT* pUserMedal =  GetMedal_UserDataAll( ui16MedalIdx );
						if( NULL == pUserMedal )	// 해킹
						{
							SendFailMedalNotifySetting();
							return FALSE;
						}
						PacketAck.m_aNotifyMedal[i].m_ui16Count = pUserMedal->m_Medal.m_ui16Count;
					}
					break;
				case MEDAL_TYPE_SET : 
					{
						MQF_MEDAL_SET_INFO* pMQFMedalSet =  g_pContextMain->GetMedalSet_ByMedalIdx( ui16MedalIdx );
						if( NULL == pMQFMedalSet )	// 메달 셋트 MQF 찾기 실패. 해킹.
						{
							SendFailMedalNotifySetting();
							return FALSE;
						}

						CUR_MEDAL_SET* pUserCurMedalSet = GetCurMedalSet_UserData(pMQFMedalSet->m_ui16Idx);
						if( NULL == pUserCurMedalSet )	// 유저 메달 셋트 찾기 실패. 해킹.
						{
							SendFailMedalNotifySetting();
							return FALSE;
						}

						// 몇번째 메달인지..
						INT8 i8MedalArrIdx = -1;
						for( UINT8 k = 0 ; k < pMQFMedalSet->m_ui8ActiveMedalCount ; k++ )
						{
							if( ui16MedalIdx == pMQFMedalSet->m_aMedalInfo[k].m_ui16Idx ) 
							{
								i8MedalArrIdx = k;
								break;
							}
						}

						if( -1 == i8MedalArrIdx )
						{
							SendFailMedalNotifySetting();
							return FALSE;
						}

						PacketAck.m_aNotifyMedal[i].m_ui16Count = pUserCurMedalSet->m_ui16Count[i8MedalArrIdx];
					}
					break;
				}				 
			}
			 
			// 성공하면 유저 메달 정보에도 Copy.
			i3mem::Copy( m_MedalAllInfo.m_aNotifyMedal, PacketAck.m_aNotifyMedal, sizeof( NOTIFY_MEDAL ) * MAX_NOTIFY_MADAL_COUNT );

			bSendMessage = TRUE;
			PacketAck.m_i32Rv = EVENT_ERROR_SUCCESS;
			pSendPacket->SetProtocol( PROTOCOL_SET_NOTIFY_MEDAL_ACK );
			pSendPacket->WriteData(&PacketAck,	sizeof(PACKET_SET_NOTIFY_MEDAL_ACK) );

			ChangeDataBaseInfo( SAVE_DATABASE_MEDAL_BASIC );
		}
		break;

	case PROTOCOL_GET_MEDAL_REWARD_REQ:	// 메달(n개) 완료에 대한 보상 요청. Client -> Game.
		{
			CHECKVALID_DBINDEX;
			
			// 보낼 패킷.
			PACKET_D_GET_MEDAL_REWARD		PacketAckD[MAX_SHOP_DB_ITEM_COUNT];
			i3mem::FillZero( PacketAckD, sizeof( PACKET_D_GET_MEDAL_REWARD )*MAX_SHOP_DB_ITEM_COUNT) ;
			PACKET_H_GET_MEDAL_REWARD_ACK	PacketAckH;
			i3mem::FillZero( &PacketAckH, sizeof( PACKET_H_GET_MEDAL_REWARD_ACK )) ;

			// 받은 패킷.
			MEDAL_REWARD	aMedalReward[MAX_SHOP_DB_ITEM_COUNT];
			PACKET_H_GET_MEDAL_REWARD_REQ	PacketReqH;
			
			pPacket->ReadData( &PacketReqH,	sizeof(PACKET_H_GET_MEDAL_REWARD_REQ) );		

			UINT8 ui8RecvCount = MIN( PacketReqH.m_ui8Count, MAX_SHOP_DB_ITEM_COUNT );

			UINT8	ui8RewardCheckFlag;
			
			MQF_MEDAL_INFO * pMQFMedal		= NULL;
			MQF_MEDAL_SET_INFO* pMQFMedalSet= NULL;
			//COM_MEDAL_SET* pUserComMedalSet	= NULL;

			MEDAL_GT* pUserMedal			= NULL;
			CUR_MEDAL_SET* pUserCurMedalSet	= NULL;
			MEDAL_REWARD_INFO	RewardInfo;		// 전체 보상 정보.
			MEDAL_REWARD_INFO	RewardItem;		// Trans서버에 보낼 보상 정보( Item보상 )
			i3mem::FillZero( &RewardItem, sizeof( MEDAL_REWARD_INFO ) );

			for( UINT8 i = 0 ; i < ui8RecvCount ; ++i )
			{
				pPacket->ReadData( &aMedalReward[i],	sizeof(MEDAL_REWARD) );		

				RewardInfo.m_aDetail[i].m_i32Rv				= EVENT_ERROR_PBV15_MEDAL_REWARD_HACK; 
				RewardInfo.m_aDetail[i].m_ui16MedalIdx		= aMedalReward[i].m_ui16MedalIdx;
				RewardInfo.m_aDetail[i].m_ui8MedalType		= aMedalReward[i].m_ui8MedalType; 
				RewardInfo.m_aDetail[i].m_ui8Level			= aMedalReward[i].m_ui8MedalLevel;

				UINT8 ui8Level = aMedalReward[i].m_ui8MedalLevel; 

				// 보낸 값 체크
				if( aMedalReward[i].m_ui8MedalType >= MEDAL_TYPE_MAX   )	continue;
				if( MEDAL_TYPE_NORMAL == aMedalReward[i].m_ui8MedalType && ( ui8Level < MEDAL_LEVEL_1 || ui8Level > MEDAL_LEVEL_MAX ) )	continue;

				// 보낸 값 체크2. 해당 메달 및 보상 내역 가져오기.
				switch ( aMedalReward[i].m_ui8MedalType )
				{
				case MEDAL_TYPE_NORMAL:	// 일반 메달
					{
						// Mqf데이터 있는지...
						pMQFMedal = g_pContextMain->GetMedal_MQFData(aMedalReward[i].m_ui16MedalIdx, MEDAL_TYPE_NORMAL, m_ui8LocaleCode);
						if( NULL == pMQFMedal )		continue;
						
						// 유저가 메달을 소지 했는지.
						pUserMedal = GetMedal_UserDataAll(aMedalReward[i].m_ui16MedalIdx) ;
						if( NULL == pUserMedal )	continue;

						// 이미 지급 받았는지 체크.
						ui8RewardCheckFlag = 1 << (ui8Level-1);
						if( pUserMedal->m_Medal.m_ui8GetReward & ui8RewardCheckFlag )	continue;

						// 카운트 조건이 만족되는지 체크.
						UINT16 ui16RewardCount	= pMQFMedal->m_ui16Count[ui8Level-1];	// 요청한 레벨의 보상을 받기 위한 카운트
						UINT16 ui16CompleteCount=  pUserMedal->m_Medal.m_ui16Count;	// 유저가 달성해놓은 카운트.
						if( ui16CompleteCount < ui16RewardCount )	continue;
					}
					break; 
				case MEDAL_TYPE_SET:	// 세트 메달
					{
						// Mqf데이터 있는지 검색.
						pMQFMedal = g_pContextMain->GetMedal_MQFData(aMedalReward[i].m_ui16MedalIdx, MEDAL_TYPE_SET ) ;
						if( NULL == pMQFMedal )	 continue;

						pMQFMedalSet =  g_pContextMain->GetMedalSet_ByMedalIdx( aMedalReward[i].m_ui16MedalIdx );
						if( NULL == pMQFMedalSet )	continue;

						// 유저의 진행중인 메달 셋트에서 검색.
						pUserCurMedalSet = GetCurMedalSet_UserData(pMQFMedalSet->m_ui16Idx) ;
						if( NULL == pUserCurMedalSet )		// 검색 실패
						{
							RewardInfo.m_aDetail[i].m_i32Rv = EVENT_ERROR_PBV15_MEDAL_REWARD_EXPIRE;
							continue;
						}

						// 몇번째 메달인지 검색
						ui8Level = g_pContextMain->GetSetMedalArrIdx_ByMedalIdx( aMedalReward[i].m_ui16MedalIdx );
						if( 255 == ui8Level ||  0 == ui8Level )	continue;

						// 이미 지급 받았는지 체크.
						ui8RewardCheckFlag = 1 << (ui8Level-1);
						if( pUserCurMedalSet->m_ui8GetReward & ui8RewardCheckFlag )			continue;
					
						// 카운트 조건이 만족되는지 체크.
						UINT16 ui16RewardCount	= pMQFMedal->m_ui16Count[MEDAL_LEVEL_MAX_SET-1];	// 요청한 레벨의 보상을 받기 위한 카운트. 셋트는 레벨이 1뿐이므로
						UINT16 ui16CompleteCount= pUserCurMedalSet->m_ui16Count[ui8Level-1];// 유저가 달성해놓은 카운트.
						if( ui16CompleteCount < ui16RewardCount )	continue;

						RewardInfo.m_aDetail[i].m_ui8Level			= ui8Level;
						RewardInfo.m_aDetail[i].m_ui16MedalSetIdx	= pUserCurMedalSet->m_ui16Idx;
						RewardInfo.m_aDetail[i].m_ui8MedalSetType	= pUserCurMedalSet->m_ui8Type; 
					}
					break;
				default : 
					continue;
				} 				

				switch( aMedalReward[i].m_ui8MedalType )
				{	// 보상 Item, Exp, Point 정보 셋팅.
				case MEDAL_TYPE_NORMAL: 
					RewardInfo.m_aDetail[i].m_ui32Exp			= pMQFMedal->m_eRewardInfo[ui8Level-1].m_ui32RewardExp;
					RewardInfo.m_aDetail[i].m_ui32Point			= pMQFMedal->m_eRewardInfo[ui8Level-1].m_ui32RewardPoint; 
					RewardInfo.m_aDetail[i].m_ui16MasterMedal	= (UINT16)pMQFMedal->m_eRewardInfo[ui8Level-1].m_ui32RewardMaster;
					RewardInfo.m_aDetail[i].m_TItemID			= pMQFMedal->m_eRewardInfo[ui8Level-1].m_ui32RewardItemID;
					break;
				case MEDAL_TYPE_SET: 

					RewardInfo.m_aDetail[i].m_ui32Exp			= pMQFMedalSet->m_aMedalInfo[ui8Level-1].m_eRewardInfo[MEDAL_LEVEL_MAX_SET-1].m_ui32RewardExp;
					RewardInfo.m_aDetail[i].m_ui32Point			= pMQFMedalSet->m_aMedalInfo[ui8Level-1].m_eRewardInfo[MEDAL_LEVEL_MAX_SET-1].m_ui32RewardPoint;
					RewardInfo.m_aDetail[i].m_ui16MasterMedal	= (UINT16)pMQFMedalSet->m_aMedalInfo[ui8Level-1].m_eRewardInfo[MEDAL_LEVEL_MAX_SET-1].m_ui32RewardMaster;
					RewardInfo.m_aDetail[i].m_TItemID			= pMQFMedalSet->m_aMedalInfo[ui8Level-1].m_eRewardInfo[MEDAL_LEVEL_MAX_SET-1].m_ui32RewardItemID;
					break;
				default : 
					continue;
				}				
				
				if( RewardInfo.m_aDetail[i].m_TItemID > 0 )
				{	// 보상 아이템 검색
					CShop* pShop = g_pContextMain->GetShop();
					SHOP_ITEM_BASE*	pItem = pShop->GetItemData( RewardInfo.m_aDetail[i].m_TItemID );
					if( NULL == pItem ) 
					{	// 보상 아이템이 서버에 등록되지 않은 아이템.
						g_pLog->WriteLogSession( this, L"[PROTOCOL_GET_MEDAL_REWARD_REQ] Not Find Item / UID : %I64d / MedalType : %d, MedalIdx : %d, ItemId : %d  ", 
							m_i64UID , aMedalReward[i].m_ui8MedalType, aMedalReward[i].m_ui16MedalIdx, RewardInfo.m_aDetail[i].m_TItemID ); 
						RewardInfo.m_aDetail[i].m_i32Rv = EVENT_ERROR_PBV15_MEDAL_REWARD_FIND_FAIL_ITEM;
						continue;
					} 

					switch ( pItem->_ui8AuthType ) 
					{
					case ITEM_TYPE_COUNT : 
						{
							if( ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType  )  
							{	 // 내구도는 지급 실패
								g_pLog->WriteLogSession( this, L"[PROTOCOL_GET_MEDAL_REWARD_REQ] Item is Durability / UID : %I64d / MedalType : %d, MedalIdx : %d, ItemId : %d  ", 
									m_i64UID , aMedalReward[i].m_ui8MedalType, aMedalReward[i].m_ui16MedalIdx, RewardInfo.m_aDetail[i].m_TItemID ); 
								RewardInfo.m_aDetail[i].m_i32Rv = EVENT_ERROR_PBV15_MEDAL_REWARD_FIND_FAIL_ITEM;
								continue;
							}

							switch( aMedalReward[i].m_ui8MedalType )
							{
							case MEDAL_TYPE_NORMAL:	RewardInfo.m_aDetail[i].m_TItemArg = pMQFMedal->m_eRewardInfo[ui8Level-1].m_ui32RewardItemCount ;  break;
							case MEDAL_TYPE_SET:	RewardInfo.m_aDetail[i].m_TItemArg = pMQFMedalSet->m_aMedalInfo[ui8Level-1].m_eRewardInfo[MEDAL_LEVEL_MAX_SET-1].m_ui32RewardItemCount; break;
							default : continue;
							}
						}
						break;
					case ITEM_TYPE_TIME : 
						{
							switch( aMedalReward[i].m_ui8MedalType )
							{
							case MEDAL_TYPE_NORMAL:	RewardInfo.m_aDetail[i].m_TItemArg = pMQFMedal->m_eRewardInfo[ui8Level-1].m_ui32RewardItemPeriod ;  break;
							case MEDAL_TYPE_SET:	RewardInfo.m_aDetail[i].m_TItemArg = pMQFMedalSet->m_aMedalInfo[ui8Level-1].m_eRewardInfo[MEDAL_LEVEL_MAX_SET-1].m_ui32RewardItemPeriod; break;
							default : continue;
							}
						}
						break;
					case ITEM_TYPE_ETERNITY : RewardInfo.m_aDetail[i].m_TItemArg = 1 ; break;	// 영구제는 Arg 1로.
					default : continue;
					}
				}

				// 보상 플래그 UPDATE. 아이템/포인트/EXP 복사 방지
				switch ( aMedalReward[i].m_ui8MedalType ) 
				{
				case MEDAL_TYPE_NORMAL: 
					pUserMedal->m_Medal.m_ui8GetReward |= ui8RewardCheckFlag; 
					pUserMedal->m_ui8IsSave = 1;
					ChangeDataBaseInfo( SAVE_DATABASE_MEDAL );
					break;
				case MEDAL_TYPE_SET: 
					pUserCurMedalSet->m_ui8GetReward |= ui8RewardCheckFlag; 
					ChangeDataBaseInfo( SAVE_DATABASE_CUR_MEDAL_SET );
					break; 
				} 

				RewardInfo.m_aDetail[i].m_i32Rv = EVENT_ERROR_SUCCESS; 
			}

			UINT8 ui8SendCount = 0;	// 클라이언트에 ACK를 보낼 메달 갯수.
			 
			// 보상에 Item이 포함된(결과값 성공) 그룹과 나머지그룹(보상에 Item 포함되지 않거나 결과값 실패)으로 분리합니다.
			for( UINT8 i = 0 ; i < ui8RecvCount ; ++i )
			{	
				if( EV_SUCCESSED( RewardInfo.m_aDetail[i].m_i32Rv ) &&  RewardInfo.m_aDetail[i].m_TItemID > 0 )  
				{	// 보상 지급에 성공한 Item 그룹은 별도로 빼놓습니다.
					i3mem::Copy( &RewardItem.m_aDetail[RewardItem.m_ui8Count], &RewardInfo.m_aDetail[i], sizeof( MEDAL_REWARD_DETAIL_INFO ) ); 
					++RewardItem.m_ui8Count; 
					continue; 
				}

				PacketAckD[ui8SendCount].m_i32Rv						= RewardInfo.m_aDetail[i].m_i32Rv;
				PacketAckD[ui8SendCount].m_aMedalReward.m_ui8MedalType	= RewardInfo.m_aDetail[i].m_ui8MedalType;
				PacketAckD[ui8SendCount].m_aMedalReward.m_ui16MedalIdx	= RewardInfo.m_aDetail[i].m_ui16MedalIdx;
				PacketAckD[ui8SendCount].m_aMedalReward.m_ui8MedalLevel	= RewardInfo.m_aDetail[i].m_ui8Level; 
				if( MEDAL_TYPE_SET == RewardInfo.m_aDetail[i].m_ui8MedalType ) PacketAckD[ui8SendCount].m_aMedalReward.m_ui8MedalLevel = 1;
				++ui8SendCount; 

				if( EV_FAILED( RewardInfo.m_aDetail[i].m_i32Rv ) ) continue;
				 
				// 메달 보상 Exp, Point 지급.
				m_UserInfoBasic.m_ui32Exp			+= RewardInfo.m_aDetail[i].m_ui32Exp;
				m_UserInfoBasic.m_ui32Point			+= RewardInfo.m_aDetail[i].m_ui32Point;
				m_ui32Master						+= RewardInfo.m_aDetail[i].m_ui16MasterMedal;
				
				if( RewardInfo.m_aDetail[i].m_ui32Point > 0 ) 
				{	// 포인트 획득시 로그 남긴다.
					INT32 i32GetRoute = GET_POINT_ROUTE_MEDAL_REWARD;
					g_pModuleContextNC->LogSendMessage( m_WorkThreadIdx, LOG_MESSAGE_U_GET_POINT, this, &RewardInfo.m_aDetail[i].m_ui32Point, &i32GetRoute );
				}
				
				if( RewardInfo.m_aDetail[i].m_ui32Exp > 0)
				{	// 계급 변화가 있는지 확인
					CheckRankUp( m_WorkThreadIdx );

					// Zlog 기록
					INT32 i32GetRoute = GET_EXP_ROUTE_MEDAL_REWARD;
					g_pModuleContextNC->LogSendMessage( m_WorkThreadIdx, LOG_MESSAGE_U_GET_EXP, this, &RewardInfo.m_aDetail[i].m_ui32Exp, &i32GetRoute );
				}
			}

			if( ui8SendCount > 0 )
			{	// 나미지 그룹은 클라이언트에 ACK 전송.
				if( 0 == RewardItem.m_ui8Count ) PacketAckH.m_bLastPacket = true;
				else PacketAckH.m_bLastPacket = false;

				PacketAckH.m_ui8Count			= ui8SendCount;
				PacketAckH.m_ui32Exp			= m_UserInfoBasic.m_ui32Exp;
				PacketAckH.m_ui32Point			= m_UserInfoBasic.m_ui32Point; 
				PacketAckH.m_ui16MasterMedal	= (UINT16)m_ui32Master;
				SendGetMedalReward( &PacketAckH, PacketAckD  ); 
				ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );
				ChangeDataBaseInfo( SAVE_DATABASE_MEDAL_BASIC );
			}

			if( RewardItem.m_ui8Count > 0 )
			{	// Item 그룹은 Trans 서버에 Send   
				
				if( FALSE == g_pModuleTrans->AuthSendMedalReward( m_WorkThreadIdx, this,  &RewardItem ) ) 
				{
					for( UINT8 i = 0 ; i < RewardItem.m_ui8Count ; ++i )	
					{
						PacketAckD[i].m_i32Rv = EVENT_ERROR_PBV15_MEDAL_REWARD_NETWORK;
						PacketAckD[i].m_aMedalReward.m_ui16MedalIdx = RewardItem.m_aDetail[i].m_ui16MedalIdx;
						PacketAckD[i].m_aMedalReward.m_ui8MedalType = RewardItem.m_aDetail[i].m_ui8MedalType;
						PacketAckD[i].m_aMedalReward.m_ui8MedalLevel = RewardItem.m_aDetail[i].m_ui8Level;  
						PacketAckD[i].m_aItem.m_TItemID = RewardItem.m_aDetail[i].m_TItemID;
						PacketAckD[i].m_aItem.m_TItemArg = RewardItem.m_aDetail[i].m_TItemArg;

						ui8RewardCheckFlag = 1 << (RewardItem.m_aDetail[i].m_ui8Level-1); 	
						switch ( RewardItem.m_aDetail[i].m_ui8MedalType ) 
						{	// 보상 플래그 롤백
						case MEDAL_TYPE_NORMAL: 
							pUserMedal = GetMedal_UserDataAll(RewardItem.m_aDetail[i].m_ui16MedalIdx);
							if( NULL == pUserMedal )	continue;  

							pUserMedal->m_Medal.m_ui8GetReward &= ~ui8RewardCheckFlag; 
							break;
						case MEDAL_TYPE_SET:  
							pMQFMedalSet =  g_pContextMain->GetMedalSet_ByMedalIdx( RewardItem.m_aDetail[i].m_ui16MedalIdx );
							if( NULL == pMQFMedalSet )	continue; 
							pUserCurMedalSet = GetCurMedalSet_UserData(pMQFMedalSet->m_ui16Idx) ;
							if( NULL == pUserCurMedalSet ) continue;

							pUserCurMedalSet->m_ui8GetReward  &= ~ui8RewardCheckFlag; 
							break;
						}
					}
					PacketAckH.m_ui32Exp			= m_UserInfoBasic.m_ui32Exp;
					PacketAckH.m_ui32Point			= m_UserInfoBasic.m_ui32Point; 
					PacketAckH.m_ui16MasterMedal	= (UINT16)m_ui32Master;
					PacketAckH.m_bLastPacket = true;
					PacketAckH.m_ui8Count = RewardItem.m_ui8Count;
					SendGetMedalReward( &PacketAckH, PacketAckD ); 
				}
			}
		}
		break;

	default : 
		break;
	}

	return bSendMessage;
}