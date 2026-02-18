#include "pch.h"
#include "HudState_1_0.h"

#include "iHudBase.h"

//#define i3_profile_enable
#include "UIHudState.h"
#include "i3Base/profile/profile.h"
#include "ui/UIMath.h"
#include "UIHudUtil.h"
#include "ChainKills.h"
#include "UIHudDefine.h"
#include "BattleSlotContext.h"
#include "GameCharaActionContext.h"

namespace inner
{
	const REAL32 perido_time = 0.002f;
	static REAL32 delta_sec = 0.f;

	void act_bullet_ani(hs_1_0::control& control);
}

//- hs_1_0 ------------------------------------//
void hs_1_0::set_control(hs_1_0::control& control, i3UIScene * pScene)
{
	i3_prof_func();

	control.player_wnd = FindControl<i3UIFrameWnd*>(pScene, "PlayerAmmoState");
	control.dino_wnd = FindControl<i3UIFrameWnd*>(pScene, "DinoAmmoState");

	//common
	control.player_bg = FindControl<i3UIButtonImageSet*>(pScene, "ps_bg");

	control.player_hp.AddNumberControl(pScene, "ps_Img_Cnt0");
	control.player_hp.AddNumberControl(pScene, "ps_Img_Cnt1");
	control.player_hp.AddNumberControl(pScene, "ps_Img_Cnt2");
	control.player_hp.EnableShowFrontZero(true);
//	control.player_hp.SetColor(128, 255, 0); //연녹색

	control.player_hp.AddMaxColorSpot(142, 183,  64); //연녹색
	control.player_hp.AddColorSpot(80, 202, 176,  54); //노란
	control.player_hp.AddColorSpot(60, 202, 134,  54); //살구
	control.player_hp.AddColorSpot(40, 211,  92,  34); //갈색
	control.player_hp.AddColorSpot(20, 181,  35,   0); //빨간


	control.player_state_imgset = FindControl<i3UIButtonImageSet*>(pScene, "ps_ImgSet_State");

	//ammo
	control.ammo_bullet[hs_1_0::CURR].AddNumberControl(pScene, "am_Img_Cnt0");
	control.ammo_bullet[hs_1_0::CURR].AddNumberControl(pScene, "am_Img_Cnt1");
	control.ammo_bullet[hs_1_0::CURR].AddNumberControl(pScene, "am_Img_Cnt2");
	control.ammo_bullet[hs_1_0::CURR].EnableShowFrontZero(true);
	control.ammo_bullet[hs_1_0::FULL].AddNumberControl(pScene, "am_Img_Cnt3");
	control.ammo_bullet[hs_1_0::FULL].AddNumberControl(pScene, "am_Img_Cnt4");
	control.ammo_bullet[hs_1_0::FULL].AddNumberControl(pScene, "am_Img_Cnt5");
	control.ammo_bullet[hs_1_0::FULL].EnableShowFrontZero(true);

	control.ammo_bullet_img = FindControl<i3UIImageBox*>(pScene, "am_Img_Bullet");
	control.ammo_bullet_img_set = FindControl<i3UIButtonImageSet*>(pScene, "am_ImgSet_Bullet");
	control.ammo_WeaponImg = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet");
	
	control.ammo_guage_progress.SetControl(pScene, "am_Progree_Guage");
	control.ammo_guage_progress.AddMaxColorSpot(255,255,255);
	control.ammo_guage_progress.AddColorSpot(90, 255,255,255); //없어도 되는 코드
	control.ammo_guage_progress.AddColorSpot(80, 255,225,225); //없어도 되는 코드
	control.ammo_guage_progress.AddColorSpot(70, 255,200,200);
	control.ammo_guage_progress.AddColorSpot(60, 255,175,175);
	control.ammo_guage_progress.AddColorSpot(50, 255,150,150);
	control.ammo_guage_progress.AddColorSpot(40, 255,125,125);
	control.ammo_guage_progress.AddColorSpot(30, 255,100,100);
	control.ammo_guage_progress.AddColorSpot(20, 255,75,75);
	control.ammo_guage_progress.AddColorSpot(10, 255,50,50);
	control.ammo_guage_progress.AddColorSpot(0, 255, 0, 0);

	control.ammo_weapon_text = FindControl<i3UIStaticText*>(pScene, "am_ST_Wp_Name");
	control.ammo_slash_text = FindControl<i3UIStaticText*>(pScene, "am_ST_Slash_Name");
	control.ammo_slash_text->SetText("/");

	control.ammo_equip_imgset[hs_1_0::EQUIP_HELMET] = FindControl<i3UIButtonImageSet*>(pScene, "am_ImgSet_Helmet");
	control.ammo_equip_imgset[hs_1_0::EQUIP_C5] = FindControl<i3UIButtonImageSet*>(pScene, "am_ImgSet_C5");
	control.ammo_equip_imgset[hs_1_0::EQUIP_MISSION] = FindControl<i3UIButtonImageSet*>(pScene, "am_ImgSet_Mission");
	control.ammo_equip_imgset[hs_1_0::EQUIP_MISSION]->SetShapeIdx(HUD::IS_HAVE_MISSION_OBJ);
	control.ammo_equip_imgset[hs_1_0::EQUIP_MISSION]->EnableCtrl(false);

	//dino
	control.dino_skill_text[hs_1_0::SKILL1] = FindControl<i3UIStaticText*>(pScene, "ds_skill1");
	control.dino_skill_text[hs_1_0::SKILL2] = FindControl<i3UIStaticText*>(pScene, "ds_skill2");

	control.dino_skill_imgset[hs_1_0::SKILL1] = FindControl<i3UIButtonImageSet*>(pScene, "ds_Attack1");
	control.dino_skill_imgset[hs_1_0::SKILL2] = FindControl<i3UIButtonImageSet*>(pScene, "ds_Attack2");

	control.dino_skill_guage	= FindControl<i3UIProgressBar*>(pScene, "ds_SkillGuage");

	control.curr_bullet_count = -1;
	control.curr_bullet_ani_idx = -1;
	control.curr_weapon_slot_type = WEAPON_SLOT_UNKNOWN;
	control.need_bullet_ani_calc = false;

	inner::act_bullet_ani(control);

	//score
	control.UI_ResultScore = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText2");
	control.UI_AddScore = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText3");

	control.UI_ResultScore->SetText("0");
	control.UI_AddScore->SetText("");

	control.AddScore = 0;
	control.ResultScore = 0;
	control.ScoreColorTime = 0.f;
}

