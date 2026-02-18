#pragma once

#include "UIHudState.h"
#include "ui/NumberCount.h"
#include "ui/ProgressBar.h"

class UIHudManager;

class UIHudPlayerState : public UIHudState
{
private:
	enum	{	CURR, MAXIMUM, MAX1,	};
	NumberCount m_HP[MAX1];
	
	enum { KILL, DEATH, ASSIST, MAX2, };
	i3UIStaticText* m_pText[MAX2];
	
	IntColorProgressBar m_ProgressBar;

public:
	UIHudPlayerState(UIHudManager* p);
	virtual ~UIHudPlayerState();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;
};