#include "pch.h"
#include "ModuleZLog.h"

#include "ModuleZLogSocket.h"

I3_CLASS_INSTANCE( CModuleZLog, i3Thread );

CModuleZLog * g_pModuleZlog = NULL;

#define MAX_ZLOG_SEND_SIZE					1024
#define MAX_BUFFER_COUNT					256

CModuleZLog::CModuleZLog(void)
{
	m_bRunning			= FALSE;
	m_pZLogSocket		= NULL;
}

CModuleZLog::~CModuleZLog(void)
{	
	Destroy();
}

BOOL CModuleZLog::Create()
{
	// ZLog 모듈이 꺼져있습니다.
	if( FALSE == g_pContextMain->m_bLogDActive ) return TRUE;

	i3String::Copy( m_strLogPath, g_pContextMain->m_strZLogFilePath, MAX_STRING_COUNT );

	// Socket Open
	m_pZLogSocket = CModuleZLogSocket::NewObject(); 
	if( FALSE == m_pZLogSocket->OnCreate( g_pContextMain->m_ui32LogDServerIp, g_pContextMain->m_ui16LogDServerPort ) )
	{
		g_pLog->WriteLog( L"[ERRO] ZLOG MODULE CONNECT FAIL" ); 
		return FALSE;
	}
		
	//Create Thread
	if( FALSE == i3Thread::Create( "ModuleZLog", 0, 4096, NULL, PRIORITY_NORMAL ) )
	{
		g_pLog->WriteLog( L"[ERRO] ZLOG MODULE Thread FAIL" );
		return FALSE; 
	}
	
	m_eUpdateState = ZLOG_UPDATE_STATE_FIND;

	return TRUE; 
}

void CModuleZLog::Destroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	I3_SAFE_RELEASE( m_pZLogSocket );
}

UINT32 CModuleZLog::OnRunning( void * pUserData )
{	
	g_pLog->AddLogThread( L"MDZL", 0, m_ThreadID ); // WriteLog시 자동으로 WorkCounter를 찾아주기때문에 반환 값을 따로 보관하지 않습니다. 

	m_bRunning = TRUE;
	INT32 i32WorkingCount = 0;

	while( m_bRunning )
	{		
		
		if(!m_pZLogSocket->IsConnected()) 
		{			
			m_pZLogSocket->OnDestroy(); 
			if( m_pZLogSocket->OnCreate(g_pContextMain->m_ui32LogDServerIp, g_pContextMain->m_ui16LogDServerPort ) )
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_TYPE_NOTICE, L"SUCCESS LOG_MODULE RECONNECT 1" );
				m_eUpdateState = ZLOG_UPDATE_STATE_FIND;
			}
			else
			{
				m_pZLogSocket->OnDestroy(); 
				g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_TYPE_NORMAL, L"FAIL LOG_MODULE RECONNECT 1" );
			}

			::WaitForSingleObject( m_Handle, 1000);
			continue;
		}
		
		m_pZLogSocket->SelectEvent();

		_UpdateFileUpload();

		if( 0 == i32WorkingCount )
		{	// 한일이 없으면 1ms 휴식.
			::WaitForSingleObject( m_Handle, 1);
		}
	}
	return 0; 
}

