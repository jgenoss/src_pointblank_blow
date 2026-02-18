#include "pch.h"
#include "UIHudGameResult_V10.h"
#include "../UI/UIHudDefine.h"
#include "../UI/UIHudUtil.h"
#include "../UI/UIBase.h"
#include "../UI/UIHudManager.h"
#include "../UI/UIUtil.h"
#include "MedalManager.h"
#include "GameStateMgr.h"
#include "MyRoomInfoContext.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/itoa.h"
#include "i3Base/ColorString.h"

#include "MissionCard/MCardUtil.h"
#include "MissionCard/MCard.h"
#include "MissionCard/MCardQuest.h"
#include "MissionCard/MCardQuestInfo.h"

UIHudGameResult_V10::UIHudGameResult_V10(UIHudManager* p) : UIHudGameResult(p)
{

}

UIHudGameResult_V10::~UIHudGameResult_V10()
{
}


void UIHudGameResult_V10::InitializeAtLoad( i3UIScene * pScene)
{
	iHudBase::AddFrameWnd(pScene, "i3UIFrameWnd7");

	result::state result_state = result::rs_end;

	bool isctr1 = i3::generic_is_iequal(pScene->getScriptFile(), i3::string("script\\pbre_game_round_result.lua"));

	if( isctr1 == true )
		result_state = result::rs_round;

	team_list_view[result_state][TEAM_RED] = FindControl<i3UIListView_Box*>(pScene, "i3UIListView_Red");
	team_list_view[result_state][TEAM_BLUE] = FindControl<i3UIListView_Box*>(pScene, "i3UIListView_Blue");

	//result::win_count
	win_count[result_state].text[TEAM_RED] = FindControl<i3UIStaticText*>(pScene, "i3UIStaticTextRed");
	win_count[result_state].text[TEAM_BLUE] = FindControl<i3UIStaticText*>(pScene, "i3UIStaticTextBlue");

	if( result_state == result::rs_round )
	{
		QuestMatArray[0] = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText1");
		QuestMatArray[1] = FindControl<i3UIImageBox*>(pScene, "i3UIImageBox0");

		QuestMatArray[2] = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText3");
		QuestMatArray[3] = FindControl<i3UIImageBox*>(pScene, "i3UIImageBox2");

		QuestMatArray[4] = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText5");
		QuestMatArray[5] = FindControl<i3UIImageBox*>(pScene, "i3UIImageBox4");

		QuestMatArray[6] = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText8");
		QuestMatArray[7] = FindControl<i3UIImageBox*>(pScene, "i3UIImageBox6");

		return ;
	} //round 결과창은 여기서 끝....

	win_count[result_state].img[TEAM_RED] = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet3");
	win_count[result_state].img[TEAM_BLUE] = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet4");


	//result::report
	report.mvp = FindControl<i3UIStaticText*>(pScene, "Text_MVP");
	report.best_assistor = FindControl<i3UIStaticText*>(pScene, "Text_BestAssistor");
	report.top_kill_user = FindControl<i3UIStaticText*>(pScene, "Text_TopKiller");
	report.kill_grade = FindControl<i3UIStaticText*>(pScene, "Text_Kill_Grade");
	report.assist_grade = FindControl<i3UIStaticText*>(pScene, "Text_Assist_Grade");
	report.max_kill_count = FindControl<i3UIStaticText*>(pScene, "Text_Max_ChainKill");
	report.headshot_count = FindControl<i3UIStaticText*>(pScene, "Text_HeadShotKill");
	report.max_be_shot_user = FindControl<i3UIStaticText*>(pScene, "Text_beShot_User");

	//result::benefit_exp
	benefit_exp.mark[result::Benefit_PC].img = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet11");
	benefit_exp.mark[result::Benefit_PC].text = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText2");
	benefit_exp.mark[result::Benefit_Event].img = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet0");
	benefit_exp.mark[result::Benefit_Event].text = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText12");
	benefit_exp.mark[result::Benefit_Item].img = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet1");
	benefit_exp.mark[result::Benefit_Item].text = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText13");
	benefit_exp.exp = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText1");

	//result::rank
	rank.wnd[result::CurrRank] = FindControl<i3UIFrameWnd*>(pScene, "i3UIFrameWnd5");
	rank.img[result::CurrRank] = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet5");
	rank.wnd[result::NextRank] = FindControl<i3UIFrameWnd*>(pScene, "i3UIFrameWnd6");
	rank.img[result::NextRank] = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet2");
	rank.progress = FindControl<i3UIProgressBar*>(pScene, "i3UIProgressBar");
	rank.text = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText16");

	
	
}



