#pragma once

#include "UIHudState.h"
#include "HudState_1_0.h"

class UIHudManager;

class UIHudPlayerAmmoState : public UIHudState
{
public:
	UIHudPlayerAmmoState(UIHudManager* p);
	virtual ~UIHudPlayerAmmoState();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;

private:
	virtual void event_respawn_battle_impl(INT32 arg) override;

private:
	void update_player(REAL32 rDeltaSeconds);

	void chang_ammo_show_type(INT32 ammo_type);
	void update_bullet(REAL32 rDeltaSeconds);
	void update_gauge(REAL32 rDeltaSeconds);
	void update_bullet_gauge(REAL32 rDeltaSeconds);
	void (UIHudPlayerAmmoState::*pf_ammo_update[UPDATE_MAX])(REAL32);
	void update_score(REAL32 rDeltaSeconds);

private:
	hs_1_0::control player_control;
	CHARA_ACTION_BODYLOWER	m_CharaLowerState;
	INT32 m_changed_bullet_type, m_changed_helicopter_type;
};