#include "pch.h"
#include "UIPopupClanSeason_IndivisualRanking.h"
#include "UIUtil.h"

#include "ClanSeasonContext.h"

extern "C" 
{
	int ClanSeasonIndivisualRanking_OKBtnClicked(LuaState* L)
	{
		GameUI::ClosePopup(UPW_CLAN_SEASON_INDIVISUAL_RANKING);
		CLAN_PRESEASON_INFO *pInfo = const_cast<CLAN_PRESEASON_INFO *>(ClanSeasonContext::i()->GetPreSeasonInfo());
		pInfo->Reset();
		return 0;
	}
}

LuaFuncReg ClanSeasonIndivisualRanking_Glue[] =
{
	{"Click_OK", ClanSeasonIndivisualRanking_OKBtnClicked},
	{nullptr, nullptr}
};

I3_CLASS_INSTANCE(UIPopupClanSeason_IndivisualRanking);

UIPopupClanSeason_IndivisualRanking::UIPopupClanSeason_IndivisualRanking()
{
}

UIPopupClanSeason_IndivisualRanking::~UIPopupClanSeason_IndivisualRanking()
{
}

void UIPopupClanSeason_IndivisualRanking::OnCreate(i3GameNode* parent)
{
	UIPopupBase::OnCreate(parent);

	AddScene("Scene/Popup/PBRe_PopUp_ClanSeason_IndivisualRanking.i3UIs", ClanSeasonIndivisualRanking_Glue, true, false);
}

bool UIPopupClanSeason_IndivisualRanking::OnEntranceStart(void* pArg1, void* pArg2)
{
	if (UIPopupBase::OnEntranceStart(pArg1, pArg2) == false) return false;

	LuaState* L = _CallLuaFunction("SetResult");
	if (L != nullptr)
	{
		i3::rc_wstring wstr_season_name = ClanSeasonContext::i()->GetPreSeasonName();
		const CLAN_PRESEASON_RANK_INFO * pInfo = ClanSeasonContext::i()->GetMyClanPreSeasonInfo();

		i3::rc_wstring wstr_clan_name = L"";
		wstr_clan_name = pInfo->_ClanInfo._name;
		INT32 ranking = pInfo->_Ranking;

		i3Lua::PushInteger(L, ranking);								// 순위
		i3Lua::PushStringUTF16To8(L, wstr_season_name.c_str());		// 클랜시즌명
		i3Lua::PushStringUTF16To8(L, wstr_clan_name.c_str());		// 클랜명

		_EndLuaFunction(L, 3);
	}

	return true;
}

