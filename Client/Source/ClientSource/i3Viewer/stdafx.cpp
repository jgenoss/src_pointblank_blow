// stdafx.cpp : source file that includes just the standard includes
// i3Viewer.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "../Source/ClientDef.h"

#include "DllGuard.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#define BUFFER_SIZE 1024


#ifdef USE_I3EXEC
static bool s_bLogStarted = false;
#else
extern bool s_bLogStarted;
#endif

static std::string g_LogName;
static std::string g_Fulllogpath;

LPCSTR __getLogName()
{

#if defined(NC_BUILD)
	// FullPath ±¸ÇÔ.
	g_Fulllogpath.clear();
	CHAR Init_Path[MAX_PATH] = {};
	GetModuleFileName(NULL, Init_Path ,MAX_PATH);
	g_Fulllogpath += Init_Path;
	g_Fulllogpath.erase(g_Fulllogpath.rfind("\\"));

	i3::sprintf(g_Fulllogpath, "%s\\%s", g_Fulllogpath.c_str(), BC_LOG );
	return g_Fulllogpath.c_str();
#else

	return g_LogName.c_str();
#endif

}

void __startlog(LPCTSTR processName)
{
#if !defined(NC_BUILD) && !defined(USE_I3EXEC)

	SYSTEMTIME		SystemTime;
	//Cal Local Time
	::GetLocalTime( &SystemTime );	

	INT32 Day		=	SystemTime.wDay; 	
	INT32 Hour		=	SystemTime.wHour; 
	INT32 Minute	=	SystemTime.wMinute; 
	INT32 Second	=	SystemTime.wSecond;

	i3::sprintf(g_LogName, "%s(%dD.%dh.%dm.%ds).log", BC_LOG, Day, Hour, Minute, Second);

#endif

	// Error Log initialize
	i3Error::Init();
	i3Error::SetExceptionHandlerOption( I3MINDUMP_WITHDATASEGS );
	i3Error::SetChannelName( I3LOG_NOTICE, (const PSTR)__getLogName() );
	i3Error::SetChannelName( I3LOG_FATAL, (const PSTR)__getLogName());
	i3Error::SetChannelName( I3LOG_WARN, (const PSTR)__getLogName());
	i3Error::SetChannelName( I3LOG_VIEWER, (const PSTR)__getLogName());
	i3Error::SetChannelName( I3LOG_DEFALUT, (const PSTR)__getLogName());

	i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG );
	i3Error::SetChannel( I3LOG_VIEWER, I3ERR_CHN_FILE);
	i3Error::SetChannel( I3LOG_DEFALUT, I3ERR_CHN_FILE);

#if defined( I3_DEBUG) || defined( I3_RELEASE_PROFILE) //???????????????????
	i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_DEBUG | I3ERR_CHN_FILE | I3ERR_CHN_CALLBACK);
	i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_DEBUG | I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);
#else
	i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_DEBUG | I3ERR_CHN_FILE);
	i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);
#endif

	i3Error::StartEncLogFile(g_LogName.c_str());
}