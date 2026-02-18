#pragma once

#include "UI/UIBattlePopupBase.h"
#include "UI/HUD/UIHUDUsurpation.h"

class UIHUDEscape : public UIHudUsurpation
{
	I3_CLASS_DEFINE(UIHUDEscape, UIHudUsurpation);
public:
	UIHUDEscape();
	virtual ~UIHUDEscape(void);

public:
	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

public:
	virtual void SetInfo() override;
};