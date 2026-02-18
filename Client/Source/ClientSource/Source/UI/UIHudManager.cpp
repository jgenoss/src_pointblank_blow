#include "pch.h"
#include "UIHudManager.h"

#include "UIBattleFrame.h"

#include "UIHudNone.h"
#include "UIHudFirst.h"
#include "UIHudLast.h"
#include "UIHudRespawnTimer.h"
#include "UIHudChallengeBoard.h"
#include "UIHudNotice.h"
#include "UIHudMissionBoard.h"
#include "UIHudQuestEffect.h"
#include "UIHudDefAutoChange.h"
#include "UIHudMinimap.h"
#include "UIHudDashBoard.h"
#include "UIHudAmmoState.h"
#include "UIHudPlayerState.h"
#include "UIHudPlayerAmmoState.h" //1.0버전
#include "UIHudCrossCounterVS.h"
#include "UIHudDinoState.h"
#include "UIHudPlayerDinoState.h"
#include "UIHudDinoAmmoState.h" //1.0버전
#include "UIHudTargetObject.h"
#include "UIHudFloatingBoard.h"
#include "UIHudKillStar.h"
#include "UIHudObserver.h"
#include "UIHudGameResult.h"
#include "UIHudC4Indicator.h"
#include "UIHudWinterMode.h"
#include "UIHudMadnessNotice.h"
#include "UIHUDBattleUseItem.h"
#include "UIHudMultiWeapon.h"

#include "UIHudUtil.h"
#include "Batmanager.h"
#include "BattleSlotContext.h"

#include "../UI_V10/UIHudGameResult_V10.h"
#include "i3Base/string/algorithm/to_lower.h"

#include <functional>

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

/***********************//***********************/

namespace hud_util
{

Context::Context(iHudBase*p, const char* n)
{
	hud[ HUD::HS_1_0 ] = hud[ HUD::HS_1_5 ] = p;
	i3uie_name[ HUD::HS_1_5 ] = n;
	i3::to_lower(i3uie_name[ HUD::HS_1_5 ]);

	i3uie_name[ HUD::HS_1_0 ] = i3uie_name[ HUD::HS_1_5 ];
}

Context::Context(iHudBase*p, const char* n1, const char* n2)
{
	hud[ HUD::HS_1_0 ] = hud[ HUD::HS_1_5 ] = p;

	i3uie_name[ HUD::HS_1_5 ] = n1;
	i3::to_lower(i3uie_name[ HUD::HS_1_5 ]);

	i3uie_name[ HUD::HS_1_0 ] = n2;
	i3::to_lower(i3uie_name[ HUD::HS_1_0 ]);
}

Context::Context(iHudBase*p1, const char* n1, iHudBase*p2, const char* n2)
{
	hud[ HUD::HS_1_5 ] = p1;
	i3uie_name[ HUD::HS_1_5 ] = n1;
	i3::to_lower(i3uie_name[ HUD::HS_1_5 ]);

	hud[ HUD::HS_1_0 ] = p2;
	i3uie_name[ HUD::HS_1_0 ] = n2;
	i3::to_lower(i3uie_name[ HUD::HS_1_0 ]);
}

bool compare_i3uie_name(Context & c, const i3::string & n)
{
	//name은 파일명만, n은 전체 파일 패스이다. 그래서 파일명을 찾고 그 다음이 '.'란 가정 구현
	size_t idx = n.find(c.i3uie_name[g_pEnvSet->m_nGameInterface]);
	if( idx != i3::string::npos )
	{
		idx += c.i3uie_name[g_pEnvSet->m_nGameInterface].size();	
		return n[idx] == '.';
	}
	return false;
}

}


/***********************//***********************/
UIHudManager* HUD::instance()
{
	UIBattleFrame * pFrame = UIBattleFrame::i();
	if( pFrame != nullptr)
		return (UIHudManager*) pFrame->GetHUD( UIHUD_MANAGER );
	else
		return nullptr;
}

extern "C" 
{
	int hud_mgr_click( LuaState * L)
	{
		char type_name[MAX_STRING_COUNT] = "";
		i3Lua::GetStringArg( L, 1, type_name, MAX_STRING_COUNT);
		INT32 value1 = i3Lua::GetIntegerArg( L, 2);
		INT32 value2 = i3Lua::GetIntegerArg( L, 3);

		HUD::instance()->Click(type_name, value1, value2);

		return 0;
	}
}

