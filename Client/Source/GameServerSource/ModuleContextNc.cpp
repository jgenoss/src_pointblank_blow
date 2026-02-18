#include "pch.h"
#include "ModuleContextNc.h"
#include "ServerContext.h"
#include "UserSession.h"
#include "Room.h"
#include "RoomManager.h"
#include "UserManager.h"

#include "ModuleGipSocket.h"
#include "ModuleLogSocket.h"

//	MM/DD/YYYY HH:MI:SS.00 -> YYMMDDHHMI
#define ConverTimeFromtSTRToUINT32( strDate )			(*(strDate+0)-'0')*10000000 +\
														(*(strDate+1)-'0')*1000000 +\
														(*(strDate+3)-'0')*100000 +\
														(*(strDate+4)-'0')*10000 +\
														(*(strDate+8)-'0')*1000000000 +\
														(*(strDate+9)-'0')*100000000 +\
														(*(strDate+11)-'0')*1000 +\
														(*(strDate+12)-'0')*100 +\
														(*(strDate+14)-'0')*10 +\
														(*(strDate+15)-'0');
//	YYMMDDHHMI -> MM/DD/YYYY HH:MI:SS.00
#define ConverTimeFromtUINT32ToSTR( strDate, ui32Date)	i3String::Format( strDate, SERVER_STRING_COUNT, "%02d/%02d/20%02d %02d:%02d:00.000",\
														((ui32Date%100000000)/1000000), ((ui32Date%1000000)/10000), (ui32Date/100000000), ((ui32Date%10000)/100), (ui32Date%100) );

#define ConverTimeFromtUINT64ToSTR( strDate, ui64Date)	i3String::Format( strLoginTime, SERVER_STRING_COUNT, "%02d/%02d/20%02d %02d:%02d:%02d.000",\
														(INT32)((ui64Date%10000000000)/100000000), (INT32)((ui64Date%100000000)/1000000), (INT32)(ui64Date/10000000000), (INT32)((ui64Date%1000000)/10000), (INT32)(ui64Date%10000)/100, (INT32)(ui64Date%100) );

UINT32 _GetDiffTime( UINT64 ui64DTLogIn, UINT64 ui64DTLogOut )
{
	struct tm tmLogIn;
	
	tmLogIn.tm_year		= (INT32)((ui64DTLogIn / 10000000000) + 100);			ui64DTLogIn %= 10000000000;
	tmLogIn.tm_mon		= (INT32)((ui64DTLogIn / 100000000) - 1);				ui64DTLogIn %= 100000000;
	tmLogIn.tm_mday		= (INT32)(ui64DTLogIn / 1000000);						ui64DTLogIn %= 1000000;
	tmLogIn.tm_hour		= (INT32)(ui64DTLogIn / 10000) ;						ui64DTLogIn %= 10000;
	tmLogIn.tm_min		= (INT32)(ui64DTLogIn / 100) ;							ui64DTLogIn %= 100; 
	tmLogIn.tm_sec		= (INT32)ui64DTLogIn; 
	tmLogIn.tm_isdst	= -1;

	time_t tmTLogin = mktime( &tmLogIn );

	struct tm tmLogOut;
	
	tmLogOut.tm_year	= (INT32)((ui64DTLogOut / 10000000000) + 100);			ui64DTLogOut %= 10000000000;
	tmLogOut.tm_mon		= (INT32)((ui64DTLogOut / 100000000) - 1);				ui64DTLogOut %= 100000000;
	tmLogOut.tm_mday	= (INT32)(ui64DTLogOut / 1000000);						ui64DTLogOut %= 1000000;
	tmLogOut.tm_hour	= (INT32)(ui64DTLogOut / 10000);						ui64DTLogOut %= 10000;
	tmLogOut.tm_min		= (INT32)(ui64DTLogOut / 100);							ui64DTLogOut %= 100;
	tmLogOut.tm_sec		= (INT32)ui64DTLogOut; 
	tmLogOut.tm_isdst	= -1;
	
	time_t tmTLogOut = mktime(&tmLogOut);

	return (UINT32)(difftime( tmTLogOut, tmTLogin ));
}

//htons() - "Host to Network Short" 
//htonl() - "Host to Network Long" 
//ntohs() - "Network to Host Short" 
//ntohl() - "Network to Host Long"
DWORD sConvertMultiToUTF8(char *pszResultBuffer, INT32 i32ResultBufferSize, char *pszSourceBuffer)
{
	INT32 i32convertM2USize = MultiByteToWideChar(CP_ACP, 0, pszSourceBuffer, -1, 0, 0);
	wchar_t *pwszTempWideBuffer = (wchar_t *)malloc(sizeof(wchar_t) * i32convertM2USize);
	MultiByteToWideChar(CP_ACP, 0, pszSourceBuffer, -1, pwszTempWideBuffer, i32convertM2USize);

	INT32 i32convertU2MSize = WideCharToMultiByte(CP_UTF8, 0, pwszTempWideBuffer, -1, 0, 0, NULL, NULL);
	if (i32convertU2MSize > i32ResultBufferSize)
		return (DWORD)-1;

	WideCharToMultiByte(CP_UTF8, 0, pwszTempWideBuffer, -1, pszResultBuffer, i32convertU2MSize, NULL, NULL);

	free(pwszTempWideBuffer);
	return 0;
}

//////////////////////////////////////////////////////////////
//COMMON	
#define MODULE_CONTEXTB_SEND_COUNT			100				//한번에 보내는 최대 카운트 //Auth, Clan, GIP, LOG에 적용합니다. 
#define MODULE_CONTEXT_MAX_SEND_IDX_COUNT	2000000000		

//////////////////////////////////////////////////////////////
//GIP
#define	GIP_SEND_BUFFER_COUNT				100
#define GIP_BACKUP_COUNT					50

//////////////////////////////////////////////////////////////
//LOG
#define	LOG_SEND_BUFFER_COUNT				100
#define STRING_START_IDX					11
#define NEW_WHERE_DATA_IDX					2
#define NEW_WHERE_STRING_IDX				3
#define NEW_STRING_START_IDX				4
#define MACHINE_NAME						"PB"
#define SERVER_TYPE							"GS"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
I3_CLASS_INSTANCE( CModuleContextNC, i3Thread );
CModuleContextNC * g_pModuleContextNC = NULL; 

CModuleContextNC::CModuleContextNC(void)
{
	m_bZLogRun				= FALSE;
	m_bRunning				= FALSE;

	//Log
	m_pLogModuleSocket		= NULL;
	m_pLogMainMessage		= NULL;
	m_i32LogMain_WritePos	= 0;
	m_i32LogMain_ReadPos	= 0;

	m_pLogControlMessage	= NULL;			// Control Thread
	m_i32LogControl_WritePos= 0;
	m_i32LogControl_ReadPos	= 0;

	m_pLogAuthMessage		= NULL;
	m_i32LogAuth_WritePos	= 0;
	m_i32LogAuth_ReadPos	= 0;

	m_pLogClanMessage		= NULL;
	m_i32LogClan_WritePos	= 0;
	m_i32LogClan_ReadPos	= 0;

	m_pLogMessMessage		= NULL;
	m_i32LogMess_WritePos	= 0;
	m_i32LogMess_ReadPos	= 0;

	m_ppLogUserMessage		= NULL;
	m_pLogUser_WritePos		= NULL;
	m_pLogUser_ReadPos		= NULL;
}

CModuleContextNC::~CModuleContextNC(void)
{	
	OnDestroy(); 
}

INT32 CModuleContextNC::OnCreate(UINT32 ui32ThreadCount)
{
	m_ui32ThreadCount = ui32ThreadCount; //Set Buffer 

	if( !_LogCreate() )		return SERVER_ERROR_LOG; 

	//Create Thread
	if( !i3Thread::Create("ModuleContextB", 0, 4096, NULL, PRIORITY_HIGH) )return FALSE; 
	g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "MODULE_B(NC LOG_D, NC GIP) CONTEXT READY!");

	return EVENT_ERROR_SUCCESS; 
}

BOOL CModuleContextNC::CheckPos( INT32* pi32WritePos, INT32* pi32ReadPos )
{
	if( *pi32ReadPos > MODULE_CONTEXT_MAX_SEND_IDX_COUNT ) return FALSE;
	if( (*pi32WritePos - *pi32ReadPos) >= LOG_SEND_BUFFER_COUNT ) return FALSE;

	if(*pi32WritePos == *pi32ReadPos)
	{
		*pi32WritePos	= 0;
		*pi32ReadPos	= 0;
	}
	return TRUE;
}

