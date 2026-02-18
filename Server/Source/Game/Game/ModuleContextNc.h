#ifndef __MODULE_CONTEXT_B_H__
#define __MODULE_CONTEXT_B_H__

#include "RingBuffer.h"

class CRoom;
class CUserSession; 

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
	LOG_MESSAGE_U_GET_EXP,						//			경험치 획득
	
	LOG_MESSAGE_U_HACKING,						//1801		해킹로그

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


	// MEDAL
	LOG_MESSAGE_U_MEDAL_LEVELUP,				// 메달 레벨 달성. ID : 4001

	// CHARA
	LOG_MESSAGE_U_CHARA_CREATE,					//	캐릭터생성(구매). ID : 1701
	LOG_MESSAGE_U_CHARA_DELETE,					//	캐릭터삭제. ID : 1702
	LOG_MESSAGE_U_CLASS_SETTING,				//	병과 선택. ID : 1703

	// COUPON
	LOG_MESSAGE_U_USED_COUPON,					// 쿠폰사용. ID : 8001

	// GACHA
	LOG_MESSAGE_U_GACHA_SHOP,					//  뽑기상점 이용
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

enum ZLOG_RING_BUFFER
{
	ZLOG_RING_BUFFER_MAIN,
	ZLOG_RING_BUFFER_CAST,
	ZLOG_RING_BUFFER_TRANS,
	ZLOG_RING_BUFFER_CLAN,
	ZLOG_RING_BUFFER_MESS,
	ZLOG_RING_BUFFER_USER,
};

#pragma pack( push, 1 )
struct LOG_MEDAL_LEVELUP_INFO
{
	UINT16			m_ui16CompareCount;	// 비교할 달성 횟수

	MEDAL_TYPE		m_eMedalType;		// 메달 타입. MEDAL_TYPE
	MEDAL_SET_TYPE	m_eMedalSetType;	// 메달 셋트 타입. MEDAL_SET_TYPE
	UINT16			m_ui16MedalIdx;		// 메달 고유 인덱스
	UINT16			m_ui16MedalSetIdx;	// 메달 셋트 고유 인덱스.
	MEDAL_LEVEL		m_eLevel;			// 달성 레벨(1~4). 셋트 메달일 경우는 달성한 메달 순번(1~6)
};

struct LOG_CHARA_CREATE
{
	UINT8	m_ui8SlotIdx;
	UINT8	m_ui8CreateType;	// CHARA_RING_TYPE
	UINT32	m_ui32CharaItemID;
	UINT32	m_ui32SpendPoint;
	UINT32	m_ui32SpendCash;
};

struct LOG_KICK
{
	T_UID				m_TCallUID;		// 신고인 UID
	USER_KICK_TYPE		m_eType;		// Kick 유형

	// m_eType이 USER_KICK_TYPE_USER_VOTE 일때만 유효
	FORCEREMOVE_CAUSE	m_eReason;		// 투표 사유
	UINT8				m_ui8Good;		// 찬성표
	UINT8				m_ui8Bad;		// 반대표
};

struct LOG_USG
{
	UINT8				m_ui8Kick;				// Kick 여부
	char				m_strId[NET_ID_SIZE];	// 유저 ID
};

#pragma pack( pop )

class CModuleContextNC : public i3Thread
{
	I3_CLASS_DEFINE( CModuleContextNC );

private:
	BOOL					m_bRunning;
	UINT32					m_ui32ThreadCount;

	HANDLE					m_hFile;
	HANDLE					m_hBackupFile;
	INT32					m_i32DateHour;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//LOG D
	INT32					m_i32RingCount;
	NSM_CRingBuffer*		m_pRingBuffer;

protected:
	BOOL					_CreateFile( HANDLE &hFile, char* strLogPath );
	UINT32					_CleanupOldBackup( INT32 i32Day );
	INT32					_GetFileDate( char* strHour );
	BOOL					_CheckZLogWrite( ZLOG_WRITE_TYPE eType )
	{
		if (g_pContextMain->m_ui32ZLogWrite & (0x01 << eType)) return TRUE;
		return FALSE;
	}

public: 
	CModuleContextNC(void); 
	virtual ~CModuleContextNC(void); 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Common

	INT32					OnCreate( UINT32 ui32ThreadCount );
	void					OnDestroy(void);
	virtual UINT32			OnRunning( void * pUserData);

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

	void	LogMedalLevelUp( INT32 i32ThreadIdx, CUserSession * pSession , LOG_MEDAL_LEVELUP_INFO *pLogInfo  );	
};

extern CModuleContextNC * g_pModuleContextNC; 

#endif
