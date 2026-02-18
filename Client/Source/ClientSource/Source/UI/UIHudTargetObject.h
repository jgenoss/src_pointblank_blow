#pragma once

#include "iHudBase.h"

class UIHudManager;

class UIHudTargetObject : public iHudBase
{
	enum
	{
		GENERATOR = 0, HELICOPTER,
		GIRAN1, GIRAN2,
		TANK1, TANK2,
		MAX,
	};
private:
	i3UIButtonImageSet* m_TargetObject[MAX];

public:
	UIHudTargetObject(UIHudManager* p);
	virtual ~UIHudTargetObject();

private:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;

private:
	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;
};