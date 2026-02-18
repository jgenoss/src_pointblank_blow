#include "pch.h"
#include "PopupRepair.h"
#include "GlobalVariables.h"

#include "ShopContext.h"

#if defined (LOCALE_CHINA)
#include "../SDOALink/SDOALink.h"
//#include "CHttpFCMInfo.h"
#endif

#if !defined( ENABLE_UI2)


//	구입후 시간동안 서버 응답을 기다립니다.
#define BUY_TIMEOUT			20

I3_CLASS_INSTANCE(CPopupRepair, CPopupBase);

CPopupRepair::CPopupRepair()
{
	m_ItemID		= 0;
	m_CanRepair		= FALSE;

	m_pGui				= NULL;
	m_pMainForm			= NULL;
	m_pMainCaption		= NULL;
	m_pButtonOk			= NULL;
	m_pButtonCancel		= NULL;

	m_pButtonCashCharge			= NULL;
	m_pButtonCashChargeMark		= NULL;
	m_pButtonCashChargeStatic	= NULL;

	m_pText				= NULL;

	m_pWeaponInfoFrame		= NULL;
	m_pWeaponInfoValue		= NULL;
	m_pWeaponInfoKey		= NULL;
	m_pWeaponInfoExtension	= NULL;
	m_pWeaponInfoImage = NULL;

	m_bConfirm	= FALSE;
	m_fTimeout	= 0.0f;
}

CPopupRepair::~CPopupRepair()
{
}

void CPopupRepair::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_SHOP_B_POPUP_REPAIR_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent )
		{
			if ( m_CanRepair == TRUE )
				SendBuyEvent();
			else
				OnClosePopup();
		}
		break;
	case GCI_SHOP_B_POPUP_REPAIR_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnClosePopup();
		break;
	case GCI_SHOP_B_POPUP_REPAIR_CASH_CHARGE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_InputCashCharge();
		break;
	}
}

BOOL CPopupRepair::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	LinkControlEx(plist, "ShopRepair", m_pGui, -1);
	LinkControlEx(plist, "shopRepairPopup", m_pMainForm, -1);
	LinkControlEx(plist, "shopRepairCaption", m_pMainCaption, -1);
	LinkControlEx(plist, "shopRepairOk", m_pButtonOk, GCI_SHOP_B_POPUP_REPAIR_OK);
	LinkControlEx(plist, "shopRepairCancel", m_pButtonCancel, GCI_SHOP_B_POPUP_REPAIR_CANCEL);

	LinkControlEx(plist, "shopRepairCharge", m_pButtonCashCharge, GCI_SHOP_B_POPUP_REPAIR_CASH_CHARGE);	
	LinkControlEx(plist, "shopRepairChargeMark", m_pButtonCashChargeMark, -1);
	LinkControlEx(plist, "shopRepairChargeStatic", m_pButtonCashChargeStatic, -1);
	
	LinkControlEx(plist, "shopRepairText", m_pText, -1);

	//	Buy weapon info
	LinkControlEx(plist, "shopRepairWeaponInfo", m_pWeaponInfoFrame, -1);
	LinkControlEx(plist, "shopRepairWeaponValue", m_pWeaponInfoValue, -1);
	LinkControlEx(plist, "shopRepairWeaponKey", m_pWeaponInfoKey, -1);
	LinkControlEx(plist, "shopRepairWeaponImage", m_pWeaponInfoImage, -1);
	LinkControlEx(plist, "shopRepairWeaponExtension", m_pWeaponInfoExtension, -1);

	return FALSE;
}

void CPopupRepair::OnInitControl(void)
{
	m_pGui->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pMainCaption, GAME_STRING("STR_POPUP_REPAIR_CAPTION"));

	INIT_CAPTION_BUTTON(m_pButtonOk, GAME_STRING("STBL_IDX_BUTTON_OK"));
	INIT_CAPTION_BUTTON(m_pButtonCancel, GAME_STRING("STBL_IDX_BUTTON_CANCEL"));

	INIT_BUTTON_EX(m_pButtonCashCharge);
	INIT_VALUE_CAPTION2(m_pButtonCashChargeStatic, GAME_STRING("STR_POPUP_CHARGE_CASH"));/*톨 충전*/

	m_pButtonCashCharge->SetEnable(FALSE);

	m_pButtonCashChargeMark->setInputDisable(TRUE);
	m_pButtonCashChargeStatic->setInputDisable(TRUE);

	m_pText->ReCreateTextEx(GetDefaultFontName(), 1024, GAME_FONT_DEFAULT_SIZE);
	m_pText->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	m_pWeaponInfoFrame->SetEnable(FALSE);
	m_pWeaponInfoFrame->SetAllShapeEnable(FALSE);
	m_pWeaponInfoFrame->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
	m_pWeaponInfoKey->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pWeaponInfoKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pWeaponInfoValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);	
	m_pWeaponInfoValue->SetTextColor(GetColor(GCT_DEFAULT));	
	m_pWeaponInfoExtension->SetEnable(FALSE);

	CPopupBase::OnInitControl();
}

