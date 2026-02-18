#include "pch.h"
#include "UIHUDTabMinimap.h"
#include "Minimap.h"
#include "Unit.h"
#include "MinimapUtil.h"
#include "BattleHUD_Define.h"
#include "UIUtil.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"
#include "../StageBattle/AIModeContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UIHudManager.h"
#include "../BattleHUD_Minimap.h"
#include "../UtilBattle.h"
#include "UIHudUtil.h"
#include "../StageBattle/C4Context.h"
#include "GameMissionManager.h"
#include "UserInfoContext.h"
#include "ClanGameContext.h"
#include "MyRoomInfoContext.h"

#include "MissionCard/MCard.h"
#include "MissionCard/MCardQuest.h"
#include "MissionCard/MCardQuestInfo.h"
#include "MissionCard/MCardUtil.h"

#include "i3Base/string/ext/utf16_to_utf8.h"

#include "ClientStageInfoUtil.h"

using namespace minimap_new;

namespace
{
	inline void PushLVBoxString(const i3::wliteral_range& wstr_rng, i3UIListView_Box* lvBox, INT32 slotIdx, INT32 cellIdx)
	{
		i3::string str_utf8;
		i3::utf16_to_utf8(wstr_rng, str_utf8);
		lvBox->SetItemCell(slotIdx, cellIdx, str_utf8.c_str() );
	}

	inline void SetCellVisible(i3UIListView_Box* lvBox, INT32 slotIdx, INT32 cellIdx, bool visible)
	{
		i3UIControl* cell = lvBox->getCellChild(slotIdx, cellIdx);
		cell->HideShapes(!visible);
		//cell->SetVisible(visible);
		if(visible == false && i3::same_of<i3UIButtonImageSet*>(cell))
		{
			//((i3UIButtonImageSet*)cell)->SetShapeIdx(-1);
			lvBox->SetItemCell(slotIdx, cellIdx, "-1");
		}
	}


	inline void SetCellClanMark(i3UIListView_Box* lvBox, INT32 slotIdx, INT32 cellIdx, UINT32 ui32MarkFlag, void * pFunc)
	{
		UINT32 nBackImageIdx = CLANMARK_FRAME_IMAGE_IDX(ui32MarkFlag);
		UINT32 nShapeImageIdx = CLANMARK_CONTENT_IMAGE_IDX(ui32MarkFlag);
		UINT32 nBackColorIdx = CLANMARK_FRAME_COLOR_IDX(ui32MarkFlag);
		UINT32 nShapeColorIdx = CLANMARK_CONTENT_COLOR_IDX(ui32MarkFlag);

		char szMarkFlag[MAX_STRING_COUNT] = {0};

		i3::snprintf(szMarkFlag, MAX_STRING_COUNT, "%03d%03d%03d%03d",
			nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx);

		lvBox->SetItemCell(slotIdx, cellIdx, szMarkFlag);
		lvBox->SetCellCallback(slotIdx, cellIdx, pFunc);
	}

	bool NeedToUpdateForTeamScore()
	{
		static INT32 oldScore = 0;
		INT32 curScore = 0;

		for (INT32 i = 0; i < TEAM_COUNT; i++)
			curScore += tu::GetTeamScore(static_cast<TEAM_TYPE>(i));

		if (oldScore != curScore)
		{
			oldScore = curScore;
			return true;
		}

		return false;
	}

	i3::rc_wstring GetScoreText(STAGE_MODE eType)
	{
		if( !CStageInfoUtil::IsKillMode( *MyRoomInfoContext::i()->getStage() ) )
		{
			if( eType == STAGE_MODE_ESCAPE)
				return GAME_RCSTRING("STR_HUD_INGAME_TABMINIMAP_SCORE");
			else
				return GAME_RCSTRING("STR_HUD_INGAME_TABMINIMAP_ROUND");
		} 

		return GAME_RCSTRING("STR_HUD_INGAME_TABMINIMAP_KILL");
	}

	INT32 GetTeamScore(TEAM_TYPE team)
	{
		INT32 score = 0;
		
		if( !CStageInfoUtil::IsKillMode(*MyRoomInfoContext::i()->getStage()) )
		{
			STAGE_MODE stageType = MyRoomInfoContext::i()->getStage()->GetStageMode();

			if( stageType == STAGE_MODE_ESCAPE)
				score = ScoreContext::i()->GetDinoEscapeScore(team);
			else
				score = ScoreContext::i()->GetTeamWinRound(team);
		}
		else
			score = ScoreContext::i()->GetTeamScore(team)->m_KillCount;

		return score;
	}

