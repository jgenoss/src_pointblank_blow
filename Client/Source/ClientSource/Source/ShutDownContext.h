#pragma once

enum ALARM_STATE {
	EMPTY_ALARM = 0,
	PRINT_PLAYTIME_ALARM,
	PRINT_SHUTDOWNTIME_ALARM,
	WAIT_PLAYTIME_ALARM,
	WAIT_SHUTDOWN_ALARM,
	PRINT_NOTICE,
};

class ShutDownContext : public i3::shared_ginst<ShutDownContext>
{
public:
	ShutDownContext();
	~ShutDownContext();
	
	void			SetPlayTime(const UINT16 playtime) { m_PlayTime = playtime; }
	UINT16			GetPlayTime() const	{ return m_PlayTime; }
	void			SetAlarmState(const UINT16 state) { m_AlarmState = state; }
	UINT16			GetAlarmState() const { return m_AlarmState; }
	void			SetFnishTime(const UINT16 fnishtime) { m_FnishTime = fnishtime; }
	UINT16			GetFnishTime() const { return m_FnishTime; }
	void			SetEnableShutDown(const bool enableShutDown) { m_EnableShutDown = enableShutDown; }
	bool			IsShutDown() const { return m_EnableShutDown; }

private:
	bool		m_EnableShutDown;
	UINT16		m_PlayTime;
	UINT16		m_AlarmState;
	UINT16		m_FnishTime;
};
 