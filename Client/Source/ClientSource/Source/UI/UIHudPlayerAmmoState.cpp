#include "pch.h"
#include "UIHudPlayerAmmoState.h"

#include "UIHudUtil.h"
#include "UIUtil.h"
#include "Camera/GameCamera_OnObject.h"

#include "BattleHud.h"
#include "UIMath.h"
#include "GameMissionManager.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

namespace inner
{
	enum { TURN_TIME = 1000/4, MAX_WARNING_CNT = 14,};

	INT32 warning_cnt, delta_time;
	bool is_update_warning;

	INT32 current_hp;
}

UIHudPlayerAmmoState::UIHudPlayerAmmoState(UIHudManager* p) : UIHudState(p)
{
	i3_prof_func();

	m_is_dino_state_hud = 0;
	
	m_changed_bullet_type = m_changed_helicopter_type = -1;
	m_CharaLowerState = CHARA_BODYLOWER_STAND;

	pf_ammo_update[BULLET] = &UIHudPlayerAmmoState::update_bullet;
	pf_ammo_update[HELICOPTER] = &UIHudPlayerAmmoState::update_gauge;
	pf_ammo_update[GAUGEBULLET] = &UIHudPlayerAmmoState::update_bullet_gauge;
}

UIHudPlayerAmmoState::~UIHudPlayerAmmoState()
{
}

/*********************************/
void UIHudPlayerAmmoState::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "PlayerAmmoState");

	hs_1_0::set_control(player_control, pScene);
}

void	UIHudPlayerAmmoState::Update( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	update_player(rDeltaSeconds);

	INT32 bullet_type = 0;

	if(i3::same_of<CGameCamera_OnObject*>(g_pCamera))
		bullet_type = 1;
	else if( hu::get_weapon_bullet_type(s_pMyChara) == true)
		bullet_type = 2;

	if( bullet_type != m_changed_helicopter_type )
		chang_ammo_show_type( bullet_type );

	update_score(rDeltaSeconds);

	(this->*pf_ammo_update[bullet_type])(rDeltaSeconds);
}

void UIHudPlayerAmmoState::update_player(REAL32 rDeltaSeconds)
{
	i3_prof_func();

	INT32 hp =0, max_hp = 0;
	hu::get_chara_hp(s_pMyChara, hp, max_hp);
	player_control.player_hp.SetNumber(hp, 0, max_hp);

	if(inner::current_hp == INT_MIN) //hp 초기 설정
		inner::current_hp = hp;
	
	if(inner::current_hp != hp ) //hp가 다른 경우,, 경고 시작
	{
		inner::current_hp = hp;

		inner::delta_time = inner::TURN_TIME;
		inner::is_update_warning = true;
		inner::warning_cnt = 0;
	}

	if(CGameMissionManager::i()->isMyUsurpatio() == true)
	{

		if( player_control.ammo_equip_imgset[hs_1_0::EQUIP_MISSION]->isEnable() == false)
			player_control.ammo_equip_imgset[hs_1_0::EQUIP_MISSION]->EnableCtrl(true);
	}
	else
	{
		if( player_control.ammo_equip_imgset[hs_1_0::EQUIP_MISSION]->isEnable() == true)
			player_control.ammo_equip_imgset[hs_1_0::EQUIP_MISSION]->EnableCtrl(false);
	}


	if(inner::current_hp == 0) //hp가 0 인 상태
	{
		inner::is_update_warning = false;
		player_control.player_state_imgset->EnableCtrl(true);
	}

	CHARA_ACTION_BODYLOWER act_state = hu::get_chara_action_state(s_pMyChara);
	if( m_CharaLowerState != act_state)
	{
		m_CharaLowerState = act_state;
		player_control.player_state_imgset->SetShapeIdx(m_CharaLowerState);
	}

	if(inner::is_update_warning == true)
	{
		inner::delta_time -= INT32(rDeltaSeconds * 1000);
		inner::delta_time = uiMath::limited(inner::delta_time, (INT32)0, (INT32)inner::TURN_TIME);

		if( inner::delta_time == 0 )
		{
			inner::delta_time = inner::TURN_TIME;

			++inner::warning_cnt;
			player_control.player_state_imgset->EnableCtrl(	( inner::warning_cnt % 2 ) != 0);

			if( inner::warning_cnt > inner::MAX_WARNING_CNT )
			{
				player_control.player_state_imgset->EnableCtrl(true);
				inner::is_update_warning = false;               
			}
		}
	}
}

void UIHudPlayerAmmoState::update_bullet( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	INT32 bullet =0, max_bullet = 0, max_load_bullet = 0;
	hu::get_bullet_cnt(s_pMyChara, bullet, max_bullet, max_load_bullet);
	WEAPON_SLOT_TYPE slot_type = hu::get_weapon_slot_type(s_pMyChara);

	player_control.ammo_bullet[hs_1_0::CURR].SetNumber(bullet);
	player_control.ammo_bullet[hs_1_0::FULL].SetNumber(max_bullet);

	hs_1_0::update_bullet_count(player_control, bullet, max_load_bullet);

	hs_1_0::calc_bullet_animation(player_control, max_load_bullet, slot_type);
	hs_1_0::update_bullet_animation(player_control, rDeltaSeconds);

	player_control.ammo_weapon_text->SetText( hu::get_weapon_name(s_pMyChara) );

	HUD::ItemState headgear_st = hu::get_helmet_protect_state(s_pMyChara);
	hc::btn_set_shape(player_control.ammo_equip_imgset[hs_1_0::EQUIP_HELMET], headgear_st);

	HUD::ItemState c5_st = hu::get_c5_state(s_pMyChara);
	hc::btn_set_shape(player_control.ammo_equip_imgset[hs_1_0::EQUIP_C5], c5_st);
}

void UIHudPlayerAmmoState::update_gauge( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	REAL32 bullet =0, max_bullet = 0;

	hu::get_m179_gauge(bullet, max_bullet);
	player_control.ammo_guage_progress.SetNumber(bullet, 0.f, max_bullet);

	player_control.ammo_weapon_text->SetText( hu::get_m179_name() );
}

void UIHudPlayerAmmoState::update_bullet_gauge( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	REAL32 bullet =0, max_bullet = 0;

	hu::get_bullet_cntF(s_pMyChara, bullet, max_bullet);

	player_control.ammo_bullet[hs_1_0::CURR].SetNumber((INT32)bullet);
	player_control.ammo_bullet[hs_1_0::FULL].SetNumber((INT32)max_bullet);

	player_control.ammo_guage_progress.SetNumber(bullet, 0.f, max_bullet);

	player_control.ammo_weapon_text->SetText( hu::get_weapon_name(s_pMyChara) );
}

void UIHudPlayerAmmoState::chang_ammo_show_type(INT32 ammo_type)
{
	i3_prof_func();

	m_changed_bullet_type = ammo_type;
	m_changed_helicopter_type = ammo_type;

	hs_1_0::change_player_ammo_tpye(player_control, ammo_type);

}

/*************************************/
/*virtual*/ void UIHudPlayerAmmoState::event_respawn_battle_impl(INT32 arg)
{
	i3_prof_func();

	hs_1_0::show_player_hud(player_control);
	player_control.player_state_imgset->SetShapeIdx(0); //인간 모습으로 변경
	chang_ammo_show_type(0);

	inner::is_update_warning = false;
	inner::current_hp = INT_MIN;
}

void UIHudPlayerAmmoState :: update_score(REAL32 rDeltaSeconds)
{
	hs_1_0::update_score_hud(player_control, rDeltaSeconds);
}