#include "pch.h"

//#ifdef	DOMI_ROUND_RESULT

#include "UIHudDomi_RoundResult.h"
#include "../DigitNumber.h"
#include "../DigitNumberFactory.h"
#include "../GameStateMgr.h"
#include "../Mode/Domination/GameMode_Domination.h"
#include "../UIUtil.h"
#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "../../DomiContext.h"

#include "ClientStageInfoUtil.h"

I3_CLASS_INSTANCE(CUIHudDomi_RoundResult);

// 기획문서 참조: http://tiki.fps-pb.com/tiki-download_wiki_attachment.php?attId=3422

#define MAX_TEAM_USER_DOMI 4
// 라운드 클리어 시 포인트 300, 경험치 100 고정으로 지급
#define REWARD_EXPERIENCE_VALUE 150
#define REWARD_POINT_VALUE	  250

CUIHudDomi_RoundResult::CUIHudDomi_RoundResult() 
	: m_scoreWindow(nullptr), m_totalScore(nullptr), m_slotBorderLine(nullptr)
{
	m_results.reserve(MAX_COUNT_DOMI_USER_REAL);
}

CUIHudDomi_RoundResult::~CUIHudDomi_RoundResult()
{
	m_slotBorderLine = nullptr;

	I3_SAFE_DELETE(m_totalScore);

	m_results.clear();
}

void CUIHudDomi_RoundResult::OnCreate(i3GameNode* parent)
{
	UIHUDDominationBase::OnCreate(parent);

	AddScene("Scene/Domination/PBRe_Domi_EndWindow.i3UIs", nullptr, true, false);
}

void CUIHudDomi_RoundResult::_InitializeAtLoad(i3UIScene* scene)
{
	UIHUDBase::_InitializeAtLoad(scene);

	m_scoreWindow = static_cast<i3UIFrameWnd*>(scene->FindChildByName("ScoreFrameWindow"));
	m_slotBorderLine = static_cast<i3UIFrameWnd*>(scene->FindChildByName("SlotBorderLine"));
}

bool CUIHudDomi_RoundResult::OnEntranceStart(void* arg1, void* arg2)
{
	if (UIHUDDominationBase::OnEntranceStart(arg1, arg2) == false) return false;

	_SetupContainer();
	_SetFlowInfo();
	_SetPlayResult();
	_SetScores();
	_SetRankUp();
	_SetRewardItem();

	return true;
}

void CUIHudDomi_RoundResult::OnEntranceEnd()
{
	UIHUDDominationBase::OnEntranceEnd();
}

bool CUIHudDomi_RoundResult::OnExitStart()
{
	UIHUDDominationBase::OnExitStart();

	I3_SAFE_DELETE(m_totalScore);

	return true;
}

void CUIHudDomi_RoundResult::OnUpdate(REAL32 tm)
{
	UIHUDDominationBase::OnUpdate(tm);

	if (m_results.empty()) return;

	if (m_totalScore)
		m_totalScore->Update(tm);
}	

void CUIHudDomi_RoundResult::_SetupContainer()
{
	if (!m_results.empty())
		m_results.clear();

	for (INT32 i=0; i<MAX_COUNT_DOMI_USER_REAL; i++)
	{
		const DomiDefs::ROUND_RESULT_INFO& personalResult = DomiContext::i()->GetRoundResult(i);
		m_results.push_back(personalResult);
	}

	struct fn 
	{
		bool operator()(const DomiDefs::ROUND_RESULT_INFO& lhs, const DomiDefs::ROUND_RESULT_INFO& rhs) const
		{
			if (lhs.base.score > rhs.base.score) return true;
			else if (lhs.base.score == rhs.base.score)
			{
				if (lhs.base.kill < rhs.base.kill) return true;
			}
			return false;
		}
	};
	i3::sort(m_results.begin(), m_results.end(), fn());
}

void CUIHudDomi_RoundResult::_SetFlowInfo()
{
	i3::rc_wstring stage_name = CStageInfoUtil::GetMapData((UINT8)MyRoomInfoContext::i()->getStage()->GetStageUID())->getDisplayName();

	LuaState* L = _CallLuaFunction("SetFlowInfo");
	i3Lua::PushInteger(L, domi_state_info::i()->round);
	i3Lua::PushStringUTF16To8(L, stage_name);			// TODO : 서버 협업 후 변경 (2014.07.23.수빈)
	_EndLuaFunction(L, 2);
}

void CUIHudDomi_RoundResult::_SetPlayResult()
{
	for (size_t i=0; i<m_results.size(); i++)
	{
		INT32 slotIdx = m_results[i].slotIdx;
		bool playing = ((m_results[i].base.kill == -1) || (m_results[i].base.score == -1) ? false : true);

		if (playing)
		{
			LuaState* L = _CallLuaFunction("SetPlayResult");

			i3Lua::PushBoolean(L, ((BattleSlotContext::i()->getMySlotIdx() == slotIdx)));
			i3Lua::PushInteger(L, i);
			i3Lua::PushInteger(L, BattleSlotContext::i()->getSlotInfo(slotIdx)->_Rank);
			i3Lua::PushInteger(L, BattleSlotContext::i()->getSlotInfo(slotIdx)->_clanMark);
			i3Lua::PushStringUTF16To8(L, BattleSlotContext::i()->getNickForSlot(slotIdx));
			i3Lua::PushInteger(L, m_results[i].base.kill);
			i3Lua::PushInteger(L, m_results[i].base.score);
			i3Lua::PushInteger(L, m_results[i].exp);
			i3Lua::PushInteger(L, m_results[i].point);
			i3Lua::PushBoolean(L, ((m_results[i].icon & RESULT_ICON_PC) || (m_results[i].icon & RESULT_ICON_PC_PLUS)) != 0);
			i3Lua::PushBoolean(L, (m_results[i].icon & RESULT_ICON_EVENT) != 0);
			i3Lua::PushBoolean(L, (m_results[i].icon & RESULT_ICON_ITEM) != 0);

			_EndLuaFunction(L, 12);
		}
		else
		{
			LuaState* L = _CallLuaFunction("ClearPlayResultSlot");
			i3Lua::PushInteger(L, i);
			_EndLuaFunction(L, 1);
		}

		if (slotIdx == BattleSlotContext::i()->getMySlotIdx())
		{
			REAL32 pos[MAX_TEAM_USER_DOMI] = {139.0f, 163.0f, 187.0f, 219.0f};
			m_slotBorderLine->setPos(8.0f, pos[i]);
		}
	}
}

void CUIHudDomi_RoundResult::_SetScores()
{
	for (size_t i=0; i<m_results.size(); i++)
	{
		if (m_results[i].slotIdx == BattleSlotContext::i()->getMySlotIdx())
		{
			INT32 totalScore = m_results[i].base.score;
			delete m_totalScore;
			m_totalScore = DigitNumberFactory::MakeDomiEndResultTotalScore(m_scoreWindow, totalScore);

			LuaState* L = _CallLuaFunction("SetRankingGrade");
			i3Lua::PushStringUTF16To8(L, domi_util::round_grade_result());
			_EndLuaFunction(L, 1);
		}
	}
}

// UIExitPopup.cpp SetLabel2()에서 따옴
void CUIHudDomi_RoundResult::_SetRankUp()
{
	float fExpRate = 100.0;
	INT32 wRank = UserInfoContext::i()->GetRealRank();
	USER_INFO_BASIC MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

	INT32 nCurrentExp = MyUserInfoBasic.m_ui32Exp - _GetExpMax(wRank);
	INT32 nExpRate = 100;
	INT32 nMaxExp = _GetExpMax(wRank + 1) - _GetExpMax(wRank);			

	nCurrentExp = MAX(nCurrentExp, 0);	// 음수값은 0
	if( nMaxExp != 0) nExpRate = (nCurrentExp * 100) / nMaxExp;

	nExpRate = MINMAX(0, nExpRate, 100);	// 100%가 넘을 경우 100%으로 고정 출력
	fExpRate = static_cast<REAL32>(nExpRate);
	if( 45 < wRank )
		fExpRate = 100.0;

	INT32 nextRank; // = wRank;
	if (wRank == RANK_51)
		nextRank = -1;
	else
		nextRank = wRank+1;

	LuaState* L = _CallLuaFunction("SetRankUp");
	i3Lua::PushInteger(L, wRank);
	i3Lua::PushInteger(L, nextRank);
	i3Lua::PushNumber(L, fExpRate);
	_EndLuaFunction(L, 3);
}

INT32 CUIHudDomi_RoundResult::_GetExpMax(INT32 rank)
{
	I3ASSERT(rank >= 0);
	I3ASSERT((rank >= 0 && rank < RANK_NEWBIE) || GameUI::is_over_hero_rank(rank));

	if (rank >= 0 && rank < RANK_51+1)
		return g_ExpValueV2[rank];
	else
		return 0;
}

void CUIHudDomi_RoundResult::_SetRewardItem()
{
	LuaState* L = _CallLuaFunction("SetRewardItem");

	i3Lua::PushInteger(L, domi_util::reward_item_image_Idx());
	i3Lua::PushStringUTF16To8(L, domi_util::reward_item_image_name());

	_EndLuaFunction(L, 2);
}
//#endif

//#endif