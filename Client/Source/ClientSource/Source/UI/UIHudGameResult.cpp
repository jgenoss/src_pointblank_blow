#include "pch.h"
#include "UIHudGameResult.h"

#include "UIHUDIngameChat.h"
#include "UIHudUtil.h"
#include "UI/UIUtil.h"
#include "UIHudManager.h"
#include "GameStateMgr.h"

#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "BattleResultContext.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/itoa.h"
#include "i3Base/ColorString.h"
#include <functional>
#include <iterator>

/*********************************/
// namespace result::kill_record
i3::vector<INT32> result::kill_record::get_top_user_killed_idx(INT32 & killed_count)
{
	i3::vector<INT32> ret;

	killed_count = 0;
	for(INT32 i=0; i<SLOT_MAX_COUNT;++i)
	{
		if( killed_count < killed_info[i] )
			killed_count = killed_info[i];
	}

	if( killed_count == 0 ) return ret;

	for(INT32 i=0; i<SLOT_MAX_COUNT;++i)
	{
		if( killed_count == killed_info[i] )
			ret.push_back(i);
	}
	return ret;
}

void result::kill_record::update_kill_count()
{
	if( max_kill_count < curr_kill_count ) 
		max_kill_count = curr_kill_count;

	curr_kill_count = 0;
}

/*********************************/
UIHudGameResult::UIHudGameResult(UIHudManager* p) : iHudBase(p)
{
	m_name = "result";

	headshot_count = 0;
	is_setted_up = false;

	default_user_inform.clan_idx = default_user_inform.clan_mark = 0;
	default_user_inform.idx = default_user_inform.score, default_user_inform.rank = 0;
	default_user_inform.point = 0; default_user_inform.exp = 0;
	default_user_inform.kill_count = default_user_inform.death_count = default_user_inform.assist_count = 0;
	default_user_inform.mission = default_user_inform.result_mark = default_user_inform.nick_color = default_user_inform.e_sport_mark = 0;
	default_user_inform.is_challenge = default_user_inform.is_my_inform = false;

	i3Color::Set(&red_color, (UINT8)237, 28, 36, 255);
	i3Color::Set(&blue_color, (UINT8)0, 162, 232, 255);
	i3Color::Set(&white_color, (UINT8)255,255,255, 255);
}

UIHudGameResult::~UIHudGameResult()
{
}

/*********************************/
void UIHudGameResult::InitializeAtLoad( i3UIScene * pScene)
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

	if(result_state == result::rs_round) return; //round 결과창은 여기서 끝....

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

	//result::medal
	medal.wnd[0] = FindControl<i3UIFrameWnd*>(pScene, "i3UIFrameWnd8");
	medal.mark[0] = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet6");
	medal.wnd[1] = FindControl<i3UIFrameWnd*>(pScene, "i3UIFrameWnd9");
	medal.mark[1] = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet7");
	medal.wnd[2] = FindControl<i3UIFrameWnd*>(pScene, "i3UIFrameWnd10");
	medal.mark[2] = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet8");
	medal.wnd[3] = FindControl<i3UIFrameWnd*>(pScene, "i3UIFrameWnd11");
	medal.mark[3] = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet9");
	medal.wnd[4] = FindControl<i3UIFrameWnd*>(pScene, "i3UIFrameWnd12");
	medal.mark[4] = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet10");
	medal.text = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText9");
}

/*********************************/

#if defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)
#include "UnitTest/GameUnitTest.h"
#endif

void UIHudGameResult::SetParameter(INT32 arg1, INT32 arg2)
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
	case HUD::GRS_Medal: medal_list.push_back(*reinterpret_cast<MEDAL_STATE *>(arg2)); break;
	case HUD::GRS_RoundShow: update_result_date(result::rs_round); break;
	case HUD::GRS_EndShow: 
#if ENABLE_UNIT_TEST
		if( UnitTest::is_act_hud_unit_test() == true )
			UnitTest::get_unit_test()->m_UnitTestFlag.remove(UNIT_TEST_HUD);
#endif
		update_result_date(result::rs_end);
		break;
	}
}

