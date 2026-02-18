#include "pch.h"
#include "ModuleLog.h"
#include "UserSession.h"
#include "Room.h"
#include "ServerContext.h"
#include "Usermanager.h"
#include "LogFile.h"
I3_CLASS_INSTANCE( CModuleLog, i3Thread );

#define	SEND_LOG_BUFFER_COUNT		50

//CModuleLog * g_pModuleLog = NULL; 

#define CHECKVALID_IDX	if(!g_pContextMain->m_bLogDActive)return;  \
						LOG_SEND_MESSAGE	* pSendBuffer; \
						INT32				* pWritePos; \
						INT32				* pReadPos; \
						UINT32				BufferIdx; \
						switch( ThreadIdx )\
						{\
						case	MAIN_THREAD_IDX		: \
							pSendBuffer = m_pMainLogMessage; \
							pWritePos	= &m_MainLog_WritePos;\
							pReadPos	= &m_MainLog_ReadPos; \
							break; \
						case	AUTH_THREAD_IDX	: \
							pSendBuffer = m_pAuthLogMessage; \
							pWritePos	= &m_AuthLog_WritePos;\
							pReadPos	= &m_AuthLog_ReadPos;\
							break; \
						default	: \
							pSendBuffer = m_ppUserLogMessage[ThreadIdx];\
							pWritePos	= &m_pUserLog_WritePos[ThreadIdx]; \
							pReadPos	= &m_pUserLog_ReadPos[ThreadIdx]; \
							break; \
						}\
						if(*pWritePos == *pReadPos)\
						{\
							*pWritePos	= 0; \
							*pReadPos	= 0; \
						}\
						if( *pReadPos > 2000000000 ) \
						{\
							WRITE_LOG(NULL,"Send Buffer Full");\
							return; \
						}\
						BufferIdx = *pWritePos - *pReadPos; \
						if( BufferIdx >= SEND_LOG_BUFFER_COUNT )\
						{\
							WRITE_LOG(NULL,"[CModuleLog::SendLogMessage] Send Buffer Overflow \n");\
							return; \
						}\
						BufferIdx = *pWritePos % SEND_LOG_BUFFER_COUNT; \
						char strDate[SERVER_STRING_COUNT]; 	\
						sprintf(strDate, "%02d/%02d/%d %02d:%02d:%02d.00", g_pContextMain->m_Month, g_pContextMain->m_Day, g_pContextMain->m_Year, g_pContextMain->m_Hour, g_pContextMain->m_Minute, g_pContextMain->m_Second); 

CModuleLog::CModuleLog(void)
{
	m_hKillEvent		= ::CreateEvent(NULL, false, false, NULL);
	m_pLogModuleSocket	= NULL; 

	m_ppUserLogMessage	= NULL;	
	m_pUserLog_WritePos = NULL;
	m_pUserLog_ReadPos	= NULL;	

	m_pMainLogMessage	= NULL;	

	m_pAuthLogMessage	= NULL;	
}

CModuleLog::~CModuleLog(void)
{		
	::SetEvent( m_hKillEvent ); 
	OnDestroy(); 
	::Sleep(1000);	
	::CloseHandle( m_hKillEvent ); 
}

