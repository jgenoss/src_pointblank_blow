#pragma once

#include "iHudBase.h"
#include "ui/ProgressBar.h"

class UIHudManager;

#define HUD_MADNESSARAM_SPRITE_COUNT 3
#define HUD_MADNESSARAM_SPRITE_TIME	 3.0F

class UIHudMadnessNotice : public iHudBase
{
public:
	UIHudMadnessNotice(UIHudManager* p);
	virtual ~UIHudMadnessNotice();

	bool is_ready() const { return m_curr_state == HUD::MN_FINISH; }

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;
	
private:
	virtual void SetParameter(INT32 arg1, INT32 arg2) override;
	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;

private:
	i3UIButtonImageSet* m_pAlarmImage;
	i3UIImageBox*		m_pTextBG;
	i3UIStaticText*		m_pText;
	
	float	m_curr_time;
	float   m_pre_time;
	int		m_curr_state;
	int		m_curr_spriteIdx;
};
