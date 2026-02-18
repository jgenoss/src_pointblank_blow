#pragma once

#include "../../../../../Common/CommonSource/CommonDediCli.h"
#include "DomiUserSkillMgr.h"

//=========================================
// ModeScript 데이터
//=========================================
namespace ModeScript
{
	//======================================================
	enum NEW_FLOW_TYPE
	{
		NF_NONE	= 0,
		NF_INTERMISSION,
		NF_DEFENSE,
		NF_SURVIVAL,
		NF_DESTROY,
		NF_COLLECTION,
		NF_TOWERDEFENSE,
		NF_BOSS,
		NF_MAX
	};	


	// Flow 목표 행위 안에서 여러 Mission을 수행한다.
	enum WAVE_MISSION_TYPE
	{
		WAVE_MISSION_NONE	= 0,
		WAVE_MISSION_TIMELIMIT,
		WAVE_MISSION_ELIMINATE,
		WAVE_MISSION_RECOVERY,
		WAVE_MISSION_RECOVERY2,
		WAVE_MISSION_DEFENCE,
		WAVE_MISSION_DESTROY,
		WAVE_MISSION_BOMB,
		WAVE_MISSION_BLOCK,

		WAVE_MISSION_MAX
	};

	const char*		GetFlowName( INT32 val);
	const char*		GetFlowMessage( INT32 val);
};

struct DOMI_BOSS_HP
{
	REAL32	boss_max_hp;
	CGameCharaBase* boss;

	DOMI_BOSS_HP( REAL32 max, CGameCharaBase* p)
	{ 
		boss_max_hp = max;
		boss = p;
	}
};

struct DOMI_SP
{
	UINT16 sp[4];
};

enum DOMI_REWARD_GRADE_STATE
{
	DOMI_REWARD_GRADE_STATE_NONE,
	DOMI_REWARD_GRADE_STATE_FIRST_WAVE,	//1wave
	DOMI_REWARD_GRADE_STATE_USER_ENGER,	//유저 난입 후 리스폰
	DOMI_REWARD_GRADE_STATE_USER_LEAVE,	//유저 나감
};

namespace DomiDefs
{
	struct FLOW
	{
		REAL32	play_time;
		INT16	round;
		INT8	max_wave;		//max 웨이브
		INT8	wave_count;		//current 웨이브

		REAL32	wave_pre_counter;
		REAL32	intermission_time;

		UINT32	total_score[4];
		INT16	max_dino;
		INT16	dino_count;
	};

	struct INTERMISSION
	{
		bool    inIntermission;
		bool	intermission_ready[4];
	};

	struct REVIVAL
	{
		UINT32	single_respawn_count;
		UINT32	everyone_respawn_count;
	};

	struct MUTANTREX_HP
	{
		REAL32	boss_max_hp;
		CGameCharaBase* boss;

		MUTANTREX_HP() {}
		MUTANTREX_HP( REAL32 max, CGameCharaBase* p)
		{ 
			boss_max_hp = max;
			boss = p;
		}
	};

	struct MUTANTREX_HOWL
	{
		INT32  boss_howling_time;

		INT32 boos_dash_target_type;
		INT32  boss_dash_target_idx;

		REAL32 boss_howl_cancel_threshold;
		REAL32 boss_cur_hp_at_howl_start;
	};

	struct AWARD
	{
		UINT32	award_item;			// 지급할 아이템ID
		UINT32	award_item_arg;		// 기간 또는 갯수
		UINT16	award_point;		// point
	};

	struct ACIDITEM_APPEARENCE
	{
		REAL32	etc_time;			// 애시드 아이템 공룡 등장 시간.
		INT8	etc_count;			// 애시드 아이템 공룡 등장 수.
	};

	struct GRADE
	{
		DOMI_REWARD_GRADE_STATE grade_state;
		i3::string grade_user_nick;
	};

	struct RESULT_BASE
	{
		INT16 kill;
		INT32 score;
	};
	struct WAVE_RESULT_INFO
	{
		RESULT_BASE base;
		INT32 slotIdx;
	};
	struct ROUND_RESULT_INFO
	{
		RESULT_BASE base;
		UINT8 icon;
		INT16 exp;
		INT16 point;
		INT32 slotIdx;
	};
}

//스테이지 점프 후에도 삭제 되지 않음.
//제압 미션이 끝낫을 경우 삭제.
template <> struct is_tinst_manual<struct domi_state_info> : std::tr1::true_type {};
struct domi_state_info
{
	domi_state_info() {}
	~domi_state_info() {}

	static domi_state_info* i() { return tinst<domi_state_info>(); }

	static void create()
	{ 		
		if( tinst<domi_state_info>() == nullptr)
		{
			tinst<domi_state_info>() = new domi_state_info();
			MEMDUMP_NEW( tinst<domi_state_info>(), sizeof( domi_state_info));

			tinst<domi_state_info>()->init();

			// 유저 스킬
			tinst<DomiUserSkillMgr>() = new DomiUserSkillMgr();
			MEMDUMP_NEW( tinst<DomiUserSkillMgr>(), sizeof( DomiUserSkillMgr));
		}
	}
	static void destroy()
	{
		if( tinst<domi_state_info>() != nullptr)
		{
			I3_SAFE_DELETE(tinst<DomiUserSkillMgr>());

			delete tinst<domi_state_info>();
			tinst<domi_state_info>() = nullptr;
		}
	}

	void init();
	void init_wave();
	void init_shop_item();

	//buffer 용.
	bool is_wave_end;


	//sp ui
	DOMI_SP sp;



	//flow ui
	REAL32	play_time;
	INT16	round;
	INT8	max_wave;		//max 웨이브
	INT8	wave_count;		//current 웨이브

	REAL32	wave_pre_counter;
	REAL32	intermission_time;

	UINT32	total_score[4];
	INT16	max_dino;
	INT16	dino_count;




	//intermission ui
	bool    inIntermission;
	bool	intermission_ready[4];





	//revival
	UINT32	single_respawn_count;
	UINT32	everyone_respawn_count;




	//boss hp ui
	i3::vector<DOMI_BOSS_HP> boss_list;




	//award ui
	UINT32	award_item;			// 지급할 아이템ID
	UINT32	award_item_arg;		// 기간 또는 갯수
	UINT16	award_point;		// point




	//item acid ui
	REAL32	etc_time;			// 애시드 아이템 공룡 등장 시간.
	INT8	etc_count;			// 애시드 아이템 공룡 등장 수.
	
	// mutanttrex howling time
	INT32  boss_howling_time;
	
	INT32 boos_dash_target_type;
	INT32  boss_dash_target_idx;
	
	REAL32 boss_howl_cancel_threshold;
	REAL32 boss_cur_hp_at_howl_start;

	
	//grade valiable
	DOMI_REWARD_GRADE_STATE grade_state;
	i3::rc_wstring grade_user_nick;
};

//namespace domi_util
//{
//	void buy_domi_item(CASHITEM_GROUP_TYPE group_type); //아이템을 구매한다.
//	void buy_domi_cash_item(CASHITEM_GROUP_TYPE group_type); //아이템을 구매한다.
//	INT32 get_cash_item_count(CASHITEM_GROUP_TYPE group_type); //없는 경우 0 리턴
//
//	bool is_buyable_domi_item(CASHITEM_GROUP_TYPE buy_group_type); //서버 상품 목록에 등록 여부에 따라 구매 가능한지..
//	bool is_buyable_domi_cash_item(CASHITEM_GROUP_TYPE buy_group_type); //서버 상품 목록에 등록 여부에 따라 구매 가능한지..
//}