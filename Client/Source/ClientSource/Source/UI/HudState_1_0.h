#pragma once

#include "ui/NumberCount.h"
#include "ui/ProgressBar.h"

namespace hs_1_0
{
	enum	{ CURR, FULL, MAXIMUN, };
	enum { EQUIP_HELMET, EQUIP_C5, EQUIP_MISSION, EQUIP_MAX, };
	enum { SKILL1, SKILL2, SKILL_MAX, };
	enum { RESULT_SCORE, ADD_SCORE, SCORE_MAX, };
	enum
	{
		ENUM_KNIFE = 0,
		ENUM_GRENADE,
		ENUM_SMOKEGRENADE,
		ENUM_KNIFE2,
		ENUM_BOMBTRIGGER,
		ENUM_M197,
	};

	struct control
	{
		i3UIFrameWnd* player_wnd, *dino_wnd;

		//player : 공통으로 사용된다.
		i3UIButtonImageSet* player_bg; //i3UIImageBox1
		ColorNumberCount player_hp;
		i3UIButtonImageSet* player_state_imgset;

		//ammo
		NumberCount ammo_bullet[hs_1_0::MAXIMUN];
		i3UIImageBox* ammo_bullet_img; //progress bar 같은거다.
		i3UIButtonImageSet* ammo_bullet_img_set; //총알 날라가는거
		RealColorProgressBar ammo_guage_progress;
		i3UIStaticText* ammo_slash_text;
		i3UIStaticText* ammo_weapon_text;
		i3UIButtonImageSet* ammo_WeaponImg;

		i3UIButtonImageSet* ammo_equip_imgset[hs_1_0::EQUIP_MAX];

		//dino
		i3UIStaticText* dino_skill_text[hs_1_0::SKILL_MAX];
		i3UIButtonImageSet* dino_skill_imgset[hs_1_0::SKILL_MAX];
		i3UIProgressBar*	dino_skill_guage;

		//original bullet size
		INT32 curr_bullet_count, curr_bullet_ani_idx; //curr_bullet_ani_idx == -1은 비활성화 상태
		WEAPON_SLOT_TYPE curr_weapon_slot_type;
		bool need_bullet_ani_calc;

		//Score
		REAL32 ScoreColorTime;
		INT32 AddScore;
		INT32 ResultScore;

		i3UIStaticText* UI_ResultScore;
		i3UIStaticText* UI_AddScore;
	};

	void set_control(hs_1_0::control& control, i3UIScene * pScene);
	
	void show_player_hud(hs_1_0::control& control);
	void show_dino_hud(hs_1_0::control& control);
	void change_player_ammo_tpye(hs_1_0::control& control, INT32 ammo_type);

	void update_bullet_count(hs_1_0::control& control, INT32 bullet, INT32 max_load_bullet);

	void calc_bullet_animation(hs_1_0::control& control, INT32 max_load_bullet, WEAPON_SLOT_TYPE slot_type);
	void update_bullet_animation(hs_1_0::control& control, REAL32 rDeltaSeconds);

	void update_score_hud(hs_1_0::control& control, REAL32 rDeltaSeconds);
}
