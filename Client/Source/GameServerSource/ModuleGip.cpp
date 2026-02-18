#include "pch.h"
#include "ModuleGip.h"
#include "ModuleGipSocket.h"
#include "UserSession.h"
#include "LogFile.h"

//htons() - "Host to Network Short" 
//htonl() - "Host to Network Long" 
//ntohs() - "Network to Host Short" 
//ntohl() - "Network to Host Long"

I3_CLASS_INSTANCE( CModuleGip, i3Thread );

#define	SEND_GIP_BUFFER_COUNT		50
#define GIP_BACKUP_COUNT			50

//CModuleGip * g_pModuleGip = NULL; 

DWORD ConvertMultiToUTF8(char *pszResultBuffer, int nResultBufferSize, char *pszSourceBuffer)
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


CModuleGip::CModuleGip(void)
{
	m_hKillEvent		= ::CreateEvent(NULL, false, false, NULL);
	m_pGipModuleSocket	= NULL; 

	m_ppUserGipMessage	= NULL;	
	m_pUserGip_WritePos = NULL;
	m_pUserGip_ReadPos	= NULL;	

	m_pMainGipMessage	= NULL;	

	m_pAuthGipMessage	= NULL;	
}

CModuleGip::~CModuleGip(void)
{	
	::SetEvent( m_hKillEvent ); 
	OnDestroy(); 
	::Sleep(1000);	
	::CloseHandle( m_hKillEvent ); 
}

BOOL CModuleGip::OnCreate(UINT32 ThreadCount)
{
	if(!g_pContextMain->m_bGipActive)return TRUE; //사용하지 않음 

	m_ThreadCount = ThreadCount; //Set Buffer 

	//1. Create Work Send Buffer
	m_ppUserGipMessage	= (GIP_SEND_MESSAGE	**)i3MemAlloc(m_ThreadCount * sizeof(GIP_SEND_MESSAGE*)); 
	m_pUserGip_WritePos = (INT32*)i3MemAlloc(m_ThreadCount * sizeof(INT32));	
	m_pUserGip_ReadPos	= (INT32*)i3MemAlloc(m_ThreadCount * sizeof(INT32));
	for(UINT32 i = 0; i < m_ThreadCount; i++)
	{
		m_ppUserGipMessage[i]	= (GIP_SEND_MESSAGE	*)i3MemAlloc(sizeof(GIP_SEND_MESSAGE) * SEND_GIP_BUFFER_COUNT);		
		i3mem::FillZero( m_ppUserGipMessage[i], sizeof(GIP_SEND_MESSAGE) * SEND_GIP_BUFFER_COUNT); 

		m_pUserGip_WritePos[i]	= 0; 
		m_pUserGip_ReadPos[i]	= 0; 
	}

	//1. Create Main Send Buffer 
	m_pMainGipMessage	= (GIP_SEND_MESSAGE	*)i3MemAlloc(sizeof(GIP_SEND_MESSAGE) * SEND_GIP_BUFFER_COUNT);	// Main Thread 
	i3mem::FillZero( m_pMainGipMessage, sizeof(GIP_SEND_MESSAGE) * SEND_GIP_BUFFER_COUNT ); 
	m_MainGip_WritePos	= 0;
	m_MainGip_ReadPos	= 0;

	//1. Create Module Send Buffer 
	m_pAuthGipMessage	= (GIP_SEND_MESSAGE	*)i3MemAlloc(sizeof(GIP_SEND_MESSAGE) * SEND_GIP_BUFFER_COUNT);	// Module Thread 
	i3mem::FillZero( m_pAuthGipMessage, sizeof(GIP_SEND_MESSAGE) * SEND_GIP_BUFFER_COUNT ); 
	m_AuthGip_WritePos	= 0;
	m_AuthGip_ReadPos	= 0;


	//Create BackUpBuffer 
	m_pBackUpBuffer		= (GIP_BACKUP*)i3MemAlloc(sizeof(GIP_BACKUP) * GIP_BACKUP_COUNT); 


	//2. Create Module
#ifdef TRACE_GIP_PACKET
	m_pGipModuleSocket = CModuleGipSocket::NewObject(); 
	m_pGipModuleSocket->m_bGreeting = TRUE;
#else 
	m_pGipModuleSocket = CModuleGipSocket::NewObject(); 
	if( !m_pGipModuleSocket->OnConnect( g_pContextMain->m_GipServerIp, g_pContextMain->m_GipServerPort) )  return FALSE; 
	_SendGreetingMessage();	//Send Greeting Message
#endif

	//3. Create Thread
	if( !i3Thread::Create("ModuleGip", 0, 4096, NULL, PRIORITY_HIGH) )return FALSE; 
	WRITE_LOG(NULL, "[CModuleGip::OnCreate] GIP CONNECT SUCCESS"); 
	return TRUE; 
}