	void ClearListViewSlot(i3UIListView_Box* lvBox, INT32 lvSlotIdx)
	{
		PushLVBoxString(L"", lvBox, lvSlotIdx, 2);

		lvBox->SetItemCell(lvSlotIdx, 3, "");
		lvBox->SetItemCell(lvSlotIdx, 4, "");
		lvBox->SetItemCell(lvSlotIdx, 5, "");
		lvBox->SetItemCell(lvSlotIdx, 6, "");
		lvBox->SetItemCell(lvSlotIdx, 7, "");

		for (int i = 0; i<MAX_TABMINIMAP_CELL_COUNT; i++)
		{
			SetCellVisible(lvBox, lvSlotIdx, i, false);	
		}

		lvBox->UnSelectAllData();
	}

	void ClearListView(i3UIListView_Box* lvBox)
	{
		for (INT32 i=0; i<9; i++)
			ClearListViewSlot(lvBox, i);
	}

	INT32 MaxTeamPlayer(const INT32 team)
	{
		if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
		{
			if (team != tu::GetMyTeamType())						
				return AIModeContext::i()->EnemyCount;
		}

		return MAX_TEAM_USER;
	}

	bool IsObserveState()
	{
		if ( CStageInfoUtil::IsObserverModeStage(*MyRoomInfoContext::i()->getStage()) )
		{
			if (ub::is_live_character(BattleSlotContext::i()->getMySlotIdx()) == false)
				return true;
		}

		return false;
	}

	void EnableMinimap(bool activate)
	{
		if (MyRoomInfoContext::i()->getStage()->IsHudOff())
		{
			activate = true;
		}

		UIHudManager* hudMgr 
			= static_cast<UIHudManager*>(UIBattleFrame::i()->GetHUD(UIHUD_MANAGER));
		if (hudMgr)
		{
			CBattleHUD_MiniMap::instance()->SetEnableMiniMap(!activate);
			hudMgr->EnableHud(HUD::MINIMAP, !activate);

			if ((MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE) || MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD)))
				hudMgr->EnableHud(HUD::CHALLENGE_BOARD, !activate);
		}
	}
}

extern "C"
{
	int EnableScene(LuaState* L)
	{
		UIBattleFrame* frame = UIBattleFrame::i();
		if (frame != nullptr)
		{
			bool visible = i3Lua::GetBooleanArg(L, 1);

			frame->GetHUD(UIHUD_TABMINIMAP)->SetVisible(visible);
		}

		return 0;
	}
}

LuaFuncReg UIHUDTabMinimap_Glue[] = 
{
	{"EnableScene", EnableScene},
	{nullptr,	nullptr}
};

I3_CLASS_INSTANCE(UIHUDTabMinimap);

UIHUDTabMinimap::UIHUDTabMinimap() 
 : m_bActivate(0), m_bMinimapUpdatable(0), m_bUserInfoUpdatable(0),
	m_pRedTeamListView(nullptr), m_pBlueTeamListView(nullptr), m_bActivateMC(false)
{
	for( int i = 0; i < MAX_QUEST_PER_MISSON_CARD; i++)
	{
		m_MC[i].MC_ClearButton = nullptr;
		m_MC[i].MC_Contents    = nullptr;
	}
}

UIHUDTabMinimap::~UIHUDTabMinimap()
{
	i3_prof_func();

	for (size_t i=0; i<m_units.size(); i++)
	{
		I3_SAFE_DELETE( m_units[i]);
	}
	m_units.clear();

	for (INT32 i=0; i<TEAM_COUNT; i++)
		m_ranking[i].clear();
}

void UIHUDTabMinimap::_InitializeAtLoad(i3UIScene* pScene)
{
	i3_prof_func();
	
	//나중에 미션카드 쓴다 안쓴다의 플래그 값이 서버로 넘겨오면 이부분을 바꿔야됨.
	if (g_pEnvSet->m_nUIVersion != UFV_1_5)
		m_bActivateMC = true;

	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
		return;

	UIHUDBase::_InitializeAtLoad(pScene);

	_ReserveTeamRankingList();

	m_bUserInfoUpdatable = true; // 리스트뷰의 유저정보 갱신가능

	_CreateMinimap(pScene);
	_CreateUnits();
	_InitMC(pScene);

	m_pRedTeamListView = static_cast<i3UIControl*>(pScene->FindChildByName("i3UIListView_Red"));
	m_pBlueTeamListView = static_cast<i3UIControl*>(pScene->FindChildByName("i3UIListView_Blue"));
}