LuaFuncReg UIHUDManager_Glue[] = 
{
	{ "Click", hud_mgr_click },
	{nullptr,						nullptr}
};

/***********************//***********************/
void UIHudManager::Click(const char* type_name, INT32 value1, INT32 value2)
{
	i3_prof_func();

	i3::string name( type_name );
	i3::to_lower( name );

	size_t size = m_hudContextList.size();
	for(size_t i=0; i<size;++i)
	{
		iHudBase* pHud = m_hudContextList[i].hud[g_pEnvSet->m_nGameInterface];

		i3::string hud_name = pHud->get_name();
		i3::to_lower( hud_name );

		if( name == hud_name )
		{
			pHud->SetParameter(value1, value2);
		}
	}
}

/***********************//***********************/
I3_CLASS_INSTANCE( UIHudManager);//, base );

UIHudManager::UIHudManager()
{
#if defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)
	hu_ut::test_factor.changing_hud = false;
#endif

	m_hudContextList.resize(HUD::UIT_MAX);

	UIHudFirst * pFirst = new UIHudFirst(this);
	MEMDUMP_NEW( pFirst, sizeof( UIHudFirst));
	m_hudContextList[HUD::FIRST_HUD] = hud_util::Context( pFirst, "");

	UIHudLast * pLast = new UIHudLast(this);
	MEMDUMP_NEW( pLast, sizeof( UIHudLast));
	m_hudContextList[HUD::LAST_HUD] = hud_util::Context( pLast, "");

	UIHudPlayerState * pPlayerState = new UIHudPlayerState(this);
	MEMDUMP_NEW( pPlayerState, sizeof( UIHudPlayerState));
	UIHudPlayerAmmoState * pPlayerAmmoState = new UIHudPlayerAmmoState(this);
	MEMDUMP_NEW( pPlayerAmmoState, sizeof(UIHudPlayerAmmoState));
	m_hudContextList[HUD::PLAYER_STATE] = hud_util::Context( pPlayerState, "PBRe_HUD", pPlayerAmmoState, "PBRe_HUD_Classic");

	UIHudAmmoState * pAmmoState = new UIHudAmmoState(this);
	MEMDUMP_NEW( pAmmoState, sizeof( UIHudAmmoState));

	UIHudNone * pNone = new UIHudNone(this);
	MEMDUMP_NEW( pNone, sizeof( UIHudNone));
	m_hudContextList[HUD::AMMO_STATE] = hud_util::Context( pAmmoState, "PBRe_HUD", pNone,"PBRe_HUD_Classic");

	UIHudPlayerDinoState * pPlayerDinoState = new UIHudPlayerDinoState(this);
	MEMDUMP_NEW( pPlayerDinoState, sizeof( UIHudPlayerDinoState));
	pNone = new UIHudNone(this);
	MEMDUMP_NEW( pNone, sizeof( UIHudNone));
	m_hudContextList[HUD::PLAYER_DINO_STATE] = hud_util::Context( pPlayerDinoState, "PBRe_HUD", pNone, "PBRe_HUD_Classic");

	UIHudDinoState * pDinoState = new UIHudDinoState(this);
	MEMDUMP_NEW( pDinoState, sizeof( UIHudDinoState));
	UIHudDinoAmmoState * pDinoAmmoState = new UIHudDinoAmmoState(this);
	MEMDUMP_NEW( pDinoAmmoState, sizeof( UIHudDinoAmmoState));
	m_hudContextList[HUD::DINO_STATE] = hud_util::Context( pDinoState, "PBRe_HUD", pDinoAmmoState, "PBRe_HUD_Classic");
	
	UIHudDashBoard * pDashBoard = new UIHudDashBoard(this);
	MEMDUMP_NEW( pDashBoard, sizeof(UIHudDashBoard));
	m_hudContextList[HUD::DASH_BOARD] = hud_util::Context( pDashBoard, "PBRe_HUD", "PBRe_HUD_Classic");

	UIHudMinimap * pMinimap = new UIHudMinimap(this);
	MEMDUMP_NEW( pMinimap, sizeof( UIHudMinimap));
	m_hudContextList[HUD::MINIMAP] = hud_util::Context( pMinimap, "PBRe_HUD", "PBRe_HUD_Classic");

	UIHudMissionBoard * pMissionBoard = new UIHudMissionBoard(this);
	MEMDUMP_NEW( pMissionBoard, sizeof( UIHudMissionBoard));
	m_hudContextList[HUD::MISSION_BOARD] = hud_util::Context( pMissionBoard, "PBRe_HUD", "PBRe_HUD_Classic");

	UIHudQuestEffect * pQuestEffect = new UIHudQuestEffect(this);
	MEMDUMP_NEW( pQuestEffect, sizeof( UIHudQuestEffect));
	pNone = new UIHudNone(this);
	MEMDUMP_NEW( pNone, sizeof( UIHudNone));
	m_hudContextList[HUD::QUEST_EFFECT] = hud_util::Context( pQuestEffect, "PBRe_HUD", pNone, "PBRe_HUD_Classic");

	UIHudNotice * pNotice = new UIHudNotice(this);
	MEMDUMP_NEW( pNotice, sizeof( UIHudNotice));
	m_hudContextList[HUD::NOTICE] = hud_util::Context( pNotice, "PBRe_HUD", "PBRe_HUD_Classic");

	UIHudChallengeBoard * pChallengeBoard = new UIHudChallengeBoard(this);
	MEMDUMP_NEW( pChallengeBoard, sizeof( UIHudChallengeBoard));
	m_hudContextList[HUD::CHALLENGE_BOARD] = hud_util::Context( pChallengeBoard, "PBRe_HUD", "PBRe_HUD_Classic");

	UIHudCrossCounterVS * pCrossCounterVS = new UIHudCrossCounterVS(this);
	MEMDUMP_NEW( pCrossCounterVS, sizeof( UIHudCrossCounterVS));
	m_hudContextList[HUD::CROSS_COUNTER_VS] = hud_util::Context( pCrossCounterVS, "PBRe_HUD", "PBRe_HUD_Classic");
	
	UIHudTargetObject * pTargetObject = new UIHudTargetObject(this);
	MEMDUMP_NEW( pTargetObject, sizeof( UIHudTargetObject));
	m_hudContextList[HUD::TARGET_OBJECT] = hud_util::Context( pTargetObject, "PBRe_HUD", "PBRe_HUD_Classic");

	UIHudFloatingBoard * pFloatingBoard = new UIHudFloatingBoard(this);
	MEMDUMP_NEW( pFloatingBoard, sizeof( UIHudFloatingBoard));
	m_hudContextList[HUD::FLOAT_BOARD] = hud_util::Context( pFloatingBoard, "UIHUDMedalBoard");

	UIHudC4Indicator * pC4Indicator = new UIHudC4Indicator(this);
	MEMDUMP_NEW( pC4Indicator, sizeof( UIHudC4Indicator));
	m_hudContextList[HUD::C4INDICATOR] = hud_util::Context( pC4Indicator, "PBRe_HUD", "PBRe_HUD_Classic");

	UIHudWinterMode* pWinterMode = new UIHudWinterMode(this);
	MEMDUMP_NEW( pWinterMode, sizeof( UIHudWinterMode));
	m_hudContextList[HUD::WINTERMODE] = hud_util::Context( pWinterMode, "PBRe_HUD" );

	UIHudRespawnTimer * pRespawnTimer = new UIHudRespawnTimer(this);
	MEMDUMP_NEW( pRespawnTimer, sizeof( UIHudRespawnTimer));
	m_hudContextList[HUD::RESPAWN_TIMER] = hud_util::Context( pRespawnTimer, "PBRe_HUDRespawn", "PBRe_HUDRespawn_Classic");

	UIHudDefAutoChange * pDefAutoChange = new UIHudDefAutoChange(this);
	MEMDUMP_NEW( pDefAutoChange, sizeof( UIHudDefAutoChange));
	m_hudContextList[HUD::DEFAUTOCHANGE] = hud_util::Context( pDefAutoChange, "PBRe_DefAutoChange");

	UIHudMadnessNotice * pMadnessNotice = new UIHudMadnessNotice(this);
	MEMDUMP_NEW( pMadnessNotice, sizeof( UIHudMadnessNotice));
	m_hudContextList[HUD::MADNESSNOTICE] = hud_util::Context( pMadnessNotice, "PBRe_HUD", "PBRe_HUD_Classic");

	UIHudBattleUseItemNotice * pBattleUseItemNotice = new UIHudBattleUseItemNotice(this);
	MEMDUMP_NEW( pBattleUseItemNotice, sizeof(UIHudBattleUseItemNotice));
	m_hudContextList[HUD::BATTLEUSEITEM] = hud_util::Context(pBattleUseItemNotice, "PBRe_HUD", "PBRe_HUD_Classic");
	
	UIHudGameResult * pGameResult = nullptr;
	if( g_pEnvSet->m_nUIVersion == UFV_1_5)
	{
		pGameResult = new UIHudGameResult(this);
		MEMDUMP_NEW( pGameResult, sizeof( UIHudGameResult));
		m_hudContextList[HUD::GAME_RESULT] = hud_util::Context( pGameResult, "PBRe_Game_Result");
	}
	else if( g_pEnvSet->IsBaseOnUIVersion_1_0() 
			|| g_pEnvSet->m_nUIVersion == UFV_1_1)
	{
		pGameResult = new UIHudGameResult_V10(this);
		MEMDUMP_NEW( pGameResult, sizeof( UIHudGameResult_V10));
		m_hudContextList[HUD::GAME_RESULT] = hud_util::Context( pGameResult, "PBRe_Game_Result");
	}

	UIHudMultiWeapon * pMultiWeapon = new UIHudMultiWeapon(this);
	MEMDUMP_NEW( pMultiWeapon, sizeof( UIHudMultiWeapon));
	m_hudContextList[HUD::MULTIWEAPON] = hud_util::Context( pMultiWeapon, "PBRe_HUD", "PBRe_HUD_Classic");

	is_load_complete_resource = false;
	is_block_msg_by_visible = false;
}

