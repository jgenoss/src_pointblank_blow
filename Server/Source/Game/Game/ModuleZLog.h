#ifndef __MODULE_ZLOG_H__
#define __MODULE_ZLOG_H__

#include "RingBuffer.h"

class CModuleZLogSocket;

enum ZLOG_UPDATE_STATE
{
	ZLOG_UPDATE_STATE_NONE,
	ZLOG_UPDATE_STATE_FIND,
	ZLOG_UPDATE_STATE_FILE_OPEN,
	ZLOG_UPDATE_STATE_FILE_UPLOAD_INFO,
	ZLOG_UPDATE_STATE_FILE_UPLOAD_CONTENTS,
	ZLOG_UPDATE_STATE_WAIT_PACKET,
	ZLOG_UPDATE_STATE_FILE_DELETE,
};

class CModuleZLog : public i3Thread
{
	I3_CLASS_DEFINE( CModuleZLog );

	BOOL					m_bRunning;
	HANDLE					m_hFile;
	
	BOOL					m_bFinish;

	CModuleZLogSocket*		m_pZLogSocket;
	char					m_strLogPath[ MAX_STRING_COUNT ];

	UINT32					m_ui32FileSize;

	ZLOG_UPDATE_STATE		m_eUpdateState;
	char					m_strUploadFile[ MAX_STRING_COUNT ];

	BOOL					_UpdateFileUpload();
	INT32					_GetFileDate( char* strHour );

	BOOL					_FindUploadFile();
	BOOL					_OpenFile();
	BOOL					_UploadFileInfo();
	BOOL					_UploadFile();
	BOOL					_DeleteFile();

	inline void				_CloseHandle()
	{
		if (0 == ::CloseHandle(m_hFile))
		{
			g_pLog->WriteLog(L"Handle Closing Fail. Error Code : %x, State : %d", GetLastError(), m_eUpdateState);
		}
	}

	DWORD					_GetFileSize();

public:
	CModuleZLog();
	~CModuleZLog();

	BOOL					Create();
	void					Destroy();
	UINT32					OnRunning( void * pUserData);
};

extern CModuleZLog * g_pModuleZlog;

#endif //__MODULE_ZLOG_H__