void CModuleGip::OnDestroy(void)
{
	if(!g_pContextMain->m_bGipActive)return; 

	//Free
	for(UINT32 i = 0; i < m_ThreadCount; i++)
	{
		I3MEM_SAFE_FREE( m_ppUserGipMessage[i] );
	}
	I3MEM_SAFE_FREE( m_ppUserGipMessage ); 
	I3MEM_SAFE_FREE( m_pUserGip_WritePos); 
	I3MEM_SAFE_FREE( m_pUserGip_ReadPos ); 
	I3MEM_SAFE_FREE( m_pMainGipMessage ); 
	I3MEM_SAFE_FREE( m_pAuthGipMessage ); 

	I3MEM_SAFE_FREE( m_pBackUpBuffer ); 
	//Release
	I3_SAFE_RELEASE( m_pGipModuleSocket ); 

	return; 
}

void CModuleGip::SendGipMessage( INT32 ThreadIdx, GIP_MESSAGE GipId, CUserSession * pSession, INT32 Arg)
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
		pSendBuffer = m_pMainGipMessage; 
		pWritePos	= &m_MainGip_WritePos; 
		pReadPos	= &m_MainGip_ReadPos; 
		break; 
	case	AUTH_THREAD_IDX	: 
		pSendBuffer = m_pAuthGipMessage; 
		pWritePos	= &m_AuthGip_WritePos; 
		pReadPos	= &m_AuthGip_ReadPos; 
		break; 
	default	: 
		pSendBuffer = m_ppUserGipMessage[ThreadIdx];
		pWritePos	= &m_pUserGip_WritePos[ThreadIdx]; 
		pReadPos	= &m_pUserGip_ReadPos[ThreadIdx]; 
		break; 
	}	

	//리드 버퍼 인덱스를 초기화 할 수 있는지 확인한다. 
	if(*pWritePos == *pReadPos)
	{
		*pWritePos	= 0;	//순서 중요 
		*pReadPos	= 0; 	
	}

	//극한 값까지 갔는지를 확인합니다. 2415919103
	if( *pReadPos > 2000000000 ) 
	{//여기로 들어올수  있는 가능성은 거의 없다. 
		WRITE_LOG(NULL,"CModuleGip::WriteBuffer FULL");		
		return; 
	}

	BufferIdx = *pWritePos - *pReadPos; 
	if( BufferIdx >= SEND_GIP_BUFFER_COUNT )
	{
		WRITE_LOG(NULL,"CModuleGip::WriteBuffer Overflow");
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
			sprintf( pTempBuffer, "<cmd action=\"CMD_InsertCharacterInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><charName v=\"%s\"/><genderId v=\"1\"/><teamId v=\"1\"/><classCodeId v=\"0\"/><localTime v=\"%d\"/></param></cmd>", \
				pSession->m_DBIdx, TempNick, iShowtime);
		}
		break; 
	case GIP_MESSAGE_CREATE_NICK_USER	:	//CMD_InsertUserInfo
		sprintf( pTempBuffer, "<cmd action=\"CMD_InsertUserInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><gamePoint v=\"0\"/><cashPoint v=\"0\"/><localTime v=\"%d\"/></param></cmd>", \
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
			sprintf( pTempBuffer, "<cmd action=\"CMD_UpdateCharacterRecordInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><charName v=\"%s\"/><exp v=\"%d\"/><win v=\"%d\"/><defeat v=\"%d\"/><kill v=\"%d\"/><death v=\"%d\"/><headshot v=\"%d\"/><genderId v=\"%d\"/><teamId v=\"%d\"/><classCodeId v=\"%d\"/><channel v=\"0\"/><clanPoint v=\"0\"/><loginStatus v=\"%d\"/><localTime v=\"%d\"/></param></cmd>", \
				pSession->m_DBIdx, TempNick, pSession->m_UserInfoBasic._exp, pSession->m_UserInfoRecord._swin, pSession->m_UserInfoRecord._slose, pSession->m_UserInfoRecord._skillcount, pSession->m_UserInfoRecord._sdeath, pSession->m_UserInfoRecord._sheadshot, pSession->m_GIP_LastUseGender, pSession->m_GIP_LastUseTeam, pSession->m_UserInfoBasic._rank + 1, Arg, iShowtime);
		}
		break; 
	case GIP_MESSAGE_USER		:	//CMD_UpdateUserInfo
		sprintf( pTempBuffer, "<cmd action=\"CMD_UpdateUserInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><gamePoint v=\"%d\"/><cashPoint v=\"%d\"/><loginStatus v=\"%d\"/><localTime v=\"%d\"/></param></cmd>", \
			pSession->m_DBIdx, pSession->m_UserInfoBasic._point, pSession->m_UserInfoBasic._cash, Arg, iShowtime);
		break; 
	case GIP_MESSAGE_MEDAL_MINI			:	//CMD_InsertCharacterPropertyInfo
		sprintf( pTempBuffer, "<cmd action=\"CMD_InsertCharacterPropertyInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><propertyId v=\"4\"/><propertySid v=\"%d\"/><localTime v=\"%d\"/></param></cmd>", \
            pSession->m_DBIdx, Arg, iShowtime);
		break; 
	case GIP_MESSAGE_MEDAL_INSI			:	//CMD_InsertCharacterPropertyInfo
		sprintf( pTempBuffer, "<cmd action=\"CMD_InsertCharacterPropertyInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><propertyId v=\"5\"/><propertySid v=\"%d\"/><localTime v=\"%d\"/></param></cmd>", \
            pSession->m_DBIdx, Arg, iShowtime);
		break;
	case GIP_MESSAGE_MEDAL_MEDA			:	//CMD_InsertCharacterPropertyInfo
		sprintf( pTempBuffer, "<cmd action=\"CMD_InsertCharacterPropertyInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><propertyId v=\"6\"/><propertySid v=\"%d\"/><localTime v=\"%d\"/></param></cmd>", \
            pSession->m_DBIdx, Arg, iShowtime);
		break; 
	case GIP_MESSAGE_CHALLENGE			:	//CMD_InsertCharacterPropertyInfo
		sprintf( pTempBuffer, "<cmd action=\"CMD_InsertCharacterPropertyInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><propertyId v=\"7\"/><propertySid v=\"%d\"/><localTime v=\"%d\"/></param></cmd>", \
            pSession->m_DBIdx, Arg + 1, iShowtime);
		break; 	
	case GIP_MESSAGE_ENTER_CHANNEL		:	//CMD_UpdateCharacterChannel
		sprintf( pTempBuffer, "<cmd action=\"CMD_UpdateCharacterChannel\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><channel v=\"%d\"/><localTime v=\"%d\"/></param></cmd>", \
			pSession->m_DBIdx, ((g_pContextMain->m_ServerIdx * 10) + (pSession->m_ChannelNum + 1)), iShowtime);
		break; 
	default: return; break; 
	}	

	ConvertMultiToUTF8(&pSendBuffer[BufferIdx]._Packet.commandList.command[0], 4096, pTempBuffer); 
	pSendBuffer[BufferIdx]._Packet.commandList.command_len	= i3String::Length(&pSendBuffer[BufferIdx]._Packet.commandList.command[0]); 
	pSendBuffer[BufferIdx]._Packet.header.length			= htons( (unsigned short)(sizeof(Packet_Header) + 4 + pSendBuffer[BufferIdx]._Packet.commandList.command_len) ); 
	pSendBuffer[BufferIdx]._Packet.commandList.command_len	= htons( pSendBuffer[BufferIdx]._Packet.commandList.command_len ); 

	//버퍼링을 위해서 저장한다.
	pSendBuffer[BufferIdx]._BackUpInfo._iDataBaseID	= pSession->m_DBIdx; 	
	i3mem::Copy( &pSendBuffer[BufferIdx]._BackUpInfo._UserInfoBasic,	&pSession->m_UserInfoBasic,		sizeof(USER_INFO_BASIC) ); 
	i3mem::Copy( &pSendBuffer[BufferIdx]._BackUpInfo._UserInfoRecord,	&pSession->m_UserInfoRecord, 	sizeof(USER_INFO_RECORD)); 

	//3. Add WritePos 
	*pWritePos = *pWritePos + 1;
}