void CModuleContextNC::SendZLog()
{
	if( FALSE == g_pContextMain->m_bLogDActive )	return;

	m_bZLogRun = FALSE;

	LOG_SEND_MESSAGE	* pSendBuffer;
	INT32				* pi32WritePos;
	INT32				* pi32ReadPos;
	UINT32				* pui32BufferSize;
	char				* pstrBuffer;
	UINT32				ui32BufferIdx;
	BOOL				bCheck;

	pi32WritePos	= &m_i32LogMain_WritePos;
	pi32ReadPos		= &m_i32LogMain_ReadPos;
	bCheck		= CheckPos( pi32WritePos, pi32ReadPos );
	if( m_ui32LogMain_Size > 0 && bCheck == TRUE )
	{
		pSendBuffer = m_pLogMainMessage;
		pui32BufferSize	= &m_ui32LogMain_Size;
		pstrBuffer	= m_strMainTempBuffer;
		ui32BufferIdx = *pi32WritePos % LOG_SEND_BUFFER_COUNT;

		i3mem::Copy( pSendBuffer[ui32BufferIdx]._szMessage, pstrBuffer, LOG_MESSAGE_MAX );
		pSendBuffer[ui32BufferIdx]._Size = *pui32BufferSize;
		*pi32WritePos = *pi32WritePos + 1;
	}

	pi32WritePos	= &m_i32LogControl_WritePos;
	pi32ReadPos		= &m_i32LogControl_ReadPos;
	bCheck = CheckPos( pi32WritePos, pi32ReadPos );
	if( m_ui32LogControl_Size > 0 && bCheck == TRUE )
	{
		pSendBuffer = m_pLogControlMessage;
		pui32BufferSize	= &m_ui32LogControl_Size;
		pstrBuffer	= m_strControlTempBuffer;
		ui32BufferIdx = *pi32WritePos % LOG_SEND_BUFFER_COUNT;

		i3mem::Copy( pSendBuffer[ui32BufferIdx]._szMessage, pstrBuffer, LOG_MESSAGE_MAX );
		pSendBuffer[ui32BufferIdx]._Size = *pui32BufferSize;
		*pi32WritePos = *pi32WritePos + 1;
	}

	pi32WritePos	= &m_i32LogAuth_WritePos;
	pi32ReadPos		= &m_i32LogAuth_ReadPos;
	bCheck = CheckPos( pi32WritePos, pi32ReadPos );
	if( m_ui32LogAuth_Size > 0 && bCheck == TRUE )
	{
		pSendBuffer = m_pLogAuthMessage;
		pui32BufferSize	= &m_ui32LogAuth_Size;
		pstrBuffer	= m_strAuthTempBuffer;
		ui32BufferIdx = *pi32WritePos % LOG_SEND_BUFFER_COUNT;

		i3mem::Copy( pSendBuffer[ui32BufferIdx]._szMessage, pstrBuffer, LOG_MESSAGE_MAX );
		pSendBuffer[ui32BufferIdx]._Size = *pui32BufferSize;
		*pi32WritePos = *pi32WritePos + 1;
	}

	pi32WritePos	= &m_i32LogClan_WritePos;
	pi32ReadPos		= &m_i32LogClan_ReadPos;
	bCheck = CheckPos( pi32WritePos, pi32ReadPos );
	if( m_ui32LogClan_Size > 0 && bCheck == TRUE )
	{
		pSendBuffer = m_pLogClanMessage;
		pui32BufferSize	= &m_ui32LogClan_Size;
		pstrBuffer	= m_strClanTempBuffer;
		ui32BufferIdx = *pi32WritePos % LOG_SEND_BUFFER_COUNT;

		i3mem::Copy( pSendBuffer[ui32BufferIdx]._szMessage, pstrBuffer, LOG_MESSAGE_MAX );
		pSendBuffer[ui32BufferIdx]._Size = *pui32BufferSize;
		*pi32WritePos = *pi32WritePos + 1;
	}

	pi32WritePos	= &m_i32LogMess_WritePos;
	pi32ReadPos		= &m_i32LogMess_ReadPos;
	bCheck = CheckPos( pi32WritePos, pi32ReadPos );
	if( m_ui32LogMess_Size > 0 && bCheck == TRUE )
	{
		pSendBuffer = m_pLogMessMessage;
		pui32BufferSize	= &m_ui32LogMess_Size;
		pstrBuffer	= m_strMessTempBuffer;
		ui32BufferIdx = *pi32WritePos % LOG_SEND_BUFFER_COUNT;

		i3mem::Copy( pSendBuffer[ui32BufferIdx]._szMessage, pstrBuffer, LOG_MESSAGE_MAX );
		pSendBuffer[ui32BufferIdx]._Size = *pui32BufferSize;
		*pi32WritePos = *pi32WritePos + 1;
	}

	for( UINT32 i = 0; i < m_ui32ThreadCount; ++i )
	{
		pi32WritePos	= &m_pLogUser_WritePos[i];
		pi32ReadPos	= &m_pLogUser_ReadPos[i];
		bCheck = CheckPos( pi32WritePos, pi32ReadPos );
		if( m_pui32LogUser_Size[i] > 0 && bCheck == TRUE )
		{				
			pSendBuffer = m_ppLogUserMessage[i];
			pui32BufferSize	= &m_pui32LogUser_Size[i];
			pstrBuffer	= m_ppstrUserTempBuffer[i];
			ui32BufferIdx	= *pi32WritePos % LOG_SEND_BUFFER_COUNT;

			i3mem::Copy( pSendBuffer[ui32BufferIdx]._szMessage, pstrBuffer, LOG_MESSAGE_MAX );
			pSendBuffer[ui32BufferIdx]._Size = *pui32BufferSize;
			*pi32WritePos = *pi32WritePos + 1;
		}
	}
}

void CModuleContextNC::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	_LogDestroy();
	return; 
}

UINT32 CModuleContextNC::OnRunning( void * pUserData)
{	
	m_bRunning = TRUE;
	INT32 i32WorkingCount = 0;

	if( TRUE == g_pContextMain->m_bLogDActive ) m_bZLogRun = TRUE;

	while( m_bRunning )
	{
		i32WorkingCount = 0;
		if(g_pContextMain->m_bLogDActive) 	i32WorkingCount += _LogUpdate();
		
		if( 0 == i32WorkingCount )
		{	// 한일이 없으면 1ms 휴식.
			::WaitForSingleObject( m_Handle, 1);
		}
	}
	return 0; 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//LOGD
#define LOG_CHECKVALID_IDX	LOG_SEND_MESSAGE	* pSendBuffer; \
							INT32				* pi32WritePos; \
							INT32				* pi32ReadPos; \
							UINT32				* pui32BufferSize; \
							char				* pstrBuffer; \
							UINT32				ui32BufferIdx; \
							\
							switch( ThreadIdx )\
							{\
							case	MAIN_THREAD_IDX			: \
								pSendBuffer = m_pLogMainMessage; \
								pi32WritePos	= &m_i32LogMain_WritePos;\
								pi32ReadPos	= &m_i32LogMain_ReadPos; \
								pui32BufferSize	= &m_ui32LogMain_Size; \
								pstrBuffer	= m_strMainTempBuffer; \
								break; \
							case	MODULE_CONTROL_THREAD_IDX	: \
								pSendBuffer = m_pLogControlMessage; \
								pi32WritePos	= &m_i32LogControl_WritePos;\
								pi32ReadPos	= &m_i32LogControl_ReadPos;\
								pui32BufferSize	= &m_ui32LogControl_Size; \
								pstrBuffer	= m_strControlTempBuffer; \
								break; \
							case	MODULE_TRANS_THREAD_IDX	: \
								pSendBuffer = m_pLogAuthMessage; \
								pi32WritePos	= &m_i32LogAuth_WritePos;\
								pi32ReadPos	= &m_i32LogAuth_ReadPos;\
								pui32BufferSize	= &m_ui32LogAuth_Size; \
								pstrBuffer	= m_strAuthTempBuffer; \
								break; \
							case	MODULE_CLAN_THREAD_IDX	: \
								pSendBuffer = m_pLogClanMessage; \
								pi32WritePos	= &m_i32LogClan_WritePos;\
								pi32ReadPos	= &m_i32LogClan_ReadPos;\
								pui32BufferSize	= &m_ui32LogClan_Size; \
								pstrBuffer	= m_strClanTempBuffer; \
								break; \
							case	MODULE_MESSENGER_THREAD_IDX	: \
								pSendBuffer = m_pLogMessMessage; \
								pi32WritePos	= &m_i32LogMess_WritePos;\
								pi32ReadPos	= &m_i32LogMess_ReadPos;\
								pui32BufferSize	= &m_ui32LogMess_Size; \
								pstrBuffer	= m_strMessTempBuffer; \
								break; \
							default	: \
								pSendBuffer = m_ppLogUserMessage[ThreadIdx];\
								pi32WritePos	= &m_pLogUser_WritePos[ThreadIdx]; \
								pi32ReadPos	= &m_pLogUser_ReadPos[ThreadIdx]; \
								pui32BufferSize	= &m_pui32LogUser_Size[ThreadIdx]; \
								pstrBuffer	= m_ppstrUserTempBuffer[ThreadIdx]; \
								break; \
							}\
							if(*pi32WritePos == *pi32ReadPos)\
							{\
								*pi32WritePos	= 0; \
								*pi32ReadPos	= 0; \
							}\
							if( *pi32ReadPos > MODULE_CONTEXT_MAX_SEND_IDX_COUNT ) \
							{\
								g_pModuleLogFile->Write_M_Log(ThreadIdx,NULL,"LogD Send Buffer Full", WARNING_NETWORK );\
								return; \
							}\
							if( (*pi32WritePos - *pi32ReadPos) >= LOG_SEND_BUFFER_COUNT )\
							{\
								g_pModuleLogFile->Write_M_Log(ThreadIdx,NULL,"LogD Send Buffer Overflow", WARNING_NETWORK );\
								return; \
							}\
							ui32BufferIdx = *pi32WritePos % LOG_SEND_BUFFER_COUNT;

BOOL CModuleContextNC::_LogCreate(void)
{	
	//1. Create Work Send Buffer
	m_ppLogUserMessage	= (LOG_SEND_MESSAGE	**)i3MemAlloc(m_ui32ThreadCount * sizeof(LOG_SEND_MESSAGE*)); 
	m_pLogUser_WritePos = (INT32*)i3MemAlloc(m_ui32ThreadCount * sizeof(INT32));
	m_pLogUser_ReadPos	= (INT32*)i3MemAlloc(m_ui32ThreadCount * sizeof(INT32));
	m_pui32LogUser_Size	= (UINT32*)i3MemAlloc(m_ui32ThreadCount * sizeof(UINT32));
	m_ppstrUserTempBuffer	= new char*[m_ui32ThreadCount];
	for(UINT32 i = 0; i < m_ui32ThreadCount; i++)
	{
		m_ppLogUserMessage[i]	= (LOG_SEND_MESSAGE	*)i3MemAlloc(sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT);	
		m_ppstrUserTempBuffer[i]= new char[LOG_MESSAGE_MAX];
		i3mem::FillZero( m_ppLogUserMessage[i], sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT);
		i3mem::FillZero( m_ppstrUserTempBuffer[i], LOG_MESSAGE_MAX );
		m_pLogUser_WritePos[i]	= 0; 
		m_pLogUser_ReadPos[i]	= 0; 
		m_pui32LogUser_Size[i]	= 0;
	}

	//2. Create Main Send Buffer 
	m_pLogMainMessage	= (LOG_SEND_MESSAGE	*)i3MemAlloc(sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT);	// Main Thread 
	i3mem::FillZero( m_pLogMainMessage, sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT );
	i3mem::FillZero( m_strMainTempBuffer, LOG_MESSAGE_MAX );
	m_i32LogMain_WritePos	= 0;
	m_i32LogMain_ReadPos	= 0;
	m_ui32LogMain_Size		= 0;

	//3-1. Create Module Send Buffer Trans
	m_pLogControlMessage	= (LOG_SEND_MESSAGE	*)i3MemAlloc(sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT);	// Module Thread 
	i3mem::FillZero( m_pLogControlMessage, sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT );
	i3mem::FillZero( m_strControlTempBuffer, LOG_MESSAGE_MAX );
	m_i32LogControl_WritePos	= 0;
	m_i32LogControl_ReadPos	= 0;
	m_ui32LogControl_Size	= 0;

	m_pLogAuthMessage	= (LOG_SEND_MESSAGE	*)i3MemAlloc(sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT);	// Module Thread 
	i3mem::FillZero( m_pLogAuthMessage, sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT );
	i3mem::FillZero( m_strAuthTempBuffer, LOG_MESSAGE_MAX );
	m_i32LogAuth_WritePos	= 0;
	m_i32LogAuth_ReadPos	= 0;
	m_ui32LogAuth_Size		= 0;

	//3-2. Create Module Send Buffer Clan
	m_pLogClanMessage	= (LOG_SEND_MESSAGE	*)i3MemAlloc(sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT);	// Module Thread 
	i3mem::FillZero( m_pLogClanMessage, sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT );
	i3mem::FillZero( m_strClanTempBuffer, LOG_MESSAGE_MAX );
	m_i32LogClan_WritePos	= 0;
	m_i32LogClan_ReadPos	= 0;
	m_ui32LogClan_Size		= 0;

	//3-3. Create Module Send Buffer Mess
	m_pLogMessMessage	= (LOG_SEND_MESSAGE	*)i3MemAlloc(sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT);	// Module Thread 
	i3mem::FillZero( m_pLogMessMessage, sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT );
	i3mem::FillZero( m_strMessTempBuffer, LOG_MESSAGE_MAX );
	m_i32LogMess_WritePos	= 0;
	m_i32LogMess_ReadPos	= 0;
	m_ui32LogMess_Size		= 0;

	BOOL bRv; 

	//4. Create Module
	m_pLogModuleSocket = CModuleLogSocket::NewObject(); 
	if( m_pLogModuleSocket->OnConnect( g_pContextMain->m_strLogDServerIp, g_pContextMain->m_ui16LogDServerPort) )
	{
		bRv = TRUE; 
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "LOG MODULE CONNECT SUCCESS"); 
	}
	else
	{
		bRv = FALSE; 
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "LOG MODULE CONNECT FAIL"); 
	}

	return bRv; 
}

