#include "pch.h"
#include "UIHudAmmoState.h"

#include "UIHudUtil.h"
#include "UIMath.h"
#include "Camera/GameCamera_OnObject.h"
#include "GameMissionManager.h"

#include "BattleHud.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

UIHudAmmoState::UIHudAmmoState(UIHudManager* p) : UIHudState(p)
{
	m_is_dino_state_hud = 0;
	m_changed_bullet_type = m_changed_helicopter_type = -1;

	pf_update[BULLET] = &UIHudAmmoState::update_bullet;
	pf_update[HELICOPTER] = &UIHudAmmoState::update_Obj_gauge;
	pf_update[GAUGEBULLET] = &UIHudAmmoState::update_gauge;	
}

UIHudAmmoState::~UIHudAmmoState()
{
}

/*********************************/
void UIHudAmmoState::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "AmmoState");

	m_Bullet[CURR].AddNumberControl(pScene, "am_Img_Cnt0");
	m_Bullet[CURR].AddNumberControl(pScene, "am_Img_Cnt1");
	m_Bullet[CURR].AddNumberControl(pScene, "am_Img_Cnt2");

	m_Slash_Img_Set = FindControl<i3UIButtonImageSet*>(pScene, "am_Img_Cnt6");
	m_Slash_Img_Set->SetShapeIdx(10);

	m_Bullet[MAXIMUM].AddNumberControl(pScene, "am_Img_Cnt3");
	m_Bullet[MAXIMUM].AddNumberControl(pScene, "am_Img_Cnt4");
	m_Bullet[MAXIMUM].AddNumberControl(pScene, "am_Img_Cnt5");

	m_pText = FindControl<i3UIStaticText*>(pScene, "am_ST_Wp_Name");
	m_WeaponImgSet = FindControl<i3UIButtonImageSet*>(pScene, "am_Img_Wp_Type");

	m_ProgressBar.SetControl(pScene, "am_Progress");
	m_ProgressBar.AddMaxColorSpot(255,255,255);
	m_ProgressBar.AddColorSpot(90, 255,255,255); //없어도 되는 코드
	m_ProgressBar.AddColorSpot(80, 255,225,225); //없어도 되는 코드
	m_ProgressBar.AddColorSpot(70, 255,200,200);
	m_ProgressBar.AddColorSpot(60, 255,175,175);
	m_ProgressBar.AddColorSpot(50, 255,150,150);
	m_ProgressBar.AddColorSpot(40, 255,125,125);
	m_ProgressBar.AddColorSpot(30, 255,100,100);
	m_ProgressBar.AddColorSpot(20, 255,75,75);
	m_ProgressBar.AddColorSpot(10, 255,50,50);
	m_ProgressBar.AddColorSpot(0, 255, 0, 0);

	m_EquipImgSet[HELMET] = FindControl<i3UIButtonImageSet*>(pScene, "am_ImgSet_Helmet");
	m_EquipImgSet[C5] = FindControl<i3UIButtonImageSet*>(pScene, "am_ImgSet_Bomb");
	m_EquipImgSet[MISSION] = FindControl<i3UIButtonImageSet*>(pScene, "am_ImgSet_Mission");
	hc::btn_set_shape(m_EquipImgSet[MISSION], HUD::IS_HAVE_MISSION_OBJ);
}

void UIHudAmmoState::chang_ammo_show_type(INT32 show_type)
{
	i3_prof_func();
	switch( show_type)
	{
	case 0:
		{
			m_Bullet[CURR].SetVisible(true); 
			m_Bullet[CURR].ResetDisplay();

			m_Bullet[MAXIMUM].SetVisible(true); 
			m_Bullet[MAXIMUM].ResetDisplay();
			m_Slash_Img_Set->EnableCtrl(true);

			m_EquipImgSet[HELMET]->EnableCtrl(true);
			m_EquipImgSet[C5]->EnableCtrl(true);

			m_ProgressBar.SetEnable( false );
		}
		break;
	case 1:
		{
			m_Bullet[CURR].SetVisible(false); 
			m_Bullet[CURR].ResetDisplay();
			m_Bullet[MAXIMUM].SetVisible(false); 
			m_Bullet[MAXIMUM].ResetDisplay();
			m_Slash_Img_Set->EnableCtrl(false);

			m_EquipImgSet[HELMET]->EnableCtrl( false );
			m_EquipImgSet[C5]->EnableCtrl( false );

			m_ProgressBar.SetEnable( true );
		}
		break;
	case 2:
		{
			m_Bullet[CURR].SetVisible(false); 
			m_Bullet[CURR].ResetDisplay();
			m_Bullet[MAXIMUM].SetVisible(false); 
			m_Bullet[MAXIMUM].ResetDisplay();
			m_Slash_Img_Set->EnableCtrl(false);

			m_EquipImgSet[HELMET]->EnableCtrl(true);
			m_EquipImgSet[C5]->EnableCtrl(true);

			m_ProgressBar.SetEnable( true );
		}
		break;
	}

	m_changed_helicopter_type = show_type;
}

void UIHudAmmoState::update_bullet( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	INT32 bullet =0, max_bullet = 0;
	hu::get_bullet_cnt(s_pMyChara, bullet, max_bullet);

	m_Bullet[CURR].SetNumber(bullet);
	m_Bullet[MAXIMUM].SetNumber(max_bullet);

	m_pText->SetText( hu::get_weapon_name(s_pMyChara) );

	HUD::ItemState headgear_st = hu::get_helmet_protect_state(s_pMyChara);
	hc::btn_set_shape(m_EquipImgSet[HELMET], headgear_st);

	HUD::ItemState c5_st = hu::get_c5_state(s_pMyChara);
	hc::btn_set_shape(m_EquipImgSet[C5], c5_st);
}

void UIHudAmmoState::update_gauge( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	REAL32 bullet =0, max_bullet = 0;
	hu::get_bullet_cntF(s_pMyChara, bullet, max_bullet);

	m_ProgressBar.SetNumber(bullet, 0.f, max_bullet);
	m_pText->SetText( hu::get_weapon_name(s_pMyChara) );

	HUD::ItemState headgear_st = hu::get_helmet_protect_state(s_pMyChara);
	hc::btn_set_shape(m_EquipImgSet[HELMET], headgear_st);

	HUD::ItemState c5_st = hu::get_c5_state(s_pMyChara);
	hc::btn_set_shape(m_EquipImgSet[C5], c5_st);
}

void UIHudAmmoState::update_Obj_gauge( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	REAL32 bullet =0, max_bullet = 0;

	hu::get_m179_gauge(bullet, max_bullet);
	m_ProgressBar.SetNumber(bullet, 0.f, max_bullet);

	m_pText->SetText( hu::get_m179_name() );
}

void	UIHudAmmoState::Update( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	INT32 helicopter_object_type = 0;
	if( i3::same_of<CGameCamera_OnObject*>(g_pCamera))
	{
		helicopter_object_type = 1;
	}
	else if( hu::get_weapon_bullet_type(s_pMyChara) == true)
	{
		helicopter_object_type = 2;
	}

	if( helicopter_object_type != m_changed_helicopter_type )
		chang_ammo_show_type( helicopter_object_type );

	bool Mission_Sate = false;
	Mission_Sate = CGameMissionManager::i()->isMyUsurpatio();
	m_EquipImgSet[MISSION]->EnableCtrl(Mission_Sate);


	(this->*pf_update[helicopter_object_type])(rDeltaSeconds);
}

/*************************************/
void UIHudAmmoState::event_respawn_battle_impl(INT32 arg)
{
	i3_prof_func();

	chang_ammo_show_type(0);
}