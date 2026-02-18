#include "pch.h"
#include "ModuleContextB.h"
#include "ServerContext.h"

#include "LogFile.h"
#include "UserSession.h"
#include "Room.h"
#include "RoomManager.h"
#include "UserManager.h"

#include "ModuleGipSocket.h"
#include "ModuleLogSocket.h"


//htons() - "Host to Network Short" 
//htonl() - "Host to Network Long" 
//ntohs() - "Network to Host Short" 
//ntohl() - "Network to Host Long"
DWORD sConvertMultiToUTF8(char *pszResultBuffer, int nResultBufferSize, char *pszSourceBuffer)
{
	int convertM2USize = MultiByteToWideChar(CP_ACP, 0, pszSourceBuffer, -1, 0, 0);
	wchar_t *pwszTempWideBuffer = (wchar_t *)malloc(sizeof(wchar_t) * convertM2USize);
	MultiByteToWideChar(CP_ACP, 0, pszSourceBuffer, -1, pwszTempWideBuffer, convertM2USize);

	int convertU2MSize = WideCharToMultiByte(CP_UTF8, 0, pwszTempWideBuffer, -1, 0, 0, NULL, NULL);
	if (convertU2MSize > nResultBufferSize)
		return (DWORD)-1;

	WideCharToMultiByte(CP_UTF8, 0, pwszTempWideBuffer, -1, pszResultBuffer, convertU2MSize, NULL, NULL);

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
#define MACHINE_NAME						"PB"
#define SERVER_TYPE							"GS"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
I3_CLASS_INSTANCE( CModuleContextNC, i3Thread );
CModuleContextNC * g_pModuleContextNC = NULL; 

CModuleContextNC::CModuleContextNC(void)
{
	m_hKillEvent		= ::CreateEvent(NULL, false, false, NULL);

	//Gip
	m_pGipModuleSocket	= NULL; 
	m_ppGipUserMessage	= NULL;	
	m_pGipUser_WritePos = NULL;
	m_pGipUser_ReadPos	= NULL;	
	m_pGipMainMessage	= NULL;	
	m_pGipAuthMessage	= NULL;	

	//Log
	m_pLogModuleSocket	= NULL; 
	m_ppLogUserMessage	= NULL;	
	m_pLogUser_WritePos = NULL;
	m_pLogUser_ReadPos	= NULL;	
	m_pLogMainMessage	= NULL;	
	m_pLogAuthMessage	= NULL;	
}

CModuleContextNC::~CModuleContextNC(void)
{	
	::SetEvent( m_hKillEvent ); 
	OnDestroy(); 
	::Sleep(1000);	
	::CloseHandle( m_hKillEvent ); 
}

INT32 CModuleContextNC::OnCreate(UINT32 ThreadCount)
{
	m_ThreadCount = ThreadCount; //Set Buffer 

	if( !_LogCreate() )		return SERVER_ERROR_LOG; 
	if( !_GipCreate() )		return SERVER_ERROR_GIP;

	//Create Thread
	if( !i3Thread::Create("ModuleContextB", 0, 4096, NULL, PRIORITY_HIGH) )return FALSE; 
	WRITE_LOG(NULL, "MODULE_B(NC LOG_D, NC GIP) CONTEXT READY!");

	//Set Log Index 맵이 추가 될때마다 추가 되어야 합니다. 중간에 추가되면 안되요.
	i3mem::FillZero( m_piStageIdx, (sizeof(INT32) * 256)); 	
	m_piStageIdx[STAGE_ORD_PORT_AKABA]		= QUEST_STAGE_ORD_PORT_AKABA; 
	m_piStageIdx[STAGE_ORD_RED_ROCK]		= QUEST_STAGE_ORD_RED_ROCK; 
	m_piStageIdx[STAGE_ORD_LIBRARY]			= QUEST_STAGE_ORD_LIBRARY; 
	m_piStageIdx[STAGE_ORD_MSTATION]		= QUEST_STAGE_ORD_MSTATION; 
	m_piStageIdx[STAGE_ORD_MIDNIGHT_ZONE]	= QUEST_STAGE_ORD_MIDNIGHT_ZONE; 
	m_piStageIdx[STAGE_ORD_UPTOWN]			= QUEST_STAGE_ORD_UPTOWN; 
	m_piStageIdx[STAGE_ORD_BURNINGHALL]		= QUEST_STAGE_ORD_BURNINGHALL; 
	m_piStageIdx[STAGE_ORD_DSQUAD]			= QUEST_STAGE_ORD_DSQUAD; 
	m_piStageIdx[STAGE_ORD_DOWNTOWN]		= QUEST_STAGE_ORD_DOWNTOWN; 
	m_piStageIdx[STAGE_ORD_LUXVILLE]		= QUEST_STAGE_ORD_LUXVILLE; 
	m_piStageIdx[STAGE_ORD_BREAKDOWN]		= QUEST_STAGE_ORD_BREAKDOWN; 
	m_piStageIdx[STAGE_ORD_GIRAN]			= QUEST_STAGE_ORD_GIRAN; 
	m_piStageIdx[STAGE_ORD_GIRAN2]			= QUEST_STAGE_ORD_GIRAN2; 
	m_piStageIdx[STAGE_ORD_OUTPOST]			= QUEST_STAGE_ORD_OUTPOST;
	m_piStageIdx[STAGE_ORD_CRACKDOWN]		= QUEST_STAGE_ORD_CRACKDOWN;
	m_piStageIdx[STAGE_ORD_BLOWCITY]		= QUEST_STAGE_ORD_BLOWCITY;
	m_piStageIdx[STAGE_ORD_STORMTUBE]		= QUEST_STAGE_ORD_STORMTUBE;	

	return EVENT_ERROR_SUCCESS; 
}

void CModuleContextNC::OnDestroy(void)
{
	if(!g_pContextMain->m_bGipActive)return; 	
	_LogDestroy(); 	
	_GipDestroy(); 
	return; 
}

UINT32 CModuleContextNC::OnRunning( void * pUserData)
{	
	INT32 iSendGipCount, iSendLogCount;
	iSendGipCount = iSendLogCount = 0; 
	while(1)
	{
		if( (iSendLogCount == 0 ) && (iSendLogCount == 0) )
		{
			if( ::WaitForSingleObject( m_hKillEvent, 1) == WAIT_OBJECT_0 )break;
		}

		if(g_pContextMain->m_bGipActive)iSendGipCount  = _GipUpdate();
		if(g_pContextMain->m_bLogDActive)iSendLogCount = _LogUpdate();
	}
	return 0; 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//GIP
BOOL CModuleContextNC::_GipCreate(void)
{
	if(!g_pContextMain->m_bGipActive)return TRUE; //사용하지 않음 
	//1. Create Work Send Buffer
	m_ppGipUserMessage	= (GIP_SEND_MESSAGE	**)i3MemAlloc(m_ThreadCount * sizeof(GIP_SEND_MESSAGE*)); 
	m_pGipUser_WritePos = (INT32*)i3MemAlloc(m_ThreadCount * sizeof(INT32));	
	m_pGipUser_ReadPos	= (INT32*)i3MemAlloc(m_ThreadCount * sizeof(INT32));
	for(UINT32 i = 0; i < m_ThreadCount; i++)
	{
		m_ppGipUserMessage[i]	= (GIP_SEND_MESSAGE	*)i3MemAlloc(sizeof(GIP_SEND_MESSAGE) * GIP_SEND_BUFFER_COUNT);		
		i3mem::FillZero( m_ppGipUserMessage[i], sizeof(GIP_SEND_MESSAGE) * GIP_SEND_BUFFER_COUNT); 

		m_pGipUser_WritePos[i]	= 0; 
		m_pGipUser_ReadPos[i]	= 0; 
	}

	//1. Create Main Send Buffer 
	m_pGipMainMessage	= (GIP_SEND_MESSAGE	*)i3MemAlloc(sizeof(GIP_SEND_MESSAGE) * GIP_SEND_BUFFER_COUNT);	// Main Thread 
	i3mem::FillZero( m_pGipMainMessage, sizeof(GIP_SEND_MESSAGE) * GIP_SEND_BUFFER_COUNT ); 
	m_iGipMain_WritePos	= 0;
	m_iGipMain_ReadPos	= 0;

	//1. Create Module Send Buffer 
	m_pGipAuthMessage	= (GIP_SEND_MESSAGE	*)i3MemAlloc(sizeof(GIP_SEND_MESSAGE) * GIP_SEND_BUFFER_COUNT);	// Module Thread 
	i3mem::FillZero( m_pGipAuthMessage, sizeof(GIP_SEND_MESSAGE) * GIP_SEND_BUFFER_COUNT ); 
	m_iGipAuth_WritePos	= 0;
	m_iGipAuth_ReadPos	= 0;

	//Create BackUpBuffer 
	m_pGipBackUpBuffer		= (GIP_BACKUP*)i3MemAlloc(sizeof(GIP_BACKUP) * GIP_BACKUP_COUNT); 

	//2. Create Module
#ifdef TRACE_GIP_PACKET
	m_pGipModuleSocket = CModuleGipSocket::NewObject(); 
	m_pGipModuleSocket->m_bGreeting = TRUE;
#else 
	m_pGipModuleSocket = CModuleGipSocket::NewObject(); 
	if( m_pGipModuleSocket->OnConnect( g_pContextMain->m_GipServerIp, g_pContextMain->m_GipServerPort) )  
	{
		_GipSendGreetingMessage();	//Send Greeting Message
		WRITE_LOG(NULL, "GIP CONNECT SUCCESS"); 
		return TRUE; 		
	}
	else
	{
		WRITE_LOG(NULL, "GIP CONNECT FAIL"); 
		return FALSE; 
	}	
#endif
	return TRUE; 
}

INT32 CModuleContextNC::_GipUpdate(void)
{
	INT32	SendCount; 
	UINT32	ReadIdx;
	UINT32	SendSize;
	UINT8	SendFailCount = 0; 
	SendSize = 0;
	UINT32 iSendCompleteCount = 0; 

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//1. Check Connect 
#ifdef TRACE_GIP_PACKET

#else
	if(!m_pGipModuleSocket->IsConnected()) 
	{			
		m_pGipModuleSocket->OnDestroy(); 
		if( m_pGipModuleSocket->OnConnect(g_pContextMain->m_GipServerIp, g_pContextMain->m_GipServerPort ) )
		{
			WRITE_LOG(NULL,"SUCCESS GIP_MODULE RECONNECT 1");
			_GipSendGreetingMessage(); 
		}
		else
		{
			WRITE_LOG(NULL,"FAIL GIP_MODULE RECONNECT 1");
			m_pGipModuleSocket->OnDestroy();
		}
		return iSendCompleteCount; 
	}
#endif
	//Main Thread 
	SendCount = m_iGipMain_WritePos - m_iGipMain_ReadPos; 

	if(SendCount > 0)
	{
		for(INT32 i = 0; i < SendCount; i++)
		{
			if(iSendCompleteCount < MODULE_CONTEXTB_SEND_COUNT)
			{
				ReadIdx = m_iGipMain_ReadPos % GIP_SEND_BUFFER_COUNT; 
			#ifdef TRACE_GIP_PACKET				
				I3TRACE(m_pGipMainMessage[ReadIdx]._Packet.commandList.command);
				I3TRACE("\n%d\n", ntohs(m_pGipMainMessage[ReadIdx]._Packet.header.length));
			#else
				_GipMessage(&m_pGipMainMessage[ReadIdx]); 
			#endif
				m_iGipMain_ReadPos++; 
				iSendCompleteCount++; 
			}
		}
	}

	//Auth Module Thread
	SendCount = m_iGipAuth_WritePos - m_iGipAuth_ReadPos; 
	if(SendCount > 0)
	{
		for(INT32 i = 0; i < SendCount; i++)
		{
			if(iSendCompleteCount < MODULE_CONTEXTB_SEND_COUNT)
			{
				ReadIdx = m_iGipAuth_ReadPos % GIP_SEND_BUFFER_COUNT; 
			#ifdef TRACE_GIP_PACKET
				I3TRACE(m_pGipAuthMessage[ReadIdx]._Packet.commandList.command); 
				I3TRACE( "\n%d\n", ntohs(m_pGipMainMessage[ReadIdx]._Packet.header.length));
			#else
				_GipMessage(&m_pGipAuthMessage[ReadIdx]);
			#endif
				m_iGipAuth_ReadPos++; 
				iSendCompleteCount++;
			}
		}
	}

	//User Thread 
	for(UINT32 i = 0; i < m_ThreadCount; i++)
	{			
		GIP_SEND_MESSAGE * pGipBuffer = m_ppGipUserMessage[i]; 
		SendCount = m_pGipUser_WritePos[i] - m_pGipUser_ReadPos[i]; 			

		if(SendCount > 0)
		{
			for(INT32 j = 0; j < SendCount; j++)
			{
				if(iSendCompleteCount < MODULE_CONTEXTB_SEND_COUNT)
				{
					ReadIdx = m_pGipUser_ReadPos[i] % GIP_SEND_BUFFER_COUNT;
				#ifdef TRACE_GIP_PACKET
					I3TRACE(pGipBuffer[ReadIdx]._Packet.commandList.command); 
					I3TRACE( "\n%d\n", ntohs(m_pGipMainMessage[ReadIdx]._Packet.header.length));
				#else
					_GipMessage(&pGipBuffer[ReadIdx]); 
				#endif
					m_pGipUser_ReadPos[i]++; 
					iSendCompleteCount++;
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2. Recv Data 
	m_pGipModuleSocket->m_RequestCount = 0; 
	m_pGipModuleSocket->SelectEvent();

	//Send Request Message 
	for(UINT32 i = 0; i < m_pGipModuleSocket->m_RequestCount; i++)
	{
		_GipSendReqMessage(i); 
	}
	return iSendCompleteCount;
}

void CModuleContextNC::_GipDestroy(void)
{
	for(UINT32 i = 0; i < m_ThreadCount; i++)
	{
		I3MEM_SAFE_FREE( m_ppGipUserMessage[i] );
	}
	I3MEM_SAFE_FREE( m_ppGipUserMessage ); 
	I3MEM_SAFE_FREE( m_pGipUser_WritePos); 
	I3MEM_SAFE_FREE( m_pGipUser_ReadPos ); 
	I3MEM_SAFE_FREE( m_pGipMainMessage ); 
	I3MEM_SAFE_FREE( m_pGipAuthMessage ); 
	I3MEM_SAFE_FREE( m_pGipBackUpBuffer ); 
	I3_SAFE_RELEASE( m_pGipModuleSocket ); 
}

void CModuleContextNC::GipSendMessage( INT32 ThreadIdx, GIP_MESSAGE GipId, CUserSession * pSession, INT32 Arg)
{
	if(!g_pContextMain->m_bGipActive)return;			//보낼필요 없음
	if(!m_pGipModuleSocket->m_bGreeting)return;			//보내지 못함
	if(pSession->m_UserInfoBasic._nick[0] == 0)return;	//닉네임이 NULL이면 처리하지않음
    
	GIP_SEND_MESSAGE	* pSendBuffer; 
	INT32				* pWritePos; 
	INT32				* pReadPos; 
	UINT32				BufferIdx; 

	//1. Set Insert Buffer 
	switch( ThreadIdx )
	{
	case	MAIN_THREAD_IDX		: 
		pSendBuffer = m_pGipMainMessage; 
		pWritePos	= &m_iGipMain_WritePos; 
		pReadPos	= &m_iGipMain_ReadPos; 
		break; 
	case	MODULE_ONE_THREAD_IDX	: 
		pSendBuffer = m_pGipAuthMessage; 
		pWritePos	= &m_iGipAuth_WritePos; 
		pReadPos	= &m_iGipAuth_ReadPos; 
		break; 
	default	: 
		pSendBuffer = m_ppGipUserMessage[ThreadIdx];
		pWritePos	= &m_pGipUser_WritePos[ThreadIdx]; 
		pReadPos	= &m_pGipUser_ReadPos[ThreadIdx]; 
		break; 
	}	

	//리드 버퍼 인덱스를 초기화 할 수 있는지 확인한다. 
	if(*pWritePos == *pReadPos)
	{
		*pWritePos	= 0;	//순서 중요 
		*pReadPos	= 0; 	
	}

	//극한 값까지 갔는지를 확인합니다. 2415919103
	if( *pReadPos > MODULE_CONTEXT_MAX_SEND_IDX_COUNT ) 
	{//여기로 들어올수  있는 가능성은 거의 없다. 
		WRITE_LOG(NULL,"GipModule WriteBuffer FULL");
		return; 
	}

	BufferIdx = *pWritePos - *pReadPos; 
	if( BufferIdx >= GIP_SEND_BUFFER_COUNT )
	{
		WRITE_LOG(NULL,"Gip Module WriteBuffer Overflow");
		return; 
	}

	//pSendBuffer[BufferIdx]._Packet.header.length	 = ; 
	pSendBuffer[BufferIdx]._Packet.header.type		 = 0x13;
	pSendBuffer[BufferIdx]._Packet.header.version	 = 1; 
	pSendBuffer[BufferIdx]._Packet.header.headerLen  = sizeof(Packet_Header); 
	pSendBuffer[BufferIdx]._Packet.header.crc		 = 0; 
	//pSendBuffer[BufferIdx]._Packet.header.pid		 = ; //보낼때 결정 
	//pSendBuffer[BufferIdx]._Packet.header.qid		 = ; //보낼때 결정 
	pSendBuffer[BufferIdx]._Packet.commandList.commandCrc	= 0; 
	pSendBuffer[BufferIdx]._Packet.commandList.commandType	= 1; 

	//Set Time
	time_t iShowtime; 
	time(&iShowtime); 	

	char pTempBuffer[4096];
	switch(GipId)
	{
	case GIP_MESSAGE_CREATE_NICK_CHAR	:	//CMD_InsertCharacterInfo
		{
			char	TempNick[SERVER_STRING_COUNT];  //크기는 최소 NET_NICK_NAME_SIZE * 6 이상이 되어야 한다. 
			INT32	End = 0; 
			TempNick[End] = 0; 

			for( INT32 i= 0; i < NET_NICK_NAME_SIZE;i++ )
			{
				switch ( pSession->m_UserInfoBasic._nick[i] )
				{
				case 0 : i = NET_NICK_NAME_SIZE;							break; //끝
				case 38: strcat(TempNick, "&amp;");		End = End + 5;		break;	//&
				case 60: strcat(TempNick, "&lt;");		End = End + 4;		break;	//<
				case 62: strcat(TempNick, "&gt;");		End = End + 4; 		break;	//>
				case 34: strcat(TempNick, "&quot;");	End = End + 6; 		break;	//"
				case 39: strcat(TempNick, "&apos;");	End = End + 6; 		break;	//'
				default: 
					TempNick[End] = pSession->m_UserInfoBasic._nick[i]; 
					End++; 
					break; 
				}
			}
			TempNick[End] = 0;			
			sprintf( pTempBuffer, "<cmd action=\"CMD_InsertCharacterInfo\" version=\"2.0\"><param><uid v=\"%d\"/><charId v=\"0\"/><serverId v=\"1\"/><charName v=\"%s\"/><genderId v=\"1\"/><teamId v=\"1\"/><classCodeId v=\"0\"/><localTime v=\"%d\"/></param></cmd>", \
				pSession->m_DBIdx, TempNick, iShowtime);
		}
		break; 
	case GIP_MESSAGE_CREATE_NICK_USER	:	//CMD_InsertUserInfo
		sprintf( pTempBuffer, "<cmd action=\"CMD_InsertUserInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><userName v=\"\"/><gamePoint v=\"0\"/><cashPoint v=\"0\"/><localTime v=\"%d\"/></param></cmd>", \
			pSession->m_DBIdx, iShowtime);
		break; 
	case GIP_MESSAGE_CHRA		:	//CMD_UpdateCharacterRecordInfo
		{
			char	TempNick[SERVER_STRING_COUNT]; 
			INT32	End = 0; 
			TempNick[End] = 0; 		

			for( INT32 i= 0; i < NET_NICK_NAME_SIZE;i++ )
			{
				switch ( pSession->m_UserInfoBasic._nick[i] )
				{
				case 0 : i = NET_NICK_NAME_SIZE;						break;  //끝
				case 38: strcat(TempNick, "&amp;");		End = End + 5;		break;	//&
				case 60: strcat(TempNick, "&lt;");		End = End + 4;		break;	//<
				case 62: strcat(TempNick, "&gt;");		End = End + 4; 		break;	//>
				case 34: strcat(TempNick, "&quot;");	End = End + 6; 		break;	//"
				case 39: strcat(TempNick, "&apos;");	End = End + 6; 		break;	//'
				default: 
					TempNick[End] = pSession->m_UserInfoBasic._nick[i]; 
					End++; 
					break; 
				}
			}
			TempNick[End] = 0;	
			sprintf( pTempBuffer, "<cmd action=\"CMD_UpdateCharacterRecordInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><charName v=\"%s\"/><exp v=\"%d\"/><win v=\"%d\"/><defeat v=\"%d\"/><kill v=\"%d\"/><death v=\"%d\"/><headshot v=\"%d\"/><genderId v=\"%d\"/><teamId v=\"%d\"/><classCodeId v=\"%d\"/><channel v=\"0\"/><clanContributionExp v=\"0\"/><loginStatus v=\"%d\"/><localTime v=\"%d\"/></param></cmd>", \
				pSession->m_DBIdx, TempNick, pSession->m_UserInfoBasic._exp, pSession->m_UserInfoRecord._swin, pSession->m_UserInfoRecord._slose, pSession->m_UserInfoRecord._skillcount, pSession->m_UserInfoRecord._sdeath, pSession->m_UserInfoRecord._sheadshot, pSession->m_GIP_LastUseGender, pSession->m_GIP_LastUseTeam, pSession->m_UserInfoBasic._rank + 1, Arg, iShowtime);
		}
		break; 
	case GIP_MESSAGE_USER		:	//CMD_UpdateUserInfo
		sprintf( pTempBuffer, "<cmd action=\"CMD_UpdateUserInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><userName v=\"\"/><gamePoint v=\"%d\"/><cashPoint v=\"%d\"/><loginStatus v=\"%d\"/><localTime v=\"%d\"/></param></cmd>", \
			pSession->m_DBIdx, pSession->m_UserInfoBasic._point, pSession->m_UserInfoBasic._cash, Arg, iShowtime);
		break; 
	case GIP_MESSAGE_MEDAL_MINI			:	//CMD_InsertCharacterPropertyInfo
		sprintf( pTempBuffer, "<cmd action=\"CMD_InsertCharacterPropertyInfo\" version=\"2.0\"><param><uid v=\"%d\"/><charId v=\"0\"/><serverId v=\"1\"/><propertyId v=\"4\"/><propertySid v=\"%d\"/><localTime v=\"%d\"/></param></cmd>", \
            pSession->m_DBIdx, Arg, iShowtime);
		break; 
	case GIP_MESSAGE_MEDAL_INSI			:	//CMD_InsertCharacterPropertyInfo
		sprintf( pTempBuffer, "<cmd action=\"CMD_InsertCharacterPropertyInfo\" version=\"2.0\"><param><uid v=\"%d\"/><charId v=\"0\"/><serverId v=\"1\"/><propertyId v=\"5\"/><propertySid v=\"%d\"/><localTime v=\"%d\"/></param></cmd>", \
            pSession->m_DBIdx, Arg, iShowtime);
		break;
	case GIP_MESSAGE_MEDAL_MEDA			:	//CMD_InsertCharacterPropertyInfo
		sprintf( pTempBuffer, "<cmd action=\"CMD_InsertCharacterPropertyInfo\" version=\"2.0\"><param><uid v=\"%d\"/><charId v=\"0\"/><serverId v=\"1\"/><propertyId v=\"6\"/><propertySid v=\"%d\"/><localTime v=\"%d\"/></param></cmd>", \
            pSession->m_DBIdx, Arg, iShowtime);
		break; 
	case GIP_MESSAGE_CHALLENGE			:	//CMD_InsertCharacterPropertyInfo
		sprintf( pTempBuffer, "<cmd action=\"CMD_InsertCharacterPropertyInfo\" version=\"2.0\"><param><uid v=\"%d\"/><charId v=\"0\"/><serverId v=\"1\"/><propertyId v=\"7\"/><propertySid v=\"%d\"/><localTime v=\"%d\"/></param></cmd>", \
            pSession->m_DBIdx, Arg + 1, iShowtime);
		break; 	
	case GIP_MESSAGE_ENTER_CHANNEL		:	//CMD_UpdateCharacterChannel
		sprintf( pTempBuffer, "<cmd action=\"CMD_UpdateCharacterChannel\" version=\"2.0\"><param><uid v=\"%d\"/><charId v=\"0\"/><serverId v=\"1\"/><channel v=\"%d\"/><localTime v=\"%d\"/></param></cmd>", \
			pSession->m_DBIdx, ((g_pContextMain->m_ServerIdx * 10) + (pSession->m_ChannelNum + 1)), iShowtime);
		break; 
	default: return; break; 
	}	

	sConvertMultiToUTF8(&pSendBuffer[BufferIdx]._Packet.commandList.command[0], 4096, pTempBuffer); 
	pSendBuffer[BufferIdx]._Packet.commandList.command_len	= i3String::Length(&pSendBuffer[BufferIdx]._Packet.commandList.command[0]); 
	pSendBuffer[BufferIdx]._Packet.header.length			= htons( (unsigned short)(sizeof(Packet_Header) + 4 + pSendBuffer[BufferIdx]._Packet.commandList.command_len) ); 
	pSendBuffer[BufferIdx]._Packet.commandList.command_len	= htons( pSendBuffer[BufferIdx]._Packet.commandList.command_len ); 

	//버퍼링을 위해서 저장한다.
	pSendBuffer[BufferIdx]._BackUpInfo._iDataBaseID	= pSession->m_DBIdx; 	
	i3mem::Copy( &pSendBuffer[BufferIdx]._BackUpInfo._UserInfoBasic,	&pSession->m_UserInfoBasic,		sizeof(USER_INFO_BASIC) ); 
	i3mem::Copy( &pSendBuffer[BufferIdx]._BackUpInfo._UserInfoRecord,	&pSession->m_UserInfoRecord, 	sizeof(USER_INFO_RECORD)); 

	//3. Add WritePos 
	*pWritePos = *pWritePos + 1;

	return; 
}

INT32 CModuleContextNC::_GipMessage(GIP_SEND_MESSAGE * pMessage)
{
	//pMessage
	pMessage->_Packet.header.pid = m_pGipModuleSocket->m_iPId;		//형변환을 하지 않는다. 
	pMessage->_Packet.header.qid = m_iGipQId;						//형변환을 하지 않는다. 	

	//버퍼링을 한다. 
	m_pGipBackUpBuffer[m_iGipQId % GIP_BACKUP_COUNT]._iDataBaseID	= pMessage->_BackUpInfo._iDataBaseID; 
	i3mem::Copy( &m_pGipBackUpBuffer[m_iGipQId % GIP_BACKUP_COUNT]._UserInfoBasic,	&pMessage->_BackUpInfo._UserInfoBasic, sizeof(USER_INFO_BASIC)); 
	i3mem::Copy( &m_pGipBackUpBuffer[m_iGipQId % GIP_BACKUP_COUNT]._UserInfoRecord,	&pMessage->_BackUpInfo._UserInfoRecord, sizeof(USER_INFO_RECORD)); 

	m_iGipQId++;
	if(m_iGipQId == 60000)m_iGipQId = 1; 

	//Send
	
	return m_pGipModuleSocket->SendPacketMessage((char *)&pMessage->_Packet, ntohs(pMessage->_Packet.header.length) );
}

void CModuleContextNC::_GipSendReqMessage(UINT32 Idx)
{
	GIP_BACKUP	* pBackUpBuffer = &m_pGipBackUpBuffer[ m_pGipModuleSocket->m_RequestInfo[Idx]._Qid % GIP_BACKUP_COUNT ]; 

	GIP_SEND_MESSAGE GipMessage; 
	char pTempBuffer[4096]; 
	time_t iShowtime; 
	time(&iShowtime); 	

	//1. 요청한 패킷 보내기 
	GipMessage._Packet.header.type				= 0x13; 	
	GipMessage._Packet.header.version			= 1; 
	GipMessage._Packet.header.headerLen			= sizeof(Packet_Header); 
	GipMessage._Packet.header.crc				= 0; 	
	GipMessage._Packet.commandList.commandCrc	= 0; 
	GipMessage._Packet.commandList.commandType	= 1; 

	GipMessage._Packet.header.pid = m_pGipModuleSocket->m_iPId;	//형변환을 하지 않는다. 
	GipMessage._Packet.header.qid = 0;							//형변환을 하지 않는다. 

	switch( m_pGipModuleSocket->m_RequestInfo[Idx]._Type )
	{
	case 1:  //Req 1	//유저 인포 
		{			
			//Send Insert			------1
			sprintf( pTempBuffer, "<cmd action=\"CMD_InsertUserInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><userName v=\"\"/><gamePoint v=\"0\"/><cashPoint v=\"0\"/><localTime v=\"%d\"/></param></cmd>", pBackUpBuffer->_iDataBaseID, iShowtime);
			sConvertMultiToUTF8(&GipMessage._Packet.commandList.command[0], 4096, pTempBuffer);
			GipMessage._Packet.commandList.command_len = i3String::Length(&GipMessage._Packet.commandList.command[0]); 
			GipMessage._Packet.header.length			= htons( (unsigned short)(sizeof(Packet_Header) + 4 + GipMessage._Packet.commandList.command_len) ); 
			GipMessage._Packet.commandList.command_len	= htons( GipMessage._Packet.commandList.command_len ); 
			m_pGipModuleSocket->SendPacketMessage((char *)&GipMessage._Packet, ntohs(GipMessage._Packet.header.length) );

			//Send Update			----2
			sprintf( pTempBuffer, "<cmd action=\"CMD_UpdateUserInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><userName v=\"\"/><gamePoint v=\"%d\"/><cashPoint v=\"%d\"/><loginStatus v=\"0\"/><localTime v=\"%d\"/></param></cmd>", pBackUpBuffer->_iDataBaseID, pBackUpBuffer->_UserInfoBasic._point, pBackUpBuffer->_UserInfoBasic._cash, iShowtime);
			sConvertMultiToUTF8(&GipMessage._Packet.commandList.command[0], 4096, pTempBuffer);
			GipMessage._Packet.commandList.command_len = i3String::Length(&GipMessage._Packet.commandList.command[0]); 
			GipMessage._Packet.header.length			= htons( (unsigned short)(sizeof(Packet_Header) + 4 + GipMessage._Packet.commandList.command_len) ); 
			GipMessage._Packet.commandList.command_len	= htons( GipMessage._Packet.commandList.command_len ); 
			m_pGipModuleSocket->SendPacketMessage((char *)&GipMessage._Packet, ntohs(GipMessage._Packet.header.length) );
		}
		break; 
	case 2:  //Req 2	//캐릭터 인포 
		{
			//Send Insert			------1
			char	TempNick[SERVER_STRING_COUNT]; 
			INT32	End = 0; 
			TempNick[End] = 0; 
			for( INT32 i= 0; i < NET_NICK_NAME_SIZE;i++ )
			{
				switch ( pBackUpBuffer->_UserInfoBasic._nick[i] )
				{
				case 0 : i = NET_NICK_NAME_SIZE;						break; //끝
				case 38: strcat(TempNick, "&amp;");		End = End + 5;		break;	//&
				case 60: strcat(TempNick, "&lt;");		End = End + 4;		break;	//<
				case 62: strcat(TempNick, "&gt;");		End = End + 4; 		break;	//>
				case 34: strcat(TempNick, "&quot;");	End = End + 6; 		break;	//"
				case 39: strcat(TempNick, "&apos;");	End = End + 6; 		break;	//'
				default: 
					TempNick[End] = pBackUpBuffer->_UserInfoBasic._nick[i]; 
					End++; 
					break; 
				}
			}
			TempNick[End] = 0;
			sprintf( pTempBuffer, "<cmd action=\"CMD_InsertCharacterInfo\" version=\"2.0\"><param><uid v=\"%d\"/><charId v=\"0\"/><serverId v=\"1\"/><charName v=\"%s\"/><genderId v=\"1\"/><teamId v=\"1\"/><classCodeId v=\"0\"/><localTime v=\"%d\"/></param></cmd>", pBackUpBuffer->_iDataBaseID, TempNick, iShowtime);
			sConvertMultiToUTF8(&GipMessage._Packet.commandList.command[0], 4096, pTempBuffer);
			GipMessage._Packet.commandList.command_len = i3String::Length(&GipMessage._Packet.commandList.command[0]); 
			GipMessage._Packet.header.length			= htons( (unsigned short)(sizeof(Packet_Header) + 4 + GipMessage._Packet.commandList.command_len) ); 
			GipMessage._Packet.commandList.command_len	= htons( GipMessage._Packet.commandList.command_len ); 
			m_pGipModuleSocket->SendPacketMessage((char *)&GipMessage._Packet, ntohs(GipMessage._Packet.header.length) );	

			//Send				-------------2
			sprintf( pTempBuffer, "<cmd action=\"CMD_UpdateCharacterRecordInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><charName v=\"%s\"/><exp v=\"%d\"/><win v=\"%d\"/><defeat v=\"%d\"/><kill v=\"%d\"/><death v=\"%d\"/><headshot v=\"%d\"/><genderId v=\"1\"/><teamId v=\"1\"/><classCodeId v=\"%d\"/><channel v=\"0\"/><clanContributionExp v=\"0\"/><loginStatus v=\"0\"/><localTime v=\"%d\"/></param></cmd>", \
				pBackUpBuffer->_iDataBaseID, TempNick, pBackUpBuffer->_UserInfoBasic._exp, pBackUpBuffer->_UserInfoRecord._swin, pBackUpBuffer->_UserInfoRecord._slose, pBackUpBuffer->_UserInfoRecord._skillcount, pBackUpBuffer->_UserInfoRecord._sdeath, pBackUpBuffer->_UserInfoRecord._sheadshot, pBackUpBuffer->_UserInfoBasic._rank + 1, iShowtime);
			sConvertMultiToUTF8(&GipMessage._Packet.commandList.command[0], 4096, pTempBuffer);
			GipMessage._Packet.commandList.command_len = i3String::Length(&GipMessage._Packet.commandList.command[0]); 
			GipMessage._Packet.header.length			= htons( (unsigned short)(sizeof(Packet_Header) + 4 + GipMessage._Packet.commandList.command_len) ); 
			GipMessage._Packet.commandList.command_len	= htons( GipMessage._Packet.commandList.command_len ); 
			m_pGipModuleSocket->SendPacketMessage((char *)&GipMessage._Packet, ntohs(GipMessage._Packet.header.length) );            
		}
		break; 
	}
	return; 
}

void CModuleContextNC::_GipSendGreetingMessage(void)
{		
	//Set Header 
	Packet_TYPE_GREETING	m_GreetingPacket;
	m_GreetingPacket.header.length		= htons(sizeof(Packet_TYPE_GREETING) ); 
	m_GreetingPacket.header.type		= 0x12; 
	m_GreetingPacket.header.version		= 1; 
	m_GreetingPacket.header.headerLen	= sizeof(Packet_Header); 
	m_GreetingPacket.header.crc			= 0; 
	m_GreetingPacket.header.pid			= 0; 
	m_GreetingPacket.header.qid			= 0;
	m_GreetingPacket.game_id			= htons(34);
	m_GreetingPacket.server_id			= htons(g_pContextMain->m_ServerIdx);
	m_GreetingPacket.sub_server_id		= 0;

	m_pGipModuleSocket->SendPacketMessage((char*)&m_GreetingPacket, sizeof(Packet_TYPE_GREETING)); 

	m_iGipQId = 1; //QId Setting
    return; 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//LOGD
#define LOG_CHECKVALID_IDX	if(!g_pContextMain->m_bLogDActive)return;  \
							LOG_SEND_MESSAGE	* pSendBuffer; \
							INT32				* pWritePos; \
							INT32				* pReadPos; \
							UINT32				BufferIdx; \
							switch( ThreadIdx )\
							{\
							case	MAIN_THREAD_IDX		: \
								pSendBuffer = m_pLogMainMessage; \
								pWritePos	= &m_iLogMain_WritePos;\
								pReadPos	= &m_iLogMain_ReadPos; \
								break; \
							case	MODULE_ONE_THREAD_IDX	: \
								pSendBuffer = m_pLogAuthMessage; \
								pWritePos	= &m_iLogAuth_WritePos;\
								pReadPos	= &m_iLogAuth_ReadPos;\
								break; \
							default	: \
								pSendBuffer = m_ppLogUserMessage[ThreadIdx];\
								pWritePos	= &m_pLogUser_WritePos[ThreadIdx]; \
								pReadPos	= &m_pLogUser_ReadPos[ThreadIdx]; \
								break; \
							}\
							if(*pWritePos == *pReadPos)\
							{\
								*pWritePos	= 0; \
								*pReadPos	= 0; \
							}\
							if( *pReadPos > MODULE_CONTEXT_MAX_SEND_IDX_COUNT ) \
							{\
								WRITE_LOG(NULL,"LogD Send Buffer Full");\
								return; \
							}\
							if( (*pWritePos - *pReadPos) >= LOG_SEND_BUFFER_COUNT )\
							{\
								WRITE_LOG(NULL,"LogD Send Buffer Overflow");\
								return; \
							}\
							BufferIdx = *pWritePos % LOG_SEND_BUFFER_COUNT; \
							char strDate[SERVER_STRING_COUNT]; 	\
							sprintf(strDate, "%02d/%02d/%d %02d:%02d:%02d.00", g_pContextMain->m_Month, g_pContextMain->m_Day, g_pContextMain->m_Year, g_pContextMain->m_Hour, g_pContextMain->m_Minute, g_pContextMain->m_Second); 

BOOL CModuleContextNC::_LogCreate(void)
{
	if(!g_pContextMain->m_bLogDActive)return TRUE; //사용하지 않음 

	//1. Create Work Send Buffer
	m_ppLogUserMessage	= (LOG_SEND_MESSAGE	**)i3MemAlloc(m_ThreadCount * sizeof(LOG_SEND_MESSAGE*)); 
	m_pLogUser_WritePos = (INT32*)i3MemAlloc(m_ThreadCount * sizeof(INT32));
	m_pLogUser_ReadPos	= (INT32*)i3MemAlloc(m_ThreadCount * sizeof(INT32));
	for(UINT32 i = 0; i < m_ThreadCount; i++)
	{
		m_ppLogUserMessage[i]	= (LOG_SEND_MESSAGE	*)i3MemAlloc(sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT);		
		i3mem::FillZero( m_ppLogUserMessage[i], sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT); 
		for(UINT32 j = 0; j < LOG_SEND_BUFFER_COUNT; j++)
		{
			m_ppLogUserMessage[i][j]._szMessage[3] = 3;
		}

		m_pLogUser_WritePos[i]	= 0; 
		m_pLogUser_ReadPos[i]	= 0; 
	}

	//2. Create Main Send Buffer 
	m_pLogMainMessage	= (LOG_SEND_MESSAGE	*)i3MemAlloc(sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT);	// Main Thread 
	i3mem::FillZero( m_pLogMainMessage, sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT ); 
	for(UINT32 j = 0; j < LOG_SEND_BUFFER_COUNT; j++)
	{
		m_pLogMainMessage[j]._szMessage[3] = 3;
	}
	m_iLogMain_WritePos	= 0;
	m_iLogMain_ReadPos	= 0;

	//3. Create Module Send Buffer 
	m_pLogAuthMessage	= (LOG_SEND_MESSAGE	*)i3MemAlloc(sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT);	// Module Thread 
	i3mem::FillZero( m_pLogAuthMessage, sizeof(LOG_SEND_MESSAGE) * LOG_SEND_BUFFER_COUNT ); 
	for(UINT32 j = 0; j < LOG_SEND_BUFFER_COUNT; j++)
	{
		m_pLogMainMessage[j]._szMessage[3] = 3;
	}
	m_iLogAuth_WritePos	= 0;
	m_iLogAuth_ReadPos	= 0;

	BOOL Rv; 

	//4. Create Module
	m_pLogModuleSocket = CModuleLogSocket::NewObject(); 
	if( m_pLogModuleSocket->OnConnect( g_pContextMain->m_LogDServerIp, g_pContextMain->m_LogDServerPort) )
	{
		Rv = TRUE; 
		_LogSendInitMessage();	
		WRITE_LOG(NULL, "LOG MODULE CONNECT SUCCESS"); 
	}
	else
	{
		Rv = FALSE; 
		WRITE_LOG(NULL, "LOG MODULE CONNECT FAIL"); 
	}

	return Rv; 
}

//CHECK_STATUS -> STARTED -> SEND_MSG
void CModuleContextNC::_LogSendInitMessage(void)
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
	Temp		= 34; 
	i3mem::Copy( &pSendBuffer[3], &Temp,		sizeof(UINT32)); 
	Temp		= 3; 
	i3mem::Copy( &pSendBuffer[7], &Temp,		sizeof(UINT32)); 
	SendSize = PacketSize;
	m_pLogModuleSocket->SendPacketMessage(pSendBuffer, SendSize);		// STARTED

	return; 
}

INT32 CModuleContextNC::_LogUpdate(void)
{
	INT32	SendCount; 
	UINT32	ReadIdx;
	UINT32	SendSize;
	UINT8	SendFailCount = 0; 
	BOOL	IsSend;
	IsSend = FALSE; 
	SendSize = 0;
	UINT32 iSendCompleteCount = 0; 

	if(!m_pLogModuleSocket->IsConnected()) 
	{			
		m_pLogModuleSocket->OnDestroy(); 
		if( m_pLogModuleSocket->OnConnect(g_pContextMain->m_LogDServerIp, g_pContextMain->m_LogDServerPort ) )
		{
			_LogSendInitMessage();
			WRITE_LOG(NULL,"SUCCESS LOG_MODULE RECONNECT 1");
		}
		else
		{
			m_pLogModuleSocket->OnDestroy(); 
			WRITE_LOG(NULL,"FAIL LOG_MODULE RECONNECT 1");
		}
	}

	//Main Thread 
 	SendCount = m_iLogMain_WritePos - m_iLogMain_ReadPos; 
	if(SendCount > 0)
	{
		for(INT32 i = 0; i < SendCount; i++)
		{
			if(iSendCompleteCount < MODULE_CONTEXTB_SEND_COUNT)
			{
				ReadIdx = m_iLogMain_ReadPos % LOG_SEND_BUFFER_COUNT; 
				SendSize += m_pLogModuleSocket->SendPacketMessage( m_pLogMainMessage[ReadIdx]._szMessage, m_pLogMainMessage[ReadIdx]._Size ); 
				m_iLogMain_ReadPos++;
				iSendCompleteCount++; 
			}
		}
		IsSend = TRUE; 
	}

	//Auth Module Thread
	SendCount = m_iLogAuth_WritePos - m_iLogAuth_ReadPos; 
	if(SendCount > 0)
	{
		for(INT32 i = 0; i < SendCount; i++)
		{
			if(iSendCompleteCount < MODULE_CONTEXTB_SEND_COUNT)
			{
				ReadIdx = m_iLogAuth_ReadPos % LOG_SEND_BUFFER_COUNT; 
				SendSize += m_pLogModuleSocket->SendPacketMessage( m_pLogAuthMessage[ReadIdx]._szMessage, m_pLogAuthMessage[ReadIdx]._Size ); 
				m_iLogAuth_ReadPos++; 
				iSendCompleteCount++; 
			}
		}
		IsSend = TRUE; 
	}

	//User Thread 
	for(UINT32 i = 0; i < m_ThreadCount; i++)
	{
		LOG_SEND_MESSAGE * pLogBuffer = m_ppLogUserMessage[i];
		SendCount = m_pLogUser_WritePos[i] - m_pLogUser_ReadPos[i];

		if(SendCount > 0)
		{
			for(INT32 j = 0; j < SendCount; j++)
			{
				if(iSendCompleteCount < MODULE_CONTEXTB_SEND_COUNT)
				{
					ReadIdx = m_pLogUser_ReadPos[i] % LOG_SEND_BUFFER_COUNT; 
					SendSize += m_pLogModuleSocket->SendPacketMessage( pLogBuffer[ReadIdx]._szMessage, pLogBuffer[ReadIdx]._Size );
					m_pLogUser_ReadPos[i]++; 
					iSendCompleteCount++; 
				}
			}
			IsSend = TRUE; 
		}
	}

	m_pLogModuleSocket->SelectEvent();  

	if( (IsSend == TRUE)&& (SendSize ==  0) )
	{
		SendFailCount++;
		if(SendFailCount > 1)
		{
			m_pLogModuleSocket->OnDestroy(); 
			if( m_pLogModuleSocket->OnConnect(g_pContextMain->m_LogDServerIp, g_pContextMain->m_LogDServerPort ) )
			{
				WRITE_LOG(NULL,"SUCESS LOG_MODULE RECONNECT 2"); 
				_LogSendInitMessage(); 
				SendFailCount = 0; 
			}
			else
			{
				m_pLogModuleSocket->OnDestroy(); 
				WRITE_LOG(NULL,"FAIL LOG_MODULE RECONNECT 2"); 
			}
		}
	}

	return iSendCompleteCount; 
}

void CModuleContextNC::_LogDestroy(void)
{
	if(!g_pContextMain->m_bLogDActive)return; 
	for(UINT32 i = 0; i < m_ThreadCount; i++)
	{
		I3MEM_SAFE_FREE( m_ppLogUserMessage[i] );
	}
	I3MEM_SAFE_FREE( m_ppLogUserMessage ); 
	I3MEM_SAFE_FREE( m_pLogUser_WritePos); 
	I3MEM_SAFE_FREE( m_pLogUser_ReadPos ); 
	I3MEM_SAFE_FREE( m_pLogMainMessage ); 
	I3MEM_SAFE_FREE( m_pLogAuthMessage ); 
	I3_SAFE_RELEASE( m_pLogModuleSocket ); 
	return; 
}

void CModuleContextNC::LogSendMessage( INT32 ThreadIdx, LOG_MESSAGE LogId, CUserSession * pSession, CRoom * pRoom )
{
	LOG_CHECKVALID_IDX;

	//2. Set Buffer 
	char strTemp[SERVER_STRING_COUNT];
	INT32 StringSize;	
	switch(LogId)
	{
	case LOG_MESSAGE_SERVER_START	:		//서버 시작 
		pSendBuffer[BufferIdx]._szMessage[7] = 7;
		StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX], "%s,101,,,,,,%s,%s,,,\r\n",strDate , MACHINE_NAME, SERVER_TYPE); 
		break; 
	case LOG_MESSAGE_SERVER_END		:		//서버 종료 
		pSendBuffer[BufferIdx]._szMessage[7] = 7;
		StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX], "%s,102,,,,,,%s,%s,,,\r\n",strDate , MACHINE_NAME, SERVER_TYPE); 
		break; 
	case LOG_MESSAGE_SERVER_STATE	:		//서버 상태			
		pSendBuffer[BufferIdx]._szMessage[7] = 7;
		StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX], "%s,103,%d,%d,0,%d,,%s,%s,,,,\r\n",strDate , g_pServerContext->GetSessionCount(), g_pContextMain->m_iCpuQuery, g_pContextMain->m_iHandleQuery,MACHINE_NAME, SERVER_TYPE); 
		break; 
	case LOG_MESSAGE_SERVER_CHANNEL_USERCOUNT:	//채널 유저 카운트 
		pSendBuffer[BufferIdx]._szMessage[7] = 7;		
		StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX], "%s,202,%d,%d,,,,,,,,\r\n",strDate , g_pUserManager->GetChannelUserCount(g_pContextMain->m_iLogUpdateChannel),g_pContextMain->m_iLogUpdateChannel); 
		break; 
	case LOG_MESSAGE_U_LOGIN		:		//로그 인 
		pSendBuffer[BufferIdx]._szMessage[7] = 6;
		sprintf( strTemp, "%d/%d/%d/%d", pSession->m_UserInfoRecord._match, pSession->m_UserInfoRecord._win, pSession->m_UserInfoRecord._lose, pSession->m_UserInfoRecord._draw ); 
		StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,1001,%d,,,,,%d,,%d,%d,,,,,,,%d,%d,%d,,,,,,,,%s,%s,%s,,,%s,,,%d,%d\r\n",strDate , pSession->m_DBIdx, pSession->m_UserInfoBasic._rank, pSession->m_iAccConnectCount, pSession->m_iAccConnectTime, pSession->m_UserInfoBasic._point, pSession->m_UserInfoBasic._cash, pSession->m_UserInfoBasic._exp, pSession->m_UserInfoBasic._nick, pSession->m_Id,strTemp, pSession->GetIpString(), pSession->m_UserInfoBasic._pcCafe, pSession->m_SessionIdx); 
		break; 
	case LOG_MESSAGE_U_LOGOUT		:		//로그 아웃 
		pSendBuffer[BufferIdx]._szMessage[7] = 6;
		sprintf( strTemp, "%d/%d/%d/%d", pSession->m_UserInfoRecord._match, pSession->m_UserInfoRecord._win, pSession->m_UserInfoRecord._lose, pSession->m_UserInfoRecord._draw ); 
		if(pSession->m_iIsKillST == 0)
			StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,1002,%d,,,,,%d,,0,,,,,,,,%d,%d,%d,,,,,,,,%s,%s,%s,,,%s,,,%d,%d\r\n",strDate , pSession->m_DBIdx, pSession->m_UserInfoBasic._rank, pSession->m_UserInfoBasic._point, pSession->m_UserInfoBasic._cash, pSession->m_UserInfoBasic._exp, pSession->m_UserInfoBasic._nick, pSession->m_Id, strTemp, pSession->GetIpString(), pSession->m_UserInfoBasic._pcCafe, pSession->m_SessionIdx); 
		else
			StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,1002,%d,,,,,%d,,1,,,,,,,,%d,%d,%d,,,,,,,,%s,%s,%s,,,%s,,,%d,%d\r\n",strDate , pSession->m_DBIdx, pSession->m_UserInfoBasic._rank, pSession->m_UserInfoBasic._point, pSession->m_UserInfoBasic._cash, pSession->m_UserInfoBasic._exp, pSession->m_UserInfoBasic._nick, pSession->m_Id, strTemp, pSession->GetIpString(), pSession->m_UserInfoBasic._pcCafe, pSession->m_SessionIdx); 
		break; 
	case LOG_MESSAGE_U_ENTER_CHANNEL:		//채널 입장 
		pSendBuffer[BufferIdx]._szMessage[7] = 6;
		StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,1003,%d,%d,,,,%d,,,,,,,,,,,,,,,,,,,,%s,%s,,,,,,,,%d\r\n",strDate , pSession->m_DBIdx, pSession->m_ChannelNum, pSession->m_UserInfoBasic._rank, pSession->m_UserInfoBasic._nick, pSession->m_Id,pSession->m_SessionIdx); 		
		break; 
	case LOG_MESSAGE_U_LEAVE_CHANNEL:		//채널 퇴장
		pSendBuffer[BufferIdx]._szMessage[7] = 6;
		StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,1004,%d,%d,,,,%d,,,,,,,,,,,,,,,,,,,,%s,%s,,,,,,,,%d\r\n",strDate , pSession->m_DBIdx, pSession->m_ChannelNum, pSession->m_UserInfoBasic._rank, pSession->m_UserInfoBasic._nick, pSession->m_Id,pSession->m_SessionIdx); 
		break; 
	case LOG_MESSAGE_U_R_CREATE_ROOM	:	//방 입장 - 만들기 
		pSendBuffer[BufferIdx]._szMessage[7] = 6;
		StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,1007,%d,%d,,%d,%d,%d,,1,0,,,,,,,,,,,,,,,,,%s,%s,,%s,,,,,,%d\r\n",strDate , pSession->m_DBIdx, pSession->m_ChannelNum, pRoom->m_UniqueNumber, pRoom->m_BattleCount, pSession->m_UserInfoBasic._rank, pSession->m_UserInfoBasic._nick, pSession->m_Id, pRoom->m_InfoBasic._Title, pSession->m_SessionIdx); 
		break; 
	case LOG_MESSAGE_U_R_ENTER_ROOM		:	//방 입장 
		pSendBuffer[BufferIdx]._szMessage[7] = 6;
		StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,1007,%d,%d,,%d,%d,%d,,0,0,,,,,,,,,,,,,,,,,%s,%s,,%s,,,,,,%d\r\n",strDate , pSession->m_DBIdx, pSession->m_ChannelNum, pRoom->m_UniqueNumber, pRoom->m_BattleCount, pSession->m_UserInfoBasic._rank, pSession->m_UserInfoBasic._nick, pSession->m_Id, pRoom->m_InfoBasic._Title,  pSession->m_SessionIdx); 
		break; 
	case LOG_MESSAGE_U_R_KICK_LEAVE_ROOM:	//방에서 나가기 - 강퇴
		pSendBuffer[BufferIdx]._szMessage[7] = 6;
		StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,1009,%d,%d,,%d,%d,%d,,1,0,,,,,,,,,,,,,,,,,%s,%s,,%s,,,,,,%d\r\n",strDate , pSession->m_DBIdx, pSession->m_ChannelNum, pRoom->m_UniqueNumber, pRoom->m_BattleCount, pSession->m_UserInfoBasic._rank, pSession->m_UserInfoBasic._nick, pSession->m_Id, pRoom->m_InfoBasic._Title, pSession->m_SessionIdx); 
		break; 
	case LOG_MESSAGE_U_R_LEAVE_ROOM		:	//방에서 나가기 
		pSendBuffer[BufferIdx]._szMessage[7] = 6;
		StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,1008,%d,%d,,%d,%d,%d,,%d,,,,,,,,,,,,,,,,,,%s,%s,,%s,,,,,,%d\r\n",strDate , pSession->m_DBIdx, pSession->m_ChannelNum, pRoom->m_UniqueNumber, pRoom->m_BattleCount, pSession->m_UserInfoBasic._rank, pSession->m_bForceRoomLeave, pSession->m_UserInfoBasic._nick, pSession->m_Id, pRoom->m_InfoBasic._Title, pSession->m_SessionIdx); 
		break; 
	case LOG_MESSAGE_U_CREATE_CHARA		:	//캐릭터 생성 
		pSendBuffer[BufferIdx]._szMessage[7] = 6;
		StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,2001,%d,,,,,,,,,,,,,,,,,,,,,,,,,%s,%s,,,,,,,,\r\n",strDate , pSession->m_DBIdx, pSession->m_UserInfoBasic._nick, pSession->m_Id); 
		break; 
	case LOG_MESSAGE_U_LEVEL_UP			:	//캐릭터 레벨 업
		pSendBuffer[BufferIdx]._szMessage[7] = 6;
		StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,2004,%d,,,,,%d,,%d,%d,,,,,,,,,%d,,,,,,,,%s,%s,,,,,,,,\r\n",strDate , pSession->m_DBIdx, pSession->m_UserInfoBasic._rank, pSession->m_iAccConnectCount, pSession->m_iAccConnectTime, pSession->m_UserInfoBasic._exp, pSession->m_UserInfoBasic._nick, pSession->m_Id); 
		break; 
	case LOG_MESSAGE_U_QUEST			:	//임무카드 
		pSendBuffer[BufferIdx]._szMessage[7] = 6;
		StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,20013,%d,,%d,%d,,%d,,,,,%d,,,,,%d,%d,,,,,,%d,,%d,,,%s,%s,,,,,,,%d\r\n",
			strDate, pSession->m_DBIdx, pSession->m_ChannelNum, 0, pSession->m_UserInfoBasic._rank, 0,pSession->m_UserInfoBasic._point, pSession->m_UserInfoBasic._exp, pSession->m_GetQuestPoint, pSession->m_GetQuestExp, pSession->m_UserInfoBasic._nick, pSession->m_Id, pSession->m_GetQuestCardSet); 
		break; 
	case LOG_MESSAGE_R_BATTLE_START_ROOM	: //배틀 시작 
		{
			INT8	GameType; 
			INT8	PrivateRoom; 
			INT8	ReadTeamCount; 
			INT8	BlueTeamCount;	
			UINT32	EndKill; 
			UINT32	EndTime; 
			char	ReadTeamNick[SERVER_STRING_COUNT]; 
			char	BlueTeamNick[SERVER_STRING_COUNT]; 		
			const char	* pStageName; 

			switch( GET_STAGETYPE( pRoom->m_InfoBasic._StageID ) )
			{			
			case STAGE_TYPE_DEATHMATCH		: 
				GameType		= 1; 
				EndKill			= g_TeamKillCountTable[   pRoom->m_InfoAdd._SubType & 0x0F ]; 
				EndTime			= g_TeamTimeTable[ (pRoom->m_InfoAdd._SubType & 0xF0) >> 4 ];
				break; 
			case STAGE_TYPE_4VS4			:
			case STAGE_TYPE_BOMB			: 
				GameType		= 2; 
				EndKill			= g_MissionRoundCountTable[ pRoom->m_InfoAdd._SubType ]; //미션 카운트 
				EndTime			= g_MissionRoundTimeTable[ pRoom->m_MiRoundTimeIdx ]; 				
				break; 
			case STAGE_TYPE_DESTROY			: 
			default							: 
				GameType		= 2; 
				EndKill			= g_MissionRoundCountTable[ pRoom->m_InfoAdd._SubType ]; //미션 카운트 
				EndTime			= g_MissionRoundTimeTable[ pRoom->m_MiRoundTimeIdx ]; 				
				break; 
			}

			pStageName		= getStageName( (STAGE_ID)pRoom->m_InfoBasic._StageID ); 

			EndTime		= (UINT32)(EndTime / 60); 

			INT32 iLogStageIdx = _GetLogStageIndex(pRoom->m_InfoBasic._StageID); 

			if( pRoom->m_InfoBasic._StageID & STAGE_FLAG_PRIVATE )	PrivateRoom = 1;
			else													PrivateRoom = 0;

			//Get Count And Nick 
			pRoom->GetRoomInfo(&ReadTeamCount, &BlueTeamCount, ReadTeamNick, BlueTeamNick);  

			pSendBuffer[BufferIdx]._szMessage[7] = 6;		
			StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,7001,0,%d,,%d,%d,%d,,%d,%d,%d,%d,%d,%d,%d,,,,,,,,,,,,,,,%s,%s,%s,%d,%s,%s,%d\r\n",strDate ,\
				pRoom->m_ChannelNum, pRoom->m_UniqueNumber, pRoom->m_BattleCount, pRoom->m_MainUserDbIdx, iLogStageIdx, GameType, pRoom->m_InfoBasic._PersonMax, PrivateRoom, ReadTeamCount, BlueTeamCount, EndTime, pRoom->m_InfoBasic._Title,  pStageName , pRoom->m_InfoAdd._Nick, EndKill, ReadTeamNick, BlueTeamNick, pRoom->m_MainUserDbIdx ); 
		}
		break; 
	case LOG_MESSAGE_R_BATTLE_END_ROOM	: //배틀 종료 
		{
			INT8	ReadTeamCount;
			INT8	BlueTeamCount;
			char	ReadTeamNick[SERVER_STRING_COUNT]; 
			char	BlueTeamNick[SERVER_STRING_COUNT]; 		

			//Get Count And Nick 
			pRoom->GetRoomInfo(&ReadTeamCount, &BlueTeamCount, ReadTeamNick, BlueTeamNick);  

			const char	* pStageName = getStageName( (STAGE_ID)pRoom->m_InfoBasic._StageID );

			INT32 iLogStageIdx = _GetLogStageIndex(pRoom->m_InfoBasic._StageID); 

			pSendBuffer[BufferIdx]._szMessage[7] = 6;		
			StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,7002,0,%d,,%d,%d,,,%d,%d,,,%d,%d,,,,,,,%d,%d,,,%d,%d,,,,%s,%s,,,%s,%s,%d\r\n",strDate ,\
				pRoom->m_ChannelNum, pRoom->m_UniqueNumber, pRoom->m_BattleCount, iLogStageIdx, pRoom->m_iWinTeam, ReadTeamCount, BlueTeamCount, pRoom->m_TeamScore[0].m_KillCount, pRoom->m_TeamScore[0].m_DeathCount, pRoom->m_TeamScore[1].m_KillCount, pRoom->m_TeamScore[1].m_DeathCount, pRoom->m_InfoBasic._Title, pStageName, ReadTeamNick, BlueTeamNick, pRoom->m_MainUserDbIdx ); 
		}
		break; 
	case LOG_MESSAGE_U_R_BATTLE_START		: //배틀 시작 (각자)
		{
			INT8 MainUser; 
			INT8 Teamidx; 
			INT8 Intrusion	= 0; 

			if( pSession->m_SlotIdx == pRoom->m_MainSlotIdx )MainUser = 1; 
			else											 MainUser = 0; 

			Teamidx = (INT8)(pSession->m_SlotIdx % TEAM_COUNT)+1; 

			const char	* pStageName; 
			INT8	GameType; 
			UINT32	EndKill; 
			UINT32	EndTime; 
			INT8	PrivateRoom; 

			switch( GET_STAGETYPE( pRoom->m_InfoBasic._StageID ) )
			{				
			case STAGE_TYPE_DEATHMATCH		:
				GameType	= 1; 
				EndKill		= g_TeamKillCountTable[   pRoom->m_InfoAdd._SubType & 0x0F ]; 
				EndTime		= g_TeamTimeTable[ (pRoom->m_InfoAdd._SubType & 0xF0) >> 4 ]; 
				break; 
			case STAGE_TYPE_BOMB			: 
			case STAGE_TYPE_4VS4			:
				GameType		= 2; 
				EndKill			= g_MissionRoundCountTable[ pRoom->m_InfoAdd._SubType ]; //미션 카운트 
				EndTime			= g_MissionRoundTimeTable[ pRoom->m_MiRoundTimeIdx ]; 			
				break; 
			case STAGE_TYPE_DESTROY			: 
			default							: 
				GameType		= 2; 
				EndKill			= g_MissionRoundCountTable[ pRoom->m_InfoAdd._SubType ]; //미션 카운트 
				EndTime			= g_MissionRoundTimeTable[ pRoom->m_MiRoundTimeIdx ]; 				
				break; 
			}

			pStageName		= getStageName( (STAGE_ID)pRoom->m_InfoBasic._StageID ); 

			if( pRoom->m_InfoBasic._StageID & STAGE_FLAG_PRIVATE )	PrivateRoom = 1;
			else													PrivateRoom = 0;

			INT32 iLogStageIdx = _GetLogStageIndex(pRoom->m_InfoBasic._StageID); 

			pSendBuffer[BufferIdx]._szMessage[7] = 6;
			sprintf( strTemp, "%d/%d/%d/%d", pSession->m_UserInfoRecord._match, pSession->m_UserInfoRecord._win, pSession->m_UserInfoRecord._lose, pSession->m_UserInfoRecord._draw ); 
			StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,4001,%d,%d,,%d,%d,%d,,%d,%d,%d,%d,0,%d,,%d,%d,,%d,,,,,,,,%s,%s,%s,%s,%s,%d,,,%d,,%d\r\n",strDate ,\
				pSession->m_DBIdx, pSession->m_ChannelNum, pRoom->m_UniqueNumber, pRoom->m_BattleCount, pSession->m_UserInfoBasic._rank, iLogStageIdx, GameType, pRoom->m_InfoBasic._PersonMax, PrivateRoom, MainUser,Teamidx, pSession->m_UserInfoBasic._point, pSession->m_UserInfoBasic._exp, pSession->m_UserInfoBasic._nick, pSession->m_Id, strTemp, pRoom->m_InfoBasic._Title, pStageName, Intrusion, pSession->m_UserInfoBasic._pcCafe,  pSession->m_SessionIdx); 
		}
		break; 
	//case LOG_MESSAGE_U_R_BATTLE_END			: break; //함수에서 처리 
	//case LOG_MESSAGE_U_CHATTING				: break; //함수에서 처리 
	case LOG_MESSAGE_U_R_RESPAWN				: //리스폰 
		{
			pSendBuffer[BufferIdx]._szMessage[7] = 6;
			StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,20004,%d,%d,,%d,%d,%d,,%d,%d,%d,%d,%d,,,%d,,,0,0,0,,,,,,%s,%s,,,,,,,,%d\r\n",strDate ,\
				pSession->m_DBIdx, pSession->m_ChannelNum, pRoom->m_UniqueNumber, pRoom->m_BattleCount, pSession->m_UserInfoBasic._rank, pSession->m_UseWeaponList[0], pSession->m_UseWeaponList[1], pSession->m_UseWeaponList[2], pSession->m_UseWeaponList[3], pSession->m_UseWeaponList[4], pSession->m_RespawnCount, pSession->m_UserInfoBasic._nick, pSession->m_Id, pSession->m_SessionIdx); 
		}
		break; 
	default: 
		StringSize = 0;
		break; 
	}

	//Set Size
	pSendBuffer[BufferIdx]._Size = (INT16)StringSize + STRING_START_IDX + 1; //널 포함 + 1엔씨 요청
	i3mem::Copy( pSendBuffer[BufferIdx]._szMessage, &pSendBuffer[BufferIdx]._Size, sizeof(UINT16));

	//3. Add WritePos 
	*pWritePos = *pWritePos + 1; 	

	return; 
}