void UIHudGameResult_V10::SetParameter(INT32 arg1, INT32 arg2)
{

	switch( (HUD::GameResultState)arg1 )
	{
	case HUD::GRS_Ok:
		{

			gstatemgr::i()->setEndTime(100.f); 
			break;
		}
	case HUD::GRS_ScreenSave: g_pFramework->SetGameKey(1, GAME_KEY_MAP_SCREENSHOT); break;
	case HUD::GRS_WinTeamState: if( arg2 == 0 || arg2 == 1) result_state = (ResultWinState)arg2; else result_state = RS_DRAW; break;
	case HUD::GRS_UpdateKillCount: kill_record.update_kill_count(); break;
	case HUD::GRS_UserRoomIn: case HUD::GRS_UserRoomOut: kill_record.reset_user_killed_count(arg2); break;
	case HUD::GRS_HeadShot: ++headshot_count; break;
	case HUD::GRS_UserKilled: kill_record.increase_user_killed_count(arg2); kill_record.increase_kill_count(); break;
	case HUD::GRS_RoundShow: update_result_date(result::rs_round); break;
	case HUD::GRS_EndShow: 

		update_result_date(result::rs_end);
		break;
	}
}

void UIHudGameResult_V10::update_result_date(result::state state)
{
	if( is_setted_up == true ) return; //여러번 호출 방지 코드
	is_setted_up = true;

	m_parent->SetRespawn(true);

	iHudBase::SetEnable(true);
	iHudBase::SetVisibleFrame(false, (state == result::rs_end) ? result::rs_round : result::rs_end );

	INT32 red_count = 0, blue_count = 0;
	hu::get_team_score(red_count, blue_count);

	i3::vector< ResultState::UserInform > list[TEAM_COUNT];
	hu::get_ingame_result_inform(list[TEAM_RED], list[TEAM_BLUE]);

	if( state == result::rs_round)
		update_win_count_only_text(win_count[state], red_count, blue_count);
	else
		update_win_count(win_count[state], red_count, blue_count, list[TEAM_RED].size(), list[TEAM_BLUE].size());

	//킬, 데스, point, exp, nick name 정렬
	hu::sort_ingame_result_inform(list[TEAM_RED]);
	hu::sort_ingame_result_inform(list[TEAM_BLUE]);

	if( state == result::rs_round)
		update_round_user_list(list);
	else 
		update_end_user_list(list);

	update_clan_mark(list, team_list_view[state]);

	//중간 결과창 임무 Clear 리스트
	if( state == result::rs_round )
	{
		MCard* pMCard = MCardUtil :: GetSelectedMCard();

		if( pMCard != nullptr)
		{
			for (int i = 0; i < 4; ++i)
			{
				MCardQuest *pMCardQuest = pMCard->GetMCardQuest(i);
				if (!pMCardQuest)
					break;
				((i3UIStaticText*)QuestMatArray[i * 2])->SetText(pMCardQuest->GetCurrHUDDesc(false));
				if (pMCardQuest->GetCurrCompleteCount_OnBattle() >= pMCardQuest->GetMCardQuestInfo()->m_TargetCompleteCount)
					((i3UIImageBox*)QuestMatArray[i * 2 + 1])->SetVisible(true);
				else
					((i3UIImageBox*)QuestMatArray[i * 2 + 1])->SetVisible(false);
			}
		}

		return;
	}

	//top exp user (MVP)
	i3::wstring top_exp_nick; INT32 top_exp_point = 0;
	get_top_exp_user(list, top_exp_nick, top_exp_point);

	//best assistor
	i3::wstring best_assistor_nick;
	get_best_assistor(list, best_assistor_nick);

	//top kill user (최대 멀티 킬)
	INT32 top_kill_count = multi_kill_info.m_ui8MaxMultiKillCount;
	i3::vector<INT32> kill_user_idx_list;
	calc_top_killer(kill_user_idx_list, multi_kill_info.m_ui16MultiKillUser);

	i3::wstring top_kill_nick; 
	if( kill_user_idx_list.empty() == false )
	{
		ResultState::UserInform* kill_user_inform = find_user_inform(list, kill_user_idx_list.front() );
		if(kill_user_inform != 0)
		{
			to_plural_user_string(top_kill_nick, kill_user_inform->wstr_nick, kill_user_idx_list.size(), white_color);
		}
	}

	//my information
	ResultState::UserInform* my_inform = find_my_inform(list);
	if( my_inform == 0 ) //이런 일이 안 생길거라 생각되는데 자꾸 생긴다..
	{
		hu::get_ingame_result_inform(list[TEAM_RED], list[TEAM_BLUE]); //다시 정보를 얻자.

		my_inform = find_my_inform(list);
		if( my_inform == 0 )
		{
			my_inform = &default_user_inform; //여기는 진짜 오면 안 된다.
		}
	}

	// last record
	ROOM_INFO_BASIC pBasic;
	MyRoomInfoContext::i()->getMyRoomInfo(&pBasic);

	CCharaInfoContext::i()->setMyLastRecord(
		true, hu::is_my_team_win(),
		(UINT8)StageID::GetStageMode(pBasic._StageID), pBasic._StageID,
		my_inform->kill_count, my_inform->death_count,
		my_inform->exp, my_inform->point, 0, 0);

	//kill grade (킬 순위)
	INT32 kill_grade = calc_kill_grade(list, my_inform->kill_count);

	//assist grade (어시스트 순위)
	INT32 assist_grade = calc_assist_grade(list, my_inform->assist_count);

	//killed (최대 연속 킬)
	INT32 max_kill_count = kill_record.get_max_kill_count();

	//최대 피격 유저
	INT32 killed_count = 0;
	i3::vector<INT32> killed_user_idx_list = kill_record.get_top_user_killed_idx(killed_count);

	i3::wstring max_killed_nick;
	max_killed_nick_string(max_killed_nick, killed_user_idx_list, list);

	//표시
	update_report(report, top_exp_nick, top_exp_point, best_assistor_nick, top_kill_nick, top_kill_count, kill_grade, assist_grade, max_kill_count, headshot_count,
		max_killed_nick, killed_count); 

	//혜택 표시
	INT32 total_exp = my_inform->exp + my_inform->total_bonus_exp;
	update_benefit_exp(my_inform->idx, benefit_exp, my_inform->result_mark, total_exp);

	//계급 표시
	update_rank(rank, rank_at_start);


	
}