void	UIHudGameResult::Update( REAL32 rDeltaSeconds)
{
	//if( GameUI::IsKeyboardPressed(I3I_KEY_ENTER) )
	if (GameUI::IsKeyboardStroked(I3I_KEY_ENTER))
	{
		SetParameter(HUD::GRS_Ok, 0);
	}
	//if( GameUI::IsKeyboardPressed(I3I_KEY_ESC) )
	if (GameUI::IsKeyboardStroked(I3I_KEY_ESC))
	{
		SetParameter(HUD::GRS_Ok, 0);
	}
}

void UIHudGameResult::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	iHudBase::SetEnable(false);
	rank_at_start = UserInfoContext::i()->GetRealRank();
	is_setted_up = false;
}

void UIHudGameResult::event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	iHudBase::SetEnable( false );
	rank_at_start = UserInfoContext::i()->GetRealRank();
	is_setted_up = false;
}

/*********************************/

//------------------------------------//
struct fn_my_inform
{
	bool operator()(const ResultState::UserInform& inform) const { return inform.is_my_inform; }
};
struct fn_idx_inform : std::binary_function< ResultState::UserInform, INT32, bool >
{
	bool operator()(const ResultState::UserInform& inform, INT32 user_idx) const { return inform.idx == user_idx; }
};
struct fn_compare_exp : std::binary_function< ResultState::UserInform, INT32, bool >
{
	bool operator()(const ResultState::UserInform& inform, INT32 exp) const { return inform.exp == exp; }
};




ResultState::UserInform* UIHudGameResult :: find_my_inform(i3::vector< ResultState::UserInform > list[TEAM_COUNT])
{
	for(size_t i=0;i<TEAM_COUNT;++i)
	{
		i3::vector< ResultState::UserInform >::iterator it = std::find_if (list[i].begin(), list[i].end(), fn_my_inform() );
		if( it != list[i].end() ) return it;
	}
	return &default_user_inform;
}
ResultState::UserInform* UIHudGameResult :: find_user_inform(i3::vector< ResultState::UserInform > list[TEAM_COUNT], INT32 user_idx)
{
	for(size_t i=0;i<TEAM_COUNT;++i)
	{
		i3::vector< ResultState::UserInform >::iterator it = std::find_if (list[i].begin(), list[i].end(), std::bind2nd( fn_idx_inform(), user_idx) );
		if( it != list[i].end() ) return it;
	}
	return &default_user_inform;
}

void UIHudGameResult :: update_result_date(result::state state)
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

	if( state == result::rs_round) return;

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
	if((MyRoomInfoContext::i()->getStage()->IsAiMode()) == false)
	{
		ROOM_INFO_BASIC pBasic;
		MyRoomInfoContext::i()->getMyRoomInfo(&pBasic);

		CCharaInfoContext::i()->setMyLastRecord(
			true, hu::is_my_team_win(),
			(UINT8)StageID::GetStageMode(pBasic._StageID), pBasic._StageID,
			my_inform->kill_count, my_inform->death_count,
			my_inform->exp, my_inform->point, 0, 0);
	}

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

	//중복 제거
	i3::vector< MEDAL_STATE >::iterator it = unique_un_sorted(medal_list.begin(), medal_list.end(), fn_compare_medal() );
	medal_list.resize( std::distance(medal_list.begin(),it) );

	//미완료 메달 필터
	CMedalManager* mgr = CMedalManager::i();
	it = std::remove_if(medal_list.begin(), medal_list.end(), std::bind2nd(fn_complete_medal(), mgr) );
	medal_list.resize( std::distance(medal_list.begin(),it) );

	//메달 고유 인덱스 순서로 정렬
	std::sort(medal_list.begin(), medal_list.end(), fn_sort_medal() );

	update_medal(medal, medal_list);

}