void CModuleContextNC::LogSendKillMessage( INT32 ThreadIdx, CUserSession * pKillSession, CUserSession * pDeathSession, CRoom * pRoom, LOG_KILL_DEATH_INFO * pKillInfo)
{
	if(!g_pContextMain->m_bLogDActive)return; 

	LOG_SEND_MESSAGE	* pSendBuffer; 
	INT32				* pWritePos; 
	INT32				* pReadPos; 
	UINT32				BufferIdx; 	

	//1. Set Insert Buffer 
	switch( ThreadIdx )
	{
	case	MAIN_THREAD_IDX		: 
		pSendBuffer = m_pLogMainMessage; 
		pWritePos	= &m_iLogMain_WritePos; 
		pReadPos	= &m_iLogMain_ReadPos; 
		break; 
	case	MODULE_ONE_THREAD_IDX	: 
		pSendBuffer = m_pLogAuthMessage; 
		pWritePos	= &m_iLogAuth_WritePos; 
		pReadPos	= &m_iLogAuth_ReadPos; 
		break; 
	default	: 
		pSendBuffer = m_ppLogUserMessage[ThreadIdx];
		pWritePos	= &m_pLogUser_WritePos[ThreadIdx]; 
		pReadPos	= &m_pLogUser_ReadPos[ThreadIdx]; 
		break; 
	}
	
	if(*pWritePos == *pReadPos)
	{
		*pWritePos	= 0; 
		*pReadPos	= 0; 
	}

	if( *pReadPos > MODULE_CONTEXT_MAX_SEND_IDX_COUNT ) 
	{//여기로 들어올수  있는 가능성은 거의 없다. 
		WRITE_LOG(NULL, "Log Send Buffer Full1");
		return; 
	}

	char strDate[SERVER_STRING_COUNT]; 
	sprintf(strDate, "%02d/%02d/%d %02d:%02d:%02d.00", g_pContextMain->m_Month, g_pContextMain->m_Day, g_pContextMain->m_Year, g_pContextMain->m_Hour, g_pContextMain->m_Minute, g_pContextMain->m_Second); 

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Kill Message
	{
		BufferIdx = *pWritePos - *pReadPos; 
		if( BufferIdx >= LOG_SEND_BUFFER_COUNT )
		{
			WRITE_LOG(NULL,"Log Send Buffer Overflow1");
			return; 
		}

		BufferIdx = *pWritePos % LOG_SEND_BUFFER_COUNT; 

		//2. Set Buffer 
		INT16 StringSize;	
		pSendBuffer[BufferIdx]._szMessage[7] = 6;
		StringSize = (INT16)sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,20001,%d,%d,,%d,%d,%d,,%d,%d,%d,%d,%d,%d,,%d,,,%d,%d,%d,,,,,,%s,%s,,,,%s,%s,,,%d\r\n",strDate ,\
			pKillSession->m_DBIdx, pKillSession->m_ChannelNum, pRoom->m_UniqueNumber, pRoom->m_BattleCount, pKillSession->m_UserInfoBasic._rank, pKillSession->m_UseWeaponList[0], pKillSession->m_UseWeaponList[1], pKillSession->m_UseWeaponList[2], pKillSession->m_UseWeaponList[3], pKillSession->m_UseWeaponList[4], pKillInfo->WeaponNum, pDeathSession->m_DBIdx,\
			pKillInfo->Kill_X, pKillInfo->Kill_Y, pKillInfo->Kill_Z, pKillSession->m_UserInfoBasic._nick, pKillSession->m_Id, pDeathSession->m_UserInfoBasic._nick, pDeathSession->m_Id, pKillSession->m_SessionIdx); 

		pSendBuffer[BufferIdx]._Size = StringSize + STRING_START_IDX + 1; 
		i3mem::Copy( &pSendBuffer[BufferIdx]._szMessage[0], &pSendBuffer[BufferIdx]._Size, sizeof(UINT16)); 

		//3. Add WritePos 
		*pWritePos = *pWritePos + 1; 		
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Death Message
	{
		BufferIdx = *pWritePos - *pReadPos; 
		if( BufferIdx >= LOG_SEND_BUFFER_COUNT )
		{
			WRITE_LOG(NULL,"Log Send Buffer Overflow2");
			return; 
		}

		BufferIdx = *pWritePos % LOG_SEND_BUFFER_COUNT; 

		//2. Set Buffer 
		INT16 StringSize;	
		pSendBuffer[BufferIdx]._szMessage[7] = 6;
		StringSize = (INT16)sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,20002,%d,%d,,%d,%d,%d,,%d,%d,%d,%d,%d,%d,,%d,,,%d,%d,%d,,,,,,%s,%s,,,,%s,%s,,,%d\r\n",strDate ,\
			pDeathSession->m_DBIdx, pDeathSession->m_ChannelNum, pRoom->m_UniqueNumber, pRoom->m_BattleCount, pDeathSession->m_UserInfoBasic._rank, pDeathSession->m_UseWeaponList[0], pDeathSession->m_UseWeaponList[1], pDeathSession->m_UseWeaponList[2], pDeathSession->m_UseWeaponList[3], pDeathSession->m_UseWeaponList[4], pKillInfo->WeaponNum, pKillSession->m_DBIdx, \
			pKillInfo->Death_X, pKillInfo->Death_Y, pKillInfo->Death_Z, pDeathSession->m_UserInfoBasic._nick, pDeathSession->m_Id, pKillSession->m_UserInfoBasic._nick, pKillSession->m_Id, pDeathSession->m_SessionIdx); 

		pSendBuffer[BufferIdx]._Size = StringSize + STRING_START_IDX + 1; 
		i3mem::Copy( &pSendBuffer[BufferIdx]._szMessage[0], &pSendBuffer[BufferIdx]._Size, sizeof(UINT16)); 

		//3. Add WritePos 
		*pWritePos = *pWritePos + 1; 		
	}
}

/*
Type
결과 1(승패 여부)
1: 승리 2: 패배
3: 무승부 4: disconnect
5: 게임 중단
*/
void CModuleContextNC::LogSendBattleEndPerson(INT32 ThreadIdx, CUserSession * pSession, CRoom * pRoom, INT32 SlotIdx, INT32 EndType)
{
	LOG_CHECKVALID_IDX;
	
	//2. Set Buffer 
	INT16 StringSize;
	pSendBuffer[BufferIdx]._szMessage[7] = 6;
	const char	* pStageName = getStageName( (STAGE_ID)pRoom->m_InfoBasic._StageID ); 
    
	char strTemp[SERVER_STRING_COUNT]; 
	sprintf( strTemp, "%d/%d/%d/%d", pSession->m_UserInfoRecord._match, pSession->m_UserInfoRecord._win, pSession->m_UserInfoRecord._lose, pSession->m_UserInfoRecord._draw ); 

	char strTemp2[SERVER_STRING_COUNT]; 
	sprintf( strTemp2, "%d/%d", pRoom->m_ScoreInfo[SlotIdx].m_KillCount, pRoom->m_ScoreInfo[SlotIdx].m_DeathCount); 

	StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,4002,%d,%d,,%d,%d,%d,,%d,,%d,,,,,,%d,,%d,,,%d,,%d,,,%s,%s,%s,%s,%s,%s,,,%d,%d\r\n",strDate ,\
		pSession->m_DBIdx, pSession->m_ChannelNum, pRoom->m_UniqueNumber, pRoom->m_BattleCount, pSession->m_UserInfoBasic._rank, EndType, pRoom->m_ScoreInfo[SlotIdx].m_KillCount, pSession->m_UserInfoBasic._point, pSession->m_UserInfoBasic._exp, pRoom->m_AccPoint[SlotIdx], pRoom->m_AccExp[SlotIdx], pSession->m_UserInfoBasic._nick, pSession->m_Id, strTemp, pRoom->m_InfoBasic._Title, pStageName, strTemp2, pSession->m_UserInfoBasic._pcCafe, pSession->m_SessionIdx); 
	
	pSendBuffer[BufferIdx]._Size = StringSize + STRING_START_IDX + 1; 
	i3mem::Copy( &pSendBuffer[BufferIdx]._szMessage[0], &pSendBuffer[BufferIdx]._Size, sizeof(UINT16)); 

	//3. Add WritePos 
	*pWritePos = *pWritePos + 1; 
	return; 
}


void CModuleContextNC::LogSendFriendInsert( INT32 ThreadIdx, CUserSession * pSession, FRIEND_INFO_STRUCT * pFriendInfo )//친구 신청	
{
	LOG_CHECKVALID_IDX;

	INT16 StringSize;	
	pSendBuffer[BufferIdx]._szMessage[7] = 6;

	StringSize = (INT16)sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,10001,%d,,,,,,,,,,,,,,%d,,,,,,,,,,,%s,%s,,,,%s,,,,%d\r\n",strDate,\
		pSession->m_DBIdx, pFriendInfo->_BasicInfo._fuid, pSession->m_UserInfoBasic._nick, pSession->m_Id, pFriendInfo->_NickName, pSession->m_SessionIdx); 
	pSendBuffer[BufferIdx]._Size = StringSize + STRING_START_IDX + 1; 
	i3mem::Copy( &pSendBuffer[BufferIdx]._szMessage[0], &pSendBuffer[BufferIdx]._Size, sizeof(UINT16)); 
	
	*pWritePos = *pWritePos + 1; 
	return; 
}

void CModuleContextNC::LogSendFriendAccept( INT32 ThreadIdx, CUserSession * pSession, FRIEND_INFO_STRUCT * pFriendInfo )//친구 추가
{
	LOG_CHECKVALID_IDX;

	INT16 StringSize;	
	pSendBuffer[BufferIdx]._szMessage[7] = 6;

	StringSize = (INT16)sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,10003,%d,,,,,,,,,,,,,,%d,,,,,,,,,,,%s,%s,,,,%s,,,%d,%d\r\n",strDate,\
		pSession->m_DBIdx, pFriendInfo->_BasicInfo._fuid, pSession->m_UserInfoBasic._nick, pSession->m_Id, pFriendInfo->_NickName, pSession->m_iFriendCount, pSession->m_SessionIdx); 
	pSendBuffer[BufferIdx]._Size = StringSize + STRING_START_IDX + 1; 
	i3mem::Copy( &pSendBuffer[BufferIdx]._szMessage[0], &pSendBuffer[BufferIdx]._Size, sizeof(UINT16)); 
	
	*pWritePos = *pWritePos + 1; 
	return; 
}

void CModuleContextNC::LogSendFriendDelete( INT32 ThreadIdx, CUserSession * pSession, FRIEND_INFO_STRUCT * pFriendInfo )//친구 삭제 
{
	LOG_CHECKVALID_IDX;

	INT16 StringSize;	
	pSendBuffer[BufferIdx]._szMessage[7] = 6;

	StringSize = (INT16)sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,10004,%d,,,,,,,,,,,,,,%d,,,,,,,,,,,%s,%s,,,,%s,,,%d,%d\r\n",strDate,\
		pSession->m_DBIdx, pFriendInfo->_BasicInfo._fuid, pSession->m_UserInfoBasic._nick, pSession->m_Id, pFriendInfo->_NickName, (pSession->m_iFriendCount - 1), pSession->m_SessionIdx); 
	pSendBuffer[BufferIdx]._Size = StringSize + STRING_START_IDX + 1; 
	i3mem::Copy( &pSendBuffer[BufferIdx]._szMessage[0], &pSendBuffer[BufferIdx]._Size, sizeof(UINT16)); 
	
	*pWritePos = *pWritePos + 1; 
	return; 
}


//일반 채팅		1	
//귓말 채팅		2
//친구 채팅		3
//같은팀 채팅	4
//클랜 채팅		5

void CModuleContextNC::LogSendChattingMessage( INT32 ThreadIdx, CUserSession * pSession, CRoom * pRoom, char * pMessage, UINT16 iType)
{
	LOG_CHECKVALID_IDX;

	{//, 을 .로 변경합니다. 
		INT32 StrLen = i3String::Length(pMessage); 
		for(INT32 i = 0;i < StrLen; i++)
		{
			if( pMessage[i] == ',' ) pMessage[i] = '.';
		}
	}

	//2. Set Buffer 
	INT16 StringSize;
	pSendBuffer[BufferIdx]._szMessage[7] = 9;
	if(pRoom == NULL)
		StringSize = (INT16)sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,%d,%d,%d,%d,,,,,,,,%s,%s,,,\r\n",strDate, iType, pSession->m_DBIdx, pSession->m_ChannelNum, pSession->m_ChannelNum, pMessage, pSession->m_UserInfoBasic._nick); 
	else 
		StringSize = (INT16)sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,%d,%d,%d,%d,%d,,,,,,,%s,%s,,,\r\n",strDate, iType, pSession->m_DBIdx, pSession->m_ChannelNum, pSession->m_ChannelNum, pRoom->m_UniqueNumber, pMessage, pSession->m_UserInfoBasic._nick); 

	pSendBuffer[BufferIdx]._Size = StringSize + STRING_START_IDX + 1; 
	i3mem::Copy( &pSendBuffer[BufferIdx]._szMessage[0], &pSendBuffer[BufferIdx]._Size, sizeof(UINT16)); 

	//3. Add WritePos 
	*pWritePos = *pWritePos + 1; 
	return; 
}