UIHudManager::~UIHudManager()
{
	struct Fn	
	{	
		void operator()(hud_util::Context & cxt) 
		{
			if( cxt.hud[HUD::HS_1_5] == cxt.hud[HUD::HS_1_0] )
			{
				I3_SAFE_DELETE( cxt.hud[HUD::HS_1_5]);
			}
			else
			{
				I3_SAFE_DELETE( cxt.hud[HUD::HS_1_5]);
				I3_SAFE_DELETE( cxt.hud[HUD::HS_1_0]); 
			}
		}
	};
	std::for_each(m_hudContextList.begin(), m_hudContextList.end(), Fn() );
	m_hudContextList.clear();
}
 
/***********************//***********************/
/*virtual*/ void UIHudManager::_InitializeAtLoad( i3UIScene * pScene)
{
	//_InitializeAtLoad이 함수 특징이 여러개의 Scene을 등록하면 모든 scene이 다 들어온다.
	//그래서 특정 scene만 하위 클래스에서 control 등록할 수 있도록 선별한다.
	i3_prof_func();

	i3::string name( pScene->getScriptFile() );
	i3::to_lower( name );

	size_t size = m_hudContextList.size();
	for(size_t i=0; i<size;++i)
	{
		if( hud_util::compare_i3uie_name(m_hudContextList[i], name) == true )	//특정 하위 클래스 선별
		{
			if(m_hudContextList[i].hud[g_pEnvSet->m_nGameInterface])
				m_hudContextList[i].hud[g_pEnvSet->m_nGameInterface]->InitializeAtLoad(pScene);
		}
	}

	bool isctr1 = i3::generic_is_iequal(pScene->getScriptFile(), i3::string("script\\pbre_game_round_result.lua"));
	if( isctr1 == true )
	{
		if(m_hudContextList[HUD::GAME_RESULT].hud[g_pEnvSet->m_nGameInterface])
			m_hudContextList[HUD::GAME_RESULT].hud[g_pEnvSet->m_nGameInterface]->InitializeAtLoad(pScene);
	}
}

