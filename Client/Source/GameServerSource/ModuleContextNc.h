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
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//유저 필요한 로그 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////    

	LOG_MESSAGE_U		= 100,		
	LOG_MESSAGE_U_LOGIN,						//1001
	LOG_MESSAGE_U_LOGOUT,						//1002
	LOG_MESSAGE_U_ENTER_CHANNEL,				//1003
	LOG_MESSAGE_U_LEAVE_CHANNEL,				//1004

	LOG_MESSAGE_U_CREATE_NICK,					//2002
	LOG_MESSAGE_U_CHANGE_NICK,					//2003
	LOG_MESSAGE_U_LEVEL_UP,						//2004
	LOG_MESSAGE_U_CHATTING,						//3001 
	LOG_MESSAGE_U_CHATTING_HACK,				//3002
	LOG_MESSAGE_U_TITLE_BUY,					//20012
	LOG_MESSAGE_U_TITLE_EQUIP,					//20014		// Zepetto 추가
	LOG_MESSAGE_U_QUEST_BUY,					//20015
	LOG_MESSAGE_U_QUEST_DELETE,					//20015
	LOG_MESSAGE_U_QUEST_FINISH,					//20013

	//Shop & Inventory							//작업중
	LOG_MESSAGE_U_SHOP_BUY,						//구입
	LOG_MESSAGE_U_SHOP_PRESENT,					//선물하기
	LOG_MESSAGE_U_INVEN_USE,					//사용

	LOG_MESSAGE_U_FRIEND_ACCEPT,				//10001		친구 신청
	LOG_MESSAGE_U_FRIEND_INSERT,				//10003		친구 추가 
	LOG_MESSAGE_U_FRIEND_DELETE,				//10004		친구 삭제

	LOG_MESSAGE_U_GET_ITEM,						//3006		아이템 획득
	LOG_MESSAGE_U_REPAIR,						//3006		아이템 수리
	LOG_MESSAGE_U_ITEM_DELETE,					//3006		아이템 삭제
	LOG_MESSAGE_U_CREATE_CLAN,					//3006		클랜 생성
	LOG_MESSAGE_U_CLAN_JOIN_REQ,				//3006		클랜 가입신청
	LOG_MESSAGE_U_CLAN_JOIN,					//3006		클랜 가입
	LOG_MESSAGE_U_CLAN_SECESSION,				//3006		클랜 탈퇴
	LOG_MESSAGE_U_CLAN_DEPORTATION,				//3006		클랜 제명
	LOG_MESSAGE_U_CLAN_DELETE,					//3006		클랜 해체
	LOG_MESSAGE_U_CLAN_MATCHING,				//3006		클랜 매칭
	LOG_MESSAGE_U_GET_POINT,					//3006		포인트 획득
	LOG_MESSAGE_U_PENALTY,						//3001		게임 도중 무단 이탈 패널티

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
	//룸만 필요한로그 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////    

	LOG_MESSAGE_R	= 200,	
	LOG_MESSAGE_R_BATTLE_START_ROOM,			//완료
	LOG_MESSAGE_R_BATTLE_END_ROOM,				//완료
	LOG_MESSAGE_R_ABUSER_CHECK,					//어뷰징 체크

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
	//유저,룸 필요한 로그 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
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
	LOG_MESSAGE_U_R_ESCAPE,						//탈출	- 101010

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
	//쿠폰 보상 내려받기
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
	//LOG_MESSAGE_U_COUPON_AWARD_ITEM,
};

enum ZLOG_CLAN_MATCH
{
	ZLOG_CLAN_NOT,
	ZLOG_CLAN_NORMAL,
	ZLOG_CLAN_MATCHING,
};

enum ZLOG_CHATTING_TYPE
{
	ZLOG_CHATTING_TYPE_LOBBY		= 1,
	ZLOG_CHATTING_TYPE_ROOM,
	ZLOG_CHATTING_TYPE_CLAN,
	ZLOG_CHATTING_TYPE_WHISPER,

	ZLOG_CHATTING_TYPE_MAX
};

#define LOG_MESSAGE_MAX					1024

struct LOG_SEND_MESSAGE
{	
	INT16	_Size; 
	char	_szMessage[LOG_MESSAGE_MAX];		//충분
};

struct REPAIR_INFO
{
	UINT32	m_ui32Point;
	UINT32	m_ui32Cash;
	UINT32	m_ui32ItemID;
	UINT32	m_ui32Arg;
};