/*********************************/
// inner
void UIHudGameResult :: update_end_user_list(const i3::vector< ResultState::UserInform > list[TEAM_COUNT])
{
	i3::rc_wstring wstr_nick;

	for(size_t i=0;i<TEAM_COUNT;++i)
	{
		{
			LuaState * L = UILua::CallLuaFunction(HUD::instance()->end_result_scene_info.scn, "ClearUserList");
			i3Lua::PushInteger(L, i); //list_idx
			UILua::EndLuaFunction(L, 1);
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

			i3Lua::PushInteger(L, list[i][j].kill_count);	// kill
			i3Lua::PushInteger(L, list[i][j].death_count);	// death
			i3Lua::PushInteger(L, list[i][j].assist_count);	// assist

			INT32 total_exp = list[i][j].exp + list[i][j].total_bonus_exp;
			INT32 total_point = list[i][j].point + list[i][j].total_bonus_point;
			i3Lua::PushInteger(L, total_exp); //experience
			i3Lua::PushInteger(L, total_point); //point

			i3Lua::PushInteger(L, hu::get_benefit_mark(list[i][j].result_mark, RESULT_ICON_PC_ALL));	//bonusPC
			i3Lua::PushInteger(L, hu::get_benefit_mark(list[i][j].result_mark, RESULT_ICON_EVENT));		//bonusEvent
			i3Lua::PushInteger(L, hu::get_benefit_mark(list[i][j].result_mark, RESULT_ICON_ITEM));		//bonusItem

			UILua::EndLuaFunction(L , 14);
		}
	}
}

void UIHudGameResult::update_round_user_list(const i3::vector< ResultState::UserInform > list[TEAM_COUNT])
{
	i3::rc_wstring wstr_nick;

	for(size_t i=0;i<TEAM_COUNT;++i)
	{
		{
			LuaState * L = UILua::CallLuaFunction(HUD::instance()->round_result_scene_info.scn, "ClearUserList");
			i3Lua::PushInteger(L, i); //list_idx
			UILua::EndLuaFunction(L, 1);
		}

		size_t size = list[i].size();
		for(size_t j=0;j<size;++j)
		{
			LuaState * L = UILua::CallLuaFunction(HUD::instance()->round_result_scene_info.scn, "SetUserList");

			i3Lua::PushInteger( L, i); //list_idx
			i3Lua::PushBoolean( L, list[i][j].is_my_inform); //isMyChara
			i3Lua::PushInteger( L, j); //slot
			i3Lua::PushInteger( L, list[i][j].clan_mark); //clan
			i3Lua::PushInteger( L, list[i][j].rank); //rank

			wstr_nick = list[i][j].wstr_nick;
			const I3COLOR * pNickColor = GameUI::GetNickNameColor( list[i][j].nick_color );
			//chunjong.song 2014-09-04, ::ColorString의 overloading 호출이잘못되어서 명시적으로 호출합니다
			::ColorString(wstr_nick, (UINT32)pNickColor->r, (UINT32)pNickColor->g, (UINT32)pNickColor->b, (UINT32)pNickColor->a );						
			i3Lua::PushStringUTF16To8( L, wstr_nick.c_str()); //nick

			i3Lua::PushInteger(L, list[i][j].kill_count);	// kill
			i3Lua::PushInteger(L, list[i][j].death_count);	// death
			i3Lua::PushInteger(L, list[i][j].assist_count);	// assist

			UILua::EndLuaFunction(L, 9);
		}
	}
}


void UIHudGameResult :: SetCellVisible(i3UIListView_Box* lvBox, INT32 slotIdx, INT32 cellIdx, bool visible)
{
	i3UIControl* cell = lvBox->getCellChild(slotIdx, cellIdx);

	if (cell == nullptr)
	{
		I3PRINTLOG(I3LOG_NOTICE, "cell is null : slot = %d(MAX:%d), cell = %d(MAX:%d)",
			slotIdx, lvBox->getItemList().size(), cellIdx, lvBox->getCellCount());

		return;
	}

	cell->HideShapes(!visible);
	cell->SetVisible(visible);
}

void UIHudGameResult :: SetCellClanMark(i3UIListView_Box* lvBox, INT32 slotIdx, INT32 cellIdx, UINT32 ui32MarkFlag, void * pFunc)
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

void UIHudGameResult :: SetCellText(i3UIListView_Box* lvBox, INT32 slotIdx, INT32 cellIdx, const char* Text)
{
	i3UIStaticText* cell = (i3UIStaticText*)lvBox->getCellChild(slotIdx, cellIdx);
	if(cell)
		cell->SetText(Text);
}
void UIHudGameResult :: SetCellText(i3UIListView_Box* lvBox, INT32 slotIdx, INT32 cellIdx, INT32 Value)
{
	i3UIStaticText* cell = (i3UIStaticText*)lvBox->getCellChild(slotIdx, cellIdx);
	if(cell)
	{
		char Text[32] = {0,};
		itoa(Value,Text,10);
		cell->SetText(Text);
	}
}

