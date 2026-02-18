#pragma once

#include "iHudBase.h"

class UIHudManager;

class UIHudi3UIFrameWnd2 : public iHudBase
{
public:
	UIHudi3UIFrameWnd2(UIHudManager* p);
	virtual ~UIHudi3UIFrameWnd2();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene);
};