BOOL CModuleLog::OnCreate(UINT32 ThreadCount)
{
	if(!g_pContextMain->m_bLogDActive)return TRUE; //사용하지 않음 

	m_ThreadCount = ThreadCount; //Set Buffer 

	//1. Create Work Send Buffer
	m_ppUserLogMessage	= (LOG_SEND_MESSAGE	**)i3MemAlloc(m_ThreadCount * sizeof(LOG_SEND_MESSAGE*)); 
	m_pUserLog_WritePos = (INT32*)i3MemAlloc(m_ThreadCount * sizeof(INT32));	
	m_pUserLog_ReadPos	= (INT32*)i3MemAlloc(m_ThreadCount * sizeof(INT32));
	for(UINT32 i = 0; i < m_ThreadCount; i++)
	{
		m_ppUserLogMessage[i]	= (LOG_SEND_MESSAGE	*)i3MemAlloc(sizeof(LOG_SEND_MESSAGE) * SEND_LOG_BUFFER_COUNT);		
		i3mem::FillZero( m_ppUserLogMessage[i], sizeof(LOG_SEND_MESSAGE) * SEND_LOG_BUFFER_COUNT); 
		for(UINT32 j = 0; j < SEND_LOG_BUFFER_COUNT; j++)
		{
			m_ppUserLogMessage[i][j]._szMessage[3] = 3;
		}

		m_pUserLog_WritePos[i]	= 0; 
		m_pUserLog_ReadPos[i]	= 0; 
	}

	//3. Create Main Send Buffer 
	m_pMainLogMessage	= (LOG_SEND_MESSAGE	*)i3MemAlloc(sizeof(LOG_SEND_MESSAGE) * SEND_LOG_BUFFER_COUNT);	// Main Thread 
	i3mem::FillZero( m_pMainLogMessage, sizeof(LOG_SEND_MESSAGE) * SEND_LOG_BUFFER_COUNT ); 
	for(UINT32 j = 0; j < SEND_LOG_BUFFER_COUNT; j++)
	{
		m_pMainLogMessage[j]._szMessage[3] = 3;
	}
	m_MainLog_WritePos	= 0;
	m_MainLog_ReadPos	= 0;

	//4. Create Module Send Buffer 
	m_pAuthLogMessage	= (LOG_SEND_MESSAGE	*)i3MemAlloc(sizeof(LOG_SEND_MESSAGE) * SEND_LOG_BUFFER_COUNT);	// Module Thread 
	i3mem::FillZero( m_pAuthLogMessage, sizeof(LOG_SEND_MESSAGE) * SEND_LOG_BUFFER_COUNT ); 
	for(UINT32 j = 0; j < SEND_LOG_BUFFER_COUNT; j++)
	{
		m_pMainLogMessage[j]._szMessage[3] = 3;
	}
	m_AuthLog_WritePos	= 0;
	m_AuthLog_ReadPos	= 0;

	BOOL Rv = TRUE; 
	//2. Create Module
	m_pLogModuleSocket = CModuleLogSocket::NewObject(); 
	if( !m_pLogModuleSocket->OnConnect( g_pContextMain->m_LogDServerIp, g_pContextMain->m_LogDServerPort) )Rv = FALSE; 
	SendInitMessage();	//Send Init Message

	//3. Create Thread
	if( !i3Thread::Create("ModuleLog", 0, 4096, NULL, PRIORITY_HIGH) )Rv = FALSE; 
	WRITE_LOG(NULL, "[CModuleLog::OnCreate] LOG_D CONNECT SUCCESS"); 
	return Rv; 
}

void CModuleLog::OnDestroy(void)
{
	if(!g_pContextMain->m_bLogDActive)return; 

	//Free
	for(UINT32 i = 0; i < m_ThreadCount; i++)
	{
		I3MEM_SAFE_FREE( m_ppUserLogMessage[i] );
	}
	I3MEM_SAFE_FREE( m_ppUserLogMessage ); 
	I3MEM_SAFE_FREE( m_pUserLog_WritePos); 
	I3MEM_SAFE_FREE( m_pUserLog_ReadPos ); 
	I3MEM_SAFE_FREE( m_pMainLogMessage ); 
	I3MEM_SAFE_FREE( m_pAuthLogMessage ); 

	//Release
	I3_SAFE_RELEASE( m_pLogModuleSocket ); 

	return; 
}

