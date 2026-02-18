#ifndef _BATTLE_SYNC_TIMER_H_
#define _BATTLE_SYNC_TIMER_H_

class BattleSyncTimer : public i3::shared_ginst<BattleSyncTimer>
{
public:
	BattleSyncTimer();
	~BattleSyncTimer();

	void Start();
	void Pause();
	void Resume();
	void Stop();
	void Update();
	REAL32 GetTimeElapsed() const;

private:
	INT64	m_startTime;
	REAL32	m_timeElapsed;
	INT64	m_PausedTime;
	bool	m_isRunning;
};

#endif