void UIHUDTabMinimap::_ReserveTeamRankingList()
{
	m_ranking[tu::GetMyTeamType()].reserve(MAX_TEAM_USER);
	if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
		m_ranking[tu::GetEnemyTeamType()].reserve(AIModeContext::i()->EnemyCount);
	else
		m_ranking[tu::GetEnemyTeamType()].reserve(MAX_TEAM_USER);
}

void UIHUDTabMinimap::_CreateMinimap(i3UIScene* scene)
{
	i3_prof_func();


	i3UIFrameWnd * pFrame = nullptr;

	pFrame = reinterpret_cast<i3UIFrameWnd*>(scene->FindChildByName("FrameWnd_MapImg"));

	if(pFrame != nullptr && MyRoomInfoContext::i()->getStage()->IsHudOff() == false)
	{
		if (Minimap::GetInstance())
			m_bMinimapUpdatable = Minimap::GetInstance()->Create(pFrame); // 미니맵 이미지(맵 배경, 유닛, 오브젝트들) 갱신가능
	}
}

void UIHUDTabMinimap::_CreateUnits()
{
	i3_prof_func();

	INT32 my_team = BattleSlotContext::i()->getMySlotIdx() % 2;

	for (INT32 idx = 0; idx < SLOT_MAX_COUNT; ++idx )
	{
		INT32 slot_team = idx % 2;

		if( slot_team == my_team )
		{
			if ( idx == BattleSlotContext::i()->getMySlotIdx() )
			{
				UnitCharaMe * p = new UnitCharaMe(idx);
				MEMDUMP_NEW( p, sizeof( UnitCharaMe));
				m_units.push_back( p);
			}
			else
			{
				UnitCharaFellow * p = new UnitCharaFellow(idx);
				MEMDUMP_NEW( p, sizeof( UnitCharaFellow));
				m_units.push_back( p);
			}
		}

		m_ranking[slot_team].push_back(idx);
	}

	if (!_IsMinimapUpdatable()) return;

	switch (MyRoomInfoContext::i()->getStage()->GetStageMode())
	{
	case STAGE_MODE_BOMB :
		{
			if (C4Context::i()->FindAreaProp(BOMB_AREA_A) != nullptr)
			{
				UnitBombA * pBombA = new UnitBombA;
				MEMDUMP_NEW( pBombA, sizeof( UnitBombA));
				m_units.push_back( pBombA);
			}

			if (C4Context::i()->FindAreaProp(BOMB_AREA_B) != nullptr)
			{
				UnitBombB * pBombB = new UnitBombB;
				MEMDUMP_NEW( pBombB, sizeof( UnitBombB));
				m_units.push_back( pBombB);
			}
		}
		break;

	case STAGE_MODE_DESTROY :	
		{
			UnitDestroyRed * pDestroyRed = new UnitDestroyRed;
			MEMDUMP_NEW( pDestroyRed, sizeof( UnitDestroyRed));
			m_units.push_back( pDestroyRed);

			UnitDestroyBlue * pDestroyBlue = new UnitDestroyBlue;
			MEMDUMP_NEW( pDestroyBlue, sizeof( UnitDestroyBlue));
			m_units.push_back( pDestroyBlue);
		}
		break;

	case STAGE_MODE_DEFENCE :
		{
			UnitDefenceRed * pDestroyRed = new UnitDefenceRed;
			MEMDUMP_NEW( pDestroyRed, sizeof( UnitDefenceRed));
			m_units.push_back( pDestroyRed);

			UnitDefenceBlue * pDestroyBlue = new UnitDefenceBlue;
			MEMDUMP_NEW( pDestroyBlue, sizeof( UnitDefenceBlue));
			m_units.push_back( pDestroyBlue);
		}
		break;

	case STAGE_MODE_CONVOY :
		{
			CGameMissionManager *pMgr = CGameMissionManager::i();
			
			UnitUsurpaiton* pUsurpation = new UnitUsurpaiton(pMgr->getUsurpationIdx());
			MEMDUMP_NEW(pUsurpation, sizeof(UnitUsurpaiton));
			m_units.push_back(pUsurpation);

			if( MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
			{
				UnitCleaverKnifeA* pCleaverKinfeA = new UnitCleaverKnifeA;
				MEMDUMP_NEW(pCleaverKinfeA, sizeof(UnitCleaverKnifeA));
				m_units.push_back(pCleaverKinfeA);
				UnitCleaverKnifeB* pCleaverKinfeB = new UnitCleaverKnifeB;
				MEMDUMP_NEW(pCleaverKinfeB, sizeof(UnitCleaverKnifeB));
				m_units.push_back(pCleaverKinfeB);
			}
			else
			{	// 정규 이벤트
			}
		}
		break;
	case STAGE_MODE_RUN_AWAY :
		break;

	default: break;
	}
}

void UIHUDTabMinimap::OnCreate(i3GameNode* nodeParent)
{
	i3_prof_func();

	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
		return;

	UIHUDBase::OnCreate(nodeParent);
	
	AddScene("Scene/HUD/TapMinimap_new.i3UIs", UIHUDTabMinimap_Glue, true, false);
	
	// 퍼포먼스 때문에 미리 올려놓고 시작.
	_UpdateUserInfo();
}

void UIHUDTabMinimap::OnUpdate(REAL32 tm)
{
	i3_prof_func();

	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
		return;

	if (_IsActivated())
	{
		if (_IsMinimapUpdatable())
		{
			UIHUDBase::OnUpdate(tm);

			Minimap::GetInstance()->Update(tm);

			for (size_t i= 0; i<m_units.size(); i++)
				m_units[i]->Update(tm);
		}

		_UpdateWhenNecessary();
	}
}	

void UIHUDTabMinimap::_UpdateWhenNecessary()
{
	i3_prof_func();

	if (_IsUserInfoUpdatable() || NeedToUpdateForTeamScore())
	{
		_UpdateTeamScore();
		_UpdateRestTeamGoalInfo();
	}
}

bool UIHUDTabMinimap::OnEntranceStart(void* arg1, void* arg2)
{
	i3_prof_func();


	if (!_IsUserInfoUpdatable() || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
		return false;

	if (UIHUDBase::OnEntranceStart(arg1, arg2) == false)
		return false;

	_InitDefaultInfo();
	_UpdateUserInfo();
	_UpdateTeamScore();
	_UpdateRestTeamGoalInfo();

	m_bActivate = true;

	return true;
}

void UIHUDTabMinimap::_InitDefaultInfo()
{
	i3_prof_func();

	LuaState * L = _CallLuaFunction("SetDefaultInfo");

	if(L != nullptr)
	{
		i3::stack_wstring wstrRoomLocation;
		_SetRoomLocation(wstrRoomLocation);

		i3Lua::PushStringUTF16To8(L , wstrRoomLocation);
		

		// 흐음...향후를 위해 UTF-8로 간주하고 처리.. 
		// TODO : 문제가 있는지 없는지 확인이 요망된다...(2014.07.23.수빈)
//		i3Lua::PushStringUTF16To8(L , g_pFramework->getStageInfo(g_pFramework->GetCurrentStageIndex())->GetName());

		// (11356) : Midtown(Downtown2) 출력 문제로 인해 주석처리 하고 다른 방식으로 맵 네임 출력합니다. gyungju.kim. 문제시 말해주세요.
		//const i3::rc_string& strStageName = g_pFramework->getStageInfo(g_pFramework->GetCurrentStageIndex())->GetNameString();
		//i3Lua::PushString(L, strStageName.c_str(), strStageName.size());

		// 현재 Room Info를 받아 GameUI를 통해 맵 네임을 받아옴.
		ROOM_INFO_BASIC info;  		MyRoomInfoContext::i()->getMyRoomInfo(&info);

		i3::rc_wstring wstrStageName;
		STAGE_ID curStageID = (STAGE_ID)info._StageID;
		GameUI::GetStageMapName(false, curStageID, wstrStageName);
		i3Lua::PushStringUTF16To8(L, wstrStageName.c_str());

		i3Lua::PushBoolean(L , tu::GetMyTeamType() == TEAM_RED ? true : false);
		_EndLuaFunction(L , 3);
	}
}

void UIHUDTabMinimap::_SetRoomLocation(i3::stack_wstring& wstrOut)
{
	i3_prof_func();

	// 서버
	i3::stack_wstring wstrServer;
	i3::sprintf(wstrServer, GAME_RCSTRING("STR_POPUP_WHAT_SERVER"), g_pServerInfo->GetCurGameServerIndex()+1);	

	// 채널
	i3::stack_wstring wstrChannel;
	i3::sprintf(wstrChannel, GAME_RCSTRING("STR_POPUP_WHAT_CHANNEL"), g_pServerInfo->GetCurChannelNumber()+1);

	// 방번호
	ROOM_INFO_BASIC roomInfo; MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);
	i3::stack_wstring wstrRoom;
	i3::sprintf(wstrRoom, GAME_RCSTRING("STR_POPUP_WHAT_ROOM"), roomInfo._Idx+1);

	i3::sprintf(wstrOut, L"%s %s %s", wstrServer, wstrChannel, wstrRoom);
}

void UIHUDTabMinimap::_UpdateUserInfo(bool BATTLE_EVNET_FLAG)
{	
	i3_prof_func();
	
	struct fn
	{
		// TabMiniMap 유저 정렬 조건
		// 1. Kill 많은 순
		// 2. Assist 많은 순
		// 3. Death 적은 순
		// 4. SlotIndex 순
		bool operator()(const INT32& l_slotIdx, const INT32& r_slotIdx)
		{

			if (tu::GetKillCount(l_slotIdx) > tu::GetKillCount(r_slotIdx)) return true;
			else if (tu::GetKillCount(l_slotIdx) == tu::GetKillCount(r_slotIdx))
			{
				if (tu::GetAssistCount(l_slotIdx) > tu::GetAssistCount(r_slotIdx)) return true;
				else if (tu::GetAssistCount(l_slotIdx) == tu::GetAssistCount(r_slotIdx))
				{
					if (tu::GetDeathCount(l_slotIdx) < tu::GetDeathCount(r_slotIdx)) return true;
					else if (tu::GetDeathCount(l_slotIdx) == tu::GetDeathCount(r_slotIdx))
					{
						if (l_slotIdx < r_slotIdx)
						{
							return true;
						}
					}
				}
			}
			return false;
		}
	};

	if(!BATTLE_EVNET_FLAG)
	{
		I3TRACE("############# Tabminimap ClearListView #############\n");
		ClearListView(static_cast<i3UIListView_Box*>(m_pBlueTeamListView));
		ClearListView(static_cast<i3UIListView_Box*>(m_pRedTeamListView));
		
		//1.0 Lua에만 있다. 현재는.
		if( m_bActivateMC )
		{
			LuaState * L = _CallLuaFunction( "MissionCardTitleInit");
			if( L != nullptr)
			{
				i3Lua::PushBoolean( L, true);
				_EndLuaFunction( L, 1);
			}
		}
	}

	for (INT32 t=0; t<TEAM_COUNT; t++) 
	{
		i3UIListView_Box * plvBox = static_cast<i3UIListView_Box*>(m_pRedTeamListView);
		if( t == 1)
			plvBox = static_cast<i3UIListView_Box*>(m_pBlueTeamListView);

		std::sort(m_ranking[t].begin(), m_ranking[t].end(), fn());


		INT32 lvSlot = 1;

		for (INT32 u=0; u<MaxTeamPlayer(t); u++)
		{
			INT32 slotIdx = m_ranking[t][u];
			if (tu::CanBeShown(slotIdx))
			{
				_SetIndivisualInfo(lvSlot, slotIdx);
				lvSlot++;
			}
		}
	}

	_UpdateMC();
}

void UIHUDTabMinimap::_SetIndivisualInfo(INT32 lvSlot, INT32 slotIdx)
{
	i3_prof_func();

	i3_prof_start("_SetIndivisualInfo 1st Block");

	i3UIListView_Box* lvBox = nullptr;

	if (TEAM_RED == tu::GetTeamType(slotIdx))
		lvBox = static_cast<i3UIListView_Box*>(m_pRedTeamListView);
	else
		lvBox = static_cast<i3UIListView_Box*>(m_pBlueTeamListView);

	// 셀렉션 표시
	if (BattleSlotContext::i()->getMySlotIdx() == slotIdx)
		lvBox->SelectData(lvSlot, true);

	i3_prof_end();

	i3_prof_start("_SetIndivisualInfo 2nd Block");

	// 클랜마크 표시
	SetCellClanMark(lvBox, lvSlot, 0, BattleSlotContext::i()->getSlotInfo(slotIdx)->_clanMark, cbSetClanTexturForTabMinimap);
	SetCellVisible(lvBox, lvSlot, 0, true);

	// rank
	char rank[256] = {0, };
	UINT8 ui8Rank = BattleSlotContext::i()->getSlotInfo(slotIdx)->_Rank;
	if( UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW)  && 
		g_pFramework->GetClanContext()->InBattleTeam())
	{
		if(ClanGameContext::i()->GetMercRank(slotIdx) != -1)
			ui8Rank = (UINT8)ClanGameContext::i()->GetMercRank(slotIdx);
	}

	lvBox->SetItemCell(lvSlot, 1, ::itoa(ui8Rank, rank, 10));
	SetCellVisible(lvBox, lvSlot, 1, true);

	// nick name
	PushLVBoxString(BattleSlotContext::i()->getNickForSlot(slotIdx), lvBox, lvSlot, 2);
	SetCellVisible(lvBox, lvSlot, 2, true);

	// kill / death
	INT32 numberOfKill = 0;
	INT32 numberOfDeath = 0;
	INT32 numberOfAssist = 0;
	hu::get_kill_death_assist(slotIdx, numberOfKill, numberOfDeath, numberOfAssist);

	lvBox->SetItemCell(lvSlot, 3, numberOfKill);
	SetCellVisible(lvBox, lvSlot, 3, true);

	lvBox->SetItemCell(lvSlot, 4, "/");
	SetCellVisible(lvBox, lvSlot, 4, true);

	lvBox->SetItemCell(lvSlot, 5, numberOfAssist);
	SetCellVisible(lvBox, lvSlot, 5, true);

	lvBox->SetItemCell(lvSlot, 6, "/");
	SetCellVisible(lvBox, lvSlot, 6, true);

	lvBox->SetItemCell(lvSlot, 7, numberOfDeath);
	SetCellVisible(lvBox, lvSlot, 7, true);

	// ping
	char ping[256] = { 0, };
	lvBox->SetItemCell(lvSlot, 8, /*"2"*/::itoa(tu::GetPing(slotIdx), ping, 10));
	SetCellVisible(lvBox, lvSlot, 8, true);

	i3_prof_end();

	i3_prof_start("_SetIndivisualInfo 3rd Block");

	// 방장 표시
	if (BattleSlotContext::i()->getMainSlotIdx() == slotIdx)
	{
		lvBox->SetItemCell(lvSlot, 9, "1");
		SetCellVisible(lvBox, lvSlot, 9, true);
	}
	else
	{
		lvBox->SetItemCell(lvSlot, 9, "-1");
		SetCellVisible(lvBox, lvSlot, 9, false);
	}

	i3_prof_end();

	i3_prof_start("_SetIndivisualInfo 4th Block");
	// 죽음 표시(텍스트 색상 변경)
	{
		COLOR colorOfKillDeath;
		COLOR colorOfNickName;

		if(tu::IsDead(slotIdx))
		{
			i3Color::Set(&colorOfKillDeath, (UINT8)100, (UINT8)100, (UINT8)100, (UINT8)255);
			i3Color::Set(&colorOfNickName, (UINT8)100, (UINT8)100, (UINT8)100, (UINT8)255);
		}
		else
		{
			i3Color::Set(&colorOfKillDeath, (UINT8)255, (UINT8)255, (UINT8)255, (UINT8)255);

			const UINT8 idxNickColor = BattleSlotContext::i()->getNickColorForSlot(slotIdx);
			const COLOR* pColor_UI = GetColorCashItem(idxNickColor);

			if (pColor_UI)
				i3Color::Copy(&colorOfNickName, pColor_UI);
			else
				i3Color::Set(&colorOfNickName, (UINT8)255, (UINT8)255, (UINT8)255, (UINT8)255);
		}

		lvBox->SetItemCellTextColor(lvSlot, 2, &colorOfNickName, false); SetCellVisible(lvBox, lvSlot, 2, true);
		lvBox->SetItemCellTextColor(lvSlot, 3, &colorOfKillDeath, false); SetCellVisible(lvBox, lvSlot, 3, true);

		for (INT32 i = 3; i < 8; i++)
		{
			lvBox->SetItemCellTextColor(lvSlot, i, &colorOfKillDeath, false); SetCellVisible(lvBox, lvSlot, i, true);
		}
	}

	i3_prof_end();
}

