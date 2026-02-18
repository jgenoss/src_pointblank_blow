#include "pch.h"
#include "BillingWorker_Korea.h"

#define PAYLETTER_USER_ID_SIZE	51

CBillingWorker_Korea::CBillingWorker_Korea()
{
}

CBillingWorker_Korea::~CBillingWorker_Korea()
{
}

BOOL CBillingWorker_Korea::_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx )
{
	switch( pInput->m_ui8Action )
	{
	case TYPE_SHOP_GOODS_BUY:
		{			
			BOQGTX_PURCHASEITEM	stSAPacketBuy;
			i3mem::FillZero( &stSAPacketBuy, sizeof(BOQGTX_PURCHASEITEM) );

			stSAPacketBuy.ReqLen		= sizeof(BOQGTX_PURCHASEITEM); 
			stSAPacketBuy.ReqType		= GTX_PK_V09_TYPE_PURCHASEITEM; 
			stSAPacketBuy.ItemCnt		= 1;

			stSAPacketBuy.ReqKey		= pInput->m_ui32MainBufferIdx * 10 + ui8GoodsIdx;
			stSAPacketBuy.ItemID		= pInput->m_stLinkBuy[ui8GoodsIdx].m_TGoodsID;
			stSAPacketBuy.ChargeAmt		= pInput->m_stLinkBuy[ui8GoodsIdx].m_ui32Price;
			stSAPacketBuy.ClientIP		= pInput->m_ui32ClientIP;

			_i64toa( pInput->m_TUID, stSAPacketBuy.UID, 10 );
			//i3mem::Copy( stSAPacketBuy.UID, pInput->m_strBillingID, BILLING_ID_SIZE );
			i3mem::Copy( stSAPacketBuy.ItemName, pInput->m_stLinkBuy[ui8GoodsIdx].m_strGoodsName, MAX_ITEM_NAME );

			stSAPacketBuy.UID[KOREA_UID_SIZE-1]	= '\0'; 
			stSAPacketBuy.ItemName[ MAX_ITEM_NAME-1 ] = '\0';

			if( 0 == SendMessage( (char*)&stSAPacketBuy, sizeof(BOQGTX_PURCHASEITEM) ) ) return FALSE;
		}
		break;
	case TYPE_SHOP_TYPE_GET_CASH:
		{
			BOQGTX_GETBALANCE	stSAPacketGetCash;
			i3mem::FillZero( &stSAPacketGetCash, sizeof(BOQGTX_GETBALANCE) );

			stSAPacketGetCash.ReqLen		= sizeof(BOQGTX_GETBALANCE);  
			stSAPacketGetCash.ReqType		= GTX_PK_V09_TYPE_GETBALANCE;

			//Set Value 
			stSAPacketGetCash.GameUID  = (unsigned __int64)pInput->m_TUID;
			_i64toa( stSAPacketGetCash.GameUID, stSAPacketGetCash.UID, 10 );
			//i3mem::Copy( stSAPacketGetCash.UID, pInput->m_strBillingID, BILLING_ID_SIZE );

			if( 0 == SendMessage( (char*)&stSAPacketGetCash, sizeof(BOQGTX_GETBALANCE) ) ) return FALSE;
			
		}
		break;
	case TYPE_SHOP_USE_GIFTCOUPON:
		{
			pOutput->m_TUID = pInput->m_TUID;
			i3mem::Copy( pOutput->m_strCouponCode, pInput->m_strCouponCode, GIFTCOUPON_LENGTH + 1 );

			GTX_PK_REGCOUPON	stSAPacketRegCoupon;

			i3mem::FillZero( &stSAPacketRegCoupon, sizeof(GTX_PK_REGCOUPON) );

			stSAPacketRegCoupon.ReqLen		= sizeof(GTX_PK_REGCOUPON);
			stSAPacketRegCoupon.ReqType		= GTX_PK_V09_TYPE_REGCOUPON;

			stSAPacketRegCoupon.ReqKey		= pInput->m_ui32MainBufferIdx;

			if( PC_NOT_CAFE == pInput->m_ui8PcCafe )
				stSAPacketRegCoupon.UseLocation = 0;
			else
				stSAPacketRegCoupon.UseLocation = 2;

			i3mem::Copy( stSAPacketRegCoupon.CouponNo, pInput->m_strCouponCode, GIFTCOUPON_LENGTH + 1);
			stSAPacketRegCoupon.CouponNo[GIFTCOUPON_LENGTH] = '\0';

			stSAPacketRegCoupon.ClientIP	= pInput->m_ui32ClientIP;
			i3String::Format( stSAPacketRegCoupon.UID, 128+1, "%I64d", pInput->m_TUID );
			i3String::Format( stSAPacketRegCoupon.BirthDay, 8+1, "%u", pInput->m_ui32BirthDay );
			i3mem::Copy( stSAPacketRegCoupon.UserID, pInput->m_strID, sizeof(char) * PAYLETTER_USER_ID_SIZE );
			stSAPacketRegCoupon.UserID[PAYLETTER_USER_ID_SIZE - 1] = '\0';

			WideCharToMultiByte( GetACP(), 0, pInput->m_strUserNick, NET_NICK_NAME_SIZE, stSAPacketRegCoupon.UserName, NET_NICK_NAME_SIZE, NULL, NULL);
			WideCharToMultiByte( GetACP(), 0, pInput->m_strUserNick, NET_NICK_NAME_SIZE, stSAPacketRegCoupon.CharacterID, NET_NICK_NAME_SIZE, NULL, NULL);

			UINT32 ui32CreateDate = pInput->m_ui32CreateDate;
			INT32 i32Year	= ui32CreateDate/100000000;			ui32CreateDate %= 100000000;
			INT32 i32Month	= ui32CreateDate/1000000;			ui32CreateDate %= 1000000;
			INT32 i32Day	= ui32CreateDate/10000;				ui32CreateDate %= 10000;
			INT32 i32Hour	= ui32CreateDate/100;
			INT32 i32Min	= ui32CreateDate%100;
			i3String::Format( stSAPacketRegCoupon.MemberRegDate, 19+1, "20%02d-%02d-%02d %02d:%02d:00", i32Year, i32Month, i32Day, i32Hour, i32Min );

			if( 0 == SendMessage( (char*)&stSAPacketRegCoupon, sizeof(GTX_PK_REGCOUPON) ) ) return FALSE;
		}
		break;
	}

	return FALSE;
}

