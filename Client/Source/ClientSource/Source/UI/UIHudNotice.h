#pragma once

#include "iHudBase.h"

#include "FlowTextBox.h"

class UIHudManager;

class UIHudNotice : public iHudBase
{
public:
	UIHudNotice(UIHudManager* p);
	virtual ~UIHudNotice();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;

	void re_position();

private:
	virtual void SetParameter(INT32 arg1 =0, INT32 arg2 =0) override;

private:
	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_first_respawn_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_death_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;

private:
	FlowTextBox m_TextBox;
	bool m_TextShow;
	bool m_need_update;

	UINT16 m_AlarmState;
};