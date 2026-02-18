#include "pch.h"
#include "UIPopupRouletteAuto.h"

#include "UIMainFrame.h"
#include "UIPhaseLobby.h"
#include "UI/UIUtil.h"

#include "../GuiNotifyReceiver.h"
#include "../TextSet.h"

I3_CLASS_INSTANCE( UIPopupRoulettAuto);//, UIPopupBase);

UIPopupRoulettAuto::UIPopupRoulettAuto()
{

	m_nIndex = 0;
}

UIPopupRoulettAuto::~UIPopupRoulettAuto()
{
}

void UIPopupRoulettAuto::_ModifyControl( void)
{

}

/*virtual*/ void UIPopupRoulettAuto::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	AddScene( "UIRe/Scene/Popup/PBRe_PopUp_AutoAdvance.i3UIe");
}

/*virtual*/ bool UIPopupRoulettAuto::OnKey_Enter( void)
{ 
	ConfirmOK();
	return true;
}

/*virtual*/ bool UIPopupRoulettAuto::OnKey_Escape( void)
{
	ConfirmOK();
	return true;
}

/*virtual*/ bool UIPopupRoulettAuto::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	_ModifyControl();
	return true;
}
/*virtual*/ BOOL UIPopupRoulettAuto::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{

	UIPopupBase::OnEvent(event,pObj,param2,code);
	

	//// ÆÀ¹ë·±½º
	{
		LuaState * L = _CallLuaFunction( "getAutoRoulette");
		m_nIndex = static_cast<UINT8>(_ReturnLuaInteger(L, 0));
	}


	LuaState * L = _CallLuaFunction(0, "setRouletteCash");
	char szTemp[ MAX_STRING_COUNT];


	INT32 TapIndex = g_pGameContext->getTapIndex();
	INT32 MaxPrice = (g_pGameContext->GetRSInfo()->_aRSTabInfo[TapIndex]._ui32Price) *m_nIndex;

	//¹«Á¦ÇÑ
	if (m_nIndex == 100)
	{
		INT32 nCount = g_pGameContext->GetMyCash() / (g_pGameContext->GetRSInfo()->_aRSTabInfo[TapIndex]._ui32Price);
		MaxPrice =  (g_pGameContext->GetRSInfo()->_aRSTabInfo[TapIndex]._ui32Price) * nCount;
	}

	if( L != NULL)
	{
		sprintf_s( szTemp, " %s = %d", GAME_STRING("STR_SHOP_INFO_CASH") ,MaxPrice);
		i3Lua::PushA2UTF8String( L, szTemp);
		_EndLuaFunction( L, 1);
	}


		return UIBase::OnEvent( event, pObj, param2, code);

}

/*virtual*/ bool UIPopupRoulettAuto::OnCloseButton( void)
{
	return UIPopupBase::OnCloseButton( );
}


/*virtual*/ bool UIPopupRoulettAuto::OnOKButton( void)
{


	ConfirmOK();

	//// ÆÀ¹ë·±½º
	{
		LuaState * L = _CallLuaFunction( "getAutoRoulette");
		m_nIndex = static_cast<UINT8>(_ReturnLuaInteger(L, 0));
	}

	INT32 TapIndex = g_pGameContext->getTapIndex();

	UINT32 MaxPrice = (g_pGameContext->GetRSInfo()->_aRSTabInfo[TapIndex]._ui32Price) *m_nIndex;
	
	if (g_pGameContext->GetMyCash() < MaxPrice)
	{
		GameUI::MsgBox( MSG_TYPE_ERROR, "Ä³½¬°¡ ºÎÁ·ÇÕ´Ï´Ù.");
		return false;
	}


VM_START
	g_pGameContext->SetAutoCount(m_nIndex);
	g_pGameContext->SetCheckAuto(true);
VM_END


	return true;
}

void UIPopupRoulettAuto::ConfirmOK(void)
{
	g_pFramework->GetUIMainframe()->TogglePopup( UPW_ROULETTE_AUTO);
}

/*virtual*/ void UIPopupRoulettAuto::OnLoadAllScenes()
{
	
}

/*virtual*/ void UIPopupRoulettAuto::OnUnloadAllScenes()
{
	
}
/*
void UIPopupRoulettAuto::_SetCondition( void)
{
	LuaState * L;

	L = _CallLuaFunction( "SetConditions");
		if( L != NULL)
		{
			i3Lua::PushInteger( L, m_nIndex);
			_EndLuaFunction( L, 1);
		}
	
}*/