void hs_1_0::show_player_hud(hs_1_0::control& control)
{
	i3_prof_func();

	control.player_wnd->EnableCtrl(true); 
	control.dino_wnd->EnableCtrl(true);

	//player : 공통으로 사용된다.
	control.player_bg->EnableCtrl(true);
	control.player_hp.SetVisible(true);
	control.player_state_imgset->EnableCtrl(true);

	//ammo
	control.ammo_bullet[hs_1_0::CURR].SetVisible(true); 
	control.ammo_bullet[hs_1_0::FULL].SetVisible(true);
	control.ammo_bullet_img->EnableCtrl(true);
	control.ammo_bullet_img_set->EnableCtrl(false);
	control.ammo_WeaponImg->EnableCtrl(false);
	control.ammo_guage_progress.SetEnable(true);
	control.ammo_weapon_text->EnableCtrl(true);
	control.ammo_slash_text->EnableCtrl(true);

	control.ammo_equip_imgset[hs_1_0::EQUIP_HELMET]->EnableCtrl(true);
	control.ammo_equip_imgset[hs_1_0::EQUIP_C5]->EnableCtrl(true);

	//dino
	control.dino_skill_text[hs_1_0::SKILL1]->EnableCtrl(false);
	control.dino_skill_text[hs_1_0::SKILL2]->EnableCtrl(false);

	control.dino_skill_imgset[hs_1_0::SKILL1]->EnableCtrl(false);
	control.dino_skill_imgset[hs_1_0::SKILL2]->EnableCtrl(false);

	control.dino_skill_guage->EnableCtrl(false);
}
void hs_1_0::show_dino_hud(hs_1_0::control& control)
{
	i3_prof_func();

	control.dino_wnd->EnableCtrl(true);
	control.player_wnd->EnableCtrl(true); 

	//player : 공통으로 사용된다.
	control.player_bg->EnableCtrl(true);
	control.player_hp.SetVisible(true);
	control.player_state_imgset->EnableCtrl(true);

	//ammo
	control.ammo_bullet[hs_1_0::CURR].SetVisible(false); 
	control.ammo_bullet[hs_1_0::FULL].SetVisible(false);
	control.ammo_bullet_img->EnableCtrl(false);
	control.ammo_bullet_img_set->EnableCtrl(false);
	control.ammo_guage_progress.SetEnable(false);
	control.ammo_weapon_text->EnableCtrl(false);
	control.ammo_slash_text->EnableCtrl(false);
	control.ammo_WeaponImg->EnableCtrl(false);

	control.ammo_equip_imgset[hs_1_0::EQUIP_HELMET]->EnableCtrl(false);
	control.ammo_equip_imgset[hs_1_0::EQUIP_C5]->EnableCtrl(false);
	control.ammo_equip_imgset[hs_1_0::EQUIP_MISSION]->EnableCtrl(false);

	//dino
	control.dino_skill_text[hs_1_0::SKILL1]->EnableCtrl(true);
	control.dino_skill_text[hs_1_0::SKILL2]->EnableCtrl(true);

	control.dino_skill_imgset[hs_1_0::SKILL1]->EnableCtrl(true);
	control.dino_skill_imgset[hs_1_0::SKILL2]->EnableCtrl(true);

	control.dino_skill_guage->EnableCtrl(true);
}

