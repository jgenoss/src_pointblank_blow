#include "pch.h"
#include "BreakIntoGame.h"

#include "../TextSet.h"
#include "UIUtil.h"
#include "UIHudUtil.h"

#include "UIPhaseReadyRoom.h"
#include "MyRoomInfoContext.h"
#include "LobbyContext.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/itoa.h"
#include "i3Base/ColorString.h"

#include "ClientStageInfoUtil.h"

namespace
{
	struct parameter
	{
		i3::rc_wstring mode_name;
		const ROOM_INFO_BASIC* room_info;
		const ROOM_NOW_SCORE* score_info;
	};

	i3::wstring to_string_goal(const ROOM_INFO_BASIC* info)
	{
		ROOM_INFO_ADD addInfo;
		MyRoomInfoContext::i()->getMyRoomInfoAdd(&addInfo);
		UINT8 GameSetPrim	= addInfo._SubType & 0x0F;
		UINT8 GameSetSec	= addInfo._SubType & 0xF0;

		i3::wstring strWinObject;
		TextSet::GetWinObjectText(strWinObject, CStageInfoUtil::GetStageData(info->_StageID), GameSetPrim, GameSetSec); //( n분 / n Kill )
		return i3::move(strWinObject);
	}

	i3::wstring to_string_play_time(const ROOM_INFO_BASIC* room_info, const ROOM_NOW_SCORE* score_info)
	{
		ROOM_INFO_ADD addInfo;
		MyRoomInfoContext::i()->getMyRoomInfoAdd(&addInfo);
		//게임총시간 - 게임남은시간 = 게임경과시간
		UINT8 GameSetSec	= addInfo._SubType & 0xF0;

		UINT32 target_time = TextSet::GetTime_Second(GameSetSec);
		target_time -= score_info->PlayTime;

		return i3::format_string(L"%s %d : %d", GAME_RCSTRING("STR2_ELAPSED_PLAY_TIME"), target_time/60, target_time%60);
	}

	i3::wstring to_string_score(const ROOM_NOW_SCORE* info)
	{
		i3::wstring red;
	
		if(info->Score[0] < 0) 
			red = L"∞";
		else
			i3::itoa(info->Score[0], red);

		::ColorString(red, 255,0,0);
		
		i3::wstring blue;
		if(info->Score[1] < 0) 
			blue = L"∞";
		else
			i3::itoa(info->Score[1], blue);

		::ColorString(blue, 0,0,255);
		
		return i3::format_string(L"%s %s : %s", GAME_RCSTRING("STR2_CURRENT_SCORE"), red, blue);
	}

	i3::wstring to_string_difficult(const ROOM_NOW_SCORE* info)
	{
		i3::rc_wstring diff_text = hu::get_challenge_difficulty_text(info->Score[0]);

		return i3::format_string(L"%s %d %s", GAME_RCSTRING("STR2_CURRENT_DIFFICULTY"), info->Score[0], diff_text);
	}
}

namespace detail
{
	void get_string_none(i3::vector< i3::wstring > & ret, const parameter & param) {}
	void to_string_none(const i3::vector< i3::wstring > & list, i3::wstring & ret) {}

	//-------------------
	void get_string_killcount_info(i3::vector< i3::wstring > & ret, const parameter & param)
	{
		i3::wstring mode( param.mode_name );
		::ColorString(mode, 163, 73, 164);

		i3::wstring room_info = to_string_goal( param.room_info );
		::ColorString(room_info, 163, 73, 164);

		i3::wstring mode_str = mode + L" (" + room_info + L")";
		ret.push_back( mode_str );

		i3::wstring play_time = to_string_play_time(param.room_info, param.score_info);
		::ColorString(play_time, 128, 255, 255);
		ret.push_back(play_time);

		i3::wstring score = to_string_score(param.score_info);
		ret.push_back(score);
	}
	void to_string_killcount_info(const i3::vector< i3::wstring > & list, i3::wstring & ret)
	{
		ret = list[0] + L"\n" + list[1] + L"\n" + list[2];
	}

	//-------------------
	void get_string_roundcount_info(i3::vector< i3::wstring > & ret, const parameter & param) 
	{
		i3::wstring mode( param.mode_name );
		::ColorString(mode, 163, 73, 164);

		i3::wstring room_info = to_string_goal(param.room_info);
		::ColorString(room_info, 163, 73, 164);

		i3::wstring mode_str = mode + L" (" + room_info + L")";
		ret.push_back( mode_str );

		i3::wstring score = to_string_score(param.score_info);
		ret.push_back(score);
	}

	void to_string_roundcount_info(const i3::vector< i3::wstring > & list, i3::wstring & ret) 
	{
		ret = list[0] + L"\n" + list[1];
	}

