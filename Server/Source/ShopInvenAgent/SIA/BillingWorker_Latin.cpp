#include "pch.h"
#include "BillingWorker_Latin.h"


CBillingWorker_Latin::CBillingWorker_Latin()
{
}


CBillingWorker_Latin::~CBillingWorker_Latin()
{
}

BOOL CBillingWorker_Latin::_WorkProcess(BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx)
{
	switch (pInput->m_ui8Action)
	{
	case TYPE_SHOP_GOODS_BUY:
	{
		GTX_PK_V09_PURCHASEITEM	stSAPacketBuy;
		i3mem::FillZero(&stSAPacketBuy, sizeof(GTX_PK_V09_PURCHASEITEM));

		stSAPacketBuy.ReqLen = sizeof(GTX_PK_V09_PURCHASEITEM);
		stSAPacketBuy.ReqType = GTX_PK_V09_TYPE_PURCHASEITEM;
		stSAPacketBuy.ItemCnt = 1;

		stSAPacketBuy.ReqKey = pInput->m_ui32MainBufferIdx * 10 + ui8GoodsIdx;
		stSAPacketBuy.ItemID = pInput->m_stLinkBuy[ui8GoodsIdx].m_TGoodsID;
		stSAPacketBuy.ChargeAmt = pInput->m_stLinkBuy[ui8GoodsIdx].m_ui32Price;
		stSAPacketBuy.ClientIP = pInput->m_ui32ClientIP;	

		i3String::Format(stSAPacketBuy.UserNo, 21, "%s", pInput->m_strBillingID);
		i3mem::Copy(stSAPacketBuy.ItemName, pInput->m_stLinkBuy[ui8GoodsIdx].m_strGoodsName, MAX_ITEM_NAME);

		stSAPacketBuy.UserNo[20] = '\0';
		stSAPacketBuy.ItemName[MAX_ITEM_NAME - 1] = '\0';

		if (0 == SendMessage((char*)&stSAPacketBuy, sizeof(GTX_PK_V09_PURCHASEITEM))) return FALSE;		
	}
	break;
	case TYPE_SHOP_TYPE_GET_CASH:
	{
		GTX_PK_V09_GETBALANCE	stSAPacketGetCash;
		i3mem::FillZero(&stSAPacketGetCash, sizeof(GTX_PK_V09_GETBALANCE));

		stSAPacketGetCash.ReqLen = sizeof(GTX_PK_V09_GETBALANCE);
		stSAPacketGetCash.ReqType = GTX_PK_V09_TYPE_GETBALANCE;

		//Set Value 
		stSAPacketGetCash.ReqKey = (DWORD)pInput->m_TUID;	
		i3String::Format(stSAPacketGetCash.UserNo, 21, "%s", pInput->m_strBillingID);

		if (0 == SendMessage((char*)&stSAPacketGetCash, sizeof(GTX_PK_V09_GETBALANCE))) return FALSE;

	}
	break;
	default : 
	{
		g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorkerLatin] default: %d", pInput->m_ui8Action);
	}
	break;	
	}

	return FALSE;
}

