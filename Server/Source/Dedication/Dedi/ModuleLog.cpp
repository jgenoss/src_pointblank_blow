#include "pch.h"
#include "LogFile.h"
#include "ModuleLog.h"
I3_CLASS_INSTANCE( CModuleLog, i3Thread );

#define	SEND_LOG_BUFFER_COUNT		100

CModuleLog * g_pModuleLog = NULL; 

CModuleLog::CModuleLog(void)
{
	m_hKillEvent		= ::CreateEvent(NULL, false, false, NULL);
	m_pLogModuleSocket	= NULL; 
	m_pMainLogMessage	= NULL;		
}

CModuleLog::~CModuleLog(void)
{	
	::SetEvent( m_hKillEvent ); 
	OnDestroy(); 
	::Sleep(1000);	
	::CloseHandle( m_hKillEvent ); 
}

BOOL CModuleLog::OnCreate(void)
{
	if(!g_pConfig->m_bLogDActive)return TRUE; //사용하지 않음 

	//1. Create Module
	m_pLogModuleSocket = CModuleLogSocket::NewObject(); 
	if( !m_pLogModuleSocket->OnConnect( g_pConfig->m_LogDServerIp, g_pConfig->m_LogDServerPort) )  return FALSE; 
	SendInitMessage();	//Send Init Message

	//2. Create Main Send Buffer 
	m_pMainLogMessage	= (LOG_SEND_MESSAGE	*)i3MemAlloc(sizeof(LOG_SEND_MESSAGE) * SEND_LOG_BUFFER_COUNT);	// Main Thread 
	i3mem::FillZero( m_pMainLogMessage, sizeof(LOG_SEND_MESSAGE) * SEND_LOG_BUFFER_COUNT ); 
	for(UINT32 j = 0; j < SEND_LOG_BUFFER_COUNT; j++)
	{
		m_pMainLogMessage[j]._szMessage[3] = 3;
	}
	m_MainLog_WritePos	= 0;
	m_MainLog_ReadPos	= 0;

	//3. Create Thread
	if( !i3Thread::Create("ModuleLog", 0, 4096, NULL, PRIORITY_NORMAL) )return FALSE; 
	DSMSG("[CModuleLog::OnCreate] LOG_D CONNECT SUCCESS\n"); 
	return TRUE; 
}

void CModuleLog::OnDestroy(void)
{
	if(!g_pConfig->m_bLogDActive)return; 

	//Free
	I3MEM_SAFE_FREE( m_pMainLogMessage ); 

	//Release
	I3_SAFE_RELEASE( m_pLogModuleSocket ); 

	return; 
}

#define STRING_START_IDX	11
#define MACHINE_NAME		"PB"
#define SERVER_TYPE			"RY"
void CModuleLog::SendLogMessage( LOG_MESSAGE LogId )
{
	if(!g_pConfig->m_bLogDActive)return; 

	LOG_SEND_MESSAGE	* pSendBuffer; 
	INT32				* pWritePos; 
	INT32				* pReadPos; 
	UINT32				BufferIdx; 

	//1. Set Insert Buffer 
	pSendBuffer = m_pMainLogMessage; 
	pWritePos	= &m_MainLog_WritePos; 
	pReadPos	= &m_MainLog_ReadPos; 

	//리드 버퍼 인덱스를 초기화 할 수 있는지 확인한다. 
	if(*pWritePos == *pReadPos)
	{
		*pWritePos	= 0; 
		*pReadPos	= 0; 		
	}

	BufferIdx = *pWritePos - *pReadPos; 
	if( BufferIdx >= SEND_LOG_BUFFER_COUNT )
	{
		I3TRACE("[CModuleLog::SendLogMessage] Send Buffer Overflow \n");
		return; 
	}

	BufferIdx = *pWritePos % SEND_LOG_BUFFER_COUNT;

	//극한 값까지 갔는지를 확인합니다. 2415919103
	if( *pReadPos > 2000000000 ) 
	{//여기로 들어올수  있는 가능성은 거의 없다. 
		I3TRACE("[CModuleLog::SendLogMessage] Send Buffer Full \n");
		return; 
	}

	char strDate[SERVER_STRING_COUNT]; 

	//Set Time 
	sprintf(strDate, "%02d/%02d/%d %02d:%02d:%02d.00", g_pConfig->m_Month, g_pConfig->m_Day, g_pConfig->m_Year, g_pConfig->m_Hour, g_pConfig->m_Minute, g_pConfig->m_Second); 

	//2. Set Buffer 
	INT16 StringSize;	
	switch(LogId)
	{
	case LOG_MESSAGE_SERVER_START	:		//서버 시작 
		{
			pSendBuffer[BufferIdx]._szMessage[7] = 7;
			StringSize = (INT16)sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX], "%s,101,,,,,,%s,%s,,,\r\n",strDate , MACHINE_NAME, SERVER_TYPE); 
		}
		break; 
	case LOG_MESSAGE_SERVER_END		:		//서버 종료 
		{
			pSendBuffer[BufferIdx]._szMessage[7] = 7;
			StringSize = (INT16)sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX], "%s,102,,,,,,%s,%s,,,\r\n",strDate , MACHINE_NAME, SERVER_TYPE); 
		}
		break; 
	case LOG_MESSAGE_SERVER_STATE	:		//서버 상태			
		{
			pSendBuffer[BufferIdx]._szMessage[7] = 7;
			StringSize = (INT16)sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX], "%s,103,0,%d,0,%d,,%s,%s,,,,\r\n",strDate , g_pConfig->m_iCpuQuery, g_pConfig->m_iHandleQuery,MACHINE_NAME, SERVER_TYPE); 
		}
		break; 
	default: 
		StringSize = 0;
		break; 
	}

	//Set Size
	pSendBuffer[BufferIdx]._Size = StringSize + STRING_START_IDX + 1; //널 포함 + 1엔씨 요청
	i3mem::Copy( pSendBuffer[BufferIdx]._szMessage, &pSendBuffer[BufferIdx]._Size, sizeof(UINT16)); 

	//3. Add WritePos 
	*pWritePos = *pWritePos + 1; 	
}