/*virtual*/ void UIHudManager::_ClearAtUnload( i3UIScene * pScene)
{
	i3_prof_func();

	//_InitializeAtLoad이 함수 특징이 여러개의 Scene을 등록하면 모든 scene이 다 들어온다.
	//그래서 특정 scene만 하위 클래스에서 control 등록할 수 있도록 선별한다.
	i3::string name( pScene->getScriptFile() );
	i3::to_lower( name );

	size_t size = m_hudContextList.size();
	for(size_t i=0; i<size;++i)
	{
		if( hud_util::compare_i3uie_name(m_hudContextList[i], name) == true )	//특정 하위 클래스 선별
		{
			m_hudContextList[i].hud[g_pEnvSet->m_nGameInterface]->ClearFrameWnd();
			m_hudContextList[i].hud[g_pEnvSet->m_nGameInterface]->ClearAtUnload(pScene);
		}
	}
}

/*virtual*/ void UIHudManager::OnCreate( i3GameNode * pParent)
{
	i3_prof_func();

	is_complete_respawn = false;

	UIHUDBase::OnCreate( pParent);

	i3::literal_range hud_str, respawn_str;

	if( g_pEnvSet->m_nUIVersion == UFV_1_5 )
	{
		if( g_pEnvSet->m_nGameInterface == HUD::HS_1_5 )
		{
			hud_str = "Scene/HUD/PBRe_HUD.i3UIs";
			respawn_str = "Scene/HUD/PBRe_HUDRespawn.i3UIs";
		}
		else
		{
			hud_str = "Scene/HUD/PBRe_HUD_Classic.i3UIs";
			respawn_str = "Scene/HUD/PBRe_HUDRespawn_Classic.i3UIs";
		}

		AddScene( hud_str.c_str(), UIHUDManager_Glue, true, false);
		AddScene( respawn_str.c_str(), UIHUDManager_Glue, true, false);

		AddScene( "Scene/HUD/UIHUDMedalBoard.i3UIs", 0, true, false);

		end_result_scene_info.idx = GetSceneCount();
		AddScene( "Scene/HUD/PBRe_Game_Result.i3UIs", UIHUDManager_Glue, true, false);

		round_result_scene_info.idx = GetSceneCount();
		AddScene( "Scene/HUD/PBRe_Game_Round_Result.i3UIs", 0, true, false);

		AddScene( "Scene/HUD/PBRe_DefAutoChange.i3UIs", nullptr, true, false);
	}
	else if( g_pEnvSet->m_nUIVersion == UFV_1_1
			|| g_pEnvSet->IsBaseOnUIVersion_1_0())
	{
		// Load Scene
		AddScene("Scene/HUD/PBRe_HUD_Classic.i3UIs", UIHUDManager_Glue, true, false);
		AddScene("Scene/HUD/PBRe_HUDRespawn_Classic.i3UIs", UIHUDManager_Glue, true, false);

		end_result_scene_info.idx = GetSceneCount();
		AddScene("Scene/HUD/PBRe_Game_Result.i3UIs", UIHUDManager_Glue, true, false);

		round_result_scene_info.idx = GetSceneCount();
		AddScene("Scene/HUD/PBRe_Game_Round_Result.i3UIs", 0, true, false);
		AddScene("Scene/HUD/PBRe_DefAutoChange.i3UIs", 0, true, false);
	}
}

