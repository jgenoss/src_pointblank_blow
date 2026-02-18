#pragma once

#include "UIHudState.h"
#include "ui/NumberCount.h"
#include "ui/ProgressBar.h"

class UIHudManager;

class UIHudPlayerDinoState : public UIHudState
{
public:
	UIHudPlayerDinoState(UIHudManager* p);
	virtual ~UIHudPlayerDinoState();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;

private:
	NumberCount	m_HP;

	enum { KILL, DEATH, ASSIST, TEXT_MAX };
	i3UIStaticText* m_pText[TEXT_MAX];

	IntColorProgressBar m_ProgressBar;
};