UINT32 CModuleLog::OnRunning( void * pUserData)
{	
	INT32	SendCount; 
	UINT32	ReadIdx;
	UINT32	SendSize;
	UINT8	SendFailCount = 0; 
	BOOL	IsSend;
	BOOL	IsWorking = TRUE; 
	while(IsWorking)
	{		
		if( ::WaitForSingleObject( m_hKillEvent, 1) == WAIT_OBJECT_0 )
		{
			IsWorking = FALSE; 
			break;
		}

		IsSend = FALSE; 
		SendSize = 0;

		if(!m_pLogModuleSocket->IsConnected()) 
		{			
			m_pLogModuleSocket->OnDestroy(); 
			if( m_pLogModuleSocket->OnConnect(g_pConfig->m_LogDServerIp, g_pConfig->m_LogDServerPort ) )
			{
				SendInitMessage();
				//WRITE_LOG(NULL,"SUCCESS LOG_MODULE RECONNECT 1");
			}
			else
			{
				m_pLogModuleSocket->OnDestroy(); 
				//WRITE_LOG(NULL,"FAIL LOG_MODULE RECONNECT 1");
			}
			Sleep(1000);	
		}

		//Main Thread 
 		SendCount = m_MainLog_WritePos - m_MainLog_ReadPos; 
		if(SendCount > 0)
		{
			for(INT32 i = 0; i < SendCount; i++)
			{
				ReadIdx = m_MainLog_ReadPos % SEND_LOG_BUFFER_COUNT; 
				SendSize += m_pLogModuleSocket->SendPacketMessage( m_pMainLogMessage[ReadIdx]._szMessage, m_pMainLogMessage[ReadIdx]._Size ); 
				m_MainLog_ReadPos++; 				
			}
			IsSend = TRUE; 
		}

		m_pLogModuleSocket->SelectEvent();  

		if( (IsSend == TRUE)&& (SendSize ==  0) )
		{
			SendFailCount++;
			if(SendFailCount > 1)
			{
				m_pLogModuleSocket->OnDestroy(); 
				if( m_pLogModuleSocket->OnConnect(g_pConfig->m_LogDServerIp, g_pConfig->m_LogDServerPort ) )
				{
					//WRITE_LOG(NULL,"SUCESS LOG_MODULE RECONNECT 2"); 
					SendInitMessage(); 
					SendFailCount = 0; 
				}
				else
				{
					m_pLogModuleSocket->OnDestroy(); 
					//WRITE_LOG(NULL,"FAIL LOG_MODULE RECONNECT 2"); 
				}
				Sleep(1000);
			}
		}
		Sleep(1); 
	}
	return 0; 
}

//CHECK_STATUS -> STARTED -> SEND_MSG
void CModuleLog::SendInitMessage(void)
{	
	UINT8  PacketId;
	UINT16 PacketSize; 
	UINT32 Temp; 

	char	pSendBuffer[8192]; 
	INT32	SendSize; 
	
	PacketSize	= 11; 
	PacketId	= 3; 	
	i3mem::Copy( &pSendBuffer[0], &PacketSize, sizeof(UINT16)); 
	i3mem::Copy( &pSendBuffer[2], &PacketId,	sizeof(UINT8)); 
	Temp		= g_pConfig->m_ncGameId; 
	i3mem::Copy( &pSendBuffer[3], &Temp,		sizeof(UINT32)); 
	Temp		= 3; 
	i3mem::Copy( &pSendBuffer[7], &Temp,		sizeof(UINT32)); 
	SendSize = PacketSize;
	m_pLogModuleSocket->SendPacketMessage(pSendBuffer, SendSize);		// STARTED

	return; 
}