class CRoom;
class CUserSession; 
class CModuleLogSocket;
class CModuleGipSocket; 

class CModuleContextNC : public i3Thread
{
	I3_CLASS_DEFINE( CModuleContextNC );
private:
	BOOL						m_bZLogRun;
	BOOL						m_bRunning;
	UINT32						m_ui32ThreadCount;	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//LOG D
	CModuleLogSocket		*	m_pLogModuleSocket; 
	LOG_SEND_MESSAGE		*	m_pLogMainMessage;				// Main Thread 
	INT32						m_i32LogMain_WritePos;
	INT32						m_i32LogMain_ReadPos;
	UINT32						m_ui32LogMain_Size;
	char						m_strMainTempBuffer[ LOG_MESSAGE_MAX ];

	LOG_SEND_MESSAGE		*	m_pLogControlMessage;			// Control Thread
	INT32						m_i32LogControl_WritePos;
	INT32						m_i32LogControl_ReadPos;
	UINT32						m_ui32LogControl_Size;
	char						m_strControlTempBuffer[ LOG_MESSAGE_MAX ];

	LOG_SEND_MESSAGE		*	m_pLogAuthMessage;				// ModuleA Thread
	INT32						m_i32LogAuth_WritePos;
	INT32						m_i32LogAuth_ReadPos;
	UINT32						m_ui32LogAuth_Size;
	char						m_strAuthTempBuffer[ LOG_MESSAGE_MAX ];

	LOG_SEND_MESSAGE		*	m_pLogClanMessage;				// ModuleA Thread
	INT32						m_i32LogClan_WritePos;
	INT32						m_i32LogClan_ReadPos;
	UINT32						m_ui32LogClan_Size;
	char						m_strClanTempBuffer[ LOG_MESSAGE_MAX ];

	LOG_SEND_MESSAGE		*	m_pLogMessMessage;				// ModuleA Thread
	INT32						m_i32LogMess_WritePos;
	INT32						m_i32LogMess_ReadPos;
	UINT32						m_ui32LogMess_Size;
	char						m_strMessTempBuffer[ LOG_MESSAGE_MAX ];

	LOG_SEND_MESSAGE		**	m_ppLogUserMessage;				// Working Thread 
	INT32					*	m_pLogUser_WritePos;
	INT32					*	m_pLogUser_ReadPos;
	UINT32					*	m_pui32LogUser_Size;
	char					**	m_ppstrUserTempBuffer;

	BOOL	CheckPos( INT32* pi32WritePos, INT32* pi32ReadPos );
	
protected:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//LOG_D
	BOOL	_LogCreate(void); 
	INT32	_LogUpdate(void); 
	void	_LogDestroy(void); 

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

public: 
	CModuleContextNC(void); 
	virtual ~CModuleContextNC(void); 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Common

	void			SendZLog();

	INT32			OnCreate( UINT32 ui32ThreadCount );
	void			OnDestroy(void);
	virtual UINT32	OnRunning( void * pUserData);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//LOGD	
	void	LogSendMessage(			INT32 i32ThreadIdx, LOG_MESSAGE LogId, CUserSession * pSession, void* pArg0=NULL, void* pArg1=NULL, void* pArg2=NULL, void* pArg3=NULL );

	void	LogChangeString( char * pDes, char * pSour, INT32 i32MaxCount)		//GIP에서 사용하는 스트링으로 변경합니다. 
	{
		INT32 i32Count = 0;
		while( '\0' != *pSour )
		{
			switch( *pSour )
			{
			case '\r':
			case '\n':
				*pDes = ' ';
				break;
			default:
				*pDes = *pSour;
				break;
			}
			i32Count++;

			pSour++;
			pDes++;
			if( i32MaxCount <= i32Count ) break;
		}
		*pDes = '\0';
	}

	INT32	GetStageMode( UINT8 ui8WeaponFlag );	// 룸의 WeaponFlag 값을 이용해 StageMode를 얻음.

	// 로그아웃시 ZLog의 Time2에 남길 로그인 시간에 대한 UINT32 <-> STRING 변경 함수
	void ConverTimeFromtUINT64ToSTR(char* pstrDate, UINT64 ui64LoginTime);

};

extern CModuleContextNC * g_pModuleContextNC; 

#endif