void UIHUDTabMinimap::_UpdateTeamScore()
{
	i3_prof_func();


	LuaState* L = _CallLuaFunction("SetTeamScore");						

	if(L != nullptr)
	{
		STAGE_MODE eStageType =  MyRoomInfoContext::i()->getStage()->GetStageMode();

		i3::stack_wstring wstrRedScore;
		i3::sprintf(wstrRedScore, L"%d %s", GetTeamScore(TEAM_RED), GetScoreText(eStageType));

		i3::stack_wstring wstrBlueScore;
		i3::sprintf(wstrBlueScore, L"%d %s", GetTeamScore(TEAM_BLUE), GetScoreText(eStageType));		

		i3Lua::PushStringUTF16To8(L , wstrRedScore);
		i3Lua::PushStringUTF16To8(L , wstrBlueScore);
		_EndLuaFunction(L , 2);
	}
}

void UIHUDTabMinimap::_UpdateRestTeamGoalInfo()
{	
	i3_prof_func();


	LuaState* L = _CallLuaFunction("SetRestTeamGoal");

	if (L != nullptr)
	{
		// 남은 조건 (Label : 남은 킬, 남은 라운드, 남은 점수)
		i3::stack_wstring wstrRestText;
		i3::sprintf(wstrRestText, GAME_RCSTRING("STR_HUD_INGAME_TABMINIMAP_REST"), GetScoreText(MyRoomInfoContext::i()->getStage()->GetStageMode()));

		// 남은 조건 수 (kill Count, Round Count, Score)
		i3::stack_wstring wstrRestCount;

		if((MyRoomInfoContext::i()->getStage()->IsAiMode()) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) )
		{
			i3::sprintf(wstrRestCount, L"~ %s", GetScoreText(MyRoomInfoContext::i()->getStage()->GetStageMode()));
		}
		else
		{
			INT32 i32RestScore = tu::GetRestTeamScore(tu::GetMyTeamType());
			i3::sprintf(wstrRestCount, L"%d %s", i32RestScore, GetScoreText(MyRoomInfoContext::i()->getStage()->GetStageMode()));
		}

		i3Lua::PushStringUTF16To8(L , wstrRestText);
		i3Lua::PushStringUTF16To8(L , wstrRestCount);

		_EndLuaFunction(L, 2);
	}
}