void hs_1_0::change_player_ammo_tpye(hs_1_0::control& control, INT32 ammo_type)
{
	i3_prof_func();

	switch(ammo_type)
	{
	case BULLET:
		{
			control.ammo_bullet[hs_1_0::CURR].SetVisible(true); 
			control.ammo_bullet[hs_1_0::CURR].ResetDisplay();
			control.ammo_bullet[hs_1_0::FULL].SetVisible(true); 
			control.ammo_bullet[hs_1_0::FULL].ResetDisplay();

			control.ammo_slash_text->EnableCtrl(true);

			control.ammo_bullet_img->EnableCtrl(true);
			control.ammo_bullet_img_set->EnableCtrl(false);
			control.ammo_guage_progress.SetEnable( false );
			control.ammo_WeaponImg->EnableCtrl(false);

			control.ammo_equip_imgset[hs_1_0::EQUIP_HELMET]->EnableCtrl(true);
			control.ammo_equip_imgset[hs_1_0::EQUIP_C5]->EnableCtrl(true);

		}
		break;
	case HELICOPTER:
		{
			control.ammo_bullet[hs_1_0::CURR].SetVisible(false); 
			control.ammo_bullet[hs_1_0::CURR].ResetDisplay();
			control.ammo_bullet[hs_1_0::FULL].SetVisible(false); 
			control.ammo_bullet[hs_1_0::FULL].ResetDisplay();

			control.ammo_slash_text->EnableCtrl(false);

			control.ammo_bullet_img->EnableCtrl(false);
			control.ammo_bullet_img_set->EnableCtrl(false);
			control.ammo_guage_progress.SetEnable( true );
			control.ammo_WeaponImg->SetShapeIdx(ENUM_M197);
			control.ammo_WeaponImg->EnableCtrl(true);

			control.ammo_equip_imgset[hs_1_0::EQUIP_HELMET]->EnableCtrl(false);
			control.ammo_equip_imgset[hs_1_0::EQUIP_C5]->EnableCtrl(false);

		}
		break;
	case GAUGEBULLET:
		{
			control.ammo_bullet[hs_1_0::CURR].SetVisible(true); 
			control.ammo_bullet[hs_1_0::CURR].ResetDisplay();
			control.ammo_bullet[hs_1_0::FULL].SetVisible(true); 
			control.ammo_bullet[hs_1_0::FULL].ResetDisplay();

			control.ammo_slash_text->EnableCtrl(true);

			control.ammo_bullet_img->EnableCtrl(false);
			control.ammo_bullet_img_set->EnableCtrl(false);
			control.ammo_guage_progress.SetEnable( true );
			control.ammo_WeaponImg->EnableCtrl(false);

			control.ammo_equip_imgset[hs_1_0::EQUIP_HELMET]->EnableCtrl(true);
			control.ammo_equip_imgset[hs_1_0::EQUIP_C5]->EnableCtrl(true);
			
		}
		break;
	}

}

void hs_1_0::update_bullet_count(hs_1_0::control& control, INT32 bullet, INT32 max_load_bullet)
{
	enum { MAX_BULLET_CNT = 20, BULLET_IMG_WIDTH = 4,  };

	if( control.curr_bullet_count == bullet ) return;
	control.curr_bullet_count = bullet;
	control.need_bullet_ani_calc = true;

	if( bullet == 0 ) //더 이상 할게 없음
	{ 
		control.ammo_bullet_img->EnableCtrl(false); 
		return; 
	}

	control.ammo_bullet_img->EnableCtrl(true);

	INT32 factor = bullet;
	if( max_load_bullet > MAX_BULLET_CNT  )
	{
		if( bullet == max_load_bullet )
		{
			factor = MAX_BULLET_CNT;
		}
		else
		{
			factor = uiMath::remap_ranges(0, (INT32)MAX_BULLET_CNT, bullet, 0, max_load_bullet);
			if( factor == 0 && bullet != 0 ) factor = 1;
		}
	}

	REAL32 shape_w = (REAL32)factor * BULLET_IMG_WIDTH;

	i3UIShape* shape = control.ammo_bullet_img->getShape(0);
	shape->setCoordW( shape_w );

	VEC2D* size = control.ammo_bullet_img->getSize();
	size->x = shape_w;
	control.ammo_bullet_img->setSize(size);
}

