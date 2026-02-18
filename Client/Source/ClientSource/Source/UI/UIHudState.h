#pragma once

#include "iHudBase.h"
#include "UIHudDefine.h"

class UIHudManager;

enum BULLET_UPDATE_TYPE
{	
	BULLET, 
	HELICOPTER, 
	GAUGEBULLET, 
	UPDATE_MAX,	
};

class UIHudState : public iHudBase
{
public:
	UIHudState(UIHudManager* p);
	virtual ~UIHudState();

private:
	virtual void SetParameter(INT32 arg1, INT32 arg2) override;

private:
	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_respawn_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;

protected:
	virtual void event_start_battle_impl(INT32 arg) {}
	virtual void event_respawn_battle_impl(INT32 arg) {}
	virtual void event_m_roundstart_battle_impl(INT32 arg) {}

public:
	void SetDinoState(const char* name, DinoState::Attack att1, DinoState::Attack att2);
	void SetDinoRatio(REAL32 ratio);

protected:
	bool m_is_dino_state_hud;
	HUD::MISSION_MODE m_setting_mission_mode;
};