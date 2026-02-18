#pragma once

#include "iHudBase.h"

class UIHudManager;

//이 클래스는 아무런 행동을 하지 않는다..
//다만 모든 hud 맨 마지막에 호출 되는 클래스 이다.
class UIHudFirst : public iHudBase
{
public:
	UIHudFirst(UIHudManager* p);
	virtual ~UIHudFirst();

private:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;

private:
	void event_start_battle(INT32 arg, const i3::user_data& UserData);
	void event_m_roundstart_battle(INT32 arg, const i3::user_data& UserData);
	void event_respawn_battle(INT32 arg, const i3::user_data& UserData);
};