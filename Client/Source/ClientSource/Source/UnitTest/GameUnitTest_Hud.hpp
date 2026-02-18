#include "pch.h"
#if ENABLE_UNIT_TEST

#include "../ui/UIHudManager.h"
#include "../ui/UIHudUtil.h"

namespace inner_hud
{
	void set_hud_death_match()
	{
		//MODE_NEW_CHEAK hu_ut::test_factor.stage_id = STAGE_ID_TD_PORTAKABA;
		hu_ut::test_factor.room_sub_type = 0;
		g_pFramework->PrintHudUnitTest("Death Match");
	}
	void set_hud_eliminate()
	{
		//MODE_NEW_CHEAK hu_ut::test_factor.stage_id = STAGE_ID_TD_PORTAKABA;
		hu_ut::test_factor.room_sub_type = 0;
		g_pFramework->PrintHudUnitTest("Eliminate");
	}
	void set_hud_challenge()
	{
		//MODE_NEW_CHEAK hu_ut::test_factor.stage_id = STAGE_ID_TD_PORTAKABA;
		hu_ut::test_factor.room_sub_type = 0;
		g_pFramework->PrintHudUnitTest("Challenge");
	}
	void set_hud_destroy()
	{
		//MODE_NEW_CHEAK hu_ut::test_factor.stage_id = STAGE_ID_DS_BREAKDOWN;
		hu_ut::test_factor.room_sub_type = 1;
		g_pFramework->PrintHudUnitTest("Destroy");
	}
	void set_hud_bomb_mission()
	{
		//MODE_NEW_CHEAK hu_ut::test_factor.stage_id = STAGE_ID_BB_DOWNTOWN;
		hu_ut::test_factor.room_sub_type = 1;
		g_pFramework->PrintHudUnitTest("Bomb Mission");
	}
	void set_hud_defense()
	{
		//MODE_NEW_CHEAK hu_ut::test_factor.stage_id = STAGE_ID_DF_BLACKPANTHER;
		hu_ut::test_factor.room_sub_type = 1;
		g_pFramework->PrintHudUnitTest("Defense");
	}
	void set_hud_escape()
	{
		//MODE_NEW_CHEAK hu_ut::test_factor.stage_id = STAGE_ID_ES_BREEDING_NEST;
		hu_ut::test_factor.room_sub_type = 1;
		g_pFramework->PrintHudUnitTest("Escape");
	}
	void set_hud_domination()
	{
		//MODE_NEW_CHEAK hu_ut::test_factor.stage_id = STAGE_ID_DM_0RANDOMSTAGE;
		hu_ut::test_factor.room_sub_type = 1;
		g_pFramework->PrintHudUnitTest("Domination");
	}
	void set_hud_cross_count()
	{
		//hu_ut::test_factor.stage_id = STAGE_ID_CC_CRACKDOWN;
		//hu_ut::test_factor.stage_mask_type = STAGE_MASK_TYPE_NONE;
		//hu_ut::test_factor.room_sub_type = 1;
		//hu_ut::test_factor.chara_dino_type = !hu_ut::test_factor.chara_dino_type; //human <-> dino 를 변경한다.

		//switch( i3Math::Rand() % 4)
		//{
		//case 0: hu_dino::set_dino_attribute( GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_ACID"), DinoState::SPIT, DinoState::SPIT ); break;
		//case 1: hu_dino::set_dino_attribute( GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_STING"), DinoState::BITE, DinoState::SPIT ); break;
		//case 2: hu_dino::set_dino_attribute( GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_STING"), DinoState::BITE, DinoState::SPIT ); break;
		//case 3: hu_dino::set_dino_attribute( GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_RAPTOR"), DinoState::SCRATCH, DinoState::BITE ); break;
		//}

		//g_pFramework->PrintHudUnitTest("Cross Count");
	}

	void (*pf_set_hud[])() = { 
		set_hud_death_match, set_hud_eliminate, set_hud_challenge,
		set_hud_destroy, set_hud_bomb_mission, set_hud_defense,
		set_hud_escape, set_hud_domination, set_hud_cross_count,
	};

	static INT32 hud_idx = 0;
	void change_test_stage_type()
	{
		inner_hud::pf_set_hud[inner_hud::hud_idx]();
		if( inner_hud::hud_idx >=  ( sizeof( inner_hud::pf_set_hud ) / sizeof( inner_hud::pf_set_hud[0] ) ) -1 )
			inner_hud::hud_idx = 0;
		else
			++inner_hud::hud_idx;

		hu_ut::test_factor.changing_hud = true;

		HUD::instance()->SetUsableAllHud();
		HUD::instance()->OnEvent(EVENT_START_BATTLE, 0, i3::user_data());
		HUD::instance()->OnEvent(EVENT_FIRST_RESPWAN_BATTLE, 0, i3::user_data());
		HUD::instance()->OnEvent(EVENT_RESPAWN_BATTLE, 0, i3::user_data());
		HUD::instance()->OnEvent(EVENT_M_ROUNDSTART_BATTLE, 0, i3::user_data());

		hu_ut::test_factor.changing_hud = false;
	}

	void change_test_factor()
	{
		hu_ut::test_factor.hp = i3Math::Rand() % 100;
		hu_ut::test_factor.kill = i3Math::Rand() % 100;
		hu_ut::test_factor.death = i3Math::Rand() % 100;
		hu_ut::test_factor.challenge_total_score = i3Math::Rand() % 1000;
		hu_ut::test_factor.remain_battle_time = i3Math::Rand() % (10*60); //10분
		hu_ut::test_factor.red_score = i3Math::Rand() % 100;
		hu_ut::test_factor.blue_score = i3Math::Rand() % 100;
		hu_ut::test_factor.red_object_hp = i3Math::Rand() % 100;
		hu_ut::test_factor.blue_object_hp = i3Math::Rand() % 100;
		hu_ut::test_factor.round_goal_count = i3Math::Rand() % 100;
		hu_ut::test_factor.escape_block_time = (REAL32)(i3Math::Rand() % 35);
		hu_ut::test_factor.chara_action_body_lower = (CHARA_ACTION_BODYLOWER)(i3Math::Rand() % CHARA_BODYLOWER_MAXCOUNT);
		hu_ut::test_factor.bullet = i3Math::Rand() % 100;

		switch( i3Math::Rand() % 3 )
		{
		case 0: hu_ut::test_factor.headgear_parts_state = HUD::IS_NONE; break;
		case 1: hu_ut::test_factor.headgear_parts_state = HUD::IS_HAVE; break;
		case 2: hu_ut::test_factor.headgear_parts_state = HUD::IS_NOT_HAVE; break;
		}
		switch( i3Math::Rand() % 3 )
		{
		case 0: hu_ut::test_factor.c5_state = HUD::IS_NONE; break;
		case 1: hu_ut::test_factor.c5_state = HUD::IS_USE; break;
		case 2: hu_ut::test_factor.c5_state = HUD::IS_NOT_USE; break;
		}

		const i3::vector<CWeaponInfo*> & weapon_list = g_pWeaponInfoDataBase->m_WeaponInfoList[ i3Math::Rand() % WEAPON_CLASS_COUNT];
		if( weapon_list.size() > 0)
			hu_ut::test_factor.pInfo = weapon_list[ i3Math::Rand() % weapon_list.size() ];
		else
			hu_ut::test_factor.pInfo = nullptr;
	}
}

#endif //end of ENABLE_UNIT_TEST
