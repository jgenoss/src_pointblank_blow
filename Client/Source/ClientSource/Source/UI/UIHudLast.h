#pragma once

#include "iHudBase.h"

class UIHudManager;

//이 클래스는 아무런 행동을 하지 않는다..
//다만 모든 hud 맨 마지막에 호출 되는 클래스 이다.
class UIHudLast : public iHudBase
{
public:
	UIHudLast(UIHudManager* p);
	virtual ~UIHudLast();

private:
	virtual void InitializeAtLoad( i3UIScene * pScene) override {}
	virtual void ClearAtUnload( i3UIScene * pScene) override;

private:
	virtual void SetParameter(INT32 arg1, INT32 arg2) override;

private:
	virtual void event_m_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_first_respawn_battle(INT32 arg, const i3::user_data& UserData) override;
};