void CPopupRepair::OnOpenPopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnOpenPopup( pArg1, pArg2);

	// 초기화.
    m_pGui->SetEnable(TRUE);
	m_bConfirm = FALSE;
	m_fTimeout = 0.0f;

	m_ItemID	= (INT32)pArg1;
	m_CanRepair	= FALSE;

	// 현재 내구도를 알아낸다.
	CShopContext * pShopContext = g_pFramework->GetShopContext();

	UINT32 Price;
	GOODS_PRICE_TYPE PriceType;
	if ( ! pShopContext->GetRepairPrice(m_ItemID, Price, PriceType) )
	{
		I3FATAL("ID : %d, Price : %d, PriceType : %d\n", m_ItemID, Price, PriceType);
		OnClosePopup();
		return;
	}



	const char * RepairPriceText = GAME_STRING("STR_POPUP_REPAIR_PRICE");

	const char * PriceTypeText = NULL;
	if( PriceType == GOODS_PRICE_CASH )		PriceTypeText = GAME_STRING("STR_SHOP_CASH");
	else									PriceTypeText = GAME_STRING("STBL_IDX_SHOP_MONEY");

	const char * NoticeText = "";
	char Text1[1024] = {};
	REPAIR_TYPE	Rv = pShopContext->CheckCanRepair(m_ItemID);
	switch ( Rv )
	{
	case REPAIR_TYPE_CAN_FULL:
		{
			StringCchPrintf(Text1, 1024, "%s : %d %s", RepairPriceText, Price, PriceTypeText);
			NoticeText = GAME_STRING("STR_POPUP_REPAIR_1");
			m_pButtonOk->SetEnable(TRUE);
			m_CanRepair = TRUE;
		}
		break;
	case REPAIR_TYPE_CAN_SOME:
		{
			StringCchPrintf(Text1, 1024, "{col:255,0,0,255}%s : %d %s(%s){/col}",
				RepairPriceText, Price, PriceTypeText, GAME_STRING("STR_POPUP_REPAIR_4"));
			NoticeText = GAME_STRING("STR_POPUP_REPAIR_2");
			m_pButtonOk->SetEnable(TRUE);
			m_CanRepair = TRUE;
		}
		break;
	case REPAIR_TYPE_CANNOT:
		{
			StringCchPrintf(Text1, 1024, "{col:255,0,0,255}%s : %d %s(%s){/col}",
				RepairPriceText, Price, PriceTypeText, GAME_STRING("STR_POPUP_REPAIR_4"));
			NoticeText = GAME_STRING("STR_POPUP_REPAIR_3");
			m_pButtonOk->SetEnable(FALSE);
			m_CanRepair = FALSE;
		}
		break;
	case REPAIR_TYPE_NEEDLESS:
		{
			StringCchCopy(Text1, 1024, GAME_STRING("STR_POPUP_REPAIR_NEEDLESS"));
			NoticeText = "";
			m_pButtonOk->SetEnable(TRUE);
			m_CanRepair = FALSE;
		}
		break;
	}


	char Text2[1024] = {};
	StringCchPrintf(Text2, 1024, "%s\n%s\n%s", Text1, NoticeText, GAME_STRING("STR_POPUP_REPAIR_TIP"));

	m_pText->SetText(Text2);


	m_pButtonCashCharge->SetEnable( g_pConfigEx->GetShop().Enable_ChargeCash );
}

void CPopupRepair::OnClosePopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnClosePopup();
	m_pGui->SetEnable(FALSE);
}

void CPopupRepair::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate( rDeltaSeconds);

	SetShopWeapon( m_ItemID, m_pWeaponInfoFrame, m_pWeaponInfoImage, m_pWeaponInfoKey, m_pWeaponInfoValue, m_pWeaponInfoExtension);

	if(! m_bConfirm)
		return;


	//	정해진 시간동안 서버 응답을 기다린다.
	if( m_fTimeout < BUY_TIMEOUT)
	{
		m_fTimeout += rDeltaSeconds;
	}
	else
	{
		//	네트워크 문제로 구입에 실패했으므로 에러 메시지를 보내고 게임을 강제 종료합니다.
		ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_SHOP_BUY_FAIL_E"), EXIT_CALLBACK(CbExitNormal), this);

		m_bConfirm = FALSE;
		m_fTimeout = 0.0f;

		//	팝업 종료
		OnClosePopup();
	}

}


void CPopupRepair::OnExitKey(void)
{
	OnClosePopup();
}


void CPopupRepair::OnGameEvent( INT32 event, INT32 arg)
{
	switch(event)
	{
	case EVENT_ITEM_REPAIR:
		i3GuiRoot::setGuiDisableInput(FALSE);

		m_bConfirm = FALSE;
		m_fTimeout = 0.0f;

		const char * pMsg = NULL;
		if( EV_SUCCESSED( arg ))
		{
			pMsg = GAME_STRING("STR_POPUP_REPAIR_SUCCESS");
		}
		else
		{
			pMsg = GAME_STRING("STR_POPUP_REPAIR_FAIL");
		}

		CAPTION_POPUP(pMsg, GAME_STRING("STR_TBL_GUI_READY_NOTICE"));
		//	팝업 종료
		OnClosePopup();
		break;
	}
}


void CPopupRepair::SendBuyEvent()
{
	//	서버에게 상품 구입 이벤트를 보냅니다.
	if ( ! g_pFramework->GetShopContext()->RepairItem(m_ItemID) )
	{
		CAPTION_POPUP(GAME_STRING("STR_POPUP_REPAIR_REQ_FAIL"), GAME_STRING("STR_TBL_GUI_READY_NOTICE"));
		return;
	}

	i3GuiRoot::setGuiDisableInput(TRUE);

	m_bConfirm = TRUE;
	m_fTimeout = 0.0f;
}

void CPopupRepair::_InputCashCharge(void)
{
	//	캐시 충전 웹페이지 호출
#if !defined(LOCALE_CHINA)
	::ExcuteCashWeb();
#else
	g_pSDOA_Link->OpenWidget();
#endif
}

#endif	// ENABLE_UI2