INT32 CModuleContextNC::_LogUpdate(void)
{
	INT32	i32SendCount; 
	UINT32	ui32ReadIdx;
	UINT32	ui32SendSize;
	UINT8	ui8SendFailCount = 0; 
	BOOL	bIsSend;
	bIsSend = FALSE; 
	ui32SendSize = 0;
	UINT32	ui32SendCompleteCount = 0; 

	if(!m_pLogModuleSocket->IsConnected()) 
	{			
		m_pLogModuleSocket->OnDestroy(); 
		if( m_pLogModuleSocket->OnConnect(g_pContextMain->m_strLogDServerIp, g_pContextMain->m_ui16LogDServerPort ) )
		{
			g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX, NULL,"SUCCESS LOG_MODULE RECONNECT 1", WARNING_NETWORK );
		}
		else
		{
			m_pLogModuleSocket->OnDestroy(); 
			g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX, NULL,"FAIL LOG_MODULE RECONNECT 1", WARNING_NETWORK );
		}
		return ui32SendCompleteCount;
	}

	//Main Thread 
 	i32SendCount = m_i32LogMain_WritePos - m_i32LogMain_ReadPos; 
	if(i32SendCount > 0)
	{
		for(INT32 i = 0; i < i32SendCount; i++)
		{
			if(ui32SendCompleteCount < MODULE_CONTEXTB_SEND_COUNT)
			{
				ui32ReadIdx = m_i32LogMain_ReadPos % LOG_SEND_BUFFER_COUNT; 
				ui32SendSize += m_pLogModuleSocket->SendPacketMessage( m_pLogMainMessage[ui32ReadIdx]._szMessage, m_pLogMainMessage[ui32ReadIdx]._Size ); 
				m_i32LogMain_ReadPos++;
				ui32SendCompleteCount++;
			}
		}
		bIsSend = TRUE; 
	}

	//Control Module Thread
	i32SendCount = m_i32LogControl_WritePos - m_i32LogControl_ReadPos; 
	if(i32SendCount > 0)
	{
		for(INT32 i = 0; i < i32SendCount; i++)
		{
			if(ui32SendCompleteCount < MODULE_CONTEXTB_SEND_COUNT)
			{
				ui32ReadIdx = m_i32LogControl_ReadPos % LOG_SEND_BUFFER_COUNT; 
				ui32SendSize += m_pLogModuleSocket->SendPacketMessage( m_pLogControlMessage[ui32ReadIdx]._szMessage, m_pLogControlMessage[ui32ReadIdx]._Size ); 
				m_i32LogControl_ReadPos++; 
				ui32SendCompleteCount++;
			}
		}
		bIsSend = TRUE; 
	}

	// Auth Thread - Trans
	i32SendCount = m_i32LogAuth_WritePos - m_i32LogAuth_ReadPos; 
	if(i32SendCount > 0)
	{
		for(INT32 i = 0; i < i32SendCount; i++)
		{
			if(ui32SendCompleteCount < MODULE_CONTEXTB_SEND_COUNT)
			{
				ui32ReadIdx = m_i32LogAuth_ReadPos % LOG_SEND_BUFFER_COUNT; 
				ui32SendSize += m_pLogModuleSocket->SendPacketMessage( m_pLogAuthMessage[ui32ReadIdx]._szMessage, m_pLogAuthMessage[ui32ReadIdx]._Size ); 
				m_i32LogAuth_ReadPos++; 
				ui32SendCompleteCount++; 
			}
		}
		bIsSend = TRUE; 
	}

	// Clan Thread
	i32SendCount = m_i32LogClan_WritePos - m_i32LogClan_ReadPos; 
	if(i32SendCount > 0)
	{
		for(INT32 i = 0; i < i32SendCount; i++)
		{
			if(ui32SendCompleteCount < MODULE_CONTEXTB_SEND_COUNT)
			{
				ui32ReadIdx = m_i32LogClan_ReadPos % LOG_SEND_BUFFER_COUNT; 
				ui32SendSize += m_pLogModuleSocket->SendPacketMessage( m_pLogClanMessage[ui32ReadIdx]._szMessage, m_pLogClanMessage[ui32ReadIdx]._Size ); 
				m_i32LogClan_ReadPos++; 
				ui32SendCompleteCount++; 
			}
		}
		bIsSend = TRUE; 
	}

	// Message Thread
	i32SendCount = m_i32LogMess_WritePos - m_i32LogMess_ReadPos; 
	if(i32SendCount > 0)
	{
		for(INT32 i = 0; i < i32SendCount; i++)
		{
			if(ui32SendCompleteCount < MODULE_CONTEXTB_SEND_COUNT)
			{
				ui32ReadIdx = m_i32LogMess_ReadPos % LOG_SEND_BUFFER_COUNT; 
				ui32SendSize += m_pLogModuleSocket->SendPacketMessage( m_pLogMessMessage[ui32ReadIdx]._szMessage, m_pLogMessMessage[ui32ReadIdx]._Size ); 
				m_i32LogMess_ReadPos++; 
				ui32SendCompleteCount++; 
			}
		}
		bIsSend = TRUE; 
	}


	//User Thread 
	for(UINT32 i = 0; i < m_ui32ThreadCount; i++)
	{
		LOG_SEND_MESSAGE * pLogBuffer = m_ppLogUserMessage[i];
		i32SendCount = m_pLogUser_WritePos[i] - m_pLogUser_ReadPos[i];

		if(i32SendCount > 0)
		{
			for(INT32 j = 0; j < i32SendCount; j++)
			{
				if(ui32SendCompleteCount < MODULE_CONTEXTB_SEND_COUNT)
				{
					ui32ReadIdx = m_pLogUser_ReadPos[i] % LOG_SEND_BUFFER_COUNT; 
					ui32SendSize += m_pLogModuleSocket->SendPacketMessage( pLogBuffer[ui32ReadIdx]._szMessage, pLogBuffer[ui32ReadIdx]._Size );
					m_pLogUser_ReadPos[i]++; 
					ui32SendCompleteCount++; 
				}
			}
			bIsSend = TRUE; 
		}
	}

	m_pLogModuleSocket->SelectEvent();  

	if( (bIsSend == TRUE) && (ui32SendSize ==  0) )
	{
		ui8SendFailCount++;
		if(ui8SendFailCount > 1)
		{
			m_pLogModuleSocket->OnDestroy(); 
			if( m_pLogModuleSocket->OnConnect(g_pContextMain->m_strLogDServerIp, g_pContextMain->m_ui16LogDServerPort ) )
			{
				g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"SUCESS LOG_MODULE RECONNECT 2", WARNING_NETWORK ); 
				ui8SendFailCount = 0; 
			}
			else
			{
				m_pLogModuleSocket->OnDestroy(); 
				g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"FAIL LOG_MODULE RECONNECT 2", WARNING_NETWORK); 
			}
		}
	}

	return ui32SendCompleteCount; 
}

void CModuleContextNC::_LogDestroy(void)
{	
	for(UINT32 i = 0; i < m_ui32ThreadCount; i++)
	{
		I3MEM_SAFE_FREE( m_ppLogUserMessage[i] );
		if( NULL != m_ppstrUserTempBuffer[i] ) delete [] m_ppstrUserTempBuffer[i] ;
	}
	I3MEM_SAFE_FREE( m_ppLogUserMessage ); 
	if( NULL != m_ppstrUserTempBuffer ) delete [] m_ppstrUserTempBuffer ;
	I3MEM_SAFE_FREE( m_pLogUser_WritePos); 
	I3MEM_SAFE_FREE( m_pLogUser_ReadPos ); 
	I3MEM_SAFE_FREE( m_pui32LogUser_Size );
	I3MEM_SAFE_FREE( m_pLogMainMessage ); 
	I3MEM_SAFE_FREE( m_pLogControlMessage ); 
	I3MEM_SAFE_FREE( m_pLogAuthMessage ); 
	I3MEM_SAFE_FREE( m_pLogClanMessage );
	I3MEM_SAFE_FREE( m_pLogMessMessage );
	I3_SAFE_RELEASE( m_pLogModuleSocket ); 
	
	return; 
}

enum ZLOG_INSERT_TYPE
{
	ZLOG_LDATA01,	ZLOG_LDATA02,	ZLOG_LDATA03,
	ZLOG_IDATA01,	ZLOG_IDATA02,	ZLOG_IDATA03,
	ZLOG_IDATA04,	ZLOG_IDATA05,	ZLOG_IDATA06,
	ZLOG_IDATA07,	ZLOG_IDATA08,	ZLOG_IDATA09,
	ZLOG_IDATA10,	ZLOG_IDATA11,	ZLOG_IDATA12,
	ZLOG_IDATA13,	ZLOG_IDATA14,	ZLOG_IDATA15,
	ZLOG_IDATA16,	ZLOG_IDATA17,
	ZLOG_SDATA01,	ZLOG_SDATA02,	ZLOG_SDATA03,
	ZLOG_SDATA04,	ZLOG_SDATA05,	ZLOG_SDATA06,
	ZLOG_SDATA07,	ZLOG_SDATA08,	ZLOG_SDATA09,
	ZLOG_SDATA10,
};