INT32 CBillingWorker_Korea::RecvMessage( char* pPacket, INT32 i32Size )
{	
	if( sizeof(BOQGTX_HEALTH_CHECK) > i32Size )		return 0; 

	//패킷헤더 구조체는 아니지만 ReqType 판별을 위해 사용합니다.
	BOQGTX_HEALTH_CHECK* pstPacketHeader = (BOQGTX_HEALTH_CHECK*)pPacket;
	BILLING_OUTPUT*	pOutput;

	if( i32Size < pstPacketHeader->ReqLen )		return 0; 

	pOutput = (BILLING_OUTPUT*)m_pPopBuffer->PushPointer();

	if( NULL == pOutput )
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Korea::RecvMessage] Billing OutBuffer Full." );
		return pstPacketHeader->ReqLen;
	}

	//ReqType에 따라 해당하는 구조체 포인터로 캐스팅 해줍니다.
	switch(pstPacketHeader->ReqType)
	{
	//잔액조회
	case GTX_PK_V09_TYPE_GETBALANCE:
		{
			BOQGTX_GETBALANCE* pstPacketGetCash = (BOQGTX_GETBALANCE*)pPacket;

			//알수 없는 패킷 입니다. 
			if( sizeof(BOQGTX_GETBALANCE) != pstPacketGetCash->ReqLen )
			{ 
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Korea::RecvMessage] Get Cash Error Unknown Packet / Size : %d", pstPacketGetCash->ReqLen );
				return pstPacketGetCash->ReqLen; 
			}

			pOutput->m_eLinkType			= LINK_TYPE_GET_CASH;
			pOutput->m_TUID					= (INT64)pstPacketGetCash->GameUID;
			
			switch( pstPacketGetCash->RetCode )
			{
			case GTX_ERR_CODE_SUCC :
				{
					pOutput->m_ai32Rv[0]			= EVENT_ERROR_SUCCESS;
					pOutput->m_ui32CurrentUserCash	= pstPacketGetCash->RealCash + pstPacketGetCash->BonusCash; 
				}
				break;
			default :
				{
					//	100	: 잔액부족
					//	200	: Non-Existing User
					//	210	: 존재하지 않는 과금번호
					//	211	: 존재하지 않는 쿠폰번호
					//	212	: 이미 사용한 쿠폰번호
					//	300	: 빌링 시스템 에러.
					//	900 : 유효하지 않은 값.				 
					pOutput->m_ai32Rv[0]			= EVENT_ERROR_BILLING_GET_CASH; 
					pOutput->m_ui32CurrentUserCash	= 0;
					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Korea::RecvMessage] Cash Error Return / Error code : %d", pstPacketGetCash->RetCode );
				}
				break;
			}
			
			m_pPopBuffer->PushPointerIdx();
		}
		break;

	//아이템 구입
	case GTX_PK_V09_TYPE_PURCHASEITEM:
		{
			BOQGTX_PURCHASEITEM* pstPacketBuy = (BOQGTX_PURCHASEITEM*)pPacket;

			//알수 없는 패킷 입니다. 
			if( sizeof(BOQGTX_PURCHASEITEM) != pstPacketBuy->ReqLen )
			{ 
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Korea::RecvMessage] Buy Goods Error Unknown Packet / Size : %d", pstPacketBuy->ReqLen );
				return pstPacketBuy->ReqLen; 
			}

			pOutput->m_eLinkType				= LINK_TYPE_BUY_GOODS;
			pOutput->m_ui32MainBufferIdx		= pstPacketBuy->ReqKey;
			pOutput->m_ui8GoodsCount			= 1;
			pOutput->m_ui32CurrentUserCashType	= TYPE_CURRENT_USER_CASH_USE;					
			pOutput->m_ui32CurrentUserCash		= pstPacketBuy->RealCash + pstPacketBuy->BonusCash; 
			pOutput->m_ai32Rv[0]				= EVENT_ERROR_BILLING_UNKNOWN;  

			switch( pstPacketBuy->RetCode ) 
			{
			case GTX_ERR_CODE_SUCC:		pOutput->m_ai32Rv[0]	= EVENT_ERROR_SUCCESS;  break; 
			case GTX_ERR_CODE_100:		pOutput->m_ai32Rv[0]	= EVENT_ERROR_BILLING_NOT_ENOUGH_PRCIE; break;
			case GTX_ERR_CODE_200:		pOutput->m_ai32Rv[0]	= EVENT_ERROR_BILLING_NOT_FIND_USER; break;
			default:
				{ 		
					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Korea::RecvMessage] Unknown Error / ErrorCode : %d ", pstPacketBuy->RetCode );
				}
				break;
			}

			m_pPopBuffer->PushPointerIdx();
		}
		break;

	//쿠폰 사용
	case GTX_PK_V09_TYPE_REGCOUPON:
		{
			GTX_PK_REGCOUPON* pstPacketCoupon = (GTX_PK_REGCOUPON*)pPacket;

			//알수 없는 패킷 입니다. 
			if( sizeof(GTX_PK_REGCOUPON) != pstPacketCoupon->ReqLen )
			{ 
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Korea::RecvMessage] Use Coupon Error Unknown Packet / Size : %d", pstPacketCoupon->ReqLen );
				return pstPacketCoupon->ReqLen; 
			}

			pOutput->m_eLinkType			= LINK_TYPE_USE_GIFTCOUPON;
			pOutput->m_ui32MainBufferIdx	= pstPacketCoupon->ReqKey;

			switch( pstPacketCoupon->RetCode )
			{
			case GTX_ERR_CODE_SUCC:	// 성공
				{
					pOutput->m_ai32Rv[0] = EVENT_ERROR_SUCCESS;
					pOutput->m_ui32CurrentUserCash	= pstPacketCoupon->CashReal + pstPacketCoupon->CashBonus;

					if ( 1 == pstPacketCoupon->CouponType )
					{
						pOutput->m_ui32CurrentUserCash		= pstPacketCoupon->CashAmt;
					}
					else if ( 2 == pstPacketCoupon->CouponType )
					{
						pOutput->m_ui32CurrentUserCash		= 0;
						pOutput->m_ui8GoodsCount			= 0;
						INT32 i32Ptr = 0;
						INT32 i32Len = i3String::Length( pstPacketCoupon->GameItemID );
						for( INT32 i = 0; i <= i32Len; i++ )
						{
							switch( pstPacketCoupon->GameItemID[i] )
							{
							case '^':
								{
									pstPacketCoupon->GameItemID[i] = '\0';
									if ( pOutput->m_ui8GoodsCount + 1 < MAX_SHOP_GOODS_BUY_COUNT )
									{
										pOutput->m_ui8GoodsCount++;
										pOutput->m_ai32Rv[pOutput->m_ui8GoodsCount] = atoi( &pstPacketCoupon->GameItemID[i32Ptr] );
									}
									i32Ptr = i + 1;
								}
								break;
							case '\0':
								{
									if ( pOutput->m_ui8GoodsCount + 1 < MAX_SHOP_GOODS_BUY_COUNT )
									{
										pOutput->m_ui8GoodsCount++;
										pOutput->m_ai32Rv[pOutput->m_ui8GoodsCount] = atoi( &pstPacketCoupon->GameItemID[i32Ptr] );
									}
									i = i32Len + 1;
								}
								break;
							}
						}
						if ( pOutput->m_ui8GoodsCount <= 0 )
						{
							pOutput->m_ai32Rv[0] = EVENT_ERROR_NETWORK;
						}
					}
				}
				break;
			case 5101:	// 하루에 10번 이상 잘못된 쿠폰 번호를 입력하여 인증이 불가할 경우에 발생하는 에러
			case 5105:	// 존재하지 않는 쿠폰 번호 입력 시 반환되는 에러
			case 5120:	// 사용 중지된 쿠폰
			case 5121:	// 유효기간이 만료된 쿠폰
			case 5124:	// 잘못된 게임코드로 쿠폰을 등록할 경우 발생하는 에러
				pOutput->m_ai32Rv[0] = EVENT_ERROR_COUPON_WRONG_NUMBER;
				break;
			case 5110:	// 대표 쿠폰이 아닌 일반 쿠폰의 경우 발생하는 에러로 쿠폰이 이미 사용된 경우에 발생하는 에러입니다
			case 5117:	// 입력한 쿠폰 이미 다른 사용자에게 할당된 경우 발생하는 에러
			case 5130:	// 중복 사용이 불가한 쿠폰을 2번이상 사용할 경우 발생하는 에러(CouponID 기준)
			case 5125:	// 발행된 쿠폰의 가능한 최대 사용 개수를 초과한 경우 발생하는 에러(CouponID 기준)
				pOutput->m_ai32Rv[0] = EVENT_ERROR_COUPON_ALREADY_USED;
				break;
			case 5135:	// 모바일 전용 쿠폰 입니다.
				pOutput->m_ai32Rv[0] = EVENT_ERROR_COUPON_WRONG_USE_MOBILE_COUPON;
				break;
			case 5136:	// 입력하신 쿠폰은 PC방 전용 쿠폰입니다. 가맹점에 방문하셔서 입력 부탁 드립니다.
				pOutput->m_ai32Rv[0] = EVENT_ERROR_COUPON_WRONG_USE_PCCAFE_COUPON;
				break;
			default:
				{
					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Korea::RecvMessage_Coupon] Unknown Error / ErrorCode : %d ", pstPacketCoupon->RetCode );
					pOutput->m_ai32Rv[0]	= EVENT_ERROR_COUPON_UNKNOWN;
				}
				break;
			}

			i3mem::Copy( pOutput->m_strCouponCode, pstPacketCoupon->CouponNo, GIFTCOUPON_LENGTH + 1 );

			m_pPopBuffer->PushPointerIdx();
		}
		break;

	default:
		break;

	}

	return pstPacketHeader->ReqLen;
}