void cbSetClanTexturForTabMinimap(i3UIListView_Cell * pCell, void * pData)
{
	I3ASSERT( pCell != nullptr);
	if( pCell->getCellChild() == nullptr) return;

	i3UIControl * pCtrl = pCell->getCellChild();

	if( i3::same_of<i3UIImageBoxEx*>(pCtrl))
	{
		UINT32 markFlag = GameUI::GetClanMarkFlag((char *)pData);
		GameUI::SetClanTextureAndCenterPos( (i3UIImageBoxEx*) pCtrl, pCell, CLAN_MARK_SMALL, markFlag, 20.0f);
	}
}



void UIHudGameResult::update_clan_mark(const i3::vector< ResultState::UserInform > list[TEAM_COUNT], i3UIListView_Box* team_list_view[TEAM_COUNT])
{
	for(size_t i=0;i<TEAM_COUNT;++i)
	{
		size_t size = list[i].size();
		for(size_t j=0;j<size;++j)
		{
			SetCellClanMark(team_list_view[i], j, 0, list[i][j].clan_mark, cbSetClanTexturForTabMinimap);
			SetCellVisible(team_list_view[i], j, 0, true);
		}
	}
}

void UIHudGameResult::update_win_count_only_text(result::win_count & win_count, INT32 red_count, INT32 blue_count)
{
	i3::stack_wstring wstr_stack;
	i3::itoa(red_count, wstr_stack);
	win_count.text[TEAM_RED]->SetText(wstr_stack);
	i3::itoa(blue_count, wstr_stack);
	win_count.text[TEAM_BLUE]->SetText( wstr_stack );
}

void UIHudGameResult::update_win_count(result::win_count & win_count, INT32 red_count, INT32 blue_count, INT32 redList_count, INT32 blueList_count)
{
	update_win_count_only_text(win_count, red_count, blue_count);

	enum { Lose, Win, Draw };

	if( ( redList_count ==0 ) || (blueList_count == 0))
	{
		win_count.img[TEAM_RED]->SetShapeIdx( (redList_count > blueList_count) ? Win: Lose );
		win_count.img[TEAM_BLUE]->SetShapeIdx( (redList_count > blueList_count) ? Lose: Win  );
	}
	else if(red_count != blue_count)
	{
		win_count.img[TEAM_RED]->SetShapeIdx( (red_count > blue_count) ? Win: Lose );
		win_count.img[TEAM_BLUE]->SetShapeIdx( (red_count > blue_count) ? Lose: Win  );
	}
	else //draw
	{
		win_count.img[TEAM_RED]->SetShapeIdx( Draw );
		win_count.img[TEAM_BLUE]->SetShapeIdx( Draw );
	}
}

