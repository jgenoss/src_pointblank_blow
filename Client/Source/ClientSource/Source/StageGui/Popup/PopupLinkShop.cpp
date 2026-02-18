#include "pch.h"
#include "PopupLinkShop.h"



//////////////////////////////////////////////////////////////////////////

I3_CLASS_INSTANCE(CPopupLinkShop, CPopupBase);


CPopupLinkShop::CPopupLinkShop( void )
{	

	m_pCheckPrimaryWeaponMessage = NULL;
	m_pBuyPrimaryWeaponMessage= NULL;
	m_pSelectLinkShopMessage= NULL;

	m_pCheckPrimaryWeapon =NULL;
	m_pBuyPrimaryWeapon =NULL;
	m_pSelectLinkShop =NULL;

	m_pBuyPrimaryWeaponCaption = NULL;
	m_pBuyPrimaryWeaponConfirm = NULL;
	m_pCheckPrimaryWeaponCaption = NULL;
	m_pCheckPrimaryWeaponConfirm = NULL;

	m_pSelectLinkShopCancle = NULL;
	m_pSelectLinkShopCaption = NULL;
	m_pSelectLinkShopConfirm = NULL;
}

CPopupLinkShop::~CPopupLinkShop()
{	
	

}

void CPopupLinkShop::OnGuiNotify( I3GUI_CONTROL_NOTIFY* pNotify )
{

	switch(pNotify->m_nID)
	{
	case GCI_POP_CHK_PRI_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_CheckPrimaryWeaponConfirm();
		break;

	case GCI_POP_BUY_PRI_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnClosePopup();
		break;

	case GCI_POP_SEL_CON_PRI_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_SelectLinkShopConfirm();
		break;
	case GCI_POP_SEL_CAN_PRI_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnClosePopup();
		break;
	}
	
}

BOOL CPopupLinkShop::OnLinkControl( i3BinList * plist, i3GameNode * pControl )
{


	LinkControlEx(plist, "CheckPrimaryWeapon", m_pCheckPrimaryWeapon, -1);
	LinkControlEx(plist, "BuyPrimaryWeapon", m_pBuyPrimaryWeapon, -1);
	LinkControlEx(plist, "SelectLinkShop", m_pSelectLinkShop, -1);


	LinkControlEx(plist, "CheckPrimaryWeaponMessage", m_pCheckPrimaryWeaponMessage, -1);
	LinkControlEx(plist, "BuyPrimaryWeaponMessage", m_pBuyPrimaryWeaponMessage, -1);
	LinkControlEx(plist, "SelectLinkShopMessage", m_pSelectLinkShopMessage, -1);
	LinkControlEx(plist, "CheckPrimaryWeaponCaption", m_pCheckPrimaryWeaponCaption, -1);
	LinkControlEx(plist, "BuyPrimaryWeaponCaption", m_pBuyPrimaryWeaponCaption, -1);
	LinkControlEx(plist, "SelectLinkShopCaption", m_pSelectLinkShopCaption, -1);





	LinkControlEx(plist, "BuyPrimaryWeaponConfirm", m_pBuyPrimaryWeaponConfirm, GCI_POP_BUY_PRI_WEAPON);
	LinkControlEx(plist, "CheckPrimaryWeaponConfirm", m_pCheckPrimaryWeaponConfirm,GCI_POP_CHK_PRI_WEAPON);
	LinkControlEx(plist, "SelectLinkShopConfirm", m_pSelectLinkShopConfirm, GCI_POP_SEL_CON_PRI_WEAPON);
	LinkControlEx(plist, "SelectLinkShopCancle", m_pSelectLinkShopCancle, GCI_POP_SEL_CAN_PRI_WEAPON);


	return FALSE;
}

void CPopupLinkShop::OnInitControl( void )
{
	
	INIT_CAPTION_BUTTON(m_pBuyPrimaryWeaponConfirm, GAME_STRING("STR_TBL_GUI_LOBBY_CONFIRM_TEXT"));
	INIT_CAPTION_BUTTON(m_pCheckPrimaryWeaponConfirm, GAME_STRING("STR_TBL_GUI_LOBBY_CONFIRM_TEXT"));
	INIT_CAPTION_BUTTON(m_pSelectLinkShopConfirm, GAME_STRING("STR_TBL_GUI_LOBBY_CONFIRM_TEXT"));
	INIT_CAPTION_BUTTON(m_pSelectLinkShopCancle, GAME_STRING("STR_TBL_GUI_LOBBY_CANCEL_TEXT"));


	m_pCheckPrimaryWeaponMessage->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCheckPrimaryWeaponMessage->SetText(GAME_STRING("STR_POPUP_LINK_SHOP_CHECK_PRIMARY_WEAPON"));
	m_pBuyPrimaryWeaponMessage->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
	m_pBuyPrimaryWeaponMessage->SetText( GAME_STRING("STR_POPUP_LINK_SHOP_BUY_PRIMARY_WEAPON"));
	m_pSelectLinkShopMessage->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSelectLinkShopMessage->SetText( GAME_STRING("STR_POPUP_LINK_SHOP_SELECT_PRIMARY_WEAPON"));

	m_pCheckPrimaryWeaponCaption->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCheckPrimaryWeaponCaption->SetText(GAME_STRING("STR_TBL_GLOBALDEF_GAME_ERROR_TEXT2"));
	m_pBuyPrimaryWeaponCaption->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
	m_pBuyPrimaryWeaponCaption->SetText(GAME_STRING("STR_TBL_GLOBALDEF_GAME_ERROR_TEXT2"));
	m_pSelectLinkShopCaption->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSelectLinkShopCaption->SetText(GAME_STRING("STR_TBL_GLOBALDEF_GAME_ERROR_TEXT2"));

	m_pCheckPrimaryWeapon->SetEnable(FALSE);
	m_pSelectLinkShop->SetEnable(FALSE);
	m_pBuyPrimaryWeapon->SetEnable(FALSE);
	
	CPopupBase::OnInitControl();
}

