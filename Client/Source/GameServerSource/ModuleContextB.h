#ifndef __MODULE_CONTEXT_B_H__
#define __MODULE_CONTEXT_B_H__

#include "GIP_Packet_PB.h"

enum LOG_MESSAGE
{
	//시스템 로그 
	LOG_MESSAGE_		= 0,		
	LOG_MESSAGE_SERVER_START,					//완료
	LOG_MESSAGE_SERVER_END,						//완료
	LOG_MESSAGE_SERVER_STATE,					//완료
	LOG_MESSAGE_SERVER_CHANNEL_USERCOUNT,		//완료
	LOG_MESSAGE_SERVER_ALL_USERCOUNT,			//
	
	//유저 필요한 로그 
	LOG_MESSAGE_U		= 100,		
	LOG_MESSAGE_U_LOGIN,						//완료
	LOG_MESSAGE_U_LOGOUT,						//완료
	LOG_MESSAGE_U_ENTER_CHANNEL,				//완료		
	LOG_MESSAGE_U_LEAVE_CHANNEL,				//완료

	LOG_MESSAGE_U_CREATE_CHARA,					//완료
	LOG_MESSAGE_U_LEVEL_UP,						//완료
	LOG_MESSAGE_U_CHATTING,						//완료 
	LOG_MESSAGE_U_QUEST,						// 

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

enum GIP_MESSAGE
{	
	GIP_MESSAGE_ = 0,
	GIP_MESSAGE_CREATE_NICK_CHAR,	//CMD_InsertCharacterInfo				//처음 닉네임을 생성할때 보낸다. 
	GIP_MESSAGE_CREATE_NICK_USER,	//CMD_InsertUserInfo					//처음 닉네임을 생성할때 보낸다. 
	GIP_MESSAGE_CHRA,				//CMD_UpdateChacracterRecordInfo		//로그 아웃시.
	GIP_MESSAGE_USER,				//CMD_UpdateUserInfo					//로그 아웃시.
	GIP_MESSAGE_MEDAL_MINI,			//CMD_InsertCharacterPropertyInfo		//메달 받을때.
	GIP_MESSAGE_MEDAL_INSI,			//CMD_InsertCharacterPropertyInfo		//메달 받을때.
	GIP_MESSAGE_MEDAL_MEDA,			//CMD_InsertCharacterPropertyInfo		//메달 받을때.
	GIP_MESSAGE_CHALLENGE,			//CMD_InsertCharacterPropertyInfo		//도전과제 완료.
	GIP_MESSAGE_ENTER_CHANNEL,		//CMD_UpdateCharacterChannel			//채널 이동
};

typedef struct _tagGipBackUp
{
	INT32				_iDataBaseID;	//다시 보낼패킷을 위해 저장	
	USER_INFO_BASIC		_UserInfoBasic; 
	USER_INFO_RECORD	_UserInfoRecord;
}GIP_BACKUP; 

typedef struct _tagGipBasicSendBuffer
{
	GIP_BACKUP					_BackUpInfo; 	
	Packet_TYPE_GAMESERVER_DATA	_Packet;
}GIP_SEND_MESSAGE;

typedef struct _tagLogSendBuffer
{	
	INT16	_Size; 
	char	_szMessage[4096];		//충분
}LOG_SEND_MESSAGE;

class CRoom;
class CUserSession; 
class CModuleLogSocket;
class CModuleGipSocket; 

class CModuleContextNC : public i3Thread
{
	I3_CLASS_DEFINE( CModuleContextNC );
private:	
	UINT32				m_ThreadCount;	
	HANDLE				m_hKillEvent;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//GIP
	CModuleGipSocket		*	m_pGipModuleSocket; 
	GIP_SEND_MESSAGE		*	m_pGipMainMessage;				// Main Thread 
	INT32						m_iGipMain_WritePos;	
	INT32						m_iGipMain_ReadPos;	
	GIP_SEND_MESSAGE		*	m_pGipAuthMessage;				// ModuleA Thread 
	INT32						m_iGipAuth_WritePos;	
	INT32						m_iGipAuth_ReadPos;	
	GIP_SEND_MESSAGE		**	m_ppGipUserMessage;				// Working Thread 
	INT32					*	m_pGipUser_WritePos;
	INT32					*	m_pGipUser_ReadPos;	
	GIP_BACKUP				*	m_pGipBackUpBuffer; 			// Backup Buffer		
	UINT16						m_iGipQId;						//패킷 보낼때 유니크한 값

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//LOG D
	CModuleLogSocket		*	m_pLogModuleSocket; 
	LOG_SEND_MESSAGE		*	m_pLogMainMessage;				// Main Thread 
	INT32						m_iLogMain_WritePos;				
	INT32						m_iLogMain_ReadPos;				
	LOG_SEND_MESSAGE		*	m_pLogAuthMessage;				// ModuleA Thread 
	INT32						m_iLogAuth_WritePos;				
	INT32						m_iLogAuth_ReadPos;				
	LOG_SEND_MESSAGE		**	m_ppLogUserMessage;				// Working Thread 
	INT32					*	m_pLogUser_WritePos;			
	INT32					*	m_pLogUser_ReadPos;				
	
protected:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//GIP 
	BOOL	_GipCreate(void); 
	INT32	_GipUpdate(void); 
	void	_GipDestroy(void); 
	void	_GipSendGreetingMessage(void); //컨텍후 처음 보내는 메시지 
	INT32	_GipMessage(GIP_SEND_MESSAGE * pMessage); 
	void	_GipSendReqMessage(UINT32 Idx); 
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//LOG_D
	BOOL	_LogCreate(void); 
	INT32	_LogUpdate(void); 
	void	_LogDestroy(void); 
	void	_LogSendInitMessage(void);