void UIHudGameResult::update_report(result::report & report, const i3::wstring & top_exp_nick,
									INT32 top_exp_point, const i3::wstring & best_assistor_nick,
									const i3::wstring & top_killer_nick, INT32 top_killer_count,
									INT32 kill_grade, INT32 assist_grade, INT32 max_kill_count, INT32 headshot_count,
									const i3::wstring & max_killed_nick, INT32 killed_count)
{
	const i3::rc_wstring& kill_string = GAME_RCSTRING("STR_TBL_PROFILE_KILL_TEXT"); //킬
	const i3::rc_wstring& grade_string = GAME_RCSTRING("STR_DOMI_RESULT_1"); //순위

	i3::stack_wstring wstr_stack;

	//MVP : 닉네임
	if( top_exp_point != 0)
		i3::sprintf(wstr_stack, L"%s : %s", GAME_RCSTRING("STR2_RESULT_MVP"), top_exp_nick);
	else
	{
		wstr_stack = GAME_RCSTRING("STR2_RESULT_MVP");
		wstr_stack += L" : -";		// 없는 경우..
	}

	report.mvp->SetText( wstr_stack );

	// 도움왕 : 닉네임
	if (best_assistor_nick.empty() == false)
		i3::sprintf(wstr_stack, GAME_RCSTRING("STR_TBL_LAST_RECORD_ASSISTKING"), best_assistor_nick);
	else
		i3::sprintf(wstr_stack, GAME_RCSTRING("STR_TBL_LAST_RECORD_ASSISTKING"), L"-");

	report.best_assistor->SetText(wstr_stack);

	//최대 멀티 킬 : 닉네임 (00킬)		
	if( top_killer_nick.empty() == false )
		i3::sprintf(wstr_stack, L"%s : %s (%d%s)", GAME_RCSTRING("STR2_RESULT_TOP_KILL"), top_killer_nick,
		top_killer_count, kill_string);
	else
	{
		wstr_stack = GAME_RCSTRING("STR2_RESULT_TOP_KILL");
		wstr_stack += L" : -";    //최대 멀티 킬이 없는 경우
	}
	report.top_kill_user->SetText(wstr_stack);

	//킬 순위 : 00위
	if( kill_grade != 0 )
		i3::sprintf(wstr_stack, L"%s : %d%s", GAME_RCSTRING("STR2_RESULT_KILL_GRADE"), kill_grade, grade_string);
	else
	{
		wstr_stack = GAME_RCSTRING("STR2_RESULT_KILL_GRADE");
		wstr_stack += L" : -";    // kill_grade == 0 일경우 
	}
	report.kill_grade->SetText(wstr_stack);

	//어시스트 순위 : 00위
	if (assist_grade != 0)
		i3::sprintf(wstr_stack, L"%s %d%s", GAME_RCSTRING("STR_TBL_LAST_RECORD_ASSISTRANK"), assist_grade, grade_string);
	else
	{
		wstr_stack = GAME_RCSTRING("STR_TBL_LAST_RECORD_ASSISTRANK");
		wstr_stack += L"-";
	}
	report.assist_grade->SetText(wstr_stack);

	//최대 연속 킬 : 00킬
	i3::sprintf(wstr_stack, L"%s : %d%s", GAME_RCSTRING("STR2_RESULT_MAX_KILL_COUNT"), max_kill_count, kill_string);
	report.max_kill_count->SetText(wstr_stack);

	//헤드샷  킬 : 00킬
	i3::sprintf(wstr_stack, L"%s : %d%s", GAME_RCSTRING("STR2_RESULT_HEADSHOT_KILL_COUNT"), headshot_count, kill_string);
	report.headshot_count->SetText(wstr_stack);

	//최대 피격 유저 : 닉네임 (00킬)
	if( max_killed_nick.empty() == false )
		i3::sprintf(wstr_stack, L"%s : %s (%d%s)", GAME_RCSTRING("STR2_RESULT_BE_SHOT_USER"), max_killed_nick, 
		killed_count, kill_string);
	else
	{
		wstr_stack = GAME_RCSTRING("STR2_RESULT_BE_SHOT_USER");
		wstr_stack += L" : -";   //kill이 0 일 경우
	}

	report.max_be_shot_user->SetText(wstr_stack);
}

void UIHudGameResult::update_benefit_exp(INT32 slot_idx, result::benefit_exp & benefit_exp, INT32 benefit_mark, INT32 total_exp)
{
	UI_EVENT_MARK default_mark[result::MaxBenefit]	= { UIEM_PCCAFE, UIEM_EVENT, UIEM_ITEM };
	INT32 check_mark[result::MaxBenefit]			= { RESULT_ICON_PC_ALL,	RESULT_ICON_EVENT, RESULT_ICON_ITEM };
	INT32 result_event_type[result::MaxBenefit]		= { TYPE_BATTLE_RESULT_EVENT_PCCAFE, TYPE_BATTLE_RESULT_EVENT_EVENT, TYPE_BATTLE_RESULT_EVENT_ITEM };

	for(INT32 idx = 0 ; idx < result::MaxBenefit ; ++idx)
	{
		UI_EVENT_MARK event_mark = static_cast<UI_EVENT_MARK>( hu::get_benefit_mark( benefit_mark, check_mark[idx])) ;

		if( event_mark > UIEM_NONE)
		{
			INT32 i32BonusExp = BattleResultContext::i()->GetInfo().m_BonusExp[slot_idx][ result_event_type[idx] ];

			benefit_exp.mark[idx].img->SetShapeIdx( event_mark );
			i3::wstring str = i3::format_string(L"+%d", i32BonusExp);
			benefit_exp.mark[idx].text->SetText(str);
		}
		else
		{
			benefit_exp.mark[idx].img->SetShapeIdx( default_mark[idx] );

			benefit_exp.mark[idx].text->SetText(L"+0");
		}		
	}

	//Exp +000
	i3::stack_wstring wstr_stack;
	i3::sprintf(wstr_stack, L"%s +%d", GAME_RCSTRING("STR_TBL_MYINFO_EXP"), total_exp);

	benefit_exp.exp->SetText(wstr_stack);
}

