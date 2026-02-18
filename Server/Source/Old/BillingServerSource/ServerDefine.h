#ifndef __SERVER_DEFINE_H__
#define __SERVER_DEFINE_H__

//////////////////////////////////////////////////////////////////////
// Default Define
#define NO_STR_TBL								//스트링 테이블 사용하지 않는다. 

#define	ACCEPTOR_RECV_TIME			2			//소켓 접속 확인 시간	ms
#define	SERVER_STRING_COUNT			1024		//
#define MAXPACKETBUFFERSIZE			66560		// 65kb	- IBGameInterface에서 사용

#define DATABASE_THREAD_COUNT		10
#define SQL_STRING_COUNT			1024

#define BUFFER_MAX_INT_COUNT		2000000000

enum SERVER_STATE
{
	SERVER_STATE_NONE		= 0,
	SERVER_STATE_LOADING,
	SERVER_STATE_INIT,
	SERVER_STATE_INITING,
	SERVER_STATE_START,
};

enum SERVER_START_ERROR 
{
	SERVER_ERROR_SUCCESS	=	0x00000000,
	SERVER_ERROR_MEMORY		=	0X80000001,
	SERVER_ERROR_NETWORK,
	SERVER_ERROR_SYSTEM,
	SERVER_ERROR_CONFIG,
	SERVER_ERROR_LOGFILE,

	SERVER_ERROR_CONTROLSERVER,
	SERVER_ERROR_TASK,

	//SERVER_ERROR_LOGD,
	//SERVER_ERROR_DATABASE,
	//SERVER_ERROR_CONTEXT,

	SERVER_START_ERROR_MAX,
};

//////////////////////////////////////////////////////////////////////
// Server Define

#define RING_RECIVE_COUNT					300
#define RING_DBUSERCASH_GET_IN_COUNT		300
#define RING_DBUSERCASH_GET_OUT_COUNT		300
#define RING_DBUSERCASH_BUY_IN_COUNT		300
#define RING_DBUSERCASH_BUY_OUT_COUNT		300
#define RING_DBUSERCASH_LINK_IN_COUNT		300
#define RING_DBUSERCASH_LINK_OUT_COUNT		300

enum LINK_METHOD
{
	LINK_METHOD_NO		= 1,
	LINK_METHOD_AUTO,
	LINK_METHOD_TCP,
	LINK_METHOD_WEB,
	LINK_METHOD_DB,
	LINK_METHOD_LIB,

	LINK_METHOD_MAX,
};

#pragma pack( push, 1 )

typedef struct _DataQueryInfo
{
	CADODatabase*	_pDB;
	char			_pQuery[SQL_STRING_COUNT];
}DB_QUERY_INFO;

typedef struct __tag_GetCashIn
{	
	INT32		_i32ServerIdx;
	char		_strID[ MAX_ID_SIZE ];
	INT64		_i64UID;
}TYPE_GET_CASH_IN;

typedef struct __tag_GetCashOut
{
	INT32		_i32ServerIdx;
	char		_strID[ MAX_ID_SIZE ];
	INT64		_i64UID;
	INT32		_i32ReturnCode;
	INT64		_i64Cash;
}TYPE_GET_CASH_OUT;

typedef struct __tag_ItemBuyIn
{
	INT32		_i32ServerIdx;
	char		_strID[ MAX_ID_SIZE ];
	INT64		_i64UID;
	UINT32		_ui32Price;
	UINT32		_ui32MainBufferIdx;
}TYPE_ITEM_BUY_IN;

typedef struct __tag_ItemBuyOut
{
	INT32		_i32ServerIdx;
	char		_strID[ MAX_ID_SIZE ];
	INT64		_i64UID;
	INT32		_i32ReturnCode;
	INT64		_i64Cash;
	UINT32		_ui32MainBufferIdx;
}TYPE_ITEM_BUY_OUT;

typedef struct __tag_LinkIn
{
	UINT64		_ui64Idx;
	char		_strID[ MAX_ID_SIZE ];
	INT64		_i64Cash;	
}TYPE_LINK_IN;

typedef struct __tag_LinkOut
{
	UINT64		_ui64Idx;
	char		_strID[ MAX_ID_SIZE ];
	INT64		_i64Cash;
}TYPE_LINK_OUT;


typedef struct __tag_LinkBrazil
{
	UINT64		_ui64Idx;
	char		_strID[ MAX_ID_SIZE ];
	INT64		_i64Cash;
	INT8		_i8Type;
}TYPE_LINK_IN_BRAZIL;

#pragma pack( pop )


#endif