#pragma once

#include "iHudBase.h"

//허드에서 해당 아이콘 끄기 위한 클래스

class UIHudManager;

class UIHudWinterMode : public iHudBase
{

private:
	i3UIButtonImageSet*		Icon;
	i3UIButtonImageSet*		Icon1;
	i3UIButtonImageSet*		Icon2;

public:
	UIHudWinterMode(UIHudManager* p);
	virtual ~UIHudWinterMode();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;

};