void UIHudGameResult::update_rank(result::rank & rank, INT32 rank_at_start)
{
	INT32 curr_rank = UserInfoContext::i()->GetRealRank();
	bool is_promoted_rank = false;
	if( curr_rank != rank_at_start)
	{
		is_promoted_rank = true;
		curr_rank = rank_at_start;
	}

	//장성급, 영웅,GM은 경험치로 계산을 못 함
	//UIPopupExit::SetRank 함수와 내용이 같음
	if (GameUI::is_hero_rank(curr_rank, true) == true)
	{
		rank.img[result::CurrRank]->SetShapeIdx(curr_rank);
		rank.wnd[result::NextRank]->EnableCtrl(false);
		rank.text->SetText(L"100%");
		rank.progress->setProgress( 1.f);
	}
	else if( is_promoted_rank == true ) //진급한 계급은 100% 로 만든다.
	{
		rank.text->SetText(L"100%");
		rank.progress->setProgress( 1.f);
		rank.img[result::CurrRank]->SetShapeIdx(curr_rank);
		rank.img[result::NextRank]->SetShapeIdx(curr_rank+1);
	}
	else
	{
		USER_INFO_BASIC MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

		INT32 CurrentExp = MAX(MyUserInfoBasic.m_ui32Exp - GameUI::GetMaxExp(curr_rank), 0); //음수가 나올 수 있다.(웹툴에서 계급만 수정할 경우, 해당 계급에 exp가 정상적이지 않음)
		/*INT32 MaxExp = GameUI::GetMaxExp(curr_rank + 1) - GameUI::GetMaxExp(curr_rank);
		if(MaxExp == 0)
			MaxExp = 1;*/

		INT32 ExpRate = GameUI::GetExpRate(curr_rank, CurrentExp);

		i3::stack_wstring wstrRateText;
		i3::sprintf(wstrRateText, L"%d%%", ExpRate);
		rank.text->SetText(wstrRateText);

		rank.progress->setProgress( (REAL32)ExpRate * 0.01f);

		rank.img[result::CurrRank]->SetShapeIdx(curr_rank);
		rank.img[result::NextRank]->SetShapeIdx(curr_rank+1);
	}
}

void UIHudGameResult::update_medal(result::medal & medal, const i3::vector<MEDAL_STATE> & medal_list)
{
	CMedalManager* mgr = CMedalManager::i();

	INT32 size = medal_list.size();
	for(INT32 i=0;i<result::MaxMedal;++i)
	{
		medal.wnd[i]->EnableCtrl(false);

		if( i < size )
		{
			medal.wnd[i]->EnableCtrl(true);
			INT32 idx = mgr->getImageIdx( medal_list[i].m_ui8MedalType, medal_list[i].m_ui16MedalIdx );
			medal.mark[i]->SetShapeIdx( idx );
		}
	}

	medal.text->EnableCtrl( (size > result::MaxMedal) ? true : false);
}

void UIHudGameResult::to_plural_user_string(i3::wstring & out_nick, const i3::rc_wstring & first_nick, 
											size_t user_count, I3COLOR color)
{
	out_nick = first_nick;
	if(user_count > 1 )
	{
		out_nick.append(L"[+");
		i3::stack_wstring wstr_userCount;		i3::itoa(user_count-1, wstr_userCount);
		out_nick.append( wstr_userCount );
		out_nick.append(L"]");
	}

	::ColorString(out_nick, color);
}

void UIHudGameResult::max_killed_nick_string(i3::wstring& out_max_killed_nick, const i3::vector<INT32> & killed_user_idx_list, 
											 i3::vector< ResultState::UserInform > list[TEAM_COUNT])
{
	size_t size = killed_user_idx_list.size();
	if( size == 0 ) return;

	if( size == 1 )
	{
		ResultState::UserInform* killed_user_inform = find_user_inform(list, killed_user_idx_list.front());
		out_max_killed_nick = killed_user_inform->wstr_nick;
		return;
	}

	if( size == 2 )
	{
		ResultState::UserInform* killed_user_inform = find_user_inform(list, killed_user_idx_list[0]);
		out_max_killed_nick = killed_user_inform->wstr_nick;
		out_max_killed_nick.append(L", ");
		killed_user_inform = find_user_inform(list, killed_user_idx_list[1]);
		out_max_killed_nick += killed_user_inform->wstr_nick;
		return;
	}

	ResultState::UserInform* killed_user_inform = find_user_inform(list, killed_user_idx_list.front());
	to_plural_user_string(out_max_killed_nick, killed_user_inform->wstr_nick, size, white_color);
}