void UIHUDTabMinimap::_UpdateRespawn(INT32 slotIdx)
{
	i3_prof_func();


	TEAM_TYPE team = tu::GetTeamType(slotIdx);

	i3UIListView_Box* lvBox = nullptr;
	if (team == TEAM_RED) 
		lvBox = reinterpret_cast<i3UIListView_Box*>(m_pRedTeamListView);
	else
		lvBox = reinterpret_cast<i3UIListView_Box*>(m_pBlueTeamListView);

	for (INT32 i=0; i<MAX_TEAM_USER; i++)
	{
		if (m_ranking[team][i] == slotIdx)
		{
			COLOR colorOfKillDeath;
			i3Color::Set(&colorOfKillDeath, (UINT8)255, (UINT8)255, (UINT8)255, (UINT8)255);

			COLOR colorOfNickName;
			i3Color::Set(&colorOfNickName, (UINT8)255, (UINT8)255, (UINT8)255, (UINT8)255);

			const UINT8 idxNickColor = BattleSlotContext::i()->getNickColorForSlot(slotIdx);
			const COLOR* pColorNickName = GetColorCashItem(idxNickColor);
			if (pColorNickName)
				i3Color::Copy(&colorOfNickName, pColorNickName);

			lvBox->SetItemCellTextColor(i + 1, 2, &colorOfNickName); SetCellVisible(lvBox, i + 1, 2, true);

			for (INT32 j = 3; j < 8; j++)
			{
				lvBox->SetItemCellTextColor(i + 1, j, &colorOfKillDeath); SetCellVisible(lvBox, i + 1, j, true);
			}
		}
	}
}

