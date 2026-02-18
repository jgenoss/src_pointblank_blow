#pragma once

#include "iHudBase.h"
#include "ui/NumberCount.h"

struct CCModeVSControls
{
	i3UIImageBox*		roundNameImg;
	i3UIStaticText*		roundText;
	i3UIButtonImageSet* leftTeamImg;
	i3UIButtonImageSet* leftTeamNameImg;
	i3UIButtonImageSet* rightTeamImg;
	i3UIButtonImageSet* rightTeamNameImg;
};

class UIHudManager;

class UIHudCrossCounterVS : public iHudBase
{
private:
	CCModeVSControls m_controls;

public:
	UIHudCrossCounterVS(UIHudManager* p);
	virtual ~UIHudCrossCounterVS();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;

private:
	void event_start_battle(INT32 arg, const i3::user_data& UserData);

	void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData);
	void event_respawn_battle(INT32 arg, const i3::user_data& UserData);
};