void UIHudGameResult::get_best_assistor(i3::vector< ResultState::UserInform > list[TEAM_COUNT],
	i3::wstring & best_assistor_nick)
{
	const ResultState::UserInform *assistor_inform[TEAM_COUNT] = { &default_user_inform, &default_user_inform };
	size_t best_assistor_count[TEAM_COUNT] = { 0, };

	for (size_t i = 0; i < TEAM_COUNT; ++i)
	{
		hu::sort_ingame_result_inform(list[i], fn_sort_assist_count(), true);

		if (list[i].empty() == false) assistor_inform[i] = &list[i].front();

		if (assistor_inform[i]->assist_count != 0)
		{
			best_assistor_count[i] = std::count_if(list[i].begin(), list[i].end(), std::bind2nd(fn_match_assist_count(), assistor_inform[i]->assist_count));
		}
	}

	if (assistor_inform[TEAM_RED]->assist_count == 0 && assistor_inform[TEAM_BLUE]->assist_count == 0) //아무도 없음
	{
		best_assistor_nick.clear();
		return;
	}

	if (assistor_inform[TEAM_RED]->assist_count > assistor_inform[TEAM_BLUE]->assist_count) //red
	{
		to_plural_user_string(best_assistor_nick, assistor_inform[TEAM_RED]->wstr_nick, best_assistor_count[TEAM_RED], red_color);
	}
	else if (assistor_inform[TEAM_RED]->assist_count < assistor_inform[TEAM_BLUE]->assist_count) //blue
	{
		to_plural_user_string(best_assistor_nick, assistor_inform[TEAM_BLUE]->wstr_nick, best_assistor_count[TEAM_BLUE], blue_color);
	}
	else //red, blue
	{
		i3::wstring wstr_nick[TEAM_COUNT];
		to_plural_user_string(wstr_nick[TEAM_RED], assistor_inform[TEAM_RED]->wstr_nick, best_assistor_count[TEAM_RED], red_color);
		to_plural_user_string(wstr_nick[TEAM_BLUE], assistor_inform[TEAM_BLUE]->wstr_nick, best_assistor_count[TEAM_BLUE], blue_color);

		best_assistor_nick.append(wstr_nick[TEAM_RED]);
		best_assistor_nick.append(L", ");
		best_assistor_nick.append(wstr_nick[TEAM_BLUE]);
	}
}

void UIHudGameResult::get_top_exp_user(i3::vector< ResultState::UserInform > list[TEAM_COUNT], 
									   i3::wstring & top_exp_nick, INT32 & top_exp_point)
{
	const ResultState::UserInform *mvp_inform[TEAM_COUNT] = { &default_user_inform, &default_user_inform };
	size_t top_exp_user_count[TEAM_COUNT] = {0,};

	//mvp 는 exp로 정렬 후에 red, blue에 최대 exp 유저를 비교해서 찾는다.
	for(size_t i=0;i<TEAM_COUNT;++i)
	{
		hu::sort_ingame_result_inform( list[i], fn_sort_exp(), true ); //exp로 정렬한다.

		if( list[i].empty() == false ) mvp_inform[i] = &list[i].front();

		if( mvp_inform[i]->exp != NULL_EXP )
		{
			top_exp_user_count[i] = std::count_if( list[i].begin(), list[i].end(), std::bind2nd(fn_compare_exp(), mvp_inform[i]->exp ) );
		}
	}

	if( mvp_inform[TEAM_RED]->exp == NULL_EXP && mvp_inform[TEAM_BLUE]->exp == NULL_EXP ) //아무도 없음
	{
		top_exp_nick.clear(); top_exp_point = NULL_EXP; 
		return;
	}

	if( mvp_inform[TEAM_RED]->exp > mvp_inform[TEAM_BLUE]->exp ) //red
	{
		top_exp_point = mvp_inform[TEAM_RED]->exp;
		to_plural_user_string(top_exp_nick, mvp_inform[TEAM_RED]->wstr_nick, top_exp_user_count[TEAM_RED], red_color);
	}
	else if( mvp_inform[TEAM_RED]->exp < mvp_inform[TEAM_BLUE]->exp ) //blue
	{
		top_exp_point = mvp_inform[TEAM_RED]->exp;
		to_plural_user_string(top_exp_nick, mvp_inform[TEAM_BLUE]->wstr_nick, top_exp_user_count[TEAM_BLUE], blue_color);
	}
	else //red, blue
	{
		i3::wstring wstr_nick[TEAM_COUNT];
		to_plural_user_string(wstr_nick[TEAM_RED], mvp_inform[TEAM_RED]->wstr_nick, top_exp_user_count[TEAM_RED], red_color);
		to_plural_user_string(wstr_nick[TEAM_BLUE], mvp_inform[TEAM_BLUE]->wstr_nick, top_exp_user_count[TEAM_BLUE], blue_color);

		top_exp_nick.append(wstr_nick[TEAM_RED]);
		top_exp_nick.append(L", ");
		top_exp_nick.append(wstr_nick[TEAM_BLUE]);

		top_exp_point = mvp_inform[TEAM_RED]->exp;
	}
}

