#pragma once

#include "iHudBase.h"
#include "ui/ProgressBar.h"

class UIHudManager;

class UIHudRespawnTimer : public iHudBase
{
public:
	UIHudRespawnTimer(UIHudManager* p);
	virtual ~UIHudRespawnTimer();

	bool is_working() const { return m_working_respawn; }

private:
	void init_text();
	void check_send_respawn();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;
	
private:
	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void SetParameter(INT32 arg1, INT32 arg2) override;
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;

private:
	i3UIStaticText* m_pText, *m_pRespawnTimeText;
	i3UIProgressBar* m_pProgressBar;

	REAL32 m_curr_time;
	REAL32 m_targer_time;
	bool m_enable_time;

	bool m_working_respawn;
	bool m_complete_respawn;
	bool m_block_send;
};