	//-------------------
	void get_string_diff_info(i3::vector< i3::wstring > & ret, const parameter & param) 
	{
		i3::wstring mode( GAME_RCSTRING("STR_TBL_GUI_LOBBY_TRANING_TEXT") );
		::ColorString(mode, 163, 73, 164);

		i3::wstring room_info = to_string_goal(param.room_info);
		::ColorString(room_info, 163, 73, 164);

		i3::wstring mode_str = mode + L" (" + room_info + L")";
		ret.push_back( mode_str );

		i3::wstring play_time = to_string_play_time(param.room_info, param.score_info);
		::ColorString(play_time, 128, 255, 255);
		ret.push_back( play_time );

		i3::wstring difficult = to_string_difficult(param.score_info);
		ret.push_back( difficult );
	}

	void to_string_diff_info(const i3::vector< i3::wstring > & list, i3::wstring & ret) 
	{
		ret = list[0] + L"\n" + list[1] + L"\n" + list[2];
	}

	//-------------------
	void (*pf_get_string[])(i3::vector< i3::wstring > & ret, const parameter & param) = {
		get_string_none, get_string_killcount_info, get_string_roundcount_info, get_string_diff_info };

	void (*pf_to_string[])(const i3::vector< i3::wstring > & list, i3::wstring & ret) = {
		to_string_none, to_string_killcount_info, to_string_roundcount_info, to_string_diff_info };
}

bool bing::is_break_into_game()
{
	ROOM_INFO_BASIC RoomInfo;
	MyRoomInfoContext::i()->getMyRoomInfo(&RoomInfo); 
	if( RoomInfo._State <= ROOM_STATE_COUNTDOWN_R) return false;

	UI_MODE_TYPE mode = GameUI::GetStageModeType( MyRoomInfoContext::i()->getStage(), RoomInfo._IsClanMatch );
	if( mode == UMT_NONE ) return false;

	return true;
}

void bing::print_room_info()
{
	ROOM_INFO_BASIC RoomInfo;
	MyRoomInfoContext::i()->getMyRoomInfo(&RoomInfo); 

	parameter param;
	param.room_info = &RoomInfo;
	param.score_info = MyRoomInfoContext::i()->getMyRoomScore();
	param.mode_name = CStageInfoUtil::GetRuleData(RoomInfo._StageID)->getRuleName();

	INT32 size = sizeof(detail::pf_get_string) / sizeof(detail::pf_get_string[0]);
	if(param.score_info->ScoreType > 0 && param.score_info->ScoreType < size )
	{
		i3::vector< i3::wstring > room_info_string_list;
		(*detail::pf_get_string[param.score_info->ScoreType])(room_info_string_list, param);

		i3::wstring room_info_string;
		(*detail::pf_to_string[param.score_info->ScoreType])(room_info_string_list, room_info_string);
		GameUI::PrintSystemChat(room_info_string.c_str());
	}
}

INT32 bing::get_room_info_string(i3::vector< i3::wstring > & list)
{
	list.clear();

	ROOM_INFO_BASIC RoomInfo;
	MyRoomInfoContext::i()->getMyRoomInfo(&RoomInfo); 

	parameter param;
	param.room_info  = &RoomInfo;
	param.score_info = MyRoomInfoContext::i()->getMyRoomScore();
	param.mode_name  = CStageInfoUtil::GetRuleData(RoomInfo._StageID)->getRuleName();

	INT32 size = sizeof(detail::pf_get_string) / sizeof(detail::pf_get_string[0]);
	if(param.score_info->ScoreType > 0 && param.score_info->ScoreType < size )
	{
		(*detail::pf_get_string[param.score_info->ScoreType])(list, param);
		return param.score_info->ScoreType;
	}

	return 0;
}

INT32 bing::get_room_info_string(i3::vector< i3::wstring > & list, INT32 & red_score, INT32 & blue_score)
{
	list.clear();

	ROOM_INFO_BASIC RoomInfo;
	MyRoomInfoContext::i()->getMyRoomInfo(&RoomInfo); 

	parameter param;
	param.room_info = &RoomInfo;
	param.score_info = MyRoomInfoContext::i()->getMyRoomScore();
	param.mode_name = CStageInfoUtil::GetRuleData(RoomInfo._StageID)->getRuleName();

	INT32 size = sizeof(detail::pf_get_string) / sizeof(detail::pf_get_string[0]);
	if(param.score_info->ScoreType > 0 && param.score_info->ScoreType < size )
	{
		(*detail::pf_get_string[param.score_info->ScoreType])(list, param);
		red_score = param.score_info->Score[0];
		blue_score = param.score_info->Score[1];
		return param.score_info->ScoreType;
	}

	return 0;
}