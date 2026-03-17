#include "pch.h"
#include "ModuleLog.h"
#include <cstdarg>
#include <cstdio>

// ============================================================================
// ModuleLog - Logging to log server (stub implementation)
// Currently logs to stdout; will connect to log server when available
// ============================================================================

ModuleLog::ModuleLog()
	: m_bConnected(false)
	, m_ui32IP(0)
	, m_ui16Port(0)
{
}

ModuleLog::~ModuleLog()
{
	OnDestroy();
}

bool ModuleLog::OnCreate()
{
	// TODO: Read log server IP/port from config and connect
	return true;
}

void ModuleLog::OnDestroy()
{
	m_bConnected = false;
}

void ModuleLog::SendLogMessage(LogMessage eLogId)
{
	const char* szMessages[] = {
		"",
		"[LOG] Server started",
		"[LOG] Server ended",
		"[LOG] Server state update",
		"[LOG] Hack detected",
		"[LOG] Room created",
		"[LOG] Room destroyed",
		"[LOG] Battle started",
		"[LOG] Battle ended",
	};

	if (eLogId < sizeof(szMessages) / sizeof(szMessages[0]))
		printf("%s\n", szMessages[eLogId]);
}

void ModuleLog::SendLogMessageFmt(LogMessage eLogId, const char* szFormat, ...)
{
	char szBuffer[4096];
	va_list args;
	va_start(args, szFormat);
	vsnprintf(szBuffer, sizeof(szBuffer), szFormat, args);
	va_end(args);

	printf("[LOG:%d] %s\n", (int)eLogId, szBuffer);

	// TODO: Queue message for sending to log server
}

void ModuleLog::Update()
{
	// TODO: Process outgoing log message queue
	// Reconnect to log server if disconnected
}