INT32 CBillingWorker_Latin::RecvMessage(char* pPacket, INT32 i32Size)
{
	if (sizeof(_GTX_PK_V09_HEALTH_CHECK) > i32Size)		return 0;

	//패킷헤더 구조체는 아니지만 ReqType 판별을 위해 사용합니다.
	_GTX_PK_V09_HEALTH_CHECK* pstPacketHeader = (_GTX_PK_V09_HEALTH_CHECK*)pPacket;
	BILLING_OUTPUT*	pOutput;

	if (i32Size < pstPacketHeader->ReqLen)		return 0;

	pOutput = (BILLING_OUTPUT*)m_pPopBuffer->PushPointer();

	if (NULL == pOutput)
	{
		g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorkerLatin::RecvMessage] Billing OutBuffer Full.");
		return pstPacketHeader->ReqLen;
	}

	//ReqType에 따라 해당하는 구조체 포인터로 캐스팅 해줍니다.
	switch (pstPacketHeader->ReqType)
	{
		//잔액조회
	case GTX_PK_V09_TYPE_GETBALANCE:
	{
		GTX_PK_V09_GETBALANCE* pstPacketGetCash = (GTX_PK_V09_GETBALANCE*)pPacket;

		//알수 없는 패킷 입니다. 
		if (sizeof(GTX_PK_V09_GETBALANCE) != pstPacketGetCash->ReqLen)
		{
			g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Latin::RecvMessage] Get Cash Error Unknown Packet / Size : %d", pstPacketGetCash->ReqLen);
			return pstPacketGetCash->ReqLen;
		}

		pOutput->m_eLinkType = LINK_TYPE_GET_CASH;
		pOutput->m_TUID = (INT64)pstPacketGetCash->ReqKey;

		switch (pstPacketGetCash->RetCode)
		{
		case GTX_ERR_CODE_SUCC:
		{
			pOutput->m_ai32Rv[0] = EVENT_ERROR_SUCCESS;
			pOutput->m_ui32CurrentUserCash = pstPacketGetCash->RealCash + pstPacketGetCash->BonusCash;
		}
		break;
		default:
		{
			//	100	: 잔액부족
			//	200	: Non-Existing User
			//	210	: 존재하지 않는 과금번호
			//	211	: 존재하지 않는 쿠폰번호
			//	212	: 이미 사용한 쿠폰번호
			//	300	: 빌링 시스템 에러.
			//	900 : 유효하지 않은 값.				 
			pOutput->m_ai32Rv[0] = EVENT_ERROR_BILLING_GET_CASH;
			pOutput->m_ui32CurrentUserCash = 0;
			g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Latin::RecvMessage] Cash Error Return / Error code : %d", pstPacketGetCash->RetCode);
		}
		break;
		}

		m_pPopBuffer->PushPointerIdx();
	}
	break;

	//아이템 구입
	case GTX_PK_V09_TYPE_PURCHASEITEM:
	{
		GTX_PK_V09_PURCHASEITEM* pstPacketBuy = (GTX_PK_V09_PURCHASEITEM*)pPacket;

		//알수 없는 패킷 입니다. 
		if (sizeof(GTX_PK_V09_PURCHASEITEM) != pstPacketBuy->ReqLen)
		{
			g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Latin::RecvMessage] Buy Goods Error Unknown Packet / Size : %d", pstPacketBuy->ReqLen);
			return pstPacketBuy->ReqLen;
		}

		pOutput->m_eLinkType = LINK_TYPE_BUY_GOODS;
		pOutput->m_ui32MainBufferIdx = pstPacketBuy->ReqKey;
		pOutput->m_ui8GoodsCount = 1;
		pOutput->m_ui32CurrentUserCashType = TYPE_CURRENT_USER_CASH_USE;
		pOutput->m_ui32CurrentUserCash = pstPacketBuy->RealCash + pstPacketBuy->BonusCash;
		pOutput->m_ai32Rv[0] = EVENT_ERROR_BILLING_UNKNOWN;
	
		switch (pstPacketBuy->RetCode)
		{
		case GTX_ERR_CODE_SUCC:		pOutput->m_ai32Rv[0] = EVENT_ERROR_SUCCESS;  break;
		case GTX_ERR_CODE_100:		pOutput->m_ai32Rv[0] = EVENT_ERROR_BILLING_NOT_ENOUGH_PRCIE; break;
		case GTX_ERR_CODE_200:		pOutput->m_ai32Rv[0] = EVENT_ERROR_BILLING_NOT_FIND_USER; break;
		default:
		{
			g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Latin::RecvMessage] Unknown Error / ErrorCode : %d ", pstPacketBuy->RetCode);
		}
		break;
		}

		m_pPopBuffer->PushPointerIdx();
	}
	break;
	default:
		break;

	}

	return pstPacketHeader->ReqLen;
}