#ifdef I3_DEBUG
#define ASSERT_COMP( expr )  { char ui32Temp[ (expr) ? 1 : 0 ]; ui32Temp;	} 
#else
#define ASSERT_COMP( expr )  (void)0
#endif

// ID, ID의 스트링 사이즈
#define START_DATA( ID, size )			i3mem::Copy( &strSendBuffer[i16LogSize], "&,", 2 );				i16LogSize += 2;\
										i3mem::Copy( &strSendBuffer[i16LogSize], ID, size );			i16LogSize+= size;\
										strSendBuffer[NEW_WHERE_DATA_IDX]	= 'z';		strSendBuffer[NEW_WHERE_STRING_IDX]	= 'z';
// Insert Type, 값, String 일때 size
#define ADD_LDATA( datatype, values )	ASSERT_COMP( (ZLOG_LDATA01<= datatype) && (ZLOG_LDATA03 >= datatype) );\
										_i64toa(values, strtoa, 10);	i32StrLenth = i3String::Length(strtoa);\
										i3mem::Copy( &strSendBuffer[i16LogSize], "&,", 2 );				i16LogSize += 2;\
										i3mem::Copy( &strSendBuffer[i16LogSize], strtoa, i32StrLenth );	i16LogSize += (INT16)i32StrLenth;	i32DataIdx++;
#define ADD_IDATA( datatype, values )	ASSERT_COMP( (ZLOG_IDATA01<= datatype) && (ZLOG_IDATA17 >= datatype) );\
										_itoa(values, strtoa, 10);	i32StrLenth = i3String::Length(strtoa);\
										if( ZLOG_IDATA01==datatype )	strSendBuffer[NEW_WHERE_DATA_IDX]	= ((char)i32DataIdx) + 'a';\
										i3mem::Copy( &strSendBuffer[i16LogSize], "&,", 2 );				i16LogSize += 2;\
										i3mem::Copy( &strSendBuffer[i16LogSize], strtoa, i32StrLenth );	i16LogSize += (INT16)i32StrLenth;	i32DataIdx++;
#define ADD_SDATA( datatype, values )	ASSERT_COMP( (ZLOG_SDATA01<= datatype) && (ZLOG_SDATA10 >= datatype) );\
										i32StrLenth = i3String::Length(values);\
										if( ZLOG_SDATA01==datatype )	strSendBuffer[NEW_WHERE_STRING_IDX]	= ((char)i32DataIdx) + 'a';\
										i3mem::Copy( &strSendBuffer[i16LogSize], "&,", 2 );				i16LogSize += 2;\
										i3mem::Copy( &strSendBuffer[i16LogSize], values, i32StrLenth );	i16LogSize += (INT16)i32StrLenth;
#define END_DATA()						i3mem::Copy( &strSendBuffer[i16LogSize], "\r\n\0", 3 );			i16LogSize += 2;