void UIHUDTabMinimap::ProcessGameEvent(GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	// EVENT_START_BATTLE의 경우에는 Activate == false 여도 정보갱신을 해야합니다.
	if ((evt != EVENT_START_BATTLE && 
		evt != EVENT_M_ROUNDSTART_BATTLE &&
		!_IsActivated()) || 
		!_IsUserInfoUpdatable() || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL)) 
	{
		return;
	}

	UIHUDBase::ProcessGameEvent(evt, arg, UserData);

	switch( evt)
	{
	case EVENT_START_BATTLE:
	case EVENT_HOLEPUNCH_OK:
	case EVENT_GIVEUP_BATTLE:
	case EVENT_BATTLE_TOUCHDOWN:
	case EVENT_M_ROUNDEND_BATTLE:
	case EVENT_M_HOSTLOST_BATTLE:
	case EVENT_KICK_VOTE_RESULT:
	case EVENT_BATTLE_ESCAPEBLOCK:
		{
			_InitDefaultInfo();
			_UpdateUserInfo();
		}
		break;

	case EVENT_DEATH_BATTLE:
		{
			_UpdateUserInfo(true);
		}
		break;
	case EVENT_RESPAWN_BATTLE:
	case EVENT_RESPAWN_BATTLE_FOR_AI:
		{
			_UpdateRespawn(arg);
		}
		break;
	case EVENT_M_ROUNDSTART_BATTLE:		// 라운드 시작시 진행상황창의 기본 정보들을 재설정합니다. ( hansoft. 8424 )
		{
			_InitDefaultInfo();

			for (size_t i = 0; i<m_units.size(); i++)
				m_units[i]->OnStart();
		}

	default: break;
	}
}

