#include "pch.h"

#include "UIMainFrame.h"
#include "UIBattleFrame.h"
#include "BattleGui.h"

#include "UIOptionCommunity.h"
#include "UIUtil.h"

#include "EnvInfoContext.h"
#include "TwitchTVContext.h"

I3_CLASS_INSTANCE( UIOptionCommunity);//, UIOptionBase );

namespace
{
	UIOptionCommunity* g_this = 0;
	bool g_show = false;
	enum	{ WHISPER, INVITE, ALT1, ALT2, ALT3, ALT4, ALT5, TWITCH };
}

extern "C" 
{
	int option_community_click( LuaState * L)
	{
		INT32 value1 = i3Lua::GetIntegerArg( L, 1);
		INT32 value2 = i3Lua::GetIntegerArg( L, 2);
		g_this->clicked( value1, value2 );
		return 0;
	}
}

LuaFuncReg option_community_glue[] =
{
	{"Click",		option_community_click},
	{nullptr,				nullptr}
};

/************************//************************/
void UIOptionCommunity::clicked(int idx1, int idx2)
{
	switch( idx1 )
	{
	case WHISPER: m_nWhisperType = idx2;		set_lua();	break;
	case INVITE: m_nInviteType = idx2;			set_lua();	break;
	case ALT1: case ALT2: case ALT3: case ALT4: case ALT5: 
		m_Macro[idx1 - ALT1].type = idx2 != 0;		set_lua();	break;
	case TWITCH:
		if(!g_pFramework->IsBattleStage())
		{
			if(TwitchTVContext::i()->isTwitchLogin())
				GetMainFrame()->OpenPopup(UPW_TWITCHOPTION);
			else
				GetMainFrame()->OpenPopup(UPW_TWITCHLOGIN);
		}
		else
		{
			if(TwitchTVContext::i()->isTwitchLogin())
			{
				UIBattleFrame::i()->OpenPopup(UBP_TWITCHOPTION);
			}
				
		}
		break;
	}
}
/************************//************************/

UIOptionCommunity::UIOptionCommunity() : UIOptionBase()
{
	g_this = this;
}

UIOptionCommunity::~UIOptionCommunity()
{
}

/************************//************************/
void UIOptionCommunity::_InitializeAtLoad( i3UIScene * pScene )
{
	m_Macro[0].edit.SetControl(pScene, "i3UIEditBox4" );
	m_Macro[1].edit.SetControl(pScene, "i3UIEditBox1" );
	m_Macro[2].edit.SetControl(pScene, "i3UIEditBox6" );
	m_Macro[3].edit.SetControl(pScene, "i3UIEditBox7" );
	m_Macro[4].edit.SetControl(pScene, "i3UIEditBox" );

	m_pTwitchButton = (i3UIButton *) pScene->FindChildByName( "btnTwitch");
	if(g_pFramework->IsBattleStage() && !TwitchTVContext::i()->isTwitchLogin())
	{
		m_pTwitchButton->addState(I3UI_STATE_DEACTIVATED);
		m_pTwitchButton->setInputDisable(true);
		m_pTwitchButton->setModifyState();
	}
}

/*virtual*/ void UIOptionCommunity::OnCreate( i3GameNode * pParent)
{
	UIOptionBase::OnCreate( pParent);

	AddScene("Scene/Option/PBRe_Pref_Community.i3UIs", option_community_glue);
}

void UIOptionCommunity::SetTwitchButton()
{
	LuaState * L = 0;

	L = _CallLuaFunction( "SetTwichButton");
	if( L != nullptr)
	{
		//트위치에 로그인이 되어있다면 항상 방송 설정 버튼으로.
		if(TwitchTVContext::i()->isTwitchLogin())
			i3Lua::PushBoolean( L, true);	
		else
			i3Lua::PushBoolean( L, false);	

		UILua::EndLuaFunction(L , 1);
	}
}

void UIOptionCommunity::entrance_scn()
{
	OnEntranceStart();
	g_show = true;

	_setTwitchVisible();
}

void UIOptionCommunity::show_scn()
{
	SetVisible(true);
	GetScene(0)->Attach(false);
	g_show = true;

	// 인게임에서는 Show Hide로 관리. 위에서 Visible을 켜기 때문에
	// 다시 Twitch를 꺼야 합니다.
	_setTwitchVisible();
}

void UIOptionCommunity::hide_scn()
{
	for(int i=0; i<MAX_MACRO;++i)
	{
		i3::const_wchar_range rng = m_Macro[i].edit.ExportText();
		m_Macro[i].wstr_word.assign(rng.begin(), rng.end()); 
	}

	SetVisible(false);
	g_show = false;
}

void UIOptionCommunity::exit_scn()
{
	for(int i=0; i<MAX_MACRO;++i)
	{
		i3::const_wchar_range rng = m_Macro[i].edit.ExportText();
		m_Macro[i].wstr_word.assign(rng.begin(), rng.end()); 
	}

	OnExitStart();
	g_show = false;
}