void CModuleContextNC::LogSendMessage( INT32 ThreadIdx, LOG_MESSAGE LogId, CUserSession * pSession, void* pArg0, void* pArg1, void* pArg2, void* pArg3 )
{
	if( FALSE == m_bZLogRun ) return;

	LOG_CHECKVALID_IDX;

	//2. Set Buffer
	INT16 i16LogSize = NEW_STRING_START_IDX;
	char strtoa[32];	
	INT32 i32StrLenth;	
	INT32 i32DataIdx	= 2;
	char strSendBuffer[ 1024 ];
	i3String::Format( &strSendBuffer[i16LogSize], 1024 - i16LogSize, "%02d/%02d/%04d %02d:%02d:%02d.00", g_pContextMain->m_i32Month, g_pContextMain->m_i32Day, g_pContextMain->m_i32Year, g_pContextMain->m_i32Hour, g_pContextMain->m_i32Minute, g_pContextMain->m_i32Second ); 
	i16LogSize += 22;

	switch(LogId)
	{
	case LOG_MESSAGE_U_R_KICK_LEAVE_ROOM:	//강제 종료
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return; // 1 일경우 유저 강퇴 2 일경우 GM 강퇴

			INT32 i32KickType = *(UINT8*)pArg0;
			START_DATA( "100", 3 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_i32ChannelNum );
			ADD_IDATA( ZLOG_IDATA02, pSession->m_i32RoomIdx );
			ADD_IDATA( ZLOG_IDATA03, i32KickType );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_LOGIN		:		//로그 인
		{
			if( NULL == pSession )	return;
			
			UINT32 ui32Clan = 0;
			if (0 < pSession->m_UserInfoBasic._clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < pSession->m_UserInfoBasic._clanstate)		ui32Clan = pSession->m_UserInfoBasic._clanidx;

			char strNick[ NET_NICK_NAME_SIZE ];
			LogChangeString( strNick, pSession->m_UserInfoBasic._nick, NET_NICK_NAME_SIZE );
			char strID[ NET_ID_SIZE ];
			LogChangeString( strID, pSession->m_strId, NET_ID_SIZE );

			START_DATA( "1001", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic._point );
			ADD_IDATA( ZLOG_IDATA03, pSession->m_UserInfoBasic._cash );
			ADD_IDATA( ZLOG_IDATA04, pSession->m_UserInfoBasic._exp );
			ADD_IDATA( ZLOG_IDATA05, pSession->m_UserInfoBasic._pcCafe );
			ADD_IDATA( ZLOG_IDATA06, ui32Clan );
			ADD_IDATA( ZLOG_IDATA07, pSession->m_ui32AccConnectTime );
			ADD_IDATA( ZLOG_IDATA08, pSession->m_ui32LastLoginTime );
			ADD_SDATA( ZLOG_SDATA01, strNick );
			ADD_SDATA( ZLOG_SDATA02, strID );
			ADD_SDATA( ZLOG_SDATA03, pSession->GetIpString() );
			END_DATA();
		}
		break; 
	case LOG_MESSAGE_U_LOGOUT		:		//로그 아웃
		{
			if( NULL == pSession )					return;
			if( NULL == pArg0 || NULL == pArg1 )	return;

			INT32 i32ChannelNum = (INT32)*((INT32*)pArg0);
			INT32 i32RoomIdx	= (INT32)*((INT32*)pArg1);
			
			char strNick[ NET_NICK_NAME_SIZE ];
			LogChangeString( strNick, pSession->m_UserInfoBasic._nick, NET_NICK_NAME_SIZE );
			char strID[ NET_ID_SIZE ];
			LogChangeString( strID, pSession->m_strId, NET_ID_SIZE );

			char strTRecord[SERVER_STRING_COUNT];
			char strSRecord[SERVER_STRING_COUNT];
			char strLoginTime[SERVER_STRING_COUNT];
						
			i3String::Format( strTRecord, SERVER_STRING_COUNT, "%d/%d/%d/%d/%d/%d/%d", pSession->m_UserInfoRecord._match, pSession->m_UserInfoRecord._win, pSession->m_UserInfoRecord._lose, pSession->m_UserInfoRecord._draw, pSession->m_UserInfoRecord._killcount, pSession->m_UserInfoRecord._death, pSession->m_UserInfoRecord._headshot);
			i3String::Format( strSRecord, SERVER_STRING_COUNT, "%d/%d/%d/%d/%d/%d/%d", pSession->m_UserInfoRecord._smatch, pSession->m_UserInfoRecord._swin, pSession->m_UserInfoRecord._slose, pSession->m_UserInfoRecord._sdraw, pSession->m_UserInfoRecord._skillcount, pSession->m_UserInfoRecord._sdeath, pSession->m_UserInfoRecord._sheadshot ); 
			ConverTimeFromtUINT64ToSTR(strLoginTime, pSession->m_ui64LoginTime);			

			UINT32 ui32TSTime = _GetDiffTime( pSession->m_ui64LoginTime, g_pContextMain->GetDateTime64() );

			UINT32 ui32Clan = 0;
			if (0 < pSession->m_UserInfoBasic._clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < pSession->m_UserInfoBasic._clanstate)		ui32Clan = pSession->m_UserInfoBasic._clanidx;

			START_DATA( "1002", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic._point );
			ADD_IDATA( ZLOG_IDATA03, pSession->m_UserInfoBasic._cash );
			ADD_IDATA( ZLOG_IDATA04, pSession->m_UserInfoBasic._exp );
			ADD_IDATA( ZLOG_IDATA05, pSession->m_UserInfoBasic._pcCafe );
			ADD_IDATA( ZLOG_IDATA06, ui32Clan );
			ADD_IDATA( ZLOG_IDATA07, ui32TSTime );
			ADD_IDATA( ZLOG_IDATA08, pSession->m_ui32LoginBattleTime );
			ADD_IDATA( ZLOG_IDATA09, i32ChannelNum );
			ADD_IDATA( ZLOG_IDATA10, i32RoomIdx );
			ADD_IDATA( ZLOG_IDATA11, pSession->m_ui32KillReason );
			ADD_SDATA( ZLOG_SDATA01, strNick );
			ADD_SDATA( ZLOG_SDATA02, strID );
			ADD_SDATA( ZLOG_SDATA03, pSession->GetIpString() );
			ADD_SDATA( ZLOG_SDATA04, strTRecord );
			ADD_SDATA( ZLOG_SDATA05, strSRecord );
			ADD_SDATA( ZLOG_SDATA06, strLoginTime );
			END_DATA();
		}
		break;	
	case LOG_MESSAGE_U_CREATE_NICK		:	//캐릭터 생성 
		{
			if( NULL == pSession )	return;

			char strNick[ NET_NICK_NAME_SIZE ];
			LogChangeString( strNick, pSession->m_UserInfoBasic._nick, NET_NICK_NAME_SIZE );

			START_DATA( "1003", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, 1 );
			ADD_SDATA( ZLOG_SDATA01, strNick );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CHANGE_NICK		:	// 케릭터 이름 변경
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;

			char strNick[ NET_NICK_NAME_SIZE ];
			LogChangeString( strNick, pSession->m_UserInfoBasic._nick, NET_NICK_NAME_SIZE );

			char strOldNick[ NET_NICK_NAME_SIZE ];
			LogChangeString( strOldNick, (char*)pArg0, NET_NICK_NAME_SIZE );

			START_DATA( "1003", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, 2 );
			ADD_SDATA( ZLOG_SDATA01, strNick );
			ADD_SDATA( ZLOG_SDATA02, strOldNick );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_LEVEL_UP			:	//캐릭터 레벨 업
		{
			if( NULL == pSession )	return;

			START_DATA( "1004", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA02, pSession->m_ui32AccConnectCount );
			ADD_IDATA( ZLOG_IDATA03, pSession->m_UserInfoBasic._ui32TotalBattleTime );
			END_DATA();
		}
		break; 
	case LOG_MESSAGE_U_GET_POINT		:	// 포인트 획득
		{
			if( NULL == pSession )					return;
			if( NULL == pArg0 || NULL == pArg1 )	return;
			INT32 i32GetPoint  = (INT32)*((INT32*)pArg0);
			INT32 i32GetRoute	= (INT32)*((INT32*)pArg1);

			START_DATA( "1101", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic._point );
			ADD_IDATA( ZLOG_IDATA03, i32GetPoint );
			ADD_IDATA( ZLOG_IDATA04, i32GetRoute );
			END_DATA();
		}
		break;
		////////////////////////////////////////////////////////////////////////////////////////////////////////
	case LOG_MESSAGE_U_SHOP_BUY			:	//구입
		{			
			// 포인트 소진으로 통합 - 서동권
			// 수리  에서 기록합니다. 이부분은 구매에 관한 부분

			if( NULL == pSession )									return;
			if( NULL == pArg0 || NULL == pArg1 || NULL == pArg2 )	return;
			UINT32	ui32Point	= 0;
			UINT32	ui32Cash	= 0;

			UINT32	ui32GameGoodsId = (UINT32)*((UINT32*)pArg0);
			UINT8	ui8BuyType		= (UINT8)*((UINT8*)pArg1);
			UINT8	ui8SpendType	= (UINT8)*((UINT8*)pArg2);			

			UINT32 ui32Idx = g_pContextMain->GetShop()->GetFindGoodsIdx( ui32GameGoodsId );
			if ( 0xFFFFFFFF != ui32Idx )
			{	// 검색을 한번만 하기 위해 GetFindGoodsIdx를 통해 Idx를 받아온후 GetGoods, GetGoodsEx 를 호출했습니다.
				SHOP_GOODS_BASE*		pGoods		= g_pContextMain->GetShop()->GetGoods( ui32Idx );
				ui32Point		= pGoods->_ui32Point;
				ui32Cash		= pGoods->_ui32Cash;
			}

			switch( ui8BuyType )
			{
			case GOODS_BUY_POINT :
				{
					START_DATA( "1102", 4 );
					ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
					ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
					ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic._point );
					ADD_IDATA( ZLOG_IDATA03, ui32Point );
					ADD_IDATA( ZLOG_IDATA04, ui8SpendType );
					ADD_IDATA( ZLOG_IDATA05, ui32GameGoodsId );
					ADD_IDATA( ZLOG_IDATA06, 1 );
					END_DATA();
				}
				break;
			case GOODS_BUY_CASH :
				{
					UINT32 ui32Clan = 0;
					if (0 < pSession->m_UserInfoBasic._clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < pSession->m_UserInfoBasic._clanstate)		ui32Clan = pSession->m_UserInfoBasic._clanidx;

					START_DATA( "1103", 4 );
					ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
					ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
					ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic._cash );
					ADD_IDATA( ZLOG_IDATA03, ui32Cash );
					ADD_IDATA( ZLOG_IDATA04, ui8SpendType );
					ADD_IDATA( ZLOG_IDATA05, ui32GameGoodsId );
					ADD_IDATA( ZLOG_IDATA06, 1 );
					ADD_IDATA( ZLOG_IDATA07, pSession->m_UserInfoBasic._ui32LastBuyCash );
					ADD_IDATA( ZLOG_IDATA08, ui32Clan );
					END_DATA();
				}
				break;
			case GOODS_BUY_RS :
				{
					UINT32	ui32PrizeGoodsId = (UINT32)*((UINT32*)pArg3);	// 뽑기 상점 당첨 상품
					UINT32 ui32Clan = 0;

					if (0 < pSession->m_UserInfoBasic._clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < pSession->m_UserInfoBasic._clanstate)		ui32Clan = pSession->m_UserInfoBasic._clanidx;

					START_DATA( "1104", 4 );
					ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
					ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
					ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic._cash );
					ADD_IDATA( ZLOG_IDATA03, ui32Cash );
					ADD_IDATA( ZLOG_IDATA04, ui8SpendType );
					UINT32 ui32TabIdx = GET_GOODS_FLAG_RS_TAB_IDX ( ui32GameGoodsId ); 
					ADD_IDATA( ZLOG_IDATA05, ui32TabIdx );		// 탭 번호.
					ADD_IDATA( ZLOG_IDATA06, 1 );
					ADD_IDATA( ZLOG_IDATA07, pSession->m_UserInfoBasic._ui32LastBuyCash );
					ADD_IDATA( ZLOG_IDATA08, ui32Clan );
					ADD_IDATA( ZLOG_IDATA09, 0 ); // 임시. 추후에 할인쿠폰 정보용으로 사용.
					ADD_IDATA( ZLOG_IDATA10, ui32PrizeGoodsId );	// 
					END_DATA();
 				}
				break;
			default:
				{
					char strErrorLog[ MAX_PATH ];
					i3String::Format( strErrorLog, MAX_PATH, "ZLog Error LOG_MESSAGE_U_SHOP_BUY, %d", ui8BuyType );
					g_pModuleLogFile->Write_M_Log( ThreadIdx, pSession, strErrorLog );
					return;
				}
				break;
			}
		}
		break;
	case LOG_MESSAGE_U_REPAIR				: // 수리
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;

			REPAIR_INFO* stRepair = ( REPAIR_INFO* )pArg0;

			UINT8 ui8ItemType = GET_ITEM_FLAG_CLASS( stRepair->m_ui32ItemID );

			if( stRepair->m_ui32Point > 0 )
			{
				START_DATA( "1102", 4 );
				ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
				ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
				ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic._point );
				ADD_IDATA( ZLOG_IDATA03, stRepair->m_ui32Point );
				ADD_IDATA( ZLOG_IDATA04, 4 );
				ADD_IDATA( ZLOG_IDATA05, stRepair->m_ui32ItemID );
				ADD_IDATA( ZLOG_IDATA06, stRepair->m_ui32Arg );
				END_DATA();
			}
			else if( stRepair->m_ui32Cash > 0 )
			{
				UINT32 ui32Clan = 0;
				if (0 < pSession->m_UserInfoBasic._clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < pSession->m_UserInfoBasic._clanstate)		ui32Clan = pSession->m_UserInfoBasic._clanidx;

				START_DATA( "1103", 4 );
				ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
				ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
				ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic._cash );
				ADD_IDATA( ZLOG_IDATA03, stRepair->m_ui32Cash );
				ADD_IDATA( ZLOG_IDATA04, 4 );
				ADD_IDATA( ZLOG_IDATA05, stRepair->m_ui32ItemID );
				ADD_IDATA( ZLOG_IDATA06, stRepair->m_ui32Arg );
				ADD_IDATA( ZLOG_IDATA07, pSession->m_UserInfoBasic._ui32LastBuyCash );
				ADD_IDATA( ZLOG_IDATA08, ui32Clan );
				END_DATA();
			}
			else
			{
				char strErrorLog[ MAX_PATH ];
				i3String::Format( strErrorLog, MAX_PATH, "ZLog Error LOG_MESSAGE_U_REPAIR, %d, %d, %d", stRepair->m_ui32ItemID, stRepair->m_ui32Point, stRepair->m_ui32Cash );
				g_pModuleLogFile->Write_M_Log( ThreadIdx, pSession, strErrorLog );
				return;
			}
		}
		break;
	case LOG_MESSAGE_U_INVEN_USE		:	//사용
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;

			UINT32	ui32GameItemId	= (UINT32)*((UINT32*)pArg0);
			INT32	i32NowCount		= 1;
			char	strExpireDate[ 16 ];
			char	strLogDItemName[MAX_ITEM_NAME];
			strExpireDate[0]	= '\0';
			strLogDItemName[0]	= '\0';

			
			START_DATA( "1201", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, ui32GameItemId );
			
			if( pArg1 )
			{	// pArg1 이 NULL 이 아니면 아이템인증입니다.
				INVEN_BUFFER* pInvenItem = (INVEN_BUFFER*)pArg1;
				ADD_IDATA( ZLOG_IDATA02, pInvenItem->_ui32ItemArg );
			}
			else if( pArg2 )
			{	// 이 경우 캡슐 아이템입니다.
				INT32 i32Capsule = (UINT8)*(UINT8*)(pArg2);
				ADD_IDATA( ZLOG_IDATA02, 0 );
				ADD_IDATA( ZLOG_IDATA03, i32Capsule );
			}
			END_DATA();
		}
		break; 
	case LOG_MESSAGE_U_GET_ITEM			:
		{
			if( NULL == pSession )	return;

			if( NULL == pArg0 || NULL == pArg1 || NULL == pArg2 ) return;

			INVEN_BUFFER* pItem		= (INVEN_BUFFER*)pArg0;
			INT8	i8RequestType	= (INT8)*((INT8*)pArg1);
			UINT32	ui32InsertArg	= (UINT32)*((UINT32*)pArg2);

			START_DATA( "1202", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA02, i8RequestType );
			ADD_IDATA( ZLOG_IDATA03, pItem->_ui32ItemID );
			ADD_IDATA( ZLOG_IDATA04, ui32InsertArg );
			ADD_IDATA( ZLOG_IDATA05, pItem->_ui32ItemArg );			
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_ITEM_DELETE		:
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;

			UINT32 ui32ItemID = (UINT32)*((UINT32*)pArg0);
			START_DATA( "1203", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, ui32ItemID );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CREATE_CLAN		: // 클랜 생성
		{
			if( NULL == pSession )	return;

			if( NULL == pArg0)
			{
				START_DATA( "1102", 4 );
				ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
				ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
				ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic._point );
				ADD_IDATA( ZLOG_IDATA03, g_pContextMain->m_i32ClanCreatePoint );
				ADD_IDATA( ZLOG_IDATA04, 3 );
				END_DATA();
			}
			else
			{
				START_DATA( "1301", 4 );
				ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
				ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._clanidx );
				END_DATA();
			}
		}
		break;
	case LOG_MESSAGE_U_CLAN_DELETE		: // 클랜 해체
		{
			if( NULL == pSession )										return;
			if( NULL == pArg0 || NULL == pArg1 || NULL == pArg2 )		return;

			UINT8	ui8ClanRank = (UINT8)*((UINT8*)pArg0);
			UINT32	ui32ClanExp = (UINT32)*((UINT32*)pArg1);
			INT32	i32ClanPoint = (INT32)*((REAL32*)pArg2);

			char strClanName[ NET_CLAN_NAME_SIZE ];
			LogChangeString( strClanName, pSession->m_UserInfoBasic._clanName, NET_CLAN_NAME_SIZE );

			START_DATA( "1302", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._clanidx );
			ADD_IDATA( ZLOG_IDATA02, ui8ClanRank );
			ADD_IDATA( ZLOG_IDATA03, ui32ClanExp );
			ADD_IDATA( ZLOG_IDATA04, i32ClanPoint );
			ADD_SDATA( ZLOG_SDATA01, strClanName );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CLAN_JOIN		: // 클랜 가입
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			TUID i64ClanMemberUID = (INT64)*((INT64*)pArg0);

			START_DATA( "1303", 4 );
			ADD_LDATA( ZLOG_LDATA01, i64ClanMemberUID );
			ADD_LDATA( ZLOG_LDATA02, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._clanidx );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CLAN_SECESSION			: // 클랜 탈퇴
		{
			if( NULL == pSession )		return;

			START_DATA( "1304", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._clanidx );
			ADD_IDATA( ZLOG_IDATA02, 1 );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CLAN_DEPORTATION			: // 클랜 제명
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			TUID i64MemberUID = (TUID)*((TUID*)pArg0);

			START_DATA( "1304", 4 );
			ADD_LDATA( ZLOG_LDATA01, i64MemberUID );
			ADD_LDATA( ZLOG_LDATA02, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._clanidx );
			ADD_IDATA( ZLOG_IDATA02, 2 );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CLAN_JOIN_REQ	: // 클랜 가입 요청
		{
			if( NULL == pSession )	return;

			START_DATA( "1305", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._clanidx );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_R_BATTLE_END_ROOM	: // 방 - 배틀 종료 
		{
			if( NULL == pArg0 )		return;
			CRoom* pRoom = (CRoom*)pArg0;

			INT32	ai32TeamCount[ TEAM_COUNT ];
			INT32	ai32MissionCount[ TEAM_COUNT ];
			
			i3mem::FillZero( ai32TeamCount, sizeof(INT32)*TEAM_COUNT );
			i3mem::FillZero( ai32MissionCount, sizeof(INT32)*TEAM_COUNT );

			TYPE_ROOM_INFO* pInfo = pRoom->m_pRoomMode->GetTypeRoomInfo();

			INT32 i32Team;
			for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
			{
				if( SLOT_STATE_BATTLE != pInfo->_SlotInfo[i]._State ) continue;

				i32Team = i % TEAM_COUNT;

				ai32TeamCount[i32Team]		+= 1;	
				ai32MissionCount[i32Team]	+= pInfo->_aui8AccMissionCount[i];
			}

			char	strMission[SERVER_STRING_COUNT];
			i3String::Format( strMission, SERVER_STRING_COUNT, "RED:%d/BLU:%d", ai32MissionCount[TEAM_RED], ai32MissionCount[TEAM_BLUE] );
			
			char	strRecord[ SERVER_STRING_COUNT ];
			i3String::Format( strRecord, SERVER_STRING_COUNT, "RED:%d_%d/BLU:%d_%d", pInfo->_aTeamScoreTotal[TEAM_RED].m_KillCount, pInfo->_aTeamScoreTotal[TEAM_RED].m_DeathCount, 
							pInfo->_aTeamScoreTotal[TEAM_BLUE].m_KillCount, pInfo->_aTeamScoreTotal[TEAM_BLUE].m_DeathCount );

			char	strPlayerCount[ SERVER_STRING_COUNT ];
			i3String::Format( strPlayerCount, SERVER_STRING_COUNT, "RED:%d/BLU:%d", ai32TeamCount[TEAM_RED], ai32TeamCount[TEAM_BLUE] );
			UINT32 ui32PlayTime = 
#ifdef USE_GM_PAUSE
									pRoom->GetRoomBattleTime();
#else
									pRoom->m_Info._ui32BattleEndTime - pRoom->m_Info._ui32GameStartTime;
#endif

			START_DATA( "1401", 4 );
			ADD_IDATA( ZLOG_IDATA01, pRoom->m_ui32LogUniqueNumber );
			ADD_IDATA( ZLOG_IDATA02, GET_STAGEORD(pRoom->m_Info._InfoBasic._StageID) );
			ADD_IDATA( ZLOG_IDATA03, pRoom->m_i8WinTeam );
			ADD_IDATA( ZLOG_IDATA04, pRoom->m_Info._i32StageType );
			ADD_IDATA( ZLOG_IDATA05, pRoom->m_Info._InfoBasic._StageMask );
			ADD_IDATA( ZLOG_IDATA06, pRoom->m_i8IsClanMatch );
			ADD_IDATA( ZLOG_IDATA07, pRoom->m_Info._ui32GameStartDate );
			ADD_IDATA( ZLOG_IDATA08, ui32PlayTime );			
			ADD_IDATA( ZLOG_IDATA09, pRoom->m_Info._ui8MiEndType );
			ADD_IDATA( ZLOG_IDATA10, pInfo->_InfoAdd._SubType );
			ADD_SDATA( ZLOG_SDATA01, strMission );
			ADD_SDATA( ZLOG_SDATA02, strRecord );
			ADD_SDATA( ZLOG_SDATA03, strPlayerCount );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CLAN_MATCHING	: // 클랜 매칭
		{
			if( NULL == pArg0 || NULL == pArg1 )		return;

			CRoom* pRoom					= (CRoom*)pArg0;
			CLAN_RESULT*	pClanResult		= (CLAN_RESULT*)pArg1;

			INT32 i32Win;
			INT32 i32Los;
			INT32 i32Draw;

			if( pClanResult[0]._Win == 1 )
			{
				i32Win	= 0;
				i32Los	= 1;
				i32Draw = 1;
			}
			else if( pClanResult[1]._Win == 1 )
			{
				i32Win	= 1;
				i32Los	= 0;
				i32Draw	= 2;
			}
			else // 무승부일 경우
			{
				i32Win	= 0;
				i32Los	= 1;
				i32Draw = 0;
			}

			START_DATA( "1402", 4 );
			ADD_IDATA( ZLOG_IDATA01, pRoom->m_ui32LogUniqueNumber );
			ADD_IDATA( ZLOG_IDATA02, GET_STAGEORD(pRoom->m_Info._InfoBasic._StageID) );
			ADD_IDATA( ZLOG_IDATA03, pClanResult[ i32Win ]._ClanIdx );
			ADD_IDATA( ZLOG_IDATA04, pClanResult[ i32Los ]._ClanIdx );
			ADD_IDATA( ZLOG_IDATA05, i32Draw );
			ADD_IDATA( ZLOG_IDATA06, pClanResult[ i32Win ]._Exp );
			ADD_IDATA( ZLOG_IDATA07, pClanResult[ i32Los ]._Exp );
			END_DATA();

			UINT32 ui32MaxClanUID = 999999;
#ifdef LOCALE_BRAZIL
			ui32MaxClanUID = 184131;
#endif
#ifdef LOCALE_INDONESIA
			ui32MaxClanUID = 636001;
#endif
#ifdef LOCALE_ITALY
			ui32MaxClanUID = 24845;
#endif
#ifdef LOCALE_LATIN_AMERICA
			ui32MaxClanUID = 84896;
#endif
#ifdef LOCALE_MIDDLE_EAST
			ui32MaxClanUID = 8920;
#endif
#ifdef LOCALE_NORTHAMERICA
			ui32MaxClanUID = 20415;
#endif
#ifdef LOCALE_PHILIPPINES
			ui32MaxClanUID = 118535;
#endif
#ifdef LOCALE_RUSSIA
			ui32MaxClanUID = 159655;
#endif
#ifdef LOCALE_TAIWAN
			ui32MaxClanUID = 2787;
#endif
#ifdef LOCALE_THAILAND
			ui32MaxClanUID = 165302;
#endif
#ifdef LOCALE_TURKEY
			ui32MaxClanUID = 164780;
#endif
			ui32MaxClanUID = (UINT32)(ui32MaxClanUID*1.4f);
			if ( pClanResult[ i32Win ]._ClanIdx > ui32MaxClanUID ||
				pClanResult[ i32Los ]._ClanIdx > ui32MaxClanUID
				)
			{
				char szMsg[MAX_PATH];
				i3String::Format( szMsg, MAX_PATH, "ZLOG1402_VALUE_ERROR [0]ClanIdx:%u Exp:%u Los:%u Match:%u Win:%u, [1]ClanIdx:%u Exp:%u Los:%u Match:%u Win:%u, i32Win:%d i32Los:%d",
					pClanResult[0]._ClanIdx,
					pClanResult[0]._Exp,
					pClanResult[0]._Lose,
					pClanResult[0]._Match,
					pClanResult[0]._Win,
					pClanResult[1]._ClanIdx,
					pClanResult[1]._Exp,
					pClanResult[1]._Lose,
					pClanResult[1]._Match,
					pClanResult[1]._Win,
					i32Win,
					i32Los
				);
				g_pModuleLogFile->Write_M_Log( MODULE_NC_THREAD_IDX, NULL, szMsg );
			}
		}
		break;
	case LOG_MESSAGE_U_R_BATTLE_START		: //배틀 시작 (각자)
		{
			if( NULL == pSession )			return;
			if( NULL == pArg0 )				return;
			CRoom* pRoom		= (CRoom*)pArg0;

			// 난입 유저 체크
			INT32 i32Intrusion	= 1;
			if( (pRoom->m_Info._ui32FirstBattleStartUser & (0x1 << pSession->m_i32SlotIdx)) )	i32Intrusion = 0;

			// 장착 호칭
			char	strUseTitle[SERVER_STRING_COUNT];
			i3String::Format( strUseTitle, SERVER_STRING_COUNT, "%d/%d/%d ", pSession->m_aui8EquipUserTitle[0], pSession->m_aui8EquipUserTitle[1], pSession->m_aui8EquipUserTitle[2] );

			// 방장유무
			INT32	i32MainUser;
			if( pSession->m_i32SlotIdx == pRoom->m_i32MainSlotIdx )	i32MainUser = 1; 
			else													i32MainUser = 0; 

			// 소속팀
			UINT8	ui8Teamidx = (pSession->m_i32SlotIdx % TEAM_COUNT)+1; 
			
			START_DATA( "1501", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pRoom->m_ui32LogUniqueNumber );
			ADD_IDATA( ZLOG_IDATA02, GET_STAGEORD(pRoom->m_Info._InfoBasic._StageID) );
			ADD_IDATA( ZLOG_IDATA03, ui8Teamidx );
			ADD_IDATA( ZLOG_IDATA04, pRoom->m_Info._i32StageType );
			ADD_IDATA( ZLOG_IDATA05, pRoom->m_Info._InfoBasic._StageMask );
			ADD_IDATA( ZLOG_IDATA06, pRoom->m_i8IsClanMatch );
			ADD_IDATA( ZLOG_IDATA07, i32Intrusion );
			ADD_IDATA( ZLOG_IDATA08, i32MainUser );
			ADD_IDATA( ZLOG_IDATA09, pSession->m_UserInfoEquipmentWeapon._noprim );
			ADD_IDATA( ZLOG_IDATA10, pSession->m_UserInfoEquipmentChara.GetCharaID((TEAM_TYPE)ui8Teamidx) );
			ADD_IDATA( ZLOG_IDATA11, pSession->m_UserInfoEquipmentChara.GetHairNum() );
			ADD_IDATA( ZLOG_IDATA12, pSession->m_UserInfoEquipmentChara.GetItemNum() );
			ADD_IDATA( ZLOG_IDATA13, pSession->m_UserInfoInvItemData._ui8Item );
			ADD_SDATA( ZLOG_SDATA01, strUseTitle );
			END_DATA();
		 }
		break; 

	case LOG_MESSAGE_U_R_BATTLE_END				: // 배틀 종료 (각자)
		{
			if( NULL == pSession )					return;
			if( NULL == pArg0 || NULL == pArg1 )	return;
			
			CRoom* pRoom		= (CRoom*)pArg0;
			INT32 i32EndType	= (INT32)*((INT32*)pArg1);

			// 난입 유저 체크
			INT32 i32Intrusion	= 1;
			if( (pRoom->m_Info._ui32FirstBattleStartUser & (0x1 << pSession->m_i32SlotIdx)) )	i32Intrusion = 0;
		
			// 소속팀
			UINT8	ui8Teamidx = (pSession->m_i32SlotIdx % TEAM_COUNT)+1; 
			// 장착 호칭
			char	strUseTitle[SERVER_STRING_COUNT];
			i3String::Format( strUseTitle, SERVER_STRING_COUNT, "%d/%d/%d ", pSession->m_aui8EquipUserTitle[0], pSession->m_aui8EquipUserTitle[1], pSession->m_aui8EquipUserTitle[2] );
			// Total 전적
			char strRocord[SERVER_STRING_COUNT];
			i3String::Format( strRocord, SERVER_STRING_COUNT, "%d/%d/%d/%d/%d/%d/%d", pSession->m_UserInfoRecord._match, pSession->m_UserInfoRecord._win, pSession->m_UserInfoRecord._lose, pSession->m_UserInfoRecord._draw, pSession->m_UserInfoRecord._killcount, pSession->m_UserInfoRecord._death, pSession->m_UserInfoRecord._headshot ); 
			// 얻은 전적
			char strGetRocord[SERVER_STRING_COUNT];
			i3String::Format( strGetRocord, SERVER_STRING_COUNT, "%d/%d/%d", pRoom->m_Info._aM_UserScore[pSession->m_i32SlotIdx].m_KillCount, pRoom->m_Info._aM_UserScore[pSession->m_i32SlotIdx].m_DeathCount, pRoom->m_Info._aui16M_Headshot[pSession->m_i32SlotIdx] );
			// Point 상세
			char strPoint[SERVER_STRING_COUNT];
			i3String::Format( strPoint, SERVER_STRING_COUNT, "%d/%d/%d/%d", pRoom->m_Info._aui16AccPoint[pSession->m_i32SlotIdx], pRoom->m_aSlotBonus[pSession->m_i32SlotIdx].ui16PointItem, pRoom->m_aSlotBonus[pSession->m_i32SlotIdx].ui16PointPCCafe, pRoom->m_aSlotBonus[pSession->m_i32SlotIdx].ui16PointEvent );
			// Exp 상세
			char strExp[SERVER_STRING_COUNT];
			i3String::Format( strExp, SERVER_STRING_COUNT, "%d/%d/%d/%d", pRoom->m_Info._aui16AccExp[pSession->m_i32SlotIdx], pRoom->m_aSlotBonus[pSession->m_i32SlotIdx].ui16ExpItem, pRoom->m_aSlotBonus[pSession->m_i32SlotIdx].ui16ExpPCCafe, pRoom->m_aSlotBonus[pSession->m_i32SlotIdx].ui16ExpEvent );
			
			// 포인트 상세
			// 경험치 상세
			
			UINT32 ui32PlayTime = g_pContextMain->GetServerTime() - pRoom->m_Info._ui32GameStartTime;
			INT32 i32GetPoint	= pRoom->m_Info._aui16AccPoint[pSession->m_i32SlotIdx] + pRoom->m_Info._aui16EventPoint[pSession->m_i32SlotIdx];
			INT32 i32GetExp		= pRoom->m_Info._aui16AccExp[pSession->m_i32SlotIdx] + pRoom->m_Info._aui16EventExp[pSession->m_i32SlotIdx];

			START_DATA( "1502", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pRoom->m_ui32LogUniqueNumber );
			ADD_IDATA( ZLOG_IDATA02, GET_STAGEORD(pRoom->m_Info._InfoBasic._StageID) );
			ADD_IDATA( ZLOG_IDATA03, ui8Teamidx );
			ADD_IDATA( ZLOG_IDATA04, pRoom->m_Info._i32StageType );
			ADD_IDATA( ZLOG_IDATA05, pRoom->m_Info._InfoBasic._StageMask );
			ADD_IDATA( ZLOG_IDATA06, pRoom->m_i8IsClanMatch );
			ADD_IDATA( ZLOG_IDATA07, i32Intrusion );
			ADD_IDATA( ZLOG_IDATA08, pSession->m_ui32PlayTime );
			ADD_IDATA( ZLOG_IDATA09, pRoom->m_Info._ui8MiEndType );
			ADD_IDATA( ZLOG_IDATA10, i32EndType );
			ADD_IDATA( ZLOG_IDATA11, pSession->m_ui32RoomBattleCount );
			ADD_IDATA( ZLOG_IDATA12, pSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA13, pSession->m_UserInfoBasic._point );
			ADD_IDATA( ZLOG_IDATA14, pSession->m_UserInfoBasic._exp );
			ADD_IDATA( ZLOG_IDATA15, i32GetPoint );
			ADD_IDATA( ZLOG_IDATA16, i32GetExp );
			ADD_SDATA( ZLOG_SDATA01, strUseTitle );
			ADD_SDATA( ZLOG_SDATA02, strRocord );
			ADD_SDATA( ZLOG_SDATA03, strGetRocord );
			ADD_SDATA( ZLOG_SDATA04, strPoint );
			ADD_SDATA( ZLOG_SDATA05, strExp );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_R_DEATH:
		{
			if( NULL == pSession )									return;
			if( NULL == pArg0 || NULL == pArg1 || NULL == pArg2 )	return;
			CUserSession* pKillSession		= (CUserSession*)pArg0;
			CRoom* pRoom					= (CRoom*)pArg1;
			LOG_KILL_DEATH_INFO * pKillInfo	= (LOG_KILL_DEATH_INFO*)pArg2;

			// 소속팀
			UINT8	ui8KillTeamidx	= (pKillSession->m_i32SlotIdx % TEAM_COUNT)+1; 
			UINT8	ui8DeathTeamidx = (pSession->m_i32SlotIdx % TEAM_COUNT)+1; 
			// 진행 시간
			UINT32	ui32BattleTime	= 
#ifdef USE_GM_PAUSE
										pRoom->GetRoomBattleTime();
#else
										g_pContextMain->GetServerTime() - pRoom->m_Info._ui32GameStartTime;
#endif

			char strKillXYZ[64];
			char strDeathXYZ[64];

			i3String::Format( strKillXYZ, 64, "%d/%d/%d", (INT32)pKillInfo->r32Kill_X, (INT32)pKillInfo->r32Kill_Y, (INT32)pKillInfo->r32Kill_Z );
			i3String::Format( strDeathXYZ, 64, "%d/%d/%d", (INT32)pKillInfo->r32Death_X, (INT32)pKillInfo->r32Death_Y, (INT32)pKillInfo->r32Death_Z );

			START_DATA( "1503", 4 );
			ADD_LDATA( ZLOG_LDATA01, pKillSession->m_i64UID );
			ADD_LDATA( ZLOG_LDATA02, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pRoom->m_ui32LogUniqueNumber );
			ADD_IDATA( ZLOG_IDATA02, GET_STAGEORD(pRoom->m_Info._InfoBasic._StageID) );
			ADD_IDATA( ZLOG_IDATA03, pKillInfo->ui32WeaponNum );
			ADD_IDATA( ZLOG_IDATA04, pKillSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA05, pSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA06, ui8KillTeamidx );
			ADD_IDATA( ZLOG_IDATA07, ui8DeathTeamidx );
			ADD_IDATA( ZLOG_IDATA08, ui32BattleTime );
			ADD_IDATA( ZLOG_IDATA09, pKillInfo->m_ui32DeathType );
			ADD_SDATA( ZLOG_SDATA01, strKillXYZ );
			ADD_SDATA( ZLOG_SDATA02, strDeathXYZ );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_R_RESPAWN				: //리스폰 
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			CRoom* pRoom = (CRoom*)pArg0;

			UINT8  ui8CharType;

			if( pRoom->m_Info._MiDinoInfo.IsDinoIdx( pSession->m_i32SlotIdx ) )
			{
				ui8CharType = 3;
			}
			else
			{
				ui8CharType = (pSession->m_i32SlotIdx % TEAM_COUNT)+1;
			}

			START_DATA( "1504", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pRoom->m_ui32LogUniqueNumber );
			ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA03, pSession->m_i32LogD_RespawnCount );
			ADD_IDATA( ZLOG_IDATA04, pSession->m_aui32UseWeaponList[WEAPON_USAGE_PRIMARY] );
			ADD_IDATA( ZLOG_IDATA05, pSession->m_aui32UseWeaponList[WEAPON_USAGE_SECONDARY] );
			ADD_IDATA( ZLOG_IDATA06, pSession->m_aui32UseWeaponList[WEAPON_USAGE_MELEE] );
			ADD_IDATA( ZLOG_IDATA07, pSession->m_aui32UseWeaponList[WEAPON_USAGE_THROWING] );
			ADD_IDATA( ZLOG_IDATA08, pSession->m_aui32UseWeaponList[WEAPON_USAGE_ITEM] );
			ADD_IDATA( ZLOG_IDATA09, ui8CharType );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_R_ESCAPE				: // 탈출
		{
			if( NULL == pSession )					return;
			if( NULL == pArg0 || NULL == pArg1 )	return;

			UINT8 ui8TouchDownCount			= (UINT8)*((UINT8*)pArg0);
			UINT8 ui8TouchDownChainCount	= (UINT8)*((UINT8*)pArg1);

			START_DATA( "1505", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, ui8TouchDownCount );
			ADD_IDATA( ZLOG_IDATA02, ui8TouchDownChainCount );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_TITLE_BUY		:	//호칭 구입
		{
			if( NULL == pSession )					return;
			if( NULL == pArg0 || NULL == pArg1 )	return;
			
			UINT8	ui8UserTitleId			= (UINT8)*((UINT8*)pArg0);
			LOG_GET_TITLE* pstLogGetTitle	= (LOG_GET_TITLE*)pArg1;
						
			char strMedal[SERVER_STRING_COUNT];
			i3String::Format( strMedal, SERVER_STRING_COUNT, "%d/%d/%d/%d", pSession->m_ui32Miniature, pSession->m_ui32Insignia, pSession->m_ui32Order, pSession->m_ui32Master );
			char strUse[SERVER_STRING_COUNT];
			i3String::Format( strUse, SERVER_STRING_COUNT, "%d/%d/%d/%d", pstLogGetTitle->_i32Miniature, pstLogGetTitle->_i32Insignia, pstLogGetTitle->_i32Order, pstLogGetTitle->_i32Master );

			START_DATA( "2001", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA02, ui8UserTitleId );
			ADD_IDATA( ZLOG_IDATA03, pSession->m_UserInfoBasic._ui32TotalBattleTime );
			ADD_SDATA( ZLOG_SDATA01, strMedal );
			ADD_SDATA( ZLOG_SDATA02, strUse );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_QUEST_FINISH		:	//임무카드 완료
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			LOG_MISSION* pstMission = (LOG_MISSION*)pArg0;

			char strMedal[SERVER_STRING_COUNT];
			i3String::Format( strMedal, SERVER_STRING_COUNT, "%d/%d/%d/%d", pSession->m_ui32Miniature, pSession->m_ui32Insignia, pSession->m_ui32Order, pSession->m_ui32Master );

			char strGetMedal[SERVER_STRING_COUNT];
			i3String::Format( strGetMedal, SERVER_STRING_COUNT, "%d/%d/%d/%d", pstMission->_i32Miniature, pstMission->_i32Insignia, pstMission->_i32Order, pstMission->_i32Master );
			
			char strGetItem[SERVER_STRING_COUNT];
			i3String::Format( strGetItem, SERVER_STRING_COUNT, "%d/%d/%d/%d/%d", pstMission->_aui32GetItem[0], pstMission->_aui32GetItem[1], pstMission->_aui32GetItem[2], pstMission->_aui32GetItem[3], pstMission->_aui32GetItem[4] );

			char strGetLastMedal[SERVER_STRING_COUNT];
			strGetLastMedal[0] = '\0';
			char strGetLastItem[SERVER_STRING_COUNT];
			strGetLastItem[0] = '\0';

			if( pstMission->_bFinish )
			{			
				i3String::Format( strGetLastMedal, SERVER_STRING_COUNT, "%d/%d/%d/%d", pstMission->_i32LastMiniature, pstMission->_i32LastInsignia, pstMission->_i32LastOrder, pstMission->_i32LastMaster );
				i3String::Format( strGetLastItem, SERVER_STRING_COUNT, "%d/%d/%d/%d/%d", pstMission->_aui32LastGetItem[0], pstMission->_aui32LastGetItem[1], pstMission->_aui32LastGetItem[2], pstMission->_aui32LastGetItem[3], pstMission->_aui32LastGetItem[4] );
			}

			START_DATA( "2002", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA02, pstMission->_ui8CardType );
			ADD_IDATA( ZLOG_IDATA03, pstMission->_ui8CardSet );
			ADD_IDATA( ZLOG_IDATA04, pstMission->_bFinish );
			ADD_IDATA( ZLOG_IDATA05, pSession->m_UserInfoBasic._point );
			ADD_IDATA( ZLOG_IDATA06, pSession->m_UserInfoBasic._exp );
			ADD_IDATA( ZLOG_IDATA07, pstMission->_ui32Point );
			ADD_IDATA( ZLOG_IDATA08, pstMission->_ui32Exp );
			ADD_IDATA( ZLOG_IDATA09, pstMission->_ui32LastPoint );
			ADD_IDATA( ZLOG_IDATA10, pstMission->_ui32LastExp );
			ADD_SDATA( ZLOG_SDATA01, strMedal );
			ADD_SDATA( ZLOG_SDATA02, strGetMedal );
			ADD_SDATA( ZLOG_SDATA03, strGetItem );
			ADD_SDATA( ZLOG_SDATA04, strGetLastMedal );
			ADD_SDATA( ZLOG_SDATA05, strGetLastItem );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_QUEST_DELETE		:	//임무카드 삭제
		{
			if( NULL == pSession )					return;
			if( NULL == pArg0 || NULL == pArg1 )	return;
			UINT8	ui8CardType		= (UINT8)*((UINT8*)pArg0);
			UINT16*	ui16CardActive	= (UINT16*)pArg1;

			// 15/0/0/0/15/1/ <-- 이런식으로 표시 됩니다.
			char strMission[ SERVER_STRING_COUNT ];
			char strValue[ SERVER_STRING_COUNT ];
			_itoa( ui16CardActive[0], strValue, 10 );
			i3String::Copy( strMission, strValue, SERVER_STRING_COUNT );
			for( UINT8 i = 1 ; i < MAX_CARD_PER_CARDSET ; ++i )
			{
				_itoa( ui16CardActive[i], strValue, 10 );
				i3String::Concat( strMission, "/" );
				i3String::Concat( strMission, strValue );
			}
			
			START_DATA( "2003", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA02, ui8CardType );
			ADD_SDATA( ZLOG_SDATA01, strMission );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_PENALTY				: // 게임 도중 무단 이탈로 인한 패널티
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;

			UINT32 ui32Penalty = (UINT32)*((UINT32*)pArg0);

			START_DATA( "1102", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA02, pSession->m_UserInfoBasic._point );
			ADD_IDATA( ZLOG_IDATA03, ui32Penalty );
			ADD_IDATA( ZLOG_IDATA04, 5 );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_U_CHATTING:
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			if( NULL == pArg1 )		return;

			UINT32 ui32ChattingType = (UINT32)*((UINT32*)pArg0);

			char strNick[ NET_CLAN_NAME_SIZE ];
			LogChangeString( strNick, pSession->m_UserInfoBasic._nick, NET_NICK_NAME_SIZE );

			char strID[ NET_ID_SIZE ];
			LogChangeString( strID, pSession->m_strId, NET_ID_SIZE );

			char strMessgae[ MAX_STRING_COUNT ];
			LogChangeString( strMessgae, (char*)pArg1, MAX_STRING_COUNT-2 );

			START_DATA( "3001", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA02, ui32ChattingType );
			ADD_SDATA( ZLOG_SDATA01, strNick );
			ADD_SDATA( ZLOG_SDATA02, strID );
			ADD_SDATA( ZLOG_SDATA03, pSession->GetIpString() );
			ADD_SDATA( ZLOG_SDATA04, strMessgae );
			END_DATA();
		}
		break;

	case LOG_MESSAGE_U_CHATTING_HACK:
		{
			if( NULL == pSession )	return;
			if( NULL == pArg0 )		return;
			if( NULL == pArg1 )		return;

			UINT32 ui32ChattingType = (UINT32)*((UINT32*)pArg0);

			char strNick[ NET_CLAN_NAME_SIZE ];
			LogChangeString( strNick, pSession->m_UserInfoBasic._nick, NET_NICK_NAME_SIZE );

			char strID[ NET_ID_SIZE ];
			LogChangeString( strID, pSession->m_strId, NET_ID_SIZE );

			char strMessgae[ MAX_STRING_COUNT ];
			LogChangeString( strMessgae, (char*)pArg1, MAX_STRING_COUNT-2 );

			START_DATA( "3002", 4 );
			ADD_LDATA( ZLOG_LDATA01, pSession->m_i64UID );
			ADD_IDATA( ZLOG_IDATA01, pSession->m_UserInfoBasic._rank );
			ADD_IDATA( ZLOG_IDATA02, ui32ChattingType );
			ADD_SDATA( ZLOG_SDATA01, strNick );
			ADD_SDATA( ZLOG_SDATA02, strID );
			ADD_SDATA( ZLOG_SDATA03, pSession->GetIpString() );
			ADD_SDATA( ZLOG_SDATA04, strMessgae );
			END_DATA();
		}
		break;
	
	case LOG_MESSAGE_U_SHOP_PRESENT		:	//선물하기
	case LOG_MESSAGE_U_FRIEND_ACCEPT	:	//친구 신청
	case LOG_MESSAGE_U_FRIEND_INSERT	:	//친구 추가
	case LOG_MESSAGE_U_FRIEND_DELETE	:	//친구 삭제 
		break;
	default: 
		{
			i16LogSize = 0;
		}
		break;
	}

	if( i16LogSize == 0 )
	{
		char strError[128];
		i3String::Format( strError, 128, "LogSendMessage :: Unknown Log Type - %d", LogId );
		g_pModuleLogFile->Write_M_Log( ThreadIdx, pSession, strError );
		return;
	}

	if( i16LogSize > LOG_MESSAGE_MAX )
	{
		i16LogSize = LOG_MESSAGE_MAX;
	}

	//Set Size
	i3mem::Copy( &strSendBuffer[0], &i16LogSize, sizeof(UINT16)); 

	if( LOG_MESSAGE_MAX > ( i16LogSize + *pui32BufferSize ) )
	{
		char *pTempBuffer = pstrBuffer + *pui32BufferSize;
		i3mem::Copy( pTempBuffer, strSendBuffer, i16LogSize );
		*pui32BufferSize += i16LogSize;
	}
	else
	{
		i3mem::Copy( pSendBuffer[ui32BufferIdx]._szMessage, pstrBuffer, LOG_MESSAGE_MAX );
		pSendBuffer[ui32BufferIdx]._Size = *pui32BufferSize;
		*pi32WritePos = *pi32WritePos + 1;

		i3mem::FillZero( pstrBuffer, LOG_MESSAGE_MAX );
		i3mem::Copy( pstrBuffer, strSendBuffer, i16LogSize );
		*pui32BufferSize = i16LogSize;
	}

	return; 
}

INT32 CModuleContextNC::GetStageMode( UINT8 ui8WeaponFlag ) 
{
	INT32 i32StageMode = 0;
	if( ui8WeaponFlag & LOCK_SNIPER )		i32StageMode = 1;	// 스나이퍼전 검사
	else if( ui8WeaponFlag & LOCK_SHOTGUN )	i32StageMode = 2;	// 샷건전 검사
	else if( ui8WeaponFlag & LOCK_KNUCKLE )	i32StageMode = 3;	// 너클전 검사
	return i32StageMode;
}