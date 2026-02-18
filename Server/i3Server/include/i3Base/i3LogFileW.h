#ifndef __I3_LOG_FILE_W_H__
#define __I3_LOG_FILE_W_H__

#include "i3LogFileBase.h"

class I3_EXPORT_BASE i3LogFileW : public i3LogFileBase
{
	I3_CLASS_DEFINE( i3LogFileW );

private:

	wchar_t					m_wstrFileName[ LOG_STRING_MAX ]; 
	wchar_t					m_astrLogInit[ LOG_BUFFER_MAX_COUNTER ][ LOG_INIT_STRING ]; 
	wchar_t					m_wstrStartText[ LOG_STRING_MAX ];

protected:
	BOOL					_CreateFile(void);
	void					_WriteLogFile( wchar_t * strLog );

	virtual BOOL			_Working();

	void					_WriteLog( wchar_t* strLog );

public: 	
	
	i3LogFileW();
	virtual ~i3LogFileW(void);

	BOOL					Create( wchar_t* wstrPathName, wchar_t* wstrFileName, INT32 iSaveType = FILE_SAVE_INTERVAL_HOUR, INT32 iLogBufferCount = 1000, INT32 iThreadPri = THREAD_PRIORITY_NORMAL);
	void					Destroy(void);
	INT32					AddLogThread( wchar_t* strPreString, INT32 iType, DWORD dwThreadID );
	void					WriteLog( wchar_t * strLogString, ... );
	void					WriteLog( INT32 i32WarningType, INT32 i32WarningLevel, wchar_t * strLogString, ... );

};

#endif