void UIHudManager::OnLoadAllScenes()
{
	i3_prof_func();

	UIHUDBase::OnLoadAllScenes();

	end_result_scene_info.scn = GetScene( end_result_scene_info.idx );
	round_result_scene_info.scn = GetScene( round_result_scene_info.idx );

	CGameCharaBase* pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx() );

	struct Fn	
	{
		CGameCharaBase* pMyChara;
		Fn(CGameCharaBase* p) : pMyChara(p) {}

		void operator()(hud_util::Context & cxt) 
		{ 
			if(cxt.hud[g_pEnvSet->m_nGameInterface])
			{
				cxt.hud[g_pEnvSet->m_nGameInterface]->SetMyChara(pMyChara);
				cxt.hud[g_pEnvSet->m_nGameInterface]->SetVisible(false);
			}

		} 
	};
	std::for_each(m_hudContextList.begin(), m_hudContextList.end(), Fn(pMyChara) );

	is_load_complete_resource = true;
	is_block_msg_by_visible = false;
}

//OnEvent, OnProcess 이 함수를 사용하기에 ProcessGameEvent 함수는 사용 안 햇음.. 필요한 경우 사용하면 됨
/*virtual*/ void UIHudManager::ProcessGameEvent( GAME_EVENT evt, INT32 Arg, const i3::user_data& UserData)
{
	i3_prof_func();

	UIHUDBase::ProcessGameEvent( evt, Arg, UserData);

	if( is_load_complete_resource == false ) return;
	if( is_block_msg_by_visible == true ) return;

	struct Fn	
	{
		GAME_EVENT evt;
		INT32 Arg;
		const i3::user_data& UserData;

		Fn(GAME_EVENT e, INT32 A, const i3::user_data& UserData) : evt(e), Arg(A), UserData(UserData) {}
		void operator()(hud_util::Context & cxt) 
		{ 
			if( cxt.hud[g_pEnvSet->m_nGameInterface] 
				&& cxt.hud[g_pEnvSet->m_nGameInterface]->m_enable )
			{
				cxt.hud[g_pEnvSet->m_nGameInterface]->ProcessGameEvent(evt, Arg, UserData); 
			}	
		}
	};
	std::for_each(m_hudContextList.begin(), m_hudContextList.end(), Fn(evt, Arg, UserData) );
}

