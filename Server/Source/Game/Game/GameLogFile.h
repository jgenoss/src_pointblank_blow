#ifndef __GAME_LOG_FILE_H__
#define __GAME_LOG_FILE_H__

class CUserSession;

class CGameLogFile : public i3LogFileW
{
	I3_CLASS_DEFINE( CGameLogFile );

public:
	CGameLogFile();
	~CGameLogFile();

	void					WriteLogSession( CUserSession* pSession, wchar_t * strLogString, ... );
};

#endif