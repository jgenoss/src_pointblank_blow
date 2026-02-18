#include "i3CommonType.h"
#include "i3LogFileBase.h"

#include "i3String.h"

I3_CLASS_INSTANCE( i3LogFileBase, i3ElementBase );

static UINT32 _DefThreadProc( void * pPointer )
{
	((i3LogFileBase*)pPointer)->Working(); 
	return 0;
}

i3LogFileBase::i3LogFileBase()
{
	m_hThread			= NULL;
	m_bRunning			= FALSE;
	m_hFile				= NULL;
	m_ui32WorkCounter	= 0; 

	m_ui16Ver01			= 0;
	m_ui16Ver02			= 0;
	m_ui32Ver03			= 0;
	m_ui32Ver04			= 0;
}

i3LogFileBase::~i3LogFileBase(void)
{
	Destroy(); 
}

BOOL i3LogFileBase::_Create( INT32 iSaveType, INT32 iLogBufferCount, INT32 iThreadPri )
{
	m_iSaveType			= iSaveType;
	m_iBufferCount		= iLogBufferCount;

	//Create Thread 
	UINT32	ui32ThreadID;
	m_hThread = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc ,this ,0 ,(DWORD*)&ui32ThreadID );
	if( NULL == m_hThread )										return FALSE; 
	if( FALSE == SetThreadPriority( m_hThread, iThreadPri) )	return FALSE;

	return TRUE; 
}

void i3LogFileBase::Destroy(void)
{
	m_bRunning = FALSE;
	::WaitForSingleObject( m_hThread, INFINITE );

	for(UINT32 i = 0; i < m_ui32WorkCounter; i++)
	{
		I3_SAFE_RELEASE( m_ppLogBuffer[i] ); 
	}

	::CloseHandle( m_hFile );
}

void i3LogFileBase::SetVersion( UINT16 ui16Ver01, UINT16 ui16Ver02, UINT32 ui32Ver03, UINT32 ui32Ver04 )
{
	m_ui16Ver01	= ui16Ver01;
	m_ui16Ver02	= ui16Ver02;
	m_ui32Ver03	= ui32Ver03;
	m_ui32Ver04	= ui32Ver04;
}

void i3LogFileBase::Working(void)
{
	BOOL bWorking; 

	m_bRunning = TRUE;

	while( m_bRunning )
	{
		bWorking = FALSE; 

		//Create File 
		_UpdateTime();
		switch( m_iSaveType )
		{
		case FILE_SAVE_INTERVAL_DAY		: 
			if(m_ui32FileDay != m_iDay)		_CreateFile(); 
			break; 
		case FILE_SAVE_INTERVAL_HOUR	:
		default							: 
			if(m_ui32FileHour != m_iHour)	_CreateFile(); 
			break; 
		}
		
		bWorking = _Working();

		if( FALSE == bWorking )	Sleep(1); 	 
	}

	return; 
}

INT32 i3LogFileBase::_FindWorkIdx()
{
	DWORD	dwThreadId = GetCurrentThreadId();

	for( UINT32 i = 0 ; i < m_ui32WorkCounter ; ++i )
	{
		if( dwThreadId == m_adwThreadID[i] )
		{
			return i;
		}
	}

	return RV_ADD_FAIL;
}

INT32 i3LogFileBase::_FindWorkIdx( DWORD dwThreadID )
{	

	for( UINT32 i = 0 ; i < m_ui32WorkCounter ; ++i )
	{
		if( dwThreadID == m_adwThreadID[i] )
		{
			return i;
		}
	}

	return RV_ADD_FAIL;
}

BOOL i3LogFileBase::_CreateFile(void)
{
	return TRUE;
}

BOOL i3LogFileBase::_Working()
{
	return TRUE;
}