void	UIHudManager::OnUpdate( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	if( is_load_complete_resource == false ) return;
	if( is_complete_respawn == false ) return;
	if( is_block_msg_by_visible == true ) return;

	UIHUDBase::OnUpdate(rDeltaSeconds);

	BaTMaN::BatMan_Ingame_Update(rDeltaSeconds);

	struct Fn	
	{
		REAL32 rDeltaSeconds;
		Fn(REAL32 d) : rDeltaSeconds(d) {}
		void operator()(hud_util::Context & cxt) 
		{
			if( cxt.hud[g_pEnvSet->m_nGameInterface]
				&& cxt.hud[g_pEnvSet->m_nGameInterface]->m_enable )
			{
				cxt.hud[g_pEnvSet->m_nGameInterface]->Update(rDeltaSeconds);
			}
		}
	};
	std::for_each(m_hudContextList.begin(), m_hudContextList.end(), Fn(rDeltaSeconds) );
}

/***********************//***********************/
iHudBase* UIHudManager::GetHud(HUD::UIType type)
{
	if(m_hudContextList.size() < (size_t)type)
		return nullptr;

	return m_hudContextList[type].hud[g_pEnvSet->m_nGameInterface];
}

void UIHudManager::EnableHud( HUD::UIType type, bool enable)
{
	if( is_load_complete_resource == false ) return;
	if( is_block_msg_by_visible == true ) return;

	m_hudContextList[type].hud[g_pEnvSet->m_nGameInterface]->SetEnable(enable);
}

bool UIHudManager::IsEnableHud( HUD::UIType type ) const
{
	if(m_hudContextList[type].hud[g_pEnvSet->m_nGameInterface])
		return m_hudContextList[type].hud[g_pEnvSet->m_nGameInterface]->IsVisible();
	
	return false;
	//return m_list[type].hud[g_pEnvSet->m_nGameInterface]->IsEnable();
}

void UIHudManager::Show()
{
	struct Fn	
	{
		void operator()(hud_util::Context & cxt) const
		{
			if( cxt.hud[g_pEnvSet->m_nGameInterface]->m_enable )
			{
				cxt.hud[g_pEnvSet->m_nGameInterface]->SetVisible(true);
			}
		}
	};
	std::for_each(m_hudContextList.begin(), m_hudContextList.end(), Fn() );
	is_block_msg_by_visible = false;
}

void UIHudManager::SetUsableAllHud()
{
	struct Fn	
	{
		void operator()(hud_util::Context & cxt) const
		{
			cxt.hud[g_pEnvSet->m_nGameInterface]->SetEnable(true);
			cxt.hud[g_pEnvSet->m_nGameInterface]->SetVisible(false);
		}
	};
	std::for_each(m_hudContextList.begin(), m_hudContextList.end(), Fn() );
}

void UIHudManager::Hide()
{
	struct Fn	
	{
		void operator()(hud_util::Context & cxt) const
		{
			if( cxt.hud[g_pEnvSet->m_nGameInterface]->m_enable )
			{
				cxt.hud[g_pEnvSet->m_nGameInterface]->SetVisible(false);
			}
		}
	};
	std::for_each(m_hudContextList.begin(), m_hudContextList.end(), Fn() );
	is_block_msg_by_visible = true;
}


void UIHudManager::SetParameter( HUD::UIType type, INT32 arg1, INT32 arg2 )
{
	i3_prof_func();

	if( is_load_complete_resource == false ) return;
	if( is_block_msg_by_visible == true )
	{
		//리스폰은 UIHudRespawnTimer 클래스에서 직접 처리한다.
		if( type != HUD::RESPAWN_TIMER && arg1 != HUD::RT_START ) 
			return;
	}

	if(m_hudContextList[type].hud[g_pEnvSet->m_nGameInterface])
		m_hudContextList[type].hud[g_pEnvSet->m_nGameInterface]->SetParameter(arg1, arg2);
}

