#pragma once

#include "iHudBase.h"

class UIHudManager;

//이 클래스는 아무런 행동을 하지 않는다..
class UIHudNone : public iHudBase
{
public:
	UIHudNone(UIHudManager* p) : iHudBase(p) {}
	virtual ~UIHudNone() {}

	virtual bool IsEnable() const override { return false; }
	virtual bool IsVisible() const override { return false; }

private:
	virtual void InitializeAtLoad( i3UIScene * pScene) override {}
};