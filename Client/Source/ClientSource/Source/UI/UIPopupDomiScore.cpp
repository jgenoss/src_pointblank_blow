#include "pch.h"
#include "UIPopupDomiScore.h"

#include "UIMainFrame.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"


//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupDomiScore_Confirm( LuaState * L)
	{
		g_pFramework->GetUIMainframe()->ClosePopup( UPW_DOMI_SCORE);
		return 0;
	}
}
LuaFuncReg UIPopupDomiScore_Glue[] =
{
	{"confirm",			UIPopupDomiScore_Confirm },
	{nullptr,				nullptr					}
};


I3_CLASS_INSTANCE( UIPopupDomiScore);

UIPopupDomiScore::UIPopupDomiScore()
{
}

UIPopupDomiScore::~UIPopupDomiScore()
{
}

void	UIPopupDomiScore::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);
}

void	UIPopupDomiScore::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);
}

/*virtual*/ void UIPopupDomiScore::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Domination/PBRe_DomiPopUp_score.i3UIs", UIPopupDomiScore_Glue);
}

/*virtual*/ bool UIPopupDomiScore::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	LuaState* L = _CallLuaFunction( 0, "refresh_score");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, Domination_Util::g_best_score);
		_EndLuaFunction(L, 1);
	}

	return true;
}

/*virtual*/ void UIPopupDomiScore::OnEntranceEnd()
{
	UIPopupBase::OnEntranceEnd();
}

/*virtual*/ bool UIPopupDomiScore::OnExitStart( void)
{
	return UIPopupBase::OnExitStart();
}

/*virtual*/ bool UIPopupDomiScore::OnKey_Enter()
{ 
	g_pFramework->GetUIMainframe()->ClosePopup( UPW_DOMI_SCORE);
	return true;
}

/*virtual*/ bool UIPopupDomiScore::OnKey_Escape()
{ 
	g_pFramework->GetUIMainframe()->ClosePopup( UPW_DOMI_SCORE);
	return true;
}