void UIHUDTabMinimap::Show(bool activate)
{
	i3_prof_func();

	if (!_IsUserInfoUpdatable() || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
	{
		return;
	}
	
	if (_IsActivated() != activate)
	{
		LuaState* L = _CallLuaFunction("Enable");
		i3Lua::PushBoolean(L, activate);
		_EndLuaFunction(L, 1);
	
		_AttachToTopScreen();
		_UpdateUserInfo();
	
		if (!IsObserveState())
			EnableMinimap(activate);

		m_bActivate = !m_bActivate;
	}
}

void UIHUDTabMinimap::cbSetClanTexturForTabMinimap(i3UIListView_Cell * pCell, void * pData)
{
	i3_prof_func();

	I3ASSERT( pCell != nullptr);
	if( pCell->getCellChild() != nullptr)
	{
		i3UIControl * pCtrl = pCell->getCellChild();

		if( i3::same_of<i3UIImageBoxEx*>(pCtrl))
		{
			UINT32 markFlag = GameUI::GetClanMarkFlag((char *)pData);
			GameUI::SetClanTextureAndCenterPos( (i3UIImageBoxEx*) pCtrl, pCell, CLAN_MARK_SMALL, markFlag, 20.0f);
		}
	}
}

void	UIHUDTabMinimap::_InitMC(i3UIScene* scene)
{
	if(!m_bActivateMC)
		return;

	char ctemp[MAX_STRING_COUNT] = {};
	i3UIStaticText*		static_text_temp;
	i3UIButtonImageSet* button_image_temp;
	for(int i = 0 ; i < MAX_QUEST_PER_MISSON_CARD ; i++ )
	{
		static_text_temp	= nullptr; 
		button_image_temp   = nullptr;
		
		i3::snprintf(ctemp,MAX_STRING_COUNT,"Contents_%d",i);
		static_text_temp = reinterpret_cast<i3UIStaticText*>(scene->FindChildByName(ctemp));
		i3::snprintf(ctemp,MAX_STRING_COUNT,"ClearImgBtn_%d",i);
		button_image_temp = reinterpret_cast<i3UIButtonImageSet*>(scene->FindChildByName(ctemp));

		m_MC[i].MC_Contents    = static_text_temp;
		m_MC[i].MC_ClearButton = button_image_temp;
	}
}

void	UIHUDTabMinimap::_UpdateMC()
{
	if(!m_bActivateMC)
		return;

	const MCard* pCard = MCardUtil::GetSelectedMCard();
	if (pCard == nullptr)
		return;
	
	INT32 count = 0;
	for (INT32 i = 0 ; i < MAX_QUEST_PER_CARD_NEW ; ++i)
	{
		const MCardQuest* pQuest = pCard->GetMCardQuest(i);

		if (!pQuest->IsActive() )
			continue;

		INT32 tgtCount = pQuest->GetMCardQuestInfo()->m_TargetCompleteCount;
		INT32 curCount = pQuest->GetCurrCompleteCount_OnBattle();

		i3::rc_wstring wstrDesc = pQuest->GetCurrHUDDesc(false);
		
		if(m_MC[count].MC_ClearButton)
			m_MC[count].MC_ClearButton->SetVisible( curCount >= tgtCount );
		if(m_MC[count].MC_Contents)
			m_MC[count].MC_Contents->SetText( wstrDesc);

		++count;
	}
}