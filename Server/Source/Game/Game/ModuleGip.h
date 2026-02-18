#ifndef __MODULE_GIP_H__
#define __MODULE_GIP_H__

#include "GIP_Packet_PB.h"

class CUserSession; 

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

typedef struct _Gip_BackUp
{
	INT32				_iDataBaseID;	//다시 보낼패킷을 위해 저장	
	USER_INFO_BASIC		_UserInfoBasic; 
	USER_INFO_RECORD	_UserInfoRecord;
}GIP_BACKUP; 

typedef struct _Gip_Send_Message
{
	GIP_BACKUP					_BackUpInfo; 	
	Packet_TYPE_GAMESERVER_DATA	_Packet;
}GIP_SEND_MESSAGE;

class CModuleGipSocket; 

class CModuleGip : public i3Thread
{
	I3_CLASS_DEFINE( CModuleGip );
private:	
	HANDLE				m_hKillEvent;
	UINT32				m_ThreadCount;	

	//로그
	CModuleGipSocket	*	m_pGipModuleSocket; 

	GIP_SEND_MESSAGE	*	m_pMainGipMessage;				// Main Thread 
	INT32					m_MainGip_WritePos;				//
	INT32					m_MainGip_ReadPos;				//

	GIP_SEND_MESSAGE	*	m_pAuthGipMessage;				// Module Thread 
	INT32					m_AuthGip_WritePos;				//
	INT32					m_AuthGip_ReadPos;				//

	GIP_SEND_MESSAGE	**	m_ppUserGipMessage;				// Working Thread 
	INT32				*	m_pUserGip_WritePos;			//
	INT32				*	m_pUserGip_ReadPos;				//

	GIP_BACKUP			*	m_pBackUpBuffer; 	

	//GreetingPacket 
	Packet_TYPE_GREETING	m_GreetingPacket;
	UINT16					m_iQId;					//패킷 보낼때 유니크한 값
protected:
	void	_SendGreetingMessage(void); //컨텍후 처음 보내는 메시지 
	INT32	_SendGIpMessage(GIP_SEND_MESSAGE * pMessage); 
	void	_SendReqMessage(UINT32 Idx); 	
public: 
	CModuleGip(void); 
	virtual ~CModuleGip(void); 
	
	BOOL	OnCreate( UINT32 ThreadCount ); 
	void	OnDestroy(void); 
	
	void	SendGipMessage( INT32 ThreadIdx, GIP_MESSAGE GipId, CUserSession * pSession, INT32 Arg = 0); 

	virtual UINT32 OnRunning( void * pUserData); 
};

//extern CModuleGip * g_pModuleGip; 

#endif