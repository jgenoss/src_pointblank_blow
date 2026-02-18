#pragma once

#include "iHudBase.h"
#include "ui/NumberCount.h"
#include "UIHudDefine.h"

class UIHudManager;

class UIHudChallengeBoard : public iHudBase
{
private:
	enum	{	PT, DIF, MAX,	};
	NumberCount m_Number[MAX];

	i3UIStaticText* m_pLevelText;
	i3UIButtonImageSet* m_pLevel_ImgSet;

public:
	UIHudChallengeBoard(UIHudManager* p);
	virtual ~UIHudChallengeBoard();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;

private:
	void update_level_1_5(REAL32 rDeltaSeconds);
	void update_level_1_0(REAL32 rDeltaSeconds);
	void (UIHudChallengeBoard::*pf_update_level[HUD::HS_MAX])(REAL32);

private:
	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_first_respawn_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;
};