void UIHudManager::SetParameter(const HUD::UITypeList & list, INT32 arg1, INT32 arg2 )
{
	i3_prof_func();

	if( is_load_complete_resource == false ) return;
	if( is_block_msg_by_visible == true ) return;

	m_hudContextList[HUD::FIRST_HUD].hud[g_pEnvSet->m_nGameInterface]->SetParameter(arg1, arg2);

	size_t size = list.size();
	for(size_t i=0;i<size;++i)
	{
		m_hudContextList[ list[i] ].hud[g_pEnvSet->m_nGameInterface]->SetParameter(arg1, arg2);
	}

	m_hudContextList[ HUD::LAST_HUD ].hud[g_pEnvSet->m_nGameInterface]->SetParameter(arg1, arg2);
}

/***********************//***********************/
void UIHudManager::OnEvent(GAME_EVENT type, INT32 arg1, const i3::user_data& UserData)
{
	i3_prof_func();

	if( is_load_complete_resource == false ) return;
	if( is_block_msg_by_visible == true ) return;

	if( type == EVENT_RESPAWN_BATTLE)
	{
		if( BattleSlotContext::i()->GetMySlotIdxEx() != arg1) return; //본인 event 만 처리한다.

		if( is_complete_respawn == false)
		{
			is_complete_respawn = true;
			OnEvent(EVENT_FIRST_RESPWAN_BATTLE, arg1, UserData); //EVENT_NOTHING -> event_first_respawn_battle 이 함수 호출용 이벤트
			OnEvent(type, arg1, UserData); //EVENT_RESPAWN_BATTLE 이벤트
			return;
		}
	}

	if( type == EVENT_START_BATTLE )
	{
		if( BattleSlotContext::i()->GetMySlotIdxEx() != arg1) return; //본인 event 만 처리한다.
	}

	//제압 미션에 스테이지 점프할 경우 EVENT_M_PRE_ROUNDSTART_BATTLE 로 시작된다.
	if( hu_ut::is_domination_mode() == true ) 
	{
		if( type == EVENT_M_PRE_ROUNDSTART_BATTLE )
		{
			struct Fn	
			{
				void operator()(hud_util::Context & cxt) 
				{
					if(cxt.hud[g_pEnvSet->m_nGameInterface])
						cxt.hud[g_pEnvSet->m_nGameInterface]->SetEnable(true);
				}	
			};
			std::for_each(m_hudContextList.begin(), m_hudContextList.end(), Fn() );
		}
	}


	struct Fn	
	{
		GAME_EVENT type;
		INT32 arg1;
		const i3::user_data& UserData;
		Fn(GAME_EVENT t, INT32 a1, const i3::user_data& UserData) : type(t), arg1(a1), UserData(UserData) {}
		void operator()(hud_util::Context & cxt) 
		{
			if(cxt.hud[g_pEnvSet->m_nGameInterface])
			{
				if( type == EVENT_FIRST_RESPWAN_BATTLE || type == EVENT_RESPAWN_BATTLE ) //hud 활성화 유무 없이 event 처리
				{
					cxt.hud[g_pEnvSet->m_nGameInterface]->OnEvent(type, arg1, UserData); 
				}
				else
				{
					if( cxt.hud[g_pEnvSet->m_nGameInterface] 
					&&cxt.hud[g_pEnvSet->m_nGameInterface]->m_enable ) //활성화 유무에 따라 event 처리
						cxt.hud[g_pEnvSet->m_nGameInterface]->OnEvent(type, arg1, UserData); 
				}
			}
		}	
	};
	std::for_each(m_hudContextList.begin(), m_hudContextList.end(), Fn(type,arg1, UserData) );
}

bool UIHudManager::OnRevive( i3RenderContext * pCtx)
{

	struct Fn : std::binary_function<hud_util::Context, i3RenderContext *, void>
	{
		void operator()(hud_util::Context & cxt, i3RenderContext * pctx) const
		{
			if( cxt.hud[g_pEnvSet->m_nGameInterface]->m_enable )
			{
				if(cxt.hud[g_pEnvSet->m_nGameInterface])
					cxt.hud[g_pEnvSet->m_nGameInterface]->OnRevive( pctx);
			}
		}
	};

	std::for_each(m_hudContextList.begin(), m_hudContextList.end(), std::bind2nd( Fn(), pCtx ) );
	return UIBase::OnRevive( pCtx);
}

