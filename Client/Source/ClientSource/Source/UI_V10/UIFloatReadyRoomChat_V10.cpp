#include "pch.h"
#include "UIFloatReadyRoomChat_V10.h"
#include "../UI/UIMainFrame_V10.h"

#include "ClientStageInfoUtil.h"

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
static UIFloatReadyRoomChat_V10 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	if( pFrame != nullptr)
		return (UIFloatReadyRoomChat_V10*) pFrame->GetFloating( UFW_READYROOM_CHAT);

	return nullptr;
}

extern "C" {

	int UIFloatReadyRoomChat_V10_SetChatType( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		GetThis()->SetChatTypeFromCombo( tab);
		return 0;
	}

	int UIFloatReadyRoomChat_V10_ChangeChatChannel( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		GetThis()->SelectChatChannel( tab);
		return 0;
	}

	int UIFloatReadyRoomChat_V10_ChangeLanguage( LuaState * L)
	{
		GetThis()->ToggleLang();
		return 0;
	}

	//일단 확성기에만 사용 합니다.
	int UIFloatReadyRoomChat_V10_ClickMegaPhoneOpen(LuaState * L)
	{
		GetThis()->ClickMegaPhoneOpenButton();
		return 0;
	}
}

LuaFuncReg UIFloatReadyRoomChat_V10_Glue[] = {
	{"SetChatType",				UIFloatReadyRoomChat_V10_SetChatType},	// Chatting type을 설정
	{"ChangeChatChannel",		UIFloatReadyRoomChat_V10_ChangeChatChannel		},
	{"ChangeLanguage",			UIFloatReadyRoomChat_V10_ChangeLanguage	},
	{ "ClickMegaPhoneOpen",		UIFloatReadyRoomChat_V10_ClickMegaPhoneOpen },
	{ nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIFloatReadyRoomChat_V10);

UIFloatReadyRoomChat_V10::UIFloatReadyRoomChat_V10()
{

}

UIFloatReadyRoomChat_V10::~UIFloatReadyRoomChat_V10()
{

}

void UIFloatReadyRoomChat_V10::OnCreate( i3GameNode * pParent )
{
	UIFloatingBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_ChattingWindow_ReadyRoom.i3UIs", UIFloatReadyRoomChat_V10_Glue, true, false);


	//m_pFrameResizer = UIResizer::new_object();
	//AddChild( m_pFrameResizer);

	_SearchLanguage();
	_ResetLanguage();

}

void UIFloatReadyRoomChat_V10::InitChat( CHAT_AREA area, INT32 nMaxLineCount )
{	
	LuaState * L;

	m_ChatArea = area;
	m_nMaxChatLineCount = nMaxLineCount;

	m_Tab[0] = CHAT_TYPE_ALL;
	m_Tab[1] = CHAT_TYPE_TEAM;
	m_Tab[2] = CHAT_TYPE_CLAN;
	m_Tab[3] = CHAT_TYPE_WHISPER;

	L = _CallLuaFunction( "InitTab");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, m_Tab[0]);
		i3Lua::PushInteger( L, m_Tab[1]);
		i3Lua::PushInteger( L, m_Tab[2]);
		i3Lua::PushInteger( L, m_Tab[3]);
		_EndLuaFunction( L, 4);
	}

	// Chatting combo를 설정
	INT32 cur = 0;
	switch( area)
	{
	case CHAT_AREA_CLAN_MATCH :		cur = 2;	break;
	case CHAT_AREA_CLAN_MEMBER :	cur = 2;	break;
	}
	_SetChattingCombo( cur);
}	
