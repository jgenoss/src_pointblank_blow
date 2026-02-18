#ifndef __MODULE_LOG_H__
#define __MODULE_LOG_H__

#include "LocalRingBuffer.h"

#define	SEND_LOG_BUFFER_COUNT		200
#define LOGD_BUFFER_SIZE			1024

enum LOG_MESSAGE
{
	//시스템 로그 
	LOG_MESSAGE_CCU			= 0,	//완료
	LOG_MESSAGE_LOCALE_CCU,

	// Sale	- 주의 : 
	LOG_MESSAGE_BUY_WEBSHOP,		// 구매	- DB WebGift Thread

	// 쓰레드가 달라서 요렇게.
	LOG_MESSAGE_GIFT_TASK,			// 선물 - Task Thread

	// 선물 - DB Shop Thread
	LOG_MESSAGE_GIFT_DBSHOP_0,		
	LOG_MESSAGE_GIFT_DBSHOP_1,
	LOG_MESSAGE_GIFT_DBSHOP_2,
	LOG_MESSAGE_GIFT_DBSHOP_3,
	LOG_MESSAGE_GIFT_DBSHOP_4,
	LOG_MESSAGE_GIFT_DBSHOP_5,
	LOG_MESSAGE_GIFT_DBSHOP_6,
	LOG_MESSAGE_GIFT_DBSHOP_7,
	LOG_MESSAGE_GIFT_DBSHOP_8,
	LOG_MESSAGE_GIFT_DBSHOP_9,

	LOG_MESSAGE_COUNT,
};


struct LOG_SEND_MESSAGE
{	
	INT16	_i16Size; 
	char	_strMessage[LOGD_BUFFER_SIZE];		//충분
};


#pragma pack( push, 1 )
struct LOG_BUY
{
	// 유저 정보
	T_UID			m_TUID;				// UID
	INT32			m_i32BuyPrice;		// 구매가격. 퍼블리셔가 입력
	T_GoodsID		m_TGoodsID;			// GoodsID
};

struct LOG_GIFT
{
	//SEND_GIFT*		m_pGift;
	T_UID						m_TSenderUID;				 
	T_UID						m_TReceiverUID;				 
	T_GoodsID					m_TGoodsID;		
	ITEM_INSERT_REQUEST_TYPE	m_eRequestType;
	UINT32						m_ui32ExpireDate;
	ENUM_TYPE_GIFT_DELIVER		m_eState;
};


#pragma pack(pop)

class CModuleLog : public i3Thread
{
	I3_CLASS_DEFINE( CModuleLog );
private:
	BOOL					m_bRunning;
	
	HANDLE					m_hFile;
	HANDLE					m_hBackupFile;
	INT32					m_i32DateHour;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//LOG D
	INT32					m_i32RingCount;
	Local_CRingBuffer*		m_pRingBuffer;

protected:
	BOOL					_CreateFile( HANDLE &hFile, char* strLogPath );
	UINT32					_CleanupOldBackup( INT32 i32Day );
	INT32					_GetFileDate( char* strHour );

public: 
	CModuleLog(void); 
	virtual ~CModuleLog(void); 
	
	BOOL					OnCreate( void ); 
	void					OnDestroy( void ); 
	
	virtual UINT32			OnRunning( void * pUserData); 

	void					SendLogMessage( LOG_MESSAGE LogId, UINT8 ui8LocaleCode, void* pParam1 = NULL, void* pParam2 = NULL );
};

extern CModuleLog * g_pModuleLogD; 

#endif