#define STRING_START_IDX	11
#define MACHINE_NAME		"PB"
#define SERVER_TYPE			"GS"
void CModuleLog::SendLogMessage( INT32 ThreadIdx, LOG_MESSAGE LogId, CUserSession * pSession, CRoom * pRoom )
{
	CHECKVALID_IDX;

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
	case LOG_MESSAGE_SERVER_ALL_USERCOUNT	:	//유저 카운트 
		{
			pSendBuffer[BufferIdx]._szMessage[7] = 7;		
			StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX], "%s,201,%d,,,,,,,%04d-%02d-%02d %02d:%02d:%02d,%04d-%02d-%02d %02d:%02d:%02d,\r\n",strDate , g_pUserManager->GetSessionCount(),\
				g_pContextMain->m_TimeCreateRoom.wYear, g_pContextMain->m_TimeCreateRoom.wMonth, g_pContextMain->m_TimeCreateRoom.wDay,	g_pContextMain->m_TimeCreateRoom.wHour,	g_pContextMain->m_TimeCreateRoom.wMinute,g_pContextMain->m_TimeCreateRoom.wSecond,\
				g_pContextMain->m_TimeStartBattle.wYear, g_pContextMain->m_TimeStartBattle.wMonth, g_pContextMain->m_TimeStartBattle.wDay,	g_pContextMain->m_TimeStartBattle.wHour,	g_pContextMain->m_TimeStartBattle.wMinute,g_pContextMain->m_TimeStartBattle.wSecond); 
		}
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

			if( pRoom->m_InfoBasic._StageID & STAGE_FLAG_PRIVATE )	PrivateRoom = 1;
			else													PrivateRoom = 0;

			//Get Count And Nick 
			pRoom->GetRoomInfo(&ReadTeamCount, &BlueTeamCount, ReadTeamNick, BlueTeamNick);  

			pSendBuffer[BufferIdx]._szMessage[7] = 6;		
			StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,7001,0,%d,,%d,%d,%d,,%d,%d,%d,%d,%d,%d,%d,,,,,,,,,,,,,,,%s,%s,%s,%d,%s,%s,%d\r\n",strDate ,\
				pRoom->m_ChannelNum, pRoom->m_UniqueNumber, pRoom->m_BattleCount, pRoom->m_MainUserDbIdx, pRoom->m_InfoBasic._StageID, GameType, pRoom->m_InfoBasic._PersonMax, PrivateRoom, ReadTeamCount, BlueTeamCount, EndTime, pRoom->m_InfoBasic._Title,  pStageName , pRoom->m_InfoAdd._Nick, EndKill, ReadTeamNick, BlueTeamNick, pRoom->m_MainUserDbIdx ); 
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

			pSendBuffer[BufferIdx]._szMessage[7] = 6;		
			StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,7002,0,%d,,%d,%d,,,%d,%d,,,%d,%d,,,,,,,%d,%d,,,%d,%d,,,,%s,%s,,,%s,%s,%d\r\n",strDate ,\
				pRoom->m_ChannelNum, pRoom->m_UniqueNumber, pRoom->m_BattleCount, pRoom->m_InfoBasic._StageID, pRoom->m_iWinTeam, ReadTeamCount, BlueTeamCount, pRoom->m_TeamScore[0].m_KillCount, pRoom->m_TeamScore[0].m_DeathCount, pRoom->m_TeamScore[1].m_KillCount, pRoom->m_TeamScore[1].m_DeathCount, pRoom->m_InfoBasic._Title, pStageName, ReadTeamNick, BlueTeamNick, pRoom->m_MainUserDbIdx ); 
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

			pSendBuffer[BufferIdx]._szMessage[7] = 6;
			sprintf( strTemp, "%d/%d/%d/%d", pSession->m_UserInfoRecord._match, pSession->m_UserInfoRecord._win, pSession->m_UserInfoRecord._lose, pSession->m_UserInfoRecord._draw ); 
			StringSize = sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,4001,%d,%d,,%d,%d,%d,,%d,%d,%d,%d,0,%d,,%d,%d,,%d,,,,,,,,%s,%s,%s,%s,%s,%d,,,%d,,%d\r\n",strDate ,\
				pSession->m_DBIdx, pSession->m_ChannelNum, pRoom->m_UniqueNumber, pRoom->m_BattleCount, pSession->m_UserInfoBasic._rank, pRoom->m_InfoBasic._StageID, GameType, pRoom->m_InfoBasic._PersonMax, PrivateRoom, MainUser,Teamidx, pSession->m_UserInfoBasic._point, pSession->m_UserInfoBasic._exp, pSession->m_UserInfoBasic._nick, pSession->m_Id, strTemp, pRoom->m_InfoBasic._Title, pStageName, Intrusion, pSession->m_UserInfoBasic._pcCafe,  pSession->m_SessionIdx); 
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
}

