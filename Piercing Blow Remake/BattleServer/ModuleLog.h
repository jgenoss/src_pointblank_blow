#ifndef __MODULELOG_H__
#define __MODULELOG_H__

#pragma once

// ============================================================================
// ModuleLog - Logging to log server
// Port of original CModuleLog from Dedication/Dedi/ModuleLog.h
// Sends structured log messages to centralized log server
// ============================================================================

enum LogMessage
{
	LOG_MESSAGE_NONE = 0,
	LOG_MESSAGE_SERVER_START,
	LOG_MESSAGE_SERVER_END,
	LOG_MESSAGE_SERVER_STATE,
	LOG_MESSAGE_HACK_DETECTED,
	LOG_MESSAGE_ROOM_CREATE,
	LOG_MESSAGE_ROOM_DESTROY,
	LOG_MESSAGE_BATTLE_START,
	LOG_MESSAGE_BATTLE_END,
};

class ModuleLog
{
public:
	ModuleLog();
	~ModuleLog();

	bool OnCreate();
	void OnDestroy();

	// Send a log message
	void SendLogMessage(LogMessage eLogId);

	// Send a formatted log message
	void SendLogMessageFmt(LogMessage eLogId, const char* szFormat, ...);

	// Update (process outgoing log queue)
	void Update();

private:
	bool		m_bConnected;
	uint32_t	m_ui32IP;
	uint16_t	m_ui16Port;
};

#endif // __MODULELOG_H__
