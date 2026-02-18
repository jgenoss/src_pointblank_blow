#include "pch.h"
#include "UIHudDomi_WaveResult.h"
#include "BattleSlotContext.h"
#include "../../DomiContext.h"

I3_CLASS_INSTANCE(CUIHudDomi_WaveResult);

CUIHudDomi_WaveResult::CUIHudDomi_WaveResult()
{
}

CUIHudDomi_WaveResult::~CUIHudDomi_WaveResult()
{
}

void CUIHudDomi_WaveResult::OnCreate(i3GameNode* parent)
{
	UIHUDDominationBase::OnCreate(parent);

	AddScene("Scene/Domination/PBRe_Domi_middlescore.i3UIs", nullptr, true, false); //false, true);
}

void CUIHudDomi_WaveResult::_InitializeAtLoad(i3UIScene* scene)
{
	UIHUDBase::_InitializeAtLoad(scene);
}

void CUIHudDomi_WaveResult::OnUpdate(REAL32 tm)
{
	UIHUDDominationBase::OnUpdate(tm);
}

bool CUIHudDomi_WaveResult::OnEntranceStart(void* arg1, void* arg2)
{
	if (UIHUDDominationBase::OnEntranceStart(arg1, arg2) == false)
		return false;

	i3::vector<DomiDefs::WAVE_RESULT_INFO> results;
	results.reserve(MAX_COUNT_DOMI_USER_REAL);

	for (INT32 i=0; i<MAX_COUNT_DOMI_USER_REAL; i++)
	{
		const DomiDefs::WAVE_RESULT_INFO& personalResult = DomiContext::i()->GetWaveResult(i);
		results.push_back(personalResult);
	}

	struct fn 
	{
		bool operator()(const DomiDefs::WAVE_RESULT_INFO& lhs, const DomiDefs::WAVE_RESULT_INFO& rhs) const
		{
			if (lhs.base.score > rhs.base.score) return true;
			else if (lhs.base.score == rhs.base.score)
			{
				if (lhs.base.kill < rhs.base.kill) return true;
			}
			return false;
		}
	};
	i3::sort(results.begin(), results.end(), fn());

	for (size_t i=0; i<results.size(); i++)
	{
		bool playing = ((results[i].base.kill == -1) || (results[i].base.score == -1) ? false : true);
		if (playing)
		{
			LuaState* L = _CallLuaFunction("SetScore");
			i3Lua::PushInteger(L, i);
			i3Lua::PushInteger(L, BattleSlotContext::i()->getSlotInfo(results[i].slotIdx)->_clanMark);
			i3Lua::PushInteger(L, BattleSlotContext::i()->getSlotInfo(results[i].slotIdx)->_Rank);
			i3Lua::PushStringUTF16To8(L, BattleSlotContext::i()->getNickForSlot(results[i].slotIdx));
			i3Lua::PushInteger(L, results[i].base.kill);
			i3Lua::PushInteger(L, results[i].base.score);
			_EndLuaFunction(L, 6);
		}
		else
		{
			LuaState* L = _CallLuaFunction("ClearSlot");
			i3Lua::PushInteger(L, i);
			_EndLuaFunction(L, 1);
		}
	}

	results.clear();

	return true;
}

void CUIHudDomi_WaveResult::OnEntranceEnd()
{
	UIHUDDominationBase::OnEntranceEnd();
}