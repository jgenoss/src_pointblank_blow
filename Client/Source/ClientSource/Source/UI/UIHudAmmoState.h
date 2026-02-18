#pragma once

#include "UIHudState.h"
#include "ui/NumberCount.h"
#include "ui/ProgressBar.h"

class UIHudManager;

class UIHudAmmoState : public UIHudState
{
public:
	UIHudAmmoState(UIHudManager* p);
	virtual ~UIHudAmmoState();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;

private:
	void event_respawn_battle_impl(INT32 arg);

private:
	void chang_ammo_show_type(INT32 show_type);

	void update_bullet(REAL32 rDeltaSeconds);
	void update_gauge(REAL32 rDeltaSeconds);
	void update_Obj_gauge(REAL32 rDeltaSeconds);

	void (UIHudAmmoState::*pf_update[UPDATE_MAX])(REAL32);

private:
	enum	{	CURR, MAXIMUM, MAX,	};
	NumberCount m_Bullet[MAX];

	i3UIButtonImageSet* m_WeaponImgSet;
	i3UIButtonImageSet* m_Slash_Img_Set;

	enum { HELMET, C5, MISSION, EquipMax, };
	i3UIButtonImageSet* m_EquipImgSet[EquipMax];

	i3UIStaticText* m_pText;

	RealColorProgressBar m_ProgressBar;

	INT32 m_changed_bullet_type, m_changed_helicopter_type;
};