void hs_1_0::calc_bullet_animation(hs_1_0::control& control, INT32 max_load_bullet, WEAPON_SLOT_TYPE slot_type)
{
	if( control.need_bullet_ani_calc == false ) return;

	if( control.curr_weapon_slot_type != slot_type ) //무기 변경시
	{
		control.ammo_bullet_img_set->EnableCtrl(false);
		control.curr_bullet_ani_idx = -1;
		control.curr_weapon_slot_type = slot_type;
	}
	else 
	{
		if( control.curr_bullet_count == max_load_bullet ) //가득찬 상태
		{
			control.ammo_bullet_img_set->EnableCtrl(false);
			control.curr_bullet_ani_idx = -1;
		}
		else //애니메이션 활성화
		{
			CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
			if(pPlayer && pPlayer->getActionContext()->isUpper(CHARA_UPPER_FIRE))
				control.ammo_bullet_img_set->EnableCtrl(true);

			control.curr_bullet_ani_idx = 0;
			inner::delta_sec = 0.f;
		}
	}

	inner::act_bullet_ani(control);

	control.need_bullet_ani_calc = false;
}

void hs_1_0::update_bullet_animation(hs_1_0::control& control, REAL32 rDeltaSeconds)
{
	enum { MAX_IMG_ANI = 5, };

	if( control.curr_bullet_ani_idx == -1 ) return;

	if( inner::delta_sec > inner::perido_time )
	{
		inner::delta_sec = 0.f;

		if( control.curr_bullet_ani_idx < MAX_IMG_ANI -1)
			++control.curr_bullet_ani_idx;
		else
			control.curr_bullet_ani_idx = -1;

		inner::act_bullet_ani(control);
	}
	else
	{
		inner::delta_sec += rDeltaSeconds;
	}
}


void inner::act_bullet_ani(hs_1_0::control& control)
{
	if( control.curr_bullet_ani_idx == -1 ) { control.ammo_bullet_img_set->EnableCtrl(false); return; } // -1이면 더 이상 할게 없다.

	control.ammo_bullet_img_set->SetShapeIdx( control.curr_bullet_ani_idx );

	if( control.curr_bullet_ani_idx == 0 )
	{
		//control.ammo_bullet_img에 대해서 이동
		VEC3D* pos = control.ammo_bullet_img->getPos();
		VEC2D* size = control.ammo_bullet_img->getSize();
		REAL32 x = pos->x + size->x;
		control.ammo_bullet_img_set->setPos(x ,pos->y, pos->z);
	}
	else
	{
		 //control.ammo_bullet_img_set 에 대해서 10.f 이동
		VEC3D* pos = control.ammo_bullet_img_set->getPos();
		REAL32 x = pos->x + 10.f;
		control.ammo_bullet_img_set->setPos(x ,pos->y, pos->z);
	}	
}


void hs_1_0 :: update_score_hud(hs_1_0::control& control, REAL32 rDeltaSeconds)
{
	COLOR* pColor = control.UI_AddScore->getTextColorPtr();
	if(CChainKills::instance())
	{
		INT32 AddScore = CChainKills::instance()->TakeBonusScore();
		if(AddScore > 0)
		{
			char TScore[6] = {0,}; 
			char TAddScore[6] = {0,};
			strcpy(TScore,"+");

			control.AddScore = AddScore;
			itoa(control.AddScore,TAddScore,10);
			strcat(TScore, TAddScore);
			control.UI_AddScore->SetText(TScore);

			control.ResultScore += AddScore;
			itoa(control.ResultScore,TScore,10);
			control.UI_ResultScore->SetText(TScore);

			pColor->a = 255;
			control.UI_AddScore->SetTextColor(pColor);
		}
	}
	
	if(	pColor->a > 0 )
	{
		control.ScoreColorTime += rDeltaSeconds;
		if( control.ScoreColorTime >= 2.f )
		{
			if(rDeltaSeconds * 255 - pColor->a < 0) 
			{
				pColor->a -= (UINT8)(rDeltaSeconds * 255);
			}
			else
			{
				pColor->a = 0;
				control.ScoreColorTime = 0.f;
			}
			control.UI_AddScore->SetTextColor(pColor);
		}
	}

	
	//INT32 TempKill = 0;
	//hu::get_kill_death(TempKill, control.CurDeath);
	//
	//bool isRound = false;
	//control.CurRound = hu::get_round_goal_count(isRound);

	//if(control.CurKill != TempKill)
	//{
	//	//control.AddScore = 
	//}


	//if( control.CurRound > -1 && isRound )	//라운드 모드
	//{
	//	if(control.CurRound != control.PrevRound)
	//	{
	//		control.PrevKill = control.CurKill;
	//		control.PrevDeath = control.CurDeath;
	//	}
	//}

	//if(control.CurDeath != control.PrevDeath)
	//{
	//	control.PrevKill = control.CurKill;
	//	control.PrevDeath = control.CurDeath;
	//}
}