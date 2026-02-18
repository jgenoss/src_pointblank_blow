#pragma once

#include "../../../../CommonSource/CommonDediCli.h"

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


//스테이지 점프 후에도 삭제 되지 않음.
//제압 미션이 끝낫을 경우 삭제.
template <> struct is_tinst_manual<class domi_state_info> : true_type {};
class domi_state_info
{
public:
	domi_state_info() {}
	~domi_state_info() {}

	static void create()
	{ 
		if( tinst<domi_state_info>() == NULL) 
			tinst<domi_state_info>() = new domi_state_info();

		if( tinst<domi_state_info>() != NULL) 
			tinst<domi_state_info>()->init();
	}
	static void destroy()
	{
		if( tinst<domi_state_info>() != NULL)
		{
			delete tinst<domi_state_info>();
			tinst<domi_state_info>() = NULL;
		}
	}

	void init();
	void init_wave();

	// 디버깅 정보

	ST_DOMI_PROTOCOL_FIRST_VALUE_ACK first_value;
	ST_DOMI_PROTOCOL_USER_SP_ACK sp;

	INT32	create_count;
	INT32	respawn_count;
	INT32	death_count;
	REAL32	wave_pre_counter;
	REAL32	play_time;
	REAL32	intermission_time;
	INT16	max_dino;
	INT16	dino_count;
	INT16	single_respawn_count;
	INT16	everyone_respawn_count;
	INT16	round;			//라운드
	UINT32	total_score;
	INT8	max_wave;		//max 웨이브
	INT8	wave_count;		//current 웨이브
	INT8	wave_type;		// 인터미션
	INT8	user_skill_lv[8][USER_SKILL_COUNT];
	bool    inIntermission;
	bool	intermission_ready[4];

	REAL32	boss_max_hp;


	UINT32	award_item;			// 지급할 아이템ID
	UINT32	award_item_arg;		// 기간 또는 갯수


	REAL32	etc_time;			// 애시드 아이템 공룡 등장 시간.
	INT8	etc_count;			// 애시드 아이템 공룡 등장 수.



	UINT32	debug_total_score;
	

	UINT16	debug_combo_scroe;
	UINT16	debug_sub_score;
	UINT16	debug_spe_score;

	UINT16  ui_combo_sum;
	UINT16	ui_combo_add;
	UINT16	ui_spe_score;
};
