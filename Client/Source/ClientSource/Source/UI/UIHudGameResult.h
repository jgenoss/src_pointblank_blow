#pragma once

#include "iHudBase.h"
#include "BattleHUD_Define.h"

#include "UIHudDefine.h"
#include "MedalManager.h"

class UIHudManager;

#define RESULT_ICON_PC_ALL (RESULT_ICON_PC | RESULT_ICON_PC_PLUS | RESULT_ICON_PC_ID \
	| RESULT_ICON_GWARNET | RESULT_ICON_PC_HOUSE | RESULT_ICON_GARENA_GCA)


namespace ResultState
{
	struct UserInform;
}

namespace result
{
	enum state { rs_end, rs_round, rs_max, };

	struct win_count //승리 카운트
	{
		i3UIButtonImageSet* img[TEAM_COUNT]; //win, lose
		i3UIStaticText* text[TEAM_COUNT]; //160
	};

	struct report//게임 결과
	{
		i3UIStaticText* mvp;				// MVP : 닉네임
		i3UIStaticText* best_assistor;		// 도움왕 : 닉네임
		i3UIStaticText* top_kill_user;		// 최대 멀티 킬 : 닉네임(00킬)

		i3UIStaticText* kill_grade;			// 킬 순위 : 00위
		i3UIStaticText* assist_grade;		// 어시스트 순위 : 00위
		i3UIStaticText* max_kill_count;		// 최대 연속 킬 : 00킬
		i3UIStaticText* headshot_count;		// 헤드샷 킬 : 00킬
		i3UIStaticText* max_be_shot_user;	// 최대 피격 유저 : 닉네임
	};

	struct benefit_mark
	{
		i3UIButtonImageSet* img; //pc cafe
		i3UIStaticText* text; //+000
	};

	enum { Benefit_PC, Benefit_Event, Benefit_Item, MaxBenefit };
	struct benefit_exp
	{
		result::benefit_mark mark[result::MaxBenefit];
		i3UIStaticText* exp; //EXP +000
	};

	enum { CurrRank, NextRank, MaxRank };
	struct rank
	{
		i3UIFrameWnd* wnd[result::MaxRank];
		i3UIButtonImageSet* img[result::MaxRank];
		i3UIProgressBar* progress;
		i3UIStaticText* text; //0000/0000
	};

	enum { MaxMedal = 5 };
	struct medal
	{
		i3UIFrameWnd* wnd[result::MaxMedal];
		i3UIButtonImageSet* mark[result::MaxMedal];
		i3UIStaticText* text; //....
	};
}

namespace result
{
	struct kill_record
	{
		kill_record() : max_kill_count(0), curr_kill_count(0)  { clear(); }

		void clear() { memset(killed_info, 0, sizeof(INT32) * SLOT_MAX_COUNT); }
		void reset_user_killed_count(INT32 user_idx) { killed_info[user_idx] = 0;  }
		void increase_user_killed_count(INT32 user_idx) { ++killed_info[user_idx]; }
		i3::vector<INT32> get_top_user_killed_idx(INT32 & killed_count);

		void update_kill_count();
		void increase_kill_count() { ++curr_kill_count; }
		INT32 get_max_kill_count() { return max_kill_count; }

		INT32 killed_info[SLOT_MAX_COUNT]; //죽임 당한 유저 정보
		INT32 max_kill_count, curr_kill_count; //최대 킬 수
	};
}

//medal 완료 여부
struct fn_sort_exp
{
	bool operator()(const ResultState::UserInform& lhs, const ResultState::UserInform& rhs) const { return lhs.exp > rhs.exp; }
};
struct fn_sort_kill_count
{
	bool operator()(const ResultState::UserInform& lhs, const ResultState::UserInform& rhs) const { return lhs.kill_count > rhs.kill_count; }
};
struct fn_match_kill_count : public std::binary_function<ResultState::UserInform,UINT16,bool> 
{
	bool operator()(const ResultState::UserInform& inform, const UINT16& kill_count) const { return inform.kill_count == kill_count; }
};
struct fn_sort_assist_count
{
	bool operator()(const ResultState::UserInform& lhs, const ResultState::UserInform& rhs) const { return lhs.assist_count > rhs.assist_count; }
};
struct fn_match_assist_count : public std::binary_function<ResultState::UserInform, UINT16, bool>
{
	bool operator()(const ResultState::UserInform& inform, const UINT16& assist_count) const { return inform.assist_count == assist_count; }
};

//medal 비교
struct fn_compare_medal : public std::binary_function<MEDAL_STATE,MEDAL_STATE,bool> 
{
	bool operator()(const MEDAL_STATE& lhs, const MEDAL_STATE& rhs) const
	{
		return (lhs.m_ui8MedalType == rhs.m_ui8MedalType) && (lhs.m_ui16MedalIdx == rhs.m_ui16MedalIdx);
	}
};

//medal 중복 제거(입력 순으로)
template <class InputIterator, class BinaryPredicate>
inline InputIterator unique_un_sorted(InputIterator first, InputIterator last, BinaryPredicate pred)
{
	if(first == last) return last;

	//while( first+1 < last ) { last =std::remove_if(++first, last, std::bind2nd( pred, *first) ); } //i3::vector iterator 문제로 이런식으로 하면 안됨

	InputIterator next = first+1;
	while( next < last ) 
	{ 
		last =std::remove_if(next, last, std::bind2nd( pred, *first) ); 
		++first;
		next = first+1;
	}

	return last;
}