	//땜빵코드 입니다. 확실하지 않아서 이렇게 만들어요. 성백 20080515
	INT32	m_piStageIdx[256];
	INT32 _GetLogStageIndex(INT32 iStageIdx)
	{//스테이지가 255개 이상이되면 수정해야합니다.
		return m_piStageIdx[(iStageIdx & 0x000000FF)]; 
	}

public: 
	CModuleContextNC(void); 
	virtual ~CModuleContextNC(void); 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Common
	INT32			OnCreate( UINT32 ThreadCount );
	void			OnDestroy(void);
	virtual UINT32	OnRunning( void * pUserData);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//GIP 	
	void	GipSendMessage( INT32 ThreadIdx, GIP_MESSAGE GipId, CUserSession * pSession, INT32 Arg = 0); 	

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//LOGD	
	void	LogSendMessage(			INT32 ThreadIdx, LOG_MESSAGE LogId, CUserSession * pSession, CRoom * pRoom );
	void	LogSendBattleEndPerson(	INT32 ThreadIdx, CUserSession * pSession, CRoom * pRoom, INT32 SlotIdx, INT32 EndType);											//배틀에서 나갈때 
	void	LogSendChattingMessage(	INT32 ThreadIdx, CUserSession * pSession, CRoom * pRoom, char * pMessage,UINT16 Type = 1);										//채팅 메시지
	void	LogSendKillMessage(		INT32 ThreadIdx, CUserSession * pKillSession, CUserSession * pDeathSession, CRoom * pRoom, LOG_KILL_DEATH_INFO * pKillInfo);	//킬 메시지
	void	LogSendFriendInsert(	INT32 ThreadIdx, CUserSession * pSession, FRIEND_INFO_STRUCT * pFriendInfo );													//친구 신청	
	void	LogSendFriendAccept(	INT32 ThreadIdx, CUserSession * pSession, FRIEND_INFO_STRUCT * pFriendInfo );													//친구 추가
	void	LogSendFriendDelete(	INT32 ThreadIdx, CUserSession * pSession, FRIEND_INFO_STRUCT * pFriendInfo );													//친구 삭제 
};

extern CModuleContextNC * g_pModuleContextNC; 

#endif