void CModuleLog::SendKillMessage( INT32 ThreadIdx, CUserSession * pKillSession, CUserSession * pDeathSession, CRoom * pRoom, LOG_KILL_DEATH_INFO * pKillInfo)
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
		pSendBuffer = m_pMainLogMessage; 
		pWritePos	= &m_MainLog_WritePos; 
		pReadPos	= &m_MainLog_ReadPos; 
		break; 
	case	AUTH_THREAD_IDX	: 
		pSendBuffer = m_pAuthLogMessage; 
		pWritePos	= &m_AuthLog_WritePos; 
		pReadPos	= &m_AuthLog_ReadPos; 
		break; 
	default	: 
		pSendBuffer = m_ppUserLogMessage[ThreadIdx];
		pWritePos	= &m_pUserLog_WritePos[ThreadIdx]; 
		pReadPos	= &m_pUserLog_ReadPos[ThreadIdx]; 
		break; 
	}
	
	if(*pWritePos == *pReadPos)
	{
		*pWritePos	= 0; 
		*pReadPos	= 0; 
	}

	if( *pReadPos > 2000000000 ) 
	{//여기로 들어올수  있는 가능성은 거의 없다. 
		WRITE_LOG(NULL, "[CModuleLog::SendLogMessage] Send Buffer Full");
		return; 
	}

	char strDate[SERVER_STRING_COUNT]; 
	sprintf(strDate, "%02d/%02d/%d %02d:%02d:%02d.00", g_pContextMain->m_Month, g_pContextMain->m_Day, g_pContextMain->m_Year, g_pContextMain->m_Hour, g_pContextMain->m_Minute, g_pContextMain->m_Second); 

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Kill Message
	{
		BufferIdx = *pWritePos - *pReadPos; 
		if( BufferIdx >= SEND_LOG_BUFFER_COUNT )
		{
			WRITE_LOG(NULL,"[CModuleLog::SendLogMessage] Send Buffer Overflow");
			return; 
		}

		BufferIdx = *pWritePos % SEND_LOG_BUFFER_COUNT; 

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
		if( BufferIdx >= SEND_LOG_BUFFER_COUNT )
		{
			WRITE_LOG(NULL,"[CModuleLog::SendLogMessage] Send Buffer Overflow");
			return; 
		}

		BufferIdx = *pWritePos % SEND_LOG_BUFFER_COUNT; 

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
void CModuleLog::SendBattleEndPerson(INT32 ThreadIdx, CUserSession * pSession, CRoom * pRoom, INT32 SlotIdx, INT32 EndType)
{
	CHECKVALID_IDX;
	
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


void CModuleLog::SendFriendInsert( INT32 ThreadIdx, CUserSession * pSession, FRIEND_INFO_STRUCT * pFriendInfo )//친구 신청	
{
	CHECKVALID_IDX;

	INT16 StringSize;	
	pSendBuffer[BufferIdx]._szMessage[7] = 6;

	StringSize = (INT16)sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,10001,%d,,,,,,,,,,,,,,%d,,,,,,,,,,,%s,%s,,,,%s,,,,%d\r\n",strDate,\
		pSession->m_DBIdx, pFriendInfo->_BasicInfo._fuid, pSession->m_UserInfoBasic._nick, pSession->m_Id, pFriendInfo->_NickName, pSession->m_SessionIdx); 
	pSendBuffer[BufferIdx]._Size = StringSize + STRING_START_IDX + 1; 
	i3mem::Copy( &pSendBuffer[BufferIdx]._szMessage[0], &pSendBuffer[BufferIdx]._Size, sizeof(UINT16)); 
	
	*pWritePos = *pWritePos + 1; 
	return; 
}

void CModuleLog::SendFriendAccept( INT32 ThreadIdx, CUserSession * pSession, FRIEND_INFO_STRUCT * pFriendInfo )//친구 추가
{
	CHECKVALID_IDX;

	INT16 StringSize;	
	pSendBuffer[BufferIdx]._szMessage[7] = 6;

	StringSize = (INT16)sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,10003,%d,,,,,,,,,,,,,,%d,,,,,,,,,,,%s,%s,,,,%s,,,%d,%d\r\n",strDate,\
		pSession->m_DBIdx, pFriendInfo->_BasicInfo._fuid, pSession->m_UserInfoBasic._nick, pSession->m_Id, pFriendInfo->_NickName, pSession->m_iFriendCount, pSession->m_SessionIdx); 
	pSendBuffer[BufferIdx]._Size = StringSize + STRING_START_IDX + 1; 
	i3mem::Copy( &pSendBuffer[BufferIdx]._szMessage[0], &pSendBuffer[BufferIdx]._Size, sizeof(UINT16)); 
	
	*pWritePos = *pWritePos + 1; 
	return; 
}

void CModuleLog::SendFriendDelete( INT32 ThreadIdx, CUserSession * pSession, FRIEND_INFO_STRUCT * pFriendInfo )//친구 삭제 
{
	CHECKVALID_IDX;

	INT16 StringSize;	
	pSendBuffer[BufferIdx]._szMessage[7] = 6;

	StringSize = (INT16)sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,10004,%d,,,,,,,,,,,,,,%d,,,,,,,,,,,%s,%s,,,,%s,,,%d,%d\r\n",strDate,\
		pSession->m_DBIdx, pFriendInfo->_BasicInfo._fuid, pSession->m_UserInfoBasic._nick, pSession->m_Id, pFriendInfo->_NickName, (pSession->m_iFriendCount - 1), pSession->m_SessionIdx); 
	pSendBuffer[BufferIdx]._Size = StringSize + STRING_START_IDX + 1; 
	i3mem::Copy( &pSendBuffer[BufferIdx]._szMessage[0], &pSendBuffer[BufferIdx]._Size, sizeof(UINT16)); 
	
	*pWritePos = *pWritePos + 1; 
	return; 
}

void CModuleLog::SendChattingMessage( INT32 ThreadIdx, CUserSession * pSession, CRoom * pRoom, char * pMessage)
{
	CHECKVALID_IDX;

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
	StringSize = (INT16)sprintf(&pSendBuffer[BufferIdx]._szMessage[STRING_START_IDX],"%s,1,%d,%d,%d,%d,,,,,,,%s,%s,,,\r\n",strDate ,pSession->m_DBIdx, pSession->m_ChannelNum, pSession->m_ChannelNum, pRoom->m_UniqueNumber, pMessage, pSession->m_UserInfoBasic._nick); 

	pSendBuffer[BufferIdx]._Size = StringSize + STRING_START_IDX + 1; 
	i3mem::Copy( &pSendBuffer[BufferIdx]._szMessage[0], &pSendBuffer[BufferIdx]._Size, sizeof(UINT16)); 

	//3. Add WritePos 
	*pWritePos = *pWritePos + 1; 
	return; 
}

UINT32 CModuleLog::OnRunning( void * pUserData)
{	
	INT32	SendCount; 
	UINT32	ReadIdx;
	UINT32	SendSize;
	UINT8	SendFailCount = 0; 
	BOOL	IsSend;
	BOOL	IsWorking = TRUE; 

	
	while( IsWorking )
	{		
		m_LogCount = 0; 
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
			if( m_pLogModuleSocket->OnConnect(g_pContextMain->m_LogDServerIp, g_pContextMain->m_LogDServerPort ) )
			{
				SendInitMessage();
				WRITE_LOG(NULL,"SUCCESS LOG_MODULE RECONNECT 1");
			}
			else
			{
				m_pLogModuleSocket->OnDestroy(); 
				WRITE_LOG(NULL,"FAIL LOG_MODULE RECONNECT 1");
			}
		}

		//Main Thread 
 		SendCount = m_MainLog_WritePos - m_MainLog_ReadPos; 
		if(SendCount > 0)
		{
			for(INT32 i = 0; i < SendCount; i++)
			{
				ReadIdx = m_MainLog_ReadPos % SEND_LOG_BUFFER_COUNT; 
	#ifdef TRACE_LOGD_PACKET
				I3TRACE( "%s\n",&m_pMainLogMessage[ReadIdx]._szMessage[STRING_START_IDX] ); 
				SendSize = 10; 
	#else				
				SendSize += m_pLogModuleSocket->SendPacketMessage( m_pMainLogMessage[ReadIdx]._szMessage, m_pMainLogMessage[ReadIdx]._Size ); 
	#endif
				m_LogCount++; 
				m_MainLog_ReadPos++; 				
			}
			IsSend = TRUE; 
		}

		//Auth Module Thread
		SendCount = m_AuthLog_WritePos - m_AuthLog_ReadPos; 
		if(SendCount > 0)
		{
			for(INT32 i = 0; i < SendCount; i++)
			{
				ReadIdx = m_AuthLog_ReadPos % SEND_LOG_BUFFER_COUNT; 
	#ifdef TRACE_LOGD_PACKET
				I3TRACE( "%s\n",&m_pAuthLogMessage[ReadIdx]._szMessage[STRING_START_IDX] ); 
				SendSize = 10; 
	#else				
				SendSize += m_pLogModuleSocket->SendPacketMessage( m_pAuthLogMessage[ReadIdx]._szMessage, m_pAuthLogMessage[ReadIdx]._Size ); 
	#endif
				m_LogCount++; 
				m_AuthLog_ReadPos++; 
			}
			IsSend = TRUE; 
		}

		//User Thread 
		for(UINT32 i = 0; i < m_ThreadCount; i++)
		{			
			LOG_SEND_MESSAGE * pLogBuffer = m_ppUserLogMessage[i]; 
			SendCount = m_pUserLog_WritePos[i] - m_pUserLog_ReadPos[i]; 			

			if(SendCount > 0)
			{
				for(INT32 j = 0; j < SendCount; j++)
				{
					ReadIdx = m_pUserLog_ReadPos[i] % SEND_LOG_BUFFER_COUNT; 
	#ifdef TRACE_LOGD_PACKET				
					I3TRACE( "%s\n", &pLogBuffer[ReadIdx]._szMessage[STRING_START_IDX] ); 				
					SendSize = 10; 
	#else					
					SendSize += m_pLogModuleSocket->SendPacketMessage( pLogBuffer[ReadIdx]._szMessage, pLogBuffer[ReadIdx]._Size );
	#endif
					m_LogCount++; 
					m_pUserLog_ReadPos[i]++; 
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
					SendInitMessage(); 
					SendFailCount = 0; 
				}
				else
				{
					m_pLogModuleSocket->OnDestroy(); 
					WRITE_LOG(NULL,"FAIL LOG_MODULE RECONNECT 2"); 
				}
			}
		}
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
	Temp		= 34; 
	i3mem::Copy( &pSendBuffer[3], &Temp,		sizeof(UINT32)); 
	Temp		= 3; 
	i3mem::Copy( &pSendBuffer[7], &Temp,		sizeof(UINT32)); 
	SendSize = PacketSize;
	m_pLogModuleSocket->SendPacketMessage(pSendBuffer, SendSize);		// STARTED

	return; 
}