bool UIOptionCommunity::OnKey_Escape()
{
	for(int i=0; i<MAX_MACRO;++i)
	{
		if( m_Macro[i].edit.OnKey_Escape() )
		{
			i3::const_wchar_range rng = m_Macro[i].edit.ExportText();
			m_Macro[i].wstr_word.assign(rng.begin(), rng.end()); 
		}
	}

	return 0;
}

/************************//************************/
void UIOptionCommunity::load_value()
{
	m_nWhisperType = g_pEnvSet->m_nWhisperType;
	m_nInviteType = g_pEnvSet->m_nInviteType;

	// Macro관련 변수들
	for(int i=0; i<MAX_MACRO; i++)
	{
		m_Macro[i].wstr_word = g_pEnvSet->m_wstrMacro[i];
		m_Macro[i].type = g_pEnvSet->m_bMacroEnableFriend[i];
	}
}

void UIOptionCommunity::save_value()
{
	if( is_modified_basic() )
	{
		g_pEnvSet->m_nWhisperType = m_nWhisperType;
		g_pEnvSet->m_nInviteType = m_nInviteType;
		EnvInfoContext::i()->AddNetEnvSetBasicChange(BASIC);
	}

	//Macro 변경 유무 체크
	if( !is_modified_macro() ) return;

	for(int i=0; i<MAX_MACRO; i++)
	{
		GameUI::RemoveColorKeyword(m_Macro[i].wstr_word);

		if(  EnvInfoContext::i()->GetMacro(i) != m_Macro[i].wstr_word )
		{
			// GameContext에 변화되어진 Macro의 Index와 셋팅을 알려줍니다.
			UINT32 NetEnvSetBasicChange = 0;

			NetEnvSetBasicChange |= 0x0100 << i;
			NetEnvSetBasicChange |= MACRO;

			EnvInfoContext::i()->AddNetEnvSetBasicChange( NetEnvSetBasicChange);

			g_pEnvSet->m_wstrMacro[i] = m_Macro[i].wstr_word;
		}

		if( g_pEnvSet->m_bMacroEnableFriend[i] != m_Macro[i].type)
		{
			EnvInfoContext::i()->AddNetEnvSetBasicChange( BASIC );
			g_pEnvSet->m_bMacroEnableFriend[i] = m_Macro[i].type;
		}
	}
}

void UIOptionCommunity::default_value()
{
	m_nWhisperType = 0;
	m_nInviteType = 0;

	for(int i=0; i<MAX_MACRO; i++)
	{
		m_Macro[i].wstr_word.clear();
		m_Macro[i].type = 1;
		m_Macro[i].edit.SetText(L"");
	}
}

void UIOptionCommunity::apply_game()
{
}

void UIOptionCommunity::apply_lua()
{
	LuaState * L = 0;

	L = _CallLuaFunction( "SetValue");
	i3Lua::PushInteger( L, m_nWhisperType);
	i3Lua::PushInteger( L, m_nInviteType);
	for(int i=0; i<MAX_MACRO; ++i)
		i3Lua::PushInteger( L, m_Macro[i].type);
	UILua::EndLuaFunction(L , 7);

	for(int i=0; i<MAX_MACRO; i++)
	{
		m_Macro[i].edit.SetText( m_Macro[i].wstr_word );
	}
}

void UIOptionCommunity::set_lua()
{
	LuaState * L = 0;

	L = _CallLuaFunction( "SetValue");
	i3Lua::PushInteger( L, m_nWhisperType);
	i3Lua::PushInteger( L, m_nInviteType);
	for(int i=0; i<MAX_MACRO; ++i)
		i3Lua::PushInteger( L, m_Macro[i].type);
	UILua::EndLuaFunction(L , 7);
}

bool UIOptionCommunity::is_modified()
{
	if(g_show)
	{
		for(int i=0; i<MAX_MACRO;++i)
		{
			const i3::const_wchar_range rng = m_Macro[i].edit.ExportText();
			m_Macro[i].wstr_word.assign(rng.begin(), rng.end() );
		}
	}

	if( is_modified_basic() == true ) return true;
	if( is_modified_macro() == true ) return true;
	return false;
}
bool UIOptionCommunity::is_modified_basic()
{
	if( m_nWhisperType != g_pEnvSet->m_nWhisperType )		return true;
	if( m_nInviteType != g_pEnvSet->m_nInviteType )		return true;
	return false;
}
bool UIOptionCommunity::is_modified_macro()
{
	for(int i=0; i<MAX_MACRO; ++i)
	{
		if( m_Macro[i].type != g_pEnvSet->m_bMacroEnableFriend[i] )		return true;
		GameUI::RemoveColorKeyword(m_Macro[i].wstr_word);
		if( EnvInfoContext::i()->GetMacro(i) != m_Macro[i].wstr_word ) return true;
	}
	return false;
}

void UIOptionCommunity::_setTwitchVisible()
{
	LuaState * L = 0;
	L = _CallLuaFunction( "SetTwichVisible");
	if( L != nullptr)
	{		
		if(LocaleValue::GetInt32("EnableTwitch"))
			i3Lua::PushBoolean( L, true);	
		else
			i3Lua::PushBoolean( L, false);	

		UILua::EndLuaFunction(L , 1);
	}

	SetTwitchButton();
}