UINT32 CModuleGip::OnRunning( void * pUserData)
{	
	INT32	SendCount; 
	UINT32	ReadIdx;
	UINT32	SendSize;
	UINT8	SendFailCount = 0; 
	BOOL	IsSend;

	while(1)
	{
		if( ::WaitForSingleObject( m_hKillEvent, 1) == WAIT_OBJECT_0 )break;

		IsSend = FALSE; 
		SendSize = 0;

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
				_SendGreetingMessage(); 
			}
			else
			{
				WRITE_LOG(NULL,"FAIL GIP_MODULE RECONNECT 1");
				m_pGipModuleSocket->OnDestroy(); 				
			}
		}
#endif
		//Main Thread 
 		SendCount = m_MainGip_WritePos - m_MainGip_ReadPos; 

		if(SendCount > 0)
		{
			for(INT32 i = 0; i < SendCount; i++)
			{
				ReadIdx = m_MainGip_ReadPos % SEND_GIP_BUFFER_COUNT; 
#ifdef TRACE_GIP_PACKET				
				I3TRACE(m_pMainGipMessage[ReadIdx]._Packet.commandList.command);
				I3TRACE( "\n%d\n", ntohs(m_pMainGipMessage[ReadIdx]._Packet.header.length);
#else
				_SendGIpMessage(&m_pMainGipMessage[ReadIdx]); 
#endif
				m_MainGip_ReadPos++; 
			}
			IsSend = TRUE; 
		}

		//Auth Module Thread
		SendCount = m_AuthGip_WritePos - m_AuthGip_ReadPos; 
		if(SendCount > 0)
		{
			for(INT32 i = 0; i < SendCount; i++)
			{
				ReadIdx = m_AuthGip_ReadPos % SEND_GIP_BUFFER_COUNT; 
#ifdef TRACE_GIP_PACKET
				I3TRACE(m_pAuthGipMessage[ReadIdx]._Packet.commandList.command); 
				I3TRACE( "\n%d\n", ntohs(m_pMainGipMessage[ReadIdx]._Packet.header.length);
#else
				_SendGIpMessage(&m_pAuthGipMessage[ReadIdx]); 
#endif
				m_AuthGip_ReadPos++; 
			}
			IsSend = TRUE; 
		}

		//User Thread 
		for(UINT32 i = 0; i < m_ThreadCount; i++)
		{			
			GIP_SEND_MESSAGE * pGipBuffer = m_ppUserGipMessage[i]; 
			SendCount = m_pUserGip_WritePos[i] - m_pUserGip_ReadPos[i]; 			

			if(SendCount > 0)
			{
				for(INT32 j = 0; j < SendCount; j++)
				{
					ReadIdx = m_pUserGip_ReadPos[i] % SEND_GIP_BUFFER_COUNT; 
#ifdef TRACE_GIP_PACKET				
					I3TRACE(pGipBuffer[ReadIdx]._Packet.commandList.command); 
					I3TRACE( "\n%d\n", ntohs(m_pMainGipMessage[ReadIdx]._Packet.header.length);					
#else
					_SendGIpMessage(&pGipBuffer[ReadIdx]); 
#endif
					m_pUserGip_ReadPos[i]++; 
				}
				IsSend = TRUE; 
			}
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//2. Recv Data 
		m_pGipModuleSocket->m_RequestCount = 0; 
		m_pGipModuleSocket->SelectEvent();

		//Send Request Message 
		for(UINT32 i = 0; i < m_pGipModuleSocket->m_RequestCount; i++)
		{
			_SendReqMessage(i); 
		}
	}
	return 0; 
}

INT32 CModuleGip::_SendGIpMessage(GIP_SEND_MESSAGE * pMessage)
{
	//pMessage
	pMessage->_Packet.header.pid = m_pGipModuleSocket->m_iPId;	//형변환을 하지 않는다. 
	pMessage->_Packet.header.qid = m_iQId;						//형변환을 하지 않는다. 	

	//버퍼링을 한다. 
	m_pBackUpBuffer[m_iQId % GIP_BACKUP_COUNT]._iDataBaseID	= pMessage->_BackUpInfo._iDataBaseID; 
	i3mem::Copy( &m_pBackUpBuffer[m_iQId % GIP_BACKUP_COUNT]._UserInfoBasic,	&pMessage->_BackUpInfo._UserInfoBasic, sizeof(USER_INFO_BASIC)); 
	i3mem::Copy( &m_pBackUpBuffer[m_iQId % GIP_BACKUP_COUNT]._UserInfoRecord,	&pMessage->_BackUpInfo._UserInfoRecord, sizeof(USER_INFO_RECORD)); 

	m_iQId++;
	if(m_iQId == 60000)m_iQId = 1; 

	//Send
	
	return m_pGipModuleSocket->SendPacketMessage((char *)&pMessage->_Packet, ntohs(pMessage->_Packet.header.length) );
}

void CModuleGip::_SendReqMessage(UINT32 Idx)
{
	GIP_BACKUP	* pBackUpBuffer = &m_pBackUpBuffer[ m_pGipModuleSocket->m_RequestInfo[Idx]._Qid % GIP_BACKUP_COUNT ]; 

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
			sprintf( pTempBuffer, "<cmd action=\"CMD_InsertUserInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><gamePoint v=\"0\"/><cashPoint v=\"0\"/><localTime v=\"%d\"/></param></cmd>", pBackUpBuffer->_iDataBaseID, iShowtime);
			ConvertMultiToUTF8(&GipMessage._Packet.commandList.command[0], 4096, pTempBuffer);
			GipMessage._Packet.commandList.command_len = i3String::Length(&GipMessage._Packet.commandList.command[0]); 
			GipMessage._Packet.header.length			= htons( (unsigned short)(sizeof(Packet_Header) + 4 + GipMessage._Packet.commandList.command_len) ); 
			GipMessage._Packet.commandList.command_len	= htons( GipMessage._Packet.commandList.command_len ); 
			m_pGipModuleSocket->SendPacketMessage((char *)&GipMessage._Packet, ntohs(GipMessage._Packet.header.length) );

			//Send Update			----2
			sprintf( pTempBuffer, "<cmd action=\"CMD_UpdateUserInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><gamePoint v=\"%d\"/><cashPoint v=\"%d\"/><loginStatus v=\"0\"/><localTime v=\"%d\"/></param></cmd>", pBackUpBuffer->_iDataBaseID, pBackUpBuffer->_UserInfoBasic._point, pBackUpBuffer->_UserInfoBasic._cash, iShowtime);
			ConvertMultiToUTF8(&GipMessage._Packet.commandList.command[0], 4096, pTempBuffer);
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
			sprintf( pTempBuffer, "<cmd action=\"CMD_InsertCharacterInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><charName v=\"%s\"/><genderId v=\"1\"/><teamId v=\"1\"/><classCodeId v=\"0\"/><localTime v=\"%d\"/></param></cmd>", pBackUpBuffer->_iDataBaseID, TempNick, iShowtime);
			ConvertMultiToUTF8(&GipMessage._Packet.commandList.command[0], 4096, pTempBuffer);
			GipMessage._Packet.commandList.command_len = i3String::Length(&GipMessage._Packet.commandList.command[0]); 
			GipMessage._Packet.header.length			= htons( (unsigned short)(sizeof(Packet_Header) + 4 + GipMessage._Packet.commandList.command_len) ); 
			GipMessage._Packet.commandList.command_len	= htons( GipMessage._Packet.commandList.command_len ); 
			m_pGipModuleSocket->SendPacketMessage((char *)&GipMessage._Packet, ntohs(GipMessage._Packet.header.length) );	

			//Send				-------------2
			sprintf( pTempBuffer, "<cmd action=\"CMD_UpdateCharacterRecordInfo\" version=\"2.0\"><param><uid v=\"%d\"/><serverId v=\"1\"/><charName v=\"%s\"/><exp v=\"%d\"/><win v=\"%d\"/><defeat v=\"%d\"/><kill v=\"%d\"/><death v=\"%d\"/><headshot v=\"%d\"/><genderId v=\"1\"/><teamId v=\"1\"/><classCodeId v=\"%d\"/><channel v=\"0\"/><clanPoint v=\"0\"/><loginStatus v=\"0\"/><localTime v=\"%d\"/></param></cmd>", \
				pBackUpBuffer->_iDataBaseID, TempNick, pBackUpBuffer->_UserInfoBasic._exp, pBackUpBuffer->_UserInfoRecord._swin, pBackUpBuffer->_UserInfoRecord._slose, pBackUpBuffer->_UserInfoRecord._skillcount, pBackUpBuffer->_UserInfoRecord._sdeath, pBackUpBuffer->_UserInfoRecord._sheadshot, pBackUpBuffer->_UserInfoBasic._rank + 1, iShowtime);
			ConvertMultiToUTF8(&GipMessage._Packet.commandList.command[0], 4096, pTempBuffer);
			GipMessage._Packet.commandList.command_len = i3String::Length(&GipMessage._Packet.commandList.command[0]); 
			GipMessage._Packet.header.length			= htons( (unsigned short)(sizeof(Packet_Header) + 4 + GipMessage._Packet.commandList.command_len) ); 
			GipMessage._Packet.commandList.command_len	= htons( GipMessage._Packet.commandList.command_len ); 
			m_pGipModuleSocket->SendPacketMessage((char *)&GipMessage._Packet, ntohs(GipMessage._Packet.header.length) );            
		}
		break; 
	}
	return; 
}

void CModuleGip::_SendGreetingMessage(void)
{		
	//Set Header 
	m_GreetingPacket.header.length		= htons(sizeof(Packet_TYPE_GREETING) ); 
	m_GreetingPacket.header.type		= 0x12; 
	m_GreetingPacket.header.version		= 1; 
	m_GreetingPacket.header.headerLen	= sizeof(Packet_Header); 
	m_GreetingPacket.header.crc			= 0; 
	m_GreetingPacket.header.pid			= 0; 
	m_GreetingPacket.header.qid			= 0;

	//Set Greeting 
	m_GreetingPacket.game_id			= htons(34);
	m_GreetingPacket.server_id			= htons(g_pContextMain->m_ServerIdx);
	m_GreetingPacket.sub_server_id		= 0;

	m_pGipModuleSocket->SendPacketMessage((char*)&m_GreetingPacket, sizeof(Packet_TYPE_GREETING)); 

	m_iQId = 1; //QId Setting
    return; 
}