struct fn_complete_medal : public std::binary_function<MEDAL_STATE,CMedalManager*,bool>
{
	bool operator()(const MEDAL_STATE& data, const CMedalManager* mgr) const
	{
		return !mgr->IsCompleteMedalInfo(data);
	}
};

//medal 정렬 (메달 고유 인덱스 순서) MEDAL_SET_TYPE_EVENT < MEDAL_SET_TYPE_DAILY < MEDAL_SET_TYPE_NORMAL
struct fn_sort_medal
{
	bool operator()(const MEDAL_STATE& lhs, const MEDAL_STATE& rhs) const 
	{
		if( lhs.m_ui8MedalType == MEDAL_SET_TYPE_EVENT && rhs.m_ui8MedalType == MEDAL_SET_TYPE_DAILY ) return true;
		if( lhs.m_ui8MedalType == MEDAL_SET_TYPE_DAILY && rhs.m_ui8MedalType == MEDAL_SET_TYPE_EVENT ) return false;

		if( lhs.m_ui8MedalType == rhs.m_ui8MedalType)
			return lhs.m_ui16MedalIdx < rhs.m_ui16MedalIdx;

		return lhs.m_ui8MedalType > rhs.m_ui8MedalType;
	}
};

//------------------------------------//

class UIHudGameResult : public iHudBase
{
protected://자식과 공용 데이터로 사용
	result::win_count win_count[result::rs_max];
	result::report report;
	result::benefit_exp benefit_exp;
	result::rank rank;
	result::medal medal;

	result::kill_record kill_record;
	i3::vector< MEDAL_STATE > medal_list;
	i3UIListView_Box* team_list_view[result::rs_max][TEAM_COUNT];

	INT32 headshot_count;
	bool is_setted_up;
	INT32 rank_at_start; //게임 시작 시점에 계급

public:
	UIHudGameResult(UIHudManager* p);
	virtual ~UIHudGameResult();

protected:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;

	virtual void SetParameter(INT32 arg1, INT32 arg2) override;

	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;

protected://기존 inner func
	enum { NULL_EXP = 0, };
	ResultState::UserInform default_user_inform;
	COLOR red_color, blue_color, white_color;

	virtual void update_result_date(result::state state);

	virtual void SetCellVisible(i3UIListView_Box* lvBox, INT32 slotIdx, INT32 cellIdx, bool visible);
	virtual void SetCellClanMark(i3UIListView_Box* lvBox, INT32 slotIdx, INT32 cellIdx, UINT32 ui32MarkFlag, void * pFunc);
	virtual void SetCellText(i3UIListView_Box* lvBox, INT32 slotIdx, INT32 cellIdx, const char* Text);
	virtual void SetCellText(i3UIListView_Box* lvBox, INT32 slotIdx, INT32 cellIdx, INT32 Value);

	virtual void update_end_user_list(const i3::vector< ResultState::UserInform > list[TEAM_COUNT]);
	virtual void update_round_user_list(const i3::vector< ResultState::UserInform > list[TEAM_COUNT]);

	virtual void update_clan_mark(const i3::vector< ResultState::UserInform > list[TEAM_COUNT], i3UIListView_Box* team_list_view[TEAM_COUNT]);

	virtual void update_win_count_only_text(result::win_count & win_count, INT32 red_count, INT32 blue_count);
	virtual void update_win_count(result::win_count & win_count, INT32 red_count, INT32 blue_count, INT32 redList_count, INT32 blueList_count);

	virtual void update_report(result::report & report, const i3::wstring & top_exp_nick, INT32 top_exp_point, const i3::wstring & best_assistor_nick, const i3::wstring & top_killer_nick,
		INT32 top_killer_count, INT32 kill_grade, INT32 assist_grade, INT32 max_kill_count, INT32 headshot_count, const i3::wstring & max_killed_nick, INT32 killed_count);
	virtual void update_benefit_exp(INT32 slot_idx, result::benefit_exp & benefit_exp, INT32 benefit_mark, INT32 total_exp);
	virtual void update_rank(result::rank & rank, INT32 rank_at_start);
	virtual void update_medal(result::medal & medal, const i3::vector<MEDAL_STATE> & medal_list);

	virtual void get_top_exp_user(i3::vector< ResultState::UserInform > list[TEAM_COUNT], i3::wstring & top_exp_nick, INT32 & top_exp_point);
	virtual void get_best_assistor(i3::vector< ResultState::UserInform > list[TEAM_COUNT], i3::wstring & best_assistor_nick);
	virtual INT32 calc_kill_grade(i3::vector< ResultState::UserInform > list[TEAM_COUNT], UINT16 my_kill_count); //등급을 알 수 없는 경우 0 리턴함..
	virtual INT32 calc_assist_grade(i3::vector< ResultState::UserInform > list[TEAM_COUNT], UINT16 my_assist_count);
	virtual void calc_top_killer(i3::vector<INT32> & kill_user_idx_list, UINT16 killer_user_bit_flag);
	virtual void to_plural_user_string(i3::wstring & out_nick, const i3::rc_wstring & first_nick, size_t user_count, I3COLOR color);
	virtual void max_killed_nick_string(i3::wstring & out_max_killed_nick, const i3::vector<INT32> & killed_user_idx_list, i3::vector< ResultState::UserInform > list[TEAM_COUNT]);

	ResultState::UserInform* find_my_inform(i3::vector< ResultState::UserInform > list[TEAM_COUNT]);
	ResultState::UserInform* find_user_inform(i3::vector< ResultState::UserInform > list[TEAM_COUNT], INT32 user_idx);



public: // 도대체 왜이렇게 해놨을까...
	MULTI_KILL_INFO multi_kill_info;
	ResultWinState result_state;
};