void CPopupLinkShop::OnOpenPopup( void * pArg1, void * pArg2  )
{
	
	CPopupBase::OnOpenPopup( pArg1, pArg2);
	INT32 PopupIdx  = static_cast<STAGE_TYPE>((INT32)pArg1);
	switch(PopupIdx)
	{

	case 0 :
			
		m_pCheckPrimaryWeapon->SetEnable(TRUE);

		break;


	case 1:
	
		m_pSelectLinkShop->SetEnable(TRUE);

		break;
	case 2:
		
		m_pBuyPrimaryWeapon->SetEnable(TRUE);
		break;
	}

}
void CPopupLinkShop::_BuyPrimaryWeaponConfirm()
{
	m_pBuyPrimaryWeapon->SetEnable(FALSE);
}

void CPopupLinkShop::_SelectLinkShopConfirm()
{
	m_pSelectLinkShop->SetEnable(FALSE);

	if (((CStageReady*)m_pParent)->IsEnterShop())
	{
		CStageReady * pParent = ((CStageReady*)m_pParent);
		I3ASSERT( pParent->IsTypeOf( CStageReady::GetClassMeta()));

		if( g_pFramework->GetClanContext()->InBattleTeam())
		{
			if( g_pFramework->GetClanContext()->GetInClanBattleLobby())
				pParent->ChangeStep(STEP_CLAN_BATTLE_LOBBY);		//	클랜전 로비 예외 처리
			else
				pParent->ChangeStep(STEP_SHOP);
		}
		else
		{
			pParent->ChangeStep(STEP_SHOP);
		}


	}
	else
	{
		if (FALSE == i3GuiRoot::getGuiDisableInput())
		{
			((CStageReady*)m_pParent)->ExclusedEnter(STEP_SHOP);
			g_pFramework->GetShopContext()->EnterServer();	
		}
	}

}

void CPopupLinkShop::_SelectLinkShopCancle()
{
	
	m_pSelectLinkShop->SetEnable(FALSE);

}
void CPopupLinkShop::_CheckPrimaryWeaponConfirm()
{
	m_pCheckPrimaryWeapon->SetEnable(FALSE);

	if (((CStageReady*)m_pParent)->IsEnterShop())
	{
		CStageReady * pParent = ((CStageReady*)m_pParent);
		I3ASSERT( pParent->IsTypeOf( CStageReady::GetClassMeta()));

		if( g_pFramework->GetClanContext()->InBattleTeam())
		{
			if( g_pFramework->GetClanContext()->GetInClanBattleLobby())
				pParent->ChangeStep(STEP_CLAN_BATTLE_LOBBY);		//	클랜전 로비 예외 처리
			else
				pParent->ChangeStep(STEP_SHOP);
		}
		else
		{
			pParent->ChangeStep(STEP_SHOP);
		}
		((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);	
			
	}
	else
	{
		if (FALSE == i3GuiRoot::getGuiDisableInput())
		{
			((CStageReady*)m_pParent)->ExclusedEnter(STEP_SHOP);
			g_pFramework->GetShopContext()->EnterServer();	
		
		}
	}
	//g_pGameContext->AddGameModeFlag( GAMEMODEFLAG_LINKSHOPMSG);
	
}

void CPopupLinkShop::OnClosePopup( void * pArg1, void * pArg2 )
{
	
		CPopupBase::OnClosePopup();
		m_pBuyPrimaryWeapon->SetEnable(FALSE);
		m_pSelectLinkShop->SetEnable(FALSE);

	
}
void CPopupLinkShop::OnExitKey( void )
{
	
	OnClosePopup();
}


BOOL CPopupLinkShop::OnEvent( UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code )
{
	
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{

			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

			if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
			{
				if (m_pSelectLinkShop->isEnable())
				{
					
					OnClosePopup();
					m_pSelectLinkShop->SetEnable(FALSE);
				}
				if(m_pCheckPrimaryWeaponMessage->isEnable())
				{
					_CheckPrimaryWeaponConfirm();

				}
			}
			if (pKeyboard->GetStrokeState(I3I_KEY_ENTER))
			{
				if (m_pSelectLinkShop->isEnable())
				{
					return TRUE;
				}
			}
			
			
		}
	}
	

	return CPopupBase::OnEvent(event, pObj, param2, code);
}