BOOL CModuleZLog::_UpdateFileUpload()
{
	// 현재 업데이트 진행 중인지..
	// 업로드 해야 할 파일이 있는지 체크
	switch( m_eUpdateState )
	{
	case ZLOG_UPDATE_STATE_FIND:
		{
			if( _FindUploadFile() )
			{
				m_eUpdateState = ZLOG_UPDATE_STATE_FILE_OPEN;
			}
			else
			{
				::WaitForSingleObject(m_Handle, 1000);
			}
		}
		break;
	case ZLOG_UPDATE_STATE_FILE_OPEN:
		{
			// 파일을 Open 합니다.
			if( _OpenFile() )
			{
				if( 0 == m_ui32FileSize )
				{
					_CloseHandle();
					m_eUpdateState = ZLOG_UPDATE_STATE_FILE_DELETE;
					::WaitForSingleObject(m_Handle, 1000);
				}
				else
				{
					m_eUpdateState = ZLOG_UPDATE_STATE_FILE_UPLOAD_INFO;
				}
				
			}
			else
			{
				_CloseHandle();
				m_eUpdateState = ZLOG_UPDATE_STATE_FIND;
				::WaitForSingleObject(m_Handle, 1000);
			}
		}
		break;
	case ZLOG_UPDATE_STATE_FILE_UPLOAD_INFO:
		{
			if( _UploadFileInfo() )
			{
				m_eUpdateState = ZLOG_UPDATE_STATE_FILE_UPLOAD_CONTENTS;
			}
			else
			{
				_CloseHandle();
				m_eUpdateState = ZLOG_UPDATE_STATE_FIND;
				::WaitForSingleObject(m_Handle, 1000);
			}
		}
		break;
	case ZLOG_UPDATE_STATE_FILE_UPLOAD_CONTENTS:
		{
			if( _UploadFile() )
			{
				_CloseHandle();
				m_eUpdateState = ZLOG_UPDATE_STATE_WAIT_PACKET;				
			}
			else
			{
				_CloseHandle();
				m_eUpdateState = ZLOG_UPDATE_STATE_FIND;
				::WaitForSingleObject(m_Handle, 1000);
			}
		}
		break;
	case ZLOG_UPDATE_STATE_FILE_DELETE:
		{
			if (_DeleteFile())
			{
				m_eUpdateState = ZLOG_UPDATE_STATE_FIND;
			}
			else
			{
				g_pLog->WriteLog(L"File Delete Fail. Error Code : %x", GetLastError());
				::WaitForSingleObject(m_Handle, 1000);
			}
		}
		break;
	}

	return TRUE;
}

BOOL CModuleZLog::_FindUploadFile()
{
	BOOL bRv = FALSE;
	struct _finddata_t	sc_file;
	char strSerachFolder[ MAX_PATH ];
	strcpy( strSerachFolder, m_strLogPath );
	strcat( strSerachFolder, "\\*.*" );

	intptr_t hFile = _findfirst( strSerachFolder, &sc_file );
	if( -1L == hFile )
	{
		_findclose(hFile);
		// 로그남깁니다.
		//WRITE_LOG_FILE_MANAGER( "[ERR] Make Folder List Fail" );
		return bRv;
	}

	// 대기열 폴더 리스트를 재 정렬 합니다.
	while( 0 == _findnext( hFile, &sc_file ) ) 
	{
		// ".", ".." 는 검색 대상에서 제외 시킵니다.
		if( '.' == sc_file.name[0] ) continue;

		// 지금 시간보다 1시간 넘게 지난 파일만 업로드 합니다.
		INT32 i32Time;
		i32Time = _GetFileDate(sc_file.name);
		SYSTEMTIME FileTime;

		FileTime.wYear = (WORD)(i32Time / 1000000);
		FileTime.wMonth = (i32Time / 10000) % 100;
		FileTime.wDay = (i32Time / 100) % 100;
		FileTime.wHour = i32Time % 100;
		FileTime.wMinute = 0;
		FileTime.wSecond = 0;

		DATE32 stDate;
		stDate = FileTime;

		if (i3ThreadTimer::LocalTime().DiffTime(stDate) < 3660)	continue;

		i3String::Format( m_strUploadFile, MAX_STRING_COUNT, "%s", sc_file.name );
		bRv = TRUE;
		break;
	}
	_findclose(hFile);

	return bRv;
}

INT32 CModuleZLog::_GetFileDate( char* strHour )
{	
	INT32 i32Hour = 0;
	for( INT32 i = 0 ; i < 10 ; i++ )
	{
		i32Hour = i32Hour * 10 + (strHour[i]-'0');
	}
	return i32Hour;
}

// 파일 오픈
// 파일 이름 전송
// 에크 받으면??
// 내용 전송
// 전송 후엔 파일 끝
// 파일 삭제