void UIHudGameResult_V10 :: update_end_user_list(const i3::vector< ResultState::UserInform > list[TEAM_COUNT])
{
	i3::rc_wstring wstr_nick;

	for(size_t i=0;i<TEAM_COUNT;++i)
	{
		{
			LuaState * L = UILua::CallLuaFunction(HUD::instance()->end_result_scene_info.scn, "ClearUserList");
			i3Lua::PushInteger( L, i); //list_idx
			UILua::EndLuaFunction(L , 1);
		}

		size_t size = list[i].size();
		for(size_t j=0;j<size;++j)
		{
			LuaState * L = UILua::CallLuaFunction(HUD::instance()->end_result_scene_info.scn, "SetUserList");

			i3Lua::PushInteger( L, i); //list_idx
			i3Lua::PushBoolean( L, list[i][j].is_my_inform); //isMyChara
			i3Lua::PushInteger( L, j); //slot
			i3Lua::PushInteger( L, list[i][j].clan_mark); //clan
			i3Lua::PushInteger( L, list[i][j].rank); //rank

			wstr_nick = list[i][j].wstr_nick;
			const I3COLOR * pNickColor = GameUI::GetNickNameColor( list[i][j].nick_color );
			::ColorString(wstr_nick, (UINT32)pNickColor->r, (UINT32)pNickColor->g, (UINT32)pNickColor->b, (UINT32)pNickColor->a );
			i3Lua::PushStringUTF16To8( L, wstr_nick); //nick

			i3Lua::PushInteger( L, list[i][j].kill_count);		// kill
			i3Lua::PushInteger( L, list[i][j].death_count);		// death
			i3Lua::PushInteger( L, list[i][j].assist_count);	// assist
			i3Lua::PushInteger( L, list[i][j].score);

			INT32 total_exp = list[i][j].exp + list[i][j].total_bonus_exp;
			INT32 total_point = list[i][j].point + list[i][j].total_bonus_point;
			i3Lua::PushInteger( L, total_exp);		//experience
			i3Lua::PushInteger( L, total_point);	//point

			i3Lua::PushInteger( L, list[i][j].QuestEndFlag ? 0 : -1);

			i3Lua::PushInteger( L, hu::get_benefit_mark(list[i][j].result_mark, RESULT_ICON_PC_ALL) );	//bonusPC
			i3Lua::PushInteger( L, hu::get_benefit_mark(list[i][j].result_mark, RESULT_ICON_EVENT) );	//bonusEvent
			i3Lua::PushInteger( L, hu::get_benefit_mark(list[i][j].result_mark, RESULT_ICON_ITEM) );	//bonusItem

			UILua::EndLuaFunction(L , 16);
		}
	}
}