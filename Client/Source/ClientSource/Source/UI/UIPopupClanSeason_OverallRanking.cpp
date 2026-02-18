#include "pch.h"
#include "UIPopupClanSeason_OverallRanking.h"
#include "UIUtil.h"
#include "ClanSeasonContext.h"

extern "C" 
{
	int ClanSeasonOverallRanking_OKBtnClicked(LuaState* L)
	{
		GameUI::ClosePopup(UPW_CLAN_SEASON_OVERALL_RANKING);
		CLAN_PRESEASON_INFO *pInfo = const_cast<CLAN_PRESEASON_INFO *>( ClanSeasonContext::i()->GetPreSeasonInfo());
		pInfo->Reset();
		return 0;
	}
}

LuaFuncReg ClanSeasonOverallRanking_Glue[] =
{
	{"Click_OK", ClanSeasonOverallRanking_OKBtnClicked},
	{nullptr, nullptr}
};

I3_CLASS_INSTANCE(UIPopupClanSeason_OverallRanking);

UIPopupClanSeason_OverallRanking::UIPopupClanSeason_OverallRanking()
{

}

UIPopupClanSeason_OverallRanking::~UIPopupClanSeason_OverallRanking()
{

}

void UIPopupClanSeason_OverallRanking::OnCreate(i3GameNode* parent)
{
	UIPopupBase::OnCreate(parent);

	AddScene("Scene/Popup/PBRe_PopUp_ClanSeason_OverallRanking.i3UIs", ClanSeasonOverallRanking_Glue, true, false);
}

bool UIPopupClanSeason_OverallRanking::OnEntranceStart(void* pArg1, void* pArg2)
{
	if (UIPopupBase::OnEntranceStart(pArg1, pArg2) == false) return false;

	LuaState* L = _CallLuaFunction("SetResult");
	if ( L != nullptr )
	{
		i3::rc_wstring wstr_season_name = ClanSeasonContext::i()->GetPreSeasonName();		
		i3::rc_wstring wstr_winner_clan[MAX_SEASON_RANKING_TEAM_COUNT];

		for(INT32 idx = 0 ; idx < MAX_SEASON_RANKING_TEAM_COUNT ; ++idx)
		{
			const CLAN_PRESEASON_RANK_INFO * pInfo =  ClanSeasonContext::i()->GetClanPreSeasonInfo(idx + 1);
			if (pInfo != nullptr )
				wstr_winner_clan[idx] = pInfo->_ClanInfo._name;
			else
				wstr_winner_clan[idx] = L"";

		}

		i3Lua::PushStringUTF16To8(L, wstr_season_name.c_str() );	// 클랜시즌 명
		i3Lua::PushStringUTF16To8(L, wstr_winner_clan[0].c_str() );	// 1위 클랜의 이름
		i3Lua::PushStringUTF16To8(L, wstr_winner_clan[1].c_str() );	// 2위 ...
		i3Lua::PushStringUTF16To8(L, wstr_winner_clan[2].c_str() );	// 3위 ...

		_EndLuaFunction(L, 4);
	}

	return true;
}