BOOL CModuleZLog::_OpenFile()
{
	char strUploadFile[ MAX_STRING_COUNT ];

	i3String::Format( strUploadFile, MAX_STRING_COUNT, "%s\\%s", m_strLogPath, m_strUploadFile );
	// 파일을 Open 합니다.
	m_hFile = ::CreateFile( strUploadFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( INVALID_HANDLE_VALUE == m_hFile )
	{
		// 파일이 없습니다. 작업을 종료 합니다.
		g_pLog->WriteLog(L"File Open Fail. Error Code : %x", GetLastError());
		
		return FALSE;
	}
	if(INVALID_SET_FILE_POINTER == SetFilePointer( m_hFile, 0, 0, FILE_BEGIN ) )
	{
		g_pLog->WriteLog(L"File SetPointer Fail. Error Code : %x", GetLastError());

		return FALSE;
	}


	m_ui32FileSize	= _GetFileSize();

	return TRUE;
}

BOOL CModuleZLog::_UploadFileInfo()
{
	i3NetworkPacket	SendPacket;
	
	UINT16 ui16StrLength = static_cast<UINT16>(i3String::Length(m_strUploadFile));
	
	SendPacket.SetProtocol(PROTOCOL_ZLOG_FILEINFO_SEND);
	SendPacket.WriteData( &ui16StrLength, sizeof(UINT16) );
	SendPacket.WriteData( m_strUploadFile, sizeof(char) * ui16StrLength );
	SendPacket.WriteData( &m_ui32FileSize, sizeof(UINT32) );

	m_pZLogSocket->SendPacketMessage( &SendPacket );

	return TRUE;
}

BOOL CModuleZLog::_UploadFile()
{
	INT32 i32ret;	
	i3NetworkPacket	SendPacket;
	UINT32	TotalSendSize = 0;	
	
	m_bFinish = FALSE;

	while(!m_bFinish)
	{
		SendPacket.Clear();
		SendPacket.SetProtocol(PROTOCOL_ZLOG_FILE_SEND);

		INT32 i32Size = 0;
		char PacketBuffer[PACKETBUFFERSIZE];
		
		// 파일을 읽어 옵니다.
		if (FALSE == ::ReadFile( m_hFile, PacketBuffer, PACKETBUFFERSIZE - SendPacket.GetPacketSize(), (LPDWORD)&i32Size, NULL ))
		{
			// 파일 읽기에 실패 했습니다. 끝냅니다.
			//char strLog[ MAX_PATH ];
			//sprintf( strLog, "[ERR][%03d] Read File Fail : File=%s", m_i32ThreadIdx, m_strFileName );
			//WRITE_LOG_DB_UPLOADER( m_i32ThreadIdx, strLog );
			return FALSE;
		}

		if( 0 >= i32Size )
		{	// 내용이 없습니다. 종료합니다.	

			if( TotalSendSize == m_ui32FileSize )
				m_bFinish = TRUE;
			else
				return FALSE;
		}
		else
		{
			SendPacket.WriteData(PacketBuffer, static_cast<UINT16>(i32Size));
 			i32ret = m_pZLogSocket->SendPacketMessage( &SendPacket );			
			TotalSendSize += (UINT32)i32Size;
		}

		if( TotalSendSize == m_ui32FileSize )
		{
			wchar_t wstrFile[ MAX_STRING_COUNT ];
			::MultiByteToWideChar( CP_ACP, NULL, m_strUploadFile, -1, wstrFile, MAX_STRING_COUNT );
			g_pLog->WriteLog( L"File Transfer Finished. FileName:%s - size:%d ", wstrFile, TotalSendSize);
			m_bFinish = TRUE;
		}

	}

	return TRUE;
}

BOOL CModuleZLog::_DeleteFile()
{
	char strUploadFile[ MAX_STRING_COUNT ];
	i3String::Format( strUploadFile, MAX_STRING_COUNT, "%s\\%s", m_strLogPath, m_strUploadFile );
	return DeleteFile( strUploadFile );
}

DWORD CModuleZLog::_GetFileSize()
{
	if( NULL == m_hFile )	return 0;

	DWORD dwFileSizeHigh = 0;
	DWORD dwFileSizeLow = ::GetFileSize( m_hFile, &dwFileSizeHigh );

	return (dwFileSizeHigh << 16 ) | dwFileSizeLow;
}

// 파일 복사 시작
// 파일 이름
// 파일 내용
// 파일 끝
// 파일 삭제