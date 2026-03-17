#ifndef __SERVERLOG_H__
#define __SERVERLOG_H__

#pragma once
#include <cstdio>
#include <ctime>
#include <cstdarg>

// Simple file-based logging system for persistent server logs.
// Writes to daily rotating log files in the "logs/" directory.
// Thread-safe via CRITICAL_SECTION.
class ServerLog
{
public:
	enum LogLevel
	{
		LOG_DEBUG	= 0,
		LOG_INFO	= 1,
		LOG_WARNING	= 2,
		LOG_ERROR	= 3,
		LOG_FATAL	= 4,
	};

	ServerLog();
	~ServerLog();

	bool	Initialize(const char* pszServerName, const char* pszLogDir = "logs");
	void	Shutdown();

	void	Write(LogLevel level, const char* pszFormat, ...);
	void	WriteRaw(const char* pszFormat, ...);

	// Convenience
	void	Debug(const char* pszFormat, ...);
	void	Info(const char* pszFormat, ...);
	void	Warning(const char* pszFormat, ...);
	void	Error(const char* pszFormat, ...);

	void	SetMinLevel(LogLevel level)		{ m_MinLevel = level; }
	void	SetConsoleOutput(bool b)		{ m_bConsole = b; }

	static ServerLog* GetInstance();

private:
	void	WriteV(LogLevel level, const char* pszFormat, va_list args);
	void	EnsureLogFile();
	void	CloseLogFile();

	static const char* GetLevelStr(LogLevel level);

private:
	FILE*				m_pFile;
	char				m_szServerName[64];
	char				m_szLogDir[256];
	int					m_i32CurrentDay;		// Day of year for rotation
	LogLevel			m_MinLevel;
	bool				m_bConsole;
	bool				m_bInitialized;
	CRITICAL_SECTION	m_cs;

	static ServerLog	s_Instance;
};

// Global convenience macros
#define SLOG_DEBUG(fmt, ...)	ServerLog::GetInstance()->Debug(fmt, ##__VA_ARGS__)
#define SLOG_INFO(fmt, ...)		ServerLog::GetInstance()->Info(fmt, ##__VA_ARGS__)
#define SLOG_WARN(fmt, ...)		ServerLog::GetInstance()->Warning(fmt, ##__VA_ARGS__)
#define SLOG_ERROR(fmt, ...)	ServerLog::GetInstance()->Error(fmt, ##__VA_ARGS__)

#endif // __SERVERLOG_H__
