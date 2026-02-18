#include "pch.h"
#include "UIPopupDomiEnter.h"

#include "UIMainFrame.h"
#include "MyRoomInfoContext.h"
#include "DomiContext.h"
#include "i3Base/string/ext/mb_to_utf16.h"


//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupDomiEnter_Confirm( LuaState * L)
	{
		g_pFramework->GetUIMainframe()->ClosePopup( UPW_DOMI_ENTER);
		return 0;
	}
}
LuaFuncReg UIPopupDomiEnter_Glue[] =
{
	{"confirm",			UIPopupDomiEnter_Confirm },
	{nullptr,				nullptr					}
};

I3_CLASS_INSTANCE( UIPopupDomiEnter);

UIPopupDomiEnter::UIPopupDomiEnter()
{
}

UIPopupDomiEnter::~UIPopupDomiEnter()
{
}

/*virtual*/ void UIPopupDomiEnter::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Domination/PBRe_Domi_Popup_Enter1.i3UIs", UIPopupDomiEnter_Glue);
}

void UIPopupDomiEnter::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);

	m_pNearEnd = (i3UIEditBox*) pScene->FindChildByName("i3UIEditBox0");

	m_pNearEnd->SetText( GAME_RCSTRING("STR_DOMI_GAME_NEAREND") );

	m_pLate = (i3UIEditBox*) pScene->FindChildByName("i3UIEditBox1");

	m_pLate->SetText( GAME_RCSTRING("STR_DOMI_GAME_LATE") );

}

/*virtual*/ bool UIPopupDomiEnter::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	i3::stack_wstring wstr;
	ROOM_INFO_BASIC roominfo;
	MyRoomInfoContext::i()->getMyRoomInfo(&roominfo);
	//UINT32 waveCnt = DomiContext::i()->getDomiWaveCount();

	
	//sprintf_s(str, GAME_RCSTRING("STR_DOMI_GAME_ING"), roominfo._i16CurRound, waveCnt, waveCnt);

	LuaState * L = _CallLuaFunction("setEditText");
	i3Lua::PushStringUTF16To8(L, wstr);
	_EndLuaFunction(L, 1);
	return true;
}

/*virtual*/ void UIPopupDomiEnter::OnEntranceEnd()
{
	UIPopupBase::OnEntranceEnd();
}

/*virtual*/ bool UIPopupDomiEnter::OnExitStart( void)
{
	return UIPopupBase::OnExitStart();
}

/*virtual*/ bool UIPopupDomiEnter::OnKey_Enter()
{ 
	g_pFramework->GetUIMainframe()->ClosePopup( UPW_DOMI_ENTER);
	return true;
}

/*virtual*/ bool UIPopupDomiEnter::OnKey_Escape()
{ 
	g_pFramework->GetUIMainframe()->ClosePopup( UPW_DOMI_ENTER);
	return true;
}

/*virtual*/ void UIPopupDomiEnter::ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	UIPopupBase::ProcessGameEvent(event, Arg, UserData);

	switch( event )
	{
	case EVENT_CHANGE_READYROOM_INFO:

		break;
	}
}