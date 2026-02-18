#ifndef __MODULE_LOG_H__
#define __MODULE_LOG_H__

class CRoom; 
class CUserSession; 

#include "ModuleLogSocket.h"

enum LOG_MESSAGE
{
	//시스템 로그 
	LOG_MESSAGE_		= 0,		
	LOG_MESSAGE_SERVER_START,					//완료
	LOG_MESSAGE_SERVER_END,						//완료
	LOG_MESSAGE_SERVER_STATE,					//완료
	LOG_MESSAGE_SERVER_CHANNEL_USERCOUNT,		//완료
	LOG_MESSAGE_SERVER_ALL_USERCOUNT,		//

	//유저 필요한 로그 
	LOG_MESSAGE_U		= 100,		
	LOG_MESSAGE_U_LOGIN,						//완료
	LOG_MESSAGE_U_LOGOUT,						//완료
	LOG_MESSAGE_U_ENTER_CHANNEL,				//완료		
	LOG_MESSAGE_U_LEAVE_CHANNEL,				//완료

	LOG_MESSAGE_U_CREATE_CHARA,					//완료
	LOG_MESSAGE_U_LEVEL_UP,						//완료
	LOG_MESSAGE_U_CHATTING,						//완료 

	//룸만 필요한로그 
	LOG_MESSAGE_R	= 200,	
	LOG_MESSAGE_R_BATTLE_START_ROOM,			//완료
	LOG_MESSAGE_R_BATTLE_END_ROOM,				//완료

	//유저,룸 필요한 로그 
	LOG_MESSAGE_U_R		= 300,		
	LOG_MESSAGE_U_R_CREATE_ROOM,				//완료
	LOG_MESSAGE_U_R_ENTER_ROOM,					//완료
	LOG_MESSAGE_U_R_LEAVE_ROOM,					//완료
	LOG_MESSAGE_U_R_KICK_LEAVE_ROOM,			//완료
	LOG_MESSAGE_U_R_BATTLE_START,				//완료
	LOG_MESSAGE_U_R_BATTLE_END,					//완료
	LOG_MESSAGE_U_R_KILL,						
	LOG_MESSAGE_U_R_DEATH, 
	LOG_MESSAGE_U_R_RESPAWN,					//완료
};

typedef struct 
{	
	INT16	_Size; 
	char	_szMessage[4096];		//충분
}LOG_SEND_MESSAGE;


class CModuleLog : public i3Thread
{
	I3_CLASS_DEFINE( CModuleLog );
private:	
	HANDLE				m_hKillEvent;
	UINT32				m_ThreadCount;	

	INT32				m_LogCount; 	

	//로그
	CModuleLogSocket	*	m_pLogModuleSocket; 

	LOG_SEND_MESSAGE	*	m_pMainLogMessage;				// Main Thread 
	INT32					m_MainLog_WritePos;				//
	INT32					m_MainLog_ReadPos;				//

	LOG_SEND_MESSAGE	*	m_pAuthLogMessage;				// Module Thread 
	INT32					m_AuthLog_WritePos;				//
	INT32					m_AuthLog_ReadPos;				//

	LOG_SEND_MESSAGE	**	m_ppUserLogMessage;				// Working Thread 
	INT32				*	m_pUserLog_WritePos;			//
	INT32				*	m_pUserLog_ReadPos;				//

protected:
	void	SendInitMessage(void); 	
public: 
	CModuleLog(void); 
	virtual ~CModuleLog(void); 
	
	BOOL	OnCreate( UINT32 ThreadCount ); 
	void	OnDestroy(void); 
	
	void	SendLogMessage(		INT32 ThreadIdx, LOG_MESSAGE LogId, CUserSession * pSession, CRoom * pRoom );													//공용 메시지 

	//공용에 넣을수 없는 것들은 이렇게 따로 빼서 만들었습니다. 
	void	SendBattleEndPerson(INT32 ThreadIdx, CUserSession * pSession, CRoom * pRoom, INT32 SlotIdx, INT32 EndType);											//배틀에서 나갈때 
	void	SendChattingMessage(INT32 ThreadIdx, CUserSession * pSession, CRoom * pRoom, char * pMessage);														//채팅 메시지
	void	SendKillMessage(	INT32 ThreadIdx, CUserSession * pKillSession, CUserSession * pDeathSession, CRoom * pRoom, LOG_KILL_DEATH_INFO * pKillInfo);	//킬 메시지

	//친구 로그
	void	SendFriendInsert(	INT32 ThreadIdx, CUserSession * pSession, FRIEND_INFO_STRUCT * pFriendInfo );//친구 신청	
	void	SendFriendAccept(	INT32 ThreadIdx, CUserSession * pSession, FRIEND_INFO_STRUCT * pFriendInfo );//친구 추가
	void	SendFriendDelete(	INT32 ThreadIdx, CUserSession * pSession, FRIEND_INFO_STRUCT * pFriendInfo );//친구 삭제 

	virtual UINT32 OnRunning( void * pUserData); 
};

//extern CModuleLog * g_pModuleLog; 

#endif