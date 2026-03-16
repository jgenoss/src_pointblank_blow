#ifndef __I3_LOG_FILE_BASE_H__
#define __I3_LOG_FILE_BASE_H__

#include "i3ElementBase.h"
#include "i3Mutex.h"
#include "i3RingBuffer.h"
#include "i3Warning.h"

#define LOG_BUFFER_MAX_COUNTER		100
#define LOG_INIT_STRING				10
#define LOG_STRING_MAX				1024
#define RV_ADD_FAIL					0x80000001
#define LOG_START_DATE				32

enum FILE_SAVE_INTERVAL
{
	FILE_SAVE_INTERVAL_DAY =1,
	FILE_SAVE_INTERVAL_HOUR,
};

class I3_EXPORT_BASE i3LogFileBase : public i3ElementBase
{
	I3_CLASS_DEFINE( i3LogFileBase );

protected:
	//For Thread 
	BOOL					m_bRunning;
	HANDLE					m_hThread;

	HANDLE					m_hFile;

	//For File
	INT32					m_iSaveType; 
	UINT32					m_ui32FileHour; 
	UINT32					m_ui32FileDay; 

	//For File Save Buffer 
	i3RingBuffer	*		m_ppLogBuffer[ LOG_BUFFER_MAX_COUNTER ];
	UINT32					m_ui32WorkCounter; 
	INT32					m_iBufferCount; 

	DWORD					m_adwThreadID[LOG_BUFFER_MAX_COUNTER];

	//For Add Log Buffer 
	i3Mutex					m_AddCs; 

	//For Save Time
	SYSTEMTIME				m_SystemTime;
	UINT32					m_iYear;
	UINT32					m_iMonth;
	UINT32					m_iDay;
	UINT32					m_iHour;
	UINT32					m_iMinute;
	UINT32					m_iSecond;

	// Server Version
	UINT16					m_ui16Ver01;
	UINT16					m_ui16Ver02;
	UINT32					m_ui32Ver03;
	UINT32					m_ui32Ver04;

	// Server Warning
	i3Warning				m_Warning; 

protected:	

	BOOL					_Create( INT32 iSaveType, INT32 iLogBufferCount, INT32 iThreadPri );	
	void					_UpdateTime(void)
	{
		::GetLocalTime( &m_SystemTime );
		m_iYear		= m_SystemTime.wYear; 
		m_iMonth	= m_SystemTime.wMonth; 
		m_iDay		= m_SystemTime.wDay; 	
		m_iHour		= m_SystemTime.wHour; 
		m_iMinute	= m_SystemTime.wMinute; 
		m_iSecond	= m_SystemTime.wSecond;
		return; 
	}

	INT32					_FindWorkIdx();
	INT32					_FindWorkIdx( DWORD dwThreadID );

	virtual BOOL			_CreateFile(void);
	virtual BOOL			_Working();

public: 	
	
	i3LogFileBase();
	virtual ~i3LogFileBase(void);

	void					Destroy(void);
	void					SetVersion( UINT16 ui16Ver01, UINT16 ui16Ver02, UINT32 ui32Ver03, UINT32 ui32Ver04 );
	void					Working(void);
	i3Warning				GetWarning() { return m_Warning; }

};

#endif
