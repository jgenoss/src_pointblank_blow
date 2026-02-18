#ifndef __I3_LOG_FILE_A_H__
#define __I3_LOG_FILE_A_H__

#include "i3LogFileBase.h"

class I3_EXPORT_BASE i3LogFileA : public i3LogFileBase
{
	I3_CLASS_DEFINE( i3LogFileA );

private:

	char					m_strFileName[ LOG_STRING_MAX ]; 
	char					m_astrLogInit[ LOG_BUFFER_MAX_COUNTER ][ LOG_INIT_STRING ]; 
	char					m_strStartText[ LOG_STRING_MAX ];

protected:
	BOOL					_CreateFile(void);
	void					_WriteLogFile( char * strLog );

	virtual BOOL			_Working();

	void					_WriteLog( char* strLog );

public: 	
	
	i3LogFileA();
	virtual ~i3LogFileA(void);

	BOOL					Create( TCHAR* wstrPathName, char* wstrFileName, INT32 iSaveType = FILE_SAVE_INTERVAL_HOUR, INT32 iLogBufferCount = 1000, INT32 iThreadPri = THREAD_PRIORITY_NORMAL);
	void					Destroy(void);
	INT32					AddLogThread( TCHAR* strPreString, INT32 iType, DWORD dwThreadID );
	void					WriteLog( char * strLogString, ... );
	void					WriteLog( INT32 i32WarningType, INT32 i32WarningLevel, char * strLogString, ... );
};

#endif
