#include "stdafx.h"
#include "LogFile.h"

I3_CLASS_INSTANCE(CLogFile, i3ElementBase); 

CLogFile * g_pLogFile = NULL; 

CLogFile::CLogFile(void)
{
	m_hFile = INVALID_HANDLE_VALUE; 
}

CLogFile::~CLogFile(void)
{
	OnDestroy(); 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CLogFile::OnCreate(char * strFileName)
{
	BOOL Rv;
	Rv = FileOpen(strFileName);
	return Rv; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CLogFile::FileOpen(char * strFileName)
{	
	BOOL Rv = TRUE; 

	WriteLog("Log File Close");
	m_csWrite.Enter(); 
	{	
		char FileName[128]; 		

		//Close Old File 
		if( m_hFile != INVALID_HANDLE_VALUE)
		{
			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE; 
		}
		
		//Create New File 
		sprintf(FileName, strFileName);
#ifdef UNICODE
		WCHAR16 wchar[256]; 
		i3String::CopyASCIItoWCHAR(wchar, FileName); 
		m_hFile = ::CreateFile(wchar, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#else
		m_hFile = ::CreateFile(FileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#endif
		if(m_hFile == INVALID_HANDLE_VALUE)
		{			
			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE; 
			Rv = FALSE; 
		}
		else
		{			
			SetFilePointer( m_hFile, 0, NULL, FILE_END);
		}
	}
	m_csWrite.Leave();
	WriteLog("Log File Open");

	return Rv; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CLogFile::OnDestroy(void)
{
	m_csWrite.Enter(); 
	{
		if(m_hFile == INVALID_HANDLE_VALUE)
		{	
			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE; 		
		}
	}
	m_csWrite.Leave();

}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CLogFile::OnUpdate(void)
{
	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CLogFile::WriteLog( char * pLogString )
{
	//Cal Local Time
	SYSTEMTIME		m_SystemTime;
	::GetLocalTime( &m_SystemTime );	

	m_csWrite.Enter(); 
	{
		if( m_hFile != INVALID_HANDLE_VALUE)
		{
			char Message[2048]; 			

			sprintf( Message, "[%d:%d:%d:%d:%d]%s\r\n", m_SystemTime.wMonth, m_SystemTime.wDay, m_SystemTime.wHour, m_SystemTime.wMinute, m_SystemTime.wSecond, pLogString);			

			INT32 Len = i3String::Length(Message); 
			::WriteFile( m_hFile, Message, Len, (LPDWORD)&Len, NULL ); 
		}
	}
	m_csWrite.Leave(); 

	return; 
}
