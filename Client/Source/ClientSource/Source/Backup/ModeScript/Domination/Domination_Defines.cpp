#include "pch.h"
#include "Domination_Defines.h"

void domi_state_info::init()
{
	round = 0;			//라운드
	max_wave = 0;		//max 웨이브

	wave_count = 1;		//current 웨이브
	wave_type = 0;
	wave_pre_counter = 0.f;

	max_dino = 0;
	dino_count = 0;

	total_score = 0;
	play_time = 0.f;

	intermission_time = 0.f;
	inIntermission = false;

	single_respawn_count = 0;
	everyone_respawn_count = 0;

	memset( intermission_ready, 0, sizeof(bool)*4);
	memset( user_skill_lv, 0, sizeof(INT8)*32);
	memset( &sp, 0, sizeof(ST_DOMI_PROTOCOL_USER_SP_ACK));

	create_count = 0;
	respawn_count = 0;
	death_count = 0;

	boss_max_hp = 0.f;

	award_item = 0;
	award_item_arg = 0;

	etc_time = 0.f;
	etc_count = 0;

	debug_total_score = 0;

	debug_combo_scroe = 0;
	debug_sub_score = 0;
	debug_spe_score = 0;

	ui_combo_sum = 0;
	ui_combo_add = 0;
	ui_spe_score = 0;
}

void domi_state_info::init_wave()
{
	dino_count = 0;
	memset( intermission_ready, 0, sizeof(bool)*4);

	create_count = 0;
	respawn_count = 0;
	death_count = 0;

	award_item = 0;
	award_item_arg = 0;

	etc_time = 0.f;
	etc_count = 0;
}