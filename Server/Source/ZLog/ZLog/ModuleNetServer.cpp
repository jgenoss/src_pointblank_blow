#include "pch.h"
#include "ModuleNetServer.h"
#include "ModuleConfig.h"
#include "ModuleFileManager.h"
#include <process.h>

#define SEND_MAX_PACKET_COUNT		10
#define SERVER_CONNECT_CHECK_TIME	120
#define MAX_MESSAGE_BUFFER			256

I3_CLASS_INSTANCE( CModuleNetServer, i3NetworkClientSocket2 );

// 2011.04.20 안국정 추가
static unsigned __stdcall _DefThreadProc( void* pData )
{
	((CModuleNetServer*)pData)->Working();
	_endthreadex(0);
	return 0;
}

CModuleNetServer::CModuleNetServer()
{
	m_hThread				= NULL;
	m_bIsRunning			= false;

	m_ui32Index				= 0;

	m_eReceiverState		= RECEIVER_STATE_WAITING;

	m_pFileManager			= nullptr;
	m_hFile					= INVALID_HANDLE_VALUE;
	
	m_ui32ThreadID			= 0;
	m_ui32FileSize			= 0;
	m_ui32RecvFileSize		= 0;
}

CModuleNetServer::~CModuleNetServer()
{
	Destroy();
}

bool CModuleNetServer::Create( INT32 i32Idx, CModuleFileManager* pFileManager )
{
	m_ui32Index				= i32Idx;
	m_pFileManager			= pFileManager;

	// 스레드 생성
	m_hThread	= (HANDLE)_beginthreadex( NULL, 0, _DefThreadProc, this, 0, &m_ui32ThreadID );
	if( NULL == m_hThread )																				return false;
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_NETWORK ) )								return false;

	return true;
}

void CModuleNetServer::Destroy()
{
	m_bIsRunning = false;
	::WaitForSingleObject( m_hThread, INFINITE );
	CloseHandle( m_hThread );

	// Socket 은 NSM_cTcpsocket 에서 삭제 합니다.
}

bool CModuleNetServer::SetServer( SOCKET hSocket )
{
	if( FALSE == SetSocket( hSocket ) )																	return false;

	// Reciver Buffer 를 2M 까지 늘려줍니다.
	INT32 i32optVal = 2 * MByte; //2M
	INT32 i32optLen = sizeof(INT32);
	if( SOCKET_ERROR  == setsockopt( hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&i32optVal, i32optLen ) )	return false;

	m_eReceiverState		= RECEIVER_STATE_RUNNING;

	return true;
}

// 2011.04.19 안국정 추가
// 모듈넷서버에도 쓰레드를 추가합니다.
void	CModuleNetServer::Working()
{
	wchar_t wstrInit[ MAX_PATH ];
	i3String::Format( wstrInit, MAX_PATH, L"SV%02d", m_ui32Index );
	g_pLog->AddLogThread( wstrInit, 0, m_ui32ThreadID );
		
	bool	bWorked;

	m_bIsRunning = true;
	while( m_bIsRunning )
	{
		bWorked = false;

		switch( m_eReceiverState )
		{
		case RECEIVER_STATE_WAITING:
			{
				::WaitForSingleObject( m_hThread, 100 );
			}
			break;
		case RECEIVER_STATE_RUNNING:
			{
				if( FALSE == IsConnected() )
				{	
					g_pLog->WriteLog( L"[ERR][%03d] Server Disconnect", m_ui32Index );
					goto e_Disconnect;
				}

				SelectEvent(); 
				
			}
			break;
e_Disconnect:
			m_eReceiverState = RECEIVER_STATE_ENDING;
			break;
		case RECEIVER_STATE_ENDING:
			{
				bWorked = true;
				
				DisConnect();
				m_eReceiverState = RECEIVER_STATE_WAITING;
			}
			break;
		}

		if( false == bWorked ) ::WaitForSingleObject(m_hThread, 1);
	}
}

void CModuleNetServer::PacketParsing( i3NetworkPacket  * pPacket )
{

	char  strFullPath[MAX_PATH];

	PROTOCOL	ProtocolID = pPacket->GetProtocolID();
	
	switch( ProtocolID )
	{

	case PROTOCOL_ZLOG_FILEINFO_SEND:	//로그파일 파일정보
		{
			UINT16	ui16strLength;			

			//열어논 파일이 있으면 닫는다.
			if( m_hFile != INVALID_HANDLE_VALUE )
			{
				::CloseHandle( m_hFile );
				m_hFile = INVALID_HANDLE_VALUE;
			}

			pPacket->ReadData( &ui16strLength, sizeof(UINT16) );

			if (ui16strLength >= MAX_STRING_COUNT) ui16strLength = MAX_STRING_COUNT - 1;
			
			pPacket->ReadData( m_strFileName, sizeof(char) * ui16strLength );
			pPacket->ReadData( &m_ui32FileSize, sizeof(UINT32) );
			
			m_strFileName[ui16strLength] = '\0';
			i3String::Format( strFullPath, MAX_PATH, "%s%s", g_pConfig->m_strServiceLogFilePath, m_strFileName );
			
			//전송받을 파일을 생성한다.
			m_hFile = MakeFile( strFullPath );
			if( INVALID_HANDLE_VALUE == m_hFile )
			{			
				::CloseHandle( m_hFile );
				m_hFile = INVALID_HANDLE_VALUE; 				
			}
			else
			{
				m_ui32RecvFileSize = 0;
			}

		}
		break;

	case PROTOCOL_ZLOG_FILE_SEND:	//로그파일 전송
		{			
			char strData[PACKETBUFFERSIZE];

			UINT16 ui16DataSize = pPacket->GetDataFieldSize();

			//전송받은 데이터를 파일에 기록한다.
			pPacket->ReadData( strData, sizeof(char) * ui16DataSize );
			_WriteFile( ui16DataSize, strData );
			m_ui32RecvFileSize += ui16DataSize;

			//파일의 마지막 부분인 경우 - 끝부분이 아닌이상 무조건 패킷을 꽉채워서 보낸다.
			if( ui16DataSize < PACKETBUFFERSIZE - PACKETHEADERSIZE ||
				( ui16DataSize == PACKETBUFFERSIZE - PACKETHEADERSIZE && m_ui32FileSize == m_ui32RecvFileSize ) )
			{
				::CloseHandle( m_hFile );
				m_hFile = INVALID_HANDLE_VALUE; 

				if ( m_ui32FileSize == m_ui32RecvFileSize )
				{//로그파일을 전송한 서버에 파일전송이 완료되었음을 통보하고 파일의 확장자를 변경한다.(txt -> zlog)

					_SendFileRecvFinish(true);
					i3String::Format( strFullPath, MAX_PATH, "%s\\%s", g_pConfig->m_strServiceLogFilePath, m_strFileName );
					ChangeFileExtension( strFullPath, "zlog", MAX_PATH );

				}
				else
				{//파일전송이 실패하였음을 통보한다.

					_SendFileRecvFinish(false);

				}
			}

		}
		break;

	default:
		break;

	}

	return;
}

void CModuleNetServer::_WriteFile( INT32 i32Size, char* pData )
{
	wchar_t	wstrTemp[SERVER_STRING_COUNT];
	
	if( FALSE == ::WriteFile( m_hFile, pData, i32Size, (LPDWORD)&i32Size, NULL ) )
	{
		if (i32Size >= SERVER_STRING_COUNT) i32Size = SERVER_STRING_COUNT - 1;
		MultiByteToWideChar(CP_ACP, NULL, pData, -1, wstrTemp, i32Size);
		wstrTemp[i32Size] = '\0'; 
		g_pLog->WriteLog( L"[ERR][%03d] File Write Fail : Packet:%s", m_ui32Index, wstrTemp);
	}
	else if( 0 == i32Size )
	{
		// if (i32Size >= SERVER_STRING_COUNT) i32Size = SERVER_STRING_COUNT - 1;
		MultiByteToWideChar(CP_ACP, NULL, pData, -1, wstrTemp, i32Size);
		wstrTemp[i32Size] = '\0';
		g_pLog->WriteLog( L"[ERR][%03d] Write Zero : Packet:%s", m_ui32Index, wstrTemp);
	}
}

void CModuleNetServer::_SendFileRecvFinish( BOOL result )
{
	i3NetworkPacket Sendpacket;

	Sendpacket.SetProtocol( PROTOCOL_ZLOG_FILE_FINISH );
	Sendpacket.WriteData( &result, sizeof(BOOL) );

	wchar_t m_wstrFileName[FILE_NAME_SIZE];
	MultiByteToWideChar(CP_ACP, NULL, m_strFileName, -1, m_wstrFileName, FILE_NAME_SIZE);

	g_pLog->WriteLog( L"[SUCC][%03d] File Recieve Finish. FileName:%s, RecvSize:%d, FileSize:%d, Result:%d", m_ui32Index, m_wstrFileName, m_ui32RecvFileSize, m_ui32FileSize, result);
	
	SendPacketMessage( &Sendpacket );

}