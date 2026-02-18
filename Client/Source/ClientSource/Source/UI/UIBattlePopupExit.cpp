#include "pch.h"
#include "UIBattlePopupExit.h"
#include "UIBattleFrame.h"

#include "GameExitPopup.h"
#include "BattleGui.h"

#include "UIUtil.h"
#include "MyRoomInfoContext.h"

I3_CLASS_INSTANCE( UIBattlePopupExit);//, UIBattlePopopBase);

extern "C" {
	
	int UIBattlePopup_Exit_Option(LuaState* L)
	{
		UIBattlePopupExit * pThis = static_cast<UIBattlePopupExit*>(GameUI::GetBattlePopup(UBP_EXIT));
		if( pThis != nullptr)
			pThis->ClickOption();
		
		return 0;
	}

	int	UIBattlePopup_Exit_Report( LuaState * L)
	{
		UIBattlePopupExit * pThis = static_cast<UIBattlePopupExit*>(GameUI::GetBattlePopup(UBP_EXIT));
		if( pThis != nullptr)
			pThis->ClickReport();

		return 0;
	}
	
	int BattlePopup_Exit_LeaveBattle(LuaState * L)
	{
		UIBattlePopupExit * pThis = static_cast<UIBattlePopupExit*>(GameUI::GetBattlePopup(UBP_EXIT));
		if( pThis != nullptr)
			pThis->ClickLeaveBattle();

		return 0;
	}
		
}

LuaFuncReg UIBattlePopupExit_Glue[] =
{
	{	"ClickOption",			UIBattlePopup_Exit_Option },
	{   "ClickReport",			UIBattlePopup_Exit_Report },
	{	"ClickLeaveBattle",		BattlePopup_Exit_LeaveBattle},
	{	nullptr,			nullptr}
};

/*virtual*/ void UIBattlePopupExit::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene("Scene/Popup/PBRe_PopUp_ESC.i3UIs", UIBattlePopupExit_Glue, true, false);

}

void UIBattlePopupExit::OnLoadAllScenes()
{
	UIBattlePopupBase::OnLoadAllScenes();

	LuaState * L = _CallLuaFunction("RemoveOnMouse");
	UILua::EndLuaFunction(L , 0);

	GameUI::SetMouseCenterPosition();

	//신고하기
	bool isTutorial = MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL);
	bool bReportEnable = LocaleValue::Enable("EnableCallReport") == true ? true : false;

	if( isTutorial == true || bReportEnable == false)
	{
		i3UIButton* btn = (i3UIButton*)GetScene(0)->FindChildByName( "i3UIBtn_Report" );
		btn->addState(I3UI_STATE_DEACTIVATED);
		btn->setInputDisable(true);
		btn->setModifyState();
	}
}

/*virtual*/ bool UIBattlePopupExit::OnKey_Enter( void)
{
	ClickLeaveBattle();
	return false;
}


void UIBattlePopupExit::ClickOption( void)
{
	CBattleGui::i()->SetEnablePopup();
	GetUIBattleFrame()->OpenPopup( UBP_OPTION);
}

void UIBattlePopupExit::ClickReport( void)
{
	CBattleGui::i()->SetEnablePopup();
	CRIME_UI_TYPE type = CRIME_UI_UserReport;
	GetUIBattleFrame()->OpenPopup( UBP_ACCUSE_CRIME, &type);
}

void UIBattlePopupExit::ClickLeaveBattle( void)
{
	CBattleGui::i()->SetEnablePopup();
	gexit_popup::i()->open();
}

void UIBattlePopupExit::AttachToTopScreen_ForceRemove()
{
	_AttachToTopScreen();
}