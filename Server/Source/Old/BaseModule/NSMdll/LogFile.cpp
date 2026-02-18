#include "stdafx.h"
#include "LogFile.h"
#include "CriticalSection.h"
#include "RingBuffer.h"
#include "Memory.h"
#include "String.h"
#include <stdio.h>

typedef struct _tagLogFileBuffer
{		
	char	_szMessage[1024];
}LOG_FILE_BUFFER;

static UINT32 _DefThreadProc( void * pPointer )
{
	CLogFile * pP = (CLogFile * )pPointer; 
	pP->OnWorking();
	return 0;
}

CLogFile::CLogFile(void)
{
	m_hThread			= INVALID_HANDLE_VALUE; 
	m_hFile				= INVALID_HANDLE_VALUE; 
	m_strFileName		= NULL; 
	m_pAddCs			= NULL; 
	m_iWorkCounter		= 0; 
	m_bIsRunning		= false;
}

CLogFile::~CLogFile(void)
{
	OnDestroy(); 
}

BOOL CLogFile::OnCreate(char * strFileName, INT32 iSaveType, INT32 iLogBufferCount, INT32 iThreadPri)
{
	//Set init Value
	NSM_FILLZERO( m_ppLogBuffer, sizeof(m_ppLogBuffer) );
	NSM_FILLZERO( m_ppInitString, sizeof(m_ppInitString) );
	m_iWorkCounter	= 0; 
	m_iSaveType		= iSaveType; 
	m_iBufferCount	= iLogBufferCount; 
	m_strFileName	= (char*)NSM_ALLOC( NSMString::Length(strFileName) + 1 ); 
	strcpy( m_strFileName, strFileName); 

	//Create CS
	m_pAddCs = new NSM_cCriticalSection; 
	
	//Create File 
	if(!_CreateFile() )return FALSE; 

	//Create Thread 
	UINT32	ThreadID;
	m_hThread = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc ,this ,0 ,(DWORD *)&ThreadID);
	if(m_hThread == NULL)return FALSE; 
	if( !SetThreadPriority( m_hThread, iThreadPri) )return FALSE; 

	return TRUE; 
}

BOOL CLogFile::OnDestroy(void)
{
	m_bIsRunning = false;
	::WaitForSingleObject( m_hThread, INFINITE );

	for(UINT32 i = 0; i < m_iWorkCounter; i++)
	{
		SAFE_DELETE(	m_ppLogBuffer[i] ); 
		NSM_SAFE_FREE(	m_ppInitString[i] ); 
	}

	NSM_SAFE_FREE( m_strFileName ); 
	SAFE_DELETE( m_pAddCs ); 

	return TRUE; 
}

void CLogFile::_WriteLogFile(char * pLog)
{
	INT32 Len = NSMString::Length(pLog); 
	::WriteFile( m_hFile, pLog, Len, (LPDWORD)&Len, NULL ); 
}

INT32 CLogFile::AddLogThread(char * pstrPreString, INT32 iType)
{
	INT32 Rv = RV_ADD_FAIL; 
	m_pAddCs->Enter(); 
	{
		INT32 iIdx= m_iWorkCounter; 
		if( m_iWorkCounter != LOG_BUFFER_MAX_COUNTER )
		{
			m_ppLogBuffer[iIdx] = new NSM_CRingBuffer;
			if(m_ppLogBuffer[iIdx] != NULL)
			{			
				if( m_ppLogBuffer[iIdx]->OnCreate( sizeof(LOG_FILE_BUFFER), m_iBufferCount ) )
				{
					m_iWorkCounter++; 
					Rv = iIdx; 
				}
			}
		}
	}
	m_pAddCs->Leave(); 

	return Rv; 
}

void CLogFile::OnWorking(void)
{
	INT32 iLogCount; 
	LOG_FILE_BUFFER * pLogBuffer;
	BOOL bWorking; 

	m_bIsRunning = true;
	while( m_bIsRunning )
	{
		bWorking = FALSE; 

		//Create File 
		_UpdateTime();
		switch(m_iSaveType)
		{
		case FILE_SAVE_INTERVAL_DAY		: 
			if(m_ui32FileDay != m_iDay)_CreateFile(); 
			break; 
		case FILE_SAVE_INTERVAL_HOUR	:
		default							: 
			if(m_ui32FileHour != m_iHour)_CreateFile(); 
			break; 
		}
		
		//Write Log 
		for(UINT32 j = 0; j < m_iWorkCounter; j++)
		{
			iLogCount = m_ppLogBuffer[j]->GetBufferCount(); 
			for(INT32 i = 0; i < iLogCount; i++)
			{
				pLogBuffer = (LOG_FILE_BUFFER *) m_ppLogBuffer[j]->Pop();
				_WriteLogFile( pLogBuffer->_szMessage ); 
				m_ppLogBuffer[j]->PopIdx();
				bWorking = TRUE; 
			}
		}

		if(!bWorking)Sleep(1); 	 
	}

	return; 
}

void CLogFile::Write_LogFile(INT32 iWorkIdx, char * pLogString)
{
	NSM_CRingBuffer		*	pRingBuffer = NULL; 
	LOG_FILE_BUFFER			LogBuffer; 
	char					szTime[256];	

	//Make PreString 
	pRingBuffer = m_ppLogBuffer[iWorkIdx];	
	sprintf( szTime, "[%02d:%02d:%02d][%s]", m_iHour, m_iMinute, m_iSecond, m_ppInitString[iWorkIdx]);
	sprintf(LogBuffer._szMessage, "%s%s\r\n", szTime, pLogString);
	pRingBuffer->Push( (void*)&LogBuffer );

	return; 
}

BOOL CLogFile::_CreateFile(void)
{
	BOOL Rv = TRUE; 
	char FileName[512]; 
	sprintf(FileName, "%s%02d_%02d_%02d.txt", m_strFileName, m_iMonth, m_iDay, m_iHour);

	//Close Old File 
	if( m_hFile != INVALID_HANDLE_VALUE )
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}

	//Create New File 
	m_hFile = ::CreateFile(FileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
		Rv = FALSE; 
	}
	else
	{		
		SetFilePointer( m_hFile, 0, NULL, FILE_END);
		m_ui32FileHour	= m_iHour;
		m_ui32FileDay	= m_iDay; 
	}

	return Rv;
}
