#include "pch.h"
#include "BattleSyncTimer.h"
#include "GameMaster.h"
#include <limits>

#ifdef min
#undef min
#endif

BattleSyncTimer::BattleSyncTimer() : m_timeElapsed(0.0f), m_startTime(0), m_isRunning(false)
{
}

BattleSyncTimer::~BattleSyncTimer() 
{
	Stop();
}

void BattleSyncTimer::Start()
{
	if (m_isRunning) return;

	m_startTime = timeGetTime();
	m_isRunning = true;
}

void BattleSyncTimer::Stop()
{
	if (!m_isRunning) return;

	m_startTime = std::numeric_limits<INT64>::min();
	m_timeElapsed = 0.0f;
	m_isRunning = false;
}

void BattleSyncTimer::Pause()
{
	m_PausedTime = timeGetTime();
}

void BattleSyncTimer::Resume()
{
	m_startTime += timeGetTime() - m_PausedTime;
	Update();
}

void BattleSyncTimer::Update()
{
	if (!m_isRunning) return;

	if ( CGameMaster::i()->IsPaused())
	{
		m_timeElapsed = (m_PausedTime - m_startTime) * 0.001f;
	}
	else
	{
		m_timeElapsed = (timeGetTime() - m_startTime) * 0.001f;
	}
}

REAL32 BattleSyncTimer::GetTimeElapsed() const
{
	return m_timeElapsed;
}