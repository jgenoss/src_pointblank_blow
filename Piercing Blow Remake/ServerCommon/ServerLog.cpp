#include <winsock2.h>
#include <windows.h>
#include "ServerLog.h"
#include <cstring>
#include <direct.h>

ServerLog ServerLog::s_Instance;

ServerLog::ServerLog()
	: m_pFile(nullptr)
	, m_i32CurrentDay(-1)
	, m_MinLevel(LOG_DEBUG)
	, m_bConsole(true)
	, m_bInitialized(false)
{
	m_szServerName[0] = '\0';
	m_szLogDir[0] = '\0';
	memset(&m_cs, 0, sizeof(m_cs));
}

ServerLog::~ServerLog()
{
	Shutdown();
}

ServerLog* ServerLog::GetInstance()
{
	return &s_Instance;
}

bool ServerLog::Initialize(const char* pszServerName, const char* pszLogDir)
{
	if (m_bInitialized)
		return true;

	InitializeCriticalSection(&m_cs);

	strncpy_s(m_szServerName, pszServerName, _TRUNCATE);
	strncpy_s(m_szLogDir, pszLogDir, _TRUNCATE);

	// Create log directory if it doesn't exist
	_mkdir(m_szLogDir);

	m_bInitialized = true;

	Info("ServerLog initialized for [%s]", pszServerName);
	return true;
}

void ServerLog::Shutdown()
{
	if (!m_bInitialized)
		return;

	Info("ServerLog shutting down");

	CloseLogFile();

	DeleteCriticalSection(&m_cs);
	m_bInitialized = false;
}

void ServerLog::Write(LogLevel level, const char* pszFormat, ...)
{
	va_list args;
	va_start(args, pszFormat);
	WriteV(level, pszFormat, args);
	va_end(args);
}

void ServerLog::WriteRaw(const char* pszFormat, ...)
{
	if (!m_bInitialized)
		return;

	EnterCriticalSection(&m_cs);

	EnsureLogFile();

	va_list args;
	va_start(args, pszFormat);

	if (m_pFile)
	{
		vfprintf(m_pFile, pszFormat, args);
		fprintf(m_pFile, "\n");
		fflush(m_pFile);
	}

	if (m_bConsole)
	{
		va_list argsCopy;
		va_copy(argsCopy, args);
		vprintf(pszFormat, argsCopy);
		printf("\n");
		va_end(argsCopy);
	}

	va_end(args);

	LeaveCriticalSection(&m_cs);
}

void ServerLog::Debug(const char* pszFormat, ...)
{
	va_list args;
	va_start(args, pszFormat);
	WriteV(LOG_DEBUG, pszFormat, args);
	va_end(args);
}

void ServerLog::Info(const char* pszFormat, ...)
{
	va_list args;
	va_start(args, pszFormat);
	WriteV(LOG_INFO, pszFormat, args);
	va_end(args);
}

void ServerLog::Warning(const char* pszFormat, ...)
{
	va_list args;
	va_start(args, pszFormat);
	WriteV(LOG_WARNING, pszFormat, args);
	va_end(args);
}

void ServerLog::Error(const char* pszFormat, ...)
{
	va_list args;
	va_start(args, pszFormat);
	WriteV(LOG_ERROR, pszFormat, args);
	va_end(args);
}

void ServerLog::WriteV(LogLevel level, const char* pszFormat, va_list args)
{
	if (!m_bInitialized)
		return;

	if (level < m_MinLevel)
		return;

	EnterCriticalSection(&m_cs);

	EnsureLogFile();

	// Get current time for timestamp
	time_t now = time(nullptr);
	struct tm tmNow;
	localtime_s(&tmNow, &now);

	char szTimestamp[32];
	strftime(szTimestamp, sizeof(szTimestamp), "%Y-%m-%d %H:%M:%S", &tmNow);

	const char* pszLevel = GetLevelStr(level);

	// Write to file
	if (m_pFile)
	{
		fprintf(m_pFile, "[%s] [%s] ", szTimestamp, pszLevel);
		vfprintf(m_pFile, pszFormat, args);
		fprintf(m_pFile, "\n");
		fflush(m_pFile);
	}

	// Write to console
	if (m_bConsole)
	{
		// Set console color based on level
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		WORD wColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // default white

		switch (level)
		{
		case LOG_DEBUG:		wColor = FOREGROUND_GREEN | FOREGROUND_BLUE; break;			// cyan
		case LOG_INFO:		wColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break; // white
		case LOG_WARNING:	wColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break; // yellow
		case LOG_ERROR:		wColor = FOREGROUND_RED | FOREGROUND_INTENSITY; break;		// red
		case LOG_FATAL:		wColor = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break; // magenta
		}

		SetConsoleTextAttribute(hConsole, wColor);

		printf("[%s] [%s] ", szTimestamp, pszLevel);

		va_list argsCopy;
		va_copy(argsCopy, args);
		vprintf(pszFormat, argsCopy);
		va_end(argsCopy);

		printf("\n");

		// Reset to default
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

	LeaveCriticalSection(&m_cs);
}

void ServerLog::EnsureLogFile()
{
	// Check if we need to rotate (new day)
	time_t now = time(nullptr);
	struct tm tmNow;
	localtime_s(&tmNow, &now);

	int dayOfYear = tmNow.tm_yday;

	if (m_pFile && dayOfYear == m_i32CurrentDay)
		return; // Same day, file is good

	// Close old file
	CloseLogFile();

	// Open new file with date in name
	char szFilePath[512];
	_snprintf_s(szFilePath, _TRUNCATE, "%s/%s_%04d%02d%02d.log",
		m_szLogDir,
		m_szServerName,
		tmNow.tm_year + 1900,
		tmNow.tm_mon + 1,
		tmNow.tm_mday);

	m_pFile = fopen(szFilePath, "a"); // Append mode
	m_i32CurrentDay = dayOfYear;
}

void ServerLog::CloseLogFile()
{
	if (m_pFile)
	{
		fflush(m_pFile);
		fclose(m_pFile);
		m_pFile = nullptr;
	}
}

const char* ServerLog::GetLevelStr(LogLevel level)
{
	switch (level)
	{
	case LOG_DEBUG:		return "DEBUG";
	case LOG_INFO:		return "INFO ";
	case LOG_WARNING:	return "WARN ";
	case LOG_ERROR:		return "ERROR";
	case LOG_FATAL:		return "FATAL";
	default:			return "?????";
	}
}