/*	1. kill count 정렬한다.
2. 내 킬 수(my_kill_count)가 정렬된 merge_list 리스트에 front 값과 비교한다. 같을 경우 1등급이다.
3. 그외는 내 킬 수(my_kill_count) 위치로 등급을 알 수 있다.

예제) 3,3,3,2,2,1,0 (킬)
my_kill_count == 3일 경우엔 1등급, my_kill_count == 2일 경우엔 4등급, my_kill_count == 1일 경우엔 6등급, my_kill_count == 0일 경우엔 무등급
*/
INT32 UIHudGameResult::calc_kill_grade(i3::vector< ResultState::UserInform > list[TEAM_COUNT], UINT16 my_kill_count)
{
	if (my_kill_count == 0) return 0;

	// kill grade는 red, blue 유저 정보를 합친다.
	i3::vector< ResultState::UserInform > merge_list(list[TEAM_RED]);
	std::copy(list[TEAM_BLUE].begin(), list[TEAM_BLUE].end(), std::back_inserter(merge_list));
	if (merge_list.empty() == true) return 0; //이럴 경우는 없지만...

	hu::sort_ingame_result_inform(merge_list, fn_sort_kill_count(), false); // kill count로 정렬한다.
	if (merge_list.front().kill_count == my_kill_count) return 1; //1등급 킬이다.

	i3::vector< ResultState::UserInform >::iterator it =
		std::find_if(merge_list.begin(), merge_list.end(), std::bind2nd(fn_match_kill_count(), my_kill_count));

	return (it - merge_list.begin()) + 1; //시작이 1등급 부터다.
}

INT32 UIHudGameResult::calc_assist_grade(i3::vector< ResultState::UserInform > list[TEAM_COUNT], UINT16 my_assist_count)
{
	if (my_assist_count == 0) return 0;

	// assist grade는 red, blue 유저 정보를 합친다.
	i3::vector< ResultState::UserInform > merge_list(list[TEAM_RED]);
	std::copy(list[TEAM_BLUE].begin(), list[TEAM_BLUE].end(), std::back_inserter(merge_list));
	if (merge_list.empty() == true) return 0;

	hu::sort_ingame_result_inform(merge_list, fn_sort_assist_count(), false);	// assist count로 정렬한다.
	if (merge_list.front().assist_count == my_assist_count) return 1;

	i3::vector< ResultState::UserInform >::iterator it =
		std::find_if(merge_list.begin(), merge_list.end(), std::bind2nd(fn_match_assist_count(), my_assist_count));

	return (it - merge_list.begin()) + 1;
}

#include <bitset>
void UIHudGameResult::calc_top_killer(i3::vector<INT32> & kill_user_idx_list, UINT16 killer_user_bit_flag)
{
	enum { MAX_16_BIT_COUNT = 16, };
	std::bitset< MAX_16_BIT_COUNT > flag(killer_user_bit_flag);

	kill_user_idx_list.clear();
	for(size_t i=0;i<MAX_16_BIT_COUNT;++i)
	{
		if( flag.test(i) == true )
			kill_user_idx_list.push_back(i);
	}
}