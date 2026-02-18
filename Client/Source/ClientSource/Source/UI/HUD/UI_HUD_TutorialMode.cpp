#include "pch.h"
#include "UI_HUD_TutorialMode.h"
#include "StageBattle.h"
#include "GameStateMgr.h"

I3_CLASS_INSTANCE( UI_HUD_Tutorial);//, UIBase);



extern "C" {
	INT32 UI_HUD_Tutorial_GameResutlEnter( lua_State * L)
	{

		gstatemgr::i()->setEndTime(100.f);

		return 0;
	}
}

LuaFuncReg UI_HUD_Tutorial_Glue[] = 
{
	{"GameResultEnter",				UI_HUD_Tutorial_GameResutlEnter},		// 결과창 확인
	{nullptr, nullptr}
};

UI_HUD_Tutorial::UI_HUD_Tutorial()
{
	for(INT32 i=0; i<THT_COUNT; i++)
	{
		m_pSceneHUD_Tutorial[i] = nullptr;
	}
}

UI_HUD_Tutorial::~UI_HUD_Tutorial()
{
	//OffAllTutorialHUD();

	//for(INT32 i=0; i<THT_COUNT; i++)
	{
		//I3_SAFE_RELEASE(m_pSceneHUD_Tutorial[i]);
	}
}

static void RecSetFontToCaption( i3GameNode * pNode)
{
	i3GameNode * pChild = pNode->getFirstChild();

	while( pChild)
	{
		if( i3::kind_of<i3UICaptionControl*>(pChild))
		{
			((i3UICaptionControl*)pChild)->SetFont( GetDefaultLocaleFontName(), 10);
		}

		RecSetFontToCaption( pChild);

		pChild = pChild->getNext();
	}

}

/*virtual*/ void UI_HUD_Tutorial::OnCreate( i3GameNode * pParent)
{
	UIBase::OnCreate( pParent);

	// Load Scene
	
	m_pSceneHUD_Tutorial[THT_KEY_MOVE] = AddScene( "Scene/Tutorial/PointBlankRe_Tutorial_Key_Move.i3UIs", nullptr, true);
	m_pSceneHUD_Tutorial[THT_KEY_MOVE]->EnableScene(false);

	m_pSceneHUD_Tutorial[THT_KEY_JUMP] = AddScene( "Scene/Tutorial/PointBlankRe_Tutorial_Key_Jump.i3UIs", nullptr, true);
	m_pSceneHUD_Tutorial[THT_KEY_JUMP]->EnableScene(false);

	m_pSceneHUD_Tutorial[THT_KEY_SHOOT] = AddScene( "Scene/Tutorial/PointBlankRe_Tutorial_Key_Shooting.i3UIs", nullptr, true);
	m_pSceneHUD_Tutorial[THT_KEY_SHOOT]->EnableScene(false);

	m_pSceneHUD_Tutorial[THT_KEY_EXT] = AddScene( "Scene/Tutorial/PointBlankRe_Tutorial_Key_Extension.i3UIs", nullptr, true);
	m_pSceneHUD_Tutorial[THT_KEY_EXT]->EnableScene(false);

	m_pSceneHUD_Tutorial[THT_KEY_GRENADE_LAUNCHER] = AddScene( "Scene/Tutorial/PointBlankRe_Tutorial_Key_Grenade.i3UIs", nullptr, true);
	m_pSceneHUD_Tutorial[THT_KEY_GRENADE_LAUNCHER]->EnableScene(false);

	m_pSceneHUD_Tutorial[THT_KEY_GRENADE_SMOKE] = AddScene( "Scene/Tutorial/PointBlankRe_Tutorial_Key_Thrown.i3UIs", nullptr, true);
	m_pSceneHUD_Tutorial[THT_KEY_GRENADE_SMOKE]->EnableScene(false);

	m_pSceneHUD_Tutorial[THT_KEY_C4] = AddScene( "Scene/Tutorial/PointBlankRe_Tutorial_Key_C4.i3UIs", nullptr, true);
	m_pSceneHUD_Tutorial[THT_KEY_C4]->EnableScene(false);

	m_pSceneHUD_Tutorial[THT_KEY_MELEE] = AddScene( "Scene/Tutorial/PointBlankRe_Tutorial_Key_Melee.i3UIs", nullptr, true);
	m_pSceneHUD_Tutorial[THT_KEY_MELEE]->EnableScene(false);

	m_pSceneHUD_Tutorial[THT_KEY_HELICOPTER] = AddScene( "Scene/Tutorial/PointBlankRe_Tutorial_Key_Helicopter.i3UIs", nullptr, true);
	m_pSceneHUD_Tutorial[THT_KEY_HELICOPTER]->EnableScene(false);

	m_pSceneHUD_Tutorial[THT_MISSION_START_CLEAR] = AddScene( "Scene/Tutorial/PointBlankRe_Tutorial_MissionStartClear.i3UIs", nullptr, true);
	m_pSceneHUD_Tutorial[THT_MISSION_START_CLEAR]->EnableScene(true);

	SetInputDisable(true);

	i3UIControl* pControl = (i3UIControl*)m_pSceneHUD_Tutorial[THT_KEY_MOVE]->getFirstChild();

	while( pControl != nullptr)
	{
		//_RecEnableScene(pControl, bEnable);
		pControl->addStyle(I3UI_STYLE_SIZE_RATIO);

		pControl = (i3UIControl*)pControl->getNext();
	}

	m_pSceneHUD_Tutorial[THT_GAME_RESULT] = AddScene( "Scene/Tutorial/PointBlankRe_Tutorial_GameResult.i3UIs", UI_HUD_Tutorial_Glue, true);
	m_pSceneHUD_Tutorial[THT_GAME_RESULT]->EnableScene(false);

	INT32 i;
	for( i = 0; i < THT_COUNT; i++)
	{
		RecSetFontToCaption( m_pSceneHUD_Tutorial[i]);
	}	

	GlobalFunc::PB_TRACE("SetInputDisable");

	LuaState* L = _CallLuaFunction("PointBlankRe_Tutorial_MissionStartClear", "OffAllHud");
	_EndLuaFunction( L, 0);

	L = _CallLuaFunction("PointBlankRe_Tutorial_GameResult", "SetEnableInput");
	_EndLuaFunction( L, 0);

}

void UI_HUD_Tutorial::OffAllTutorialHUD() 
{
	for(INT32 i=0; i<THT_COUNT; i++)
	{
		if(m_pSceneHUD_Tutorial[i] != nullptr)
		{
			m_pSceneHUD_Tutorial[i]->EnableScene(false);
		}
	}
}

void UI_HUD_Tutorial::TurnOnResultHUD(INT32 iExp, INT32 iPoint, UINT32 uiTime)
{
	i3::stack_wstring wstrExp; 
	i3::stack_wstring wstrPoint;
	i3::stack_wstring wstrTime;

	LuaState * L = _CallLuaFunction("PointBlankRe_Tutorial_GameResult", "SetExpPointTime");

	g_pSndMng->StartAnnounceMessage( GTA_MISSION_SUCCESS);

	UINT32 uiMin, uiSec;

	uiMin = uiTime/60;
	uiSec = uiTime%60;
	
	i3::sprintf(wstrExp, L"%s : %d", GAME_RCSTRING("STR_TBL_PROFILE_CHALLENGE_PRIZE_EXP_TEXT"), iExp);
	i3::sprintf(wstrPoint, L"%s : %d", GAME_RCSTRING("STR_TBL_TUTORIAL_RESULT_POINT"), iPoint);
	i3::sprintf(wstrTime, L"%02d : %02d", uiMin, uiSec);

	i3Lua::PushStringUTF16To8(L, wstrExp);
	i3Lua::PushStringUTF16To8(L, wstrPoint);
	i3Lua::PushStringUTF16To8(L, wstrTime);
	_EndLuaFunction( L, 3);

	OnOffTutorialHUD(TH_RESULT, true);
}

void UI_HUD_Tutorial::OnOffTutorialHUD(TUTORIAL_HUD eHud, bool bFlag) 
{ 
	LuaState * L;

	
	switch(eHud)
	{
	case TH_KEY_MOVE:
	case TH_KEY_JUMP:
	case TH_KEY_SHOOT:
	case TH_KEY_EXT:
	case TH_KEY_GRENADE_LAUNCHER:
	case TH_KEY_GRENADE_SMOKE:
	case TH_KEY_C4:
	case TH_KEY_MELLE:
	case TH_KEY_HELICOPTER:
		{
			TUTORIAL_HUD_TYPE eType = (TUTORIAL_HUD_TYPE)eHud;
			m_pSceneHUD_Tutorial[eType]->EnableScene(bFlag);
		}
		break;

	case TH_MISSION_START:
	case TH_MISSION_CLEAR:
	case TH_GAME_OVER:
		{
			L = _CallLuaFunction("PointBlankRe_Tutorial_MissionStartClear", "OnOffHud");
			i3Lua::PushInteger( L, (INT32)eHud);
			i3Lua::PushBoolean(L, bFlag);
			_EndLuaFunction( L, 2);
			
		}
		break;
	case TH_RESULT:
		{
			m_pSceneHUD_Tutorial[THT_GAME_RESULT]->EnableScene(bFlag);
		}
		break;
	}
}


/*virtual*/ void UI_HUD_Tutorial::OnUpdate( REAL32 rDeltaSeconds)
{
	UIBase::OnUpdate( rDeltaSeconds);
}

