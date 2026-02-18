#pragma once

#include "iHudBase.h"

class UIHudManager;

class UIHudMissionBoard : public iHudBase
{
	struct Mission
	{
		i3UIButtonImageSet*	check;
		i3UIStaticText* text;
//		VEC2D			origSize;
		i3::rc_wstring	strDescForSkip;

		Mission() :check(nullptr), text(nullptr)
		{
		}
		void set(i3UIScene * pScene, const char* c, const char* t);
		void set_check(INT32 shapeIdx);
		void set_text(const i3::rc_wstring& text);
		
		void enable_ctrl(bool bEnable);

		REAL32 getHeight();
	};
	enum { MAX = MAX_CHALLENGE_COUNT, };

private:
	Mission				m_Mission[MAX];
	
	UINT32	m_NotifyMedalCount;
	REAL32	m_NotifyHeight;

public:
	UIHudMissionBoard(UIHudManager* p);
	virtual ~UIHudMissionBoard();

	void		UpdateForMissionCard();
	void		CloseMissionBoard();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;

private:

	void		AdjustTextPos(INT32 missionCount);

	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_respawn_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_death_battle(INT32 arg, const i3::user_data& UserData) override;
};