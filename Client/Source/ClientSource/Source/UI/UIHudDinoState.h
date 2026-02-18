#pragma once

#include "UIHudState.h"
#include "ui/ProgressBar.h"

class UIHudManager;

class UIHudDinoState : public UIHudState
{
public:
	UIHudDinoState(UIHudManager* p);
	virtual ~UIHudDinoState();

public:
	virtual void InitializeAtLoad(i3UIScene* scene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;

private:
	virtual void event_respawn_battle_impl(INT32 arg) override;

private:
	i3UIButtonImageSet* m_attack1Img;
	i3UIButtonImageSet* m_attack2Img;
	i3UIProgressBar*	m_skillGuage;
	i3UIStaticText*		m_dinoName;
};