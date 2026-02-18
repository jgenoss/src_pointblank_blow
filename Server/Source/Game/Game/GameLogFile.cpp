#include "pch.h"
#include "GameLogFile.h"
#include "UserSession.h"

I3_CLASS_INSTANCE( CGameLogFile, i3LogFileW );

CGameLogFile::CGameLogFile()
{
}

CGameLogFile::~CGameLogFile()
{
}

void CGameLogFile::WriteLogSession( CUserSession* pSession, wchar_t * wstrLogString, ... )
{
	wchar_t wstrTemp[ LOG_STRING_MAX ];

	va_list marker;
	va_start( marker, wstrLogString );
	_vsnwprintf( wstrTemp, LOG_STRING_MAX, wstrLogString, marker );
	va_end( marker);

	wchar_t wstrLog[ LOG_STRING_MAX ];
	if( NULL != pSession )
	{
		i3String::Format( wstrLog, LOG_STRING_MAX, L"[%s:%d:%I64d:%d]%s", pSession->GetIPStringW(), pSession->m_ConPort, pSession->m_i64UID, pSession->m_ui32KillReason, wstrTemp );
	}
	else
	{
		i3String::Copy( wstrLog, wstrTemp, LOG_STRING_MAX );
	}

	_WriteLog( wstrLog );
}