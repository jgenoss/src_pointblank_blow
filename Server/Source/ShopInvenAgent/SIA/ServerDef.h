#ifndef __SERVERDEF_H
#define	__SERVERDEF_H

#include <pdh.h>
#include <pdhmsg.h>

#define USE_SYSTEM_LOG

#if defined(BUILDMODE_SEV)
//-----------------------------------------------------
//배포판 빌드
#define	BUILD_VER				"DIST"
//-----------------------------------------------------
#else
//-----------------------------------------------------
//디버깅용
#define	BUILD_VER				"DEBUG"
//-----------------------------------------------------
#endif
#define DEF_TRANS_SERVER_IDX	1		// Default 트랜스 서버 인덱스. 트랜스 서버 n개로 바뀌면 삭제 해야 합니다.

#define USP_SM_GET_CASH	L"EXEC usp_GameCash_sel %I64d;"

#define USP_SM_BUY_GOODS L"EXECUTE usp_BuyGoods_ins %I64d,%d,%d,%d,%d"

#define SERVER_EVENT_FAIL_REASON	30 

#define MAX_LINK_THREAD_COUNT		64
#define MAX_LINK_BUFFER_COUNT		2048

enum SERVER_STATE
{
	SERVER_STATE_NONE		= 0,
	SERVER_STATE_LOADING,
	SERVER_STATE_INIT,
	SERVER_STATE_INITING,
	SERVER_STATE_START,
};

#define DEF_LOGIN_DELAY_TIME		5

enum LINK_TYPE
{
	LINK_TYPE_BUY_GOODS	 = 0,
	LINK_TYPE_GET_CASH,
	LINK_TYPE_USE_GIFTCOUPON,
};

#define SERVER_MAIN_UPDATE_TIME			100

//#define USE_SERVER_WARNING
//#define DEF_LINK_TIME_OVER				0.5f	// 빌링단의 느려짐 감지( 500ms )
//#define DEF_DB_LIMIT_TIME				500		// DB의 느려짐 감지

#define WARNING_LEVEL_COUNT					5

#define WARNING_NO							0			// 워닝 요소가 아님.
#define WARNING_DATABASE_TIMEOUT			1			// DB 시간 초과, DB 관련 Buffer Overflow
#define WARNING_DATABASE_RECONNECT			2			// DB Reconnect
#define WARNING_AUTH						3			// Web, Billing, Login / Logout 관련
#define WARNING_TASK						4			// Task, Update Time 관련
#define WARNING_NETWORK						5			// Server Reconnect, Server TimeOut

#define WARNING_NOT_EXIST					0			// 워닝이 존재하지 않음
#define WARNING_EXIST						1			// 워닝이 존재함

// GCA PC방
#define GCAPLUS_MASK_THAI					0x0200		// 태국 GCA PLUS PC방 확인 비트마스크
#define GCA_MASK_THAI						0x0100		// 태국 GCA PC방 확인 비트마스크
#define	GCA_MASK_SINGMAL					0x0001		// 싱/말 GCA PC방 확인 비트마스크
#define GCA_MASK_VIETNAM					0x0001		// 베트남 GCA PC방 확인 비트마스크
#define GCA_MASK_PHLIPPINES					0x0100		// 필리핀 GCA PC방 확인 비트마스크

//----------------------------------------------------------------------------------------------
//Evnet Return Code 
#define EVENT_SUCCESS										0X00000001
#define EVENT_FAIL_ERROR									0x80000000	// 기본 실패

#define EVENT_FAIL_CREATE_TASK								0X80000001
#define EVENT_FAIL_CREATE_DATABASE							0X80000002
#define EVENT_FAIL_CREATE_INI								0X80000003
#define EVENT_FAIL_CREATE_LINK								0X80000004
#define EVENT_FAIL_CREATE_LOG								0X80000006
#define EVENT_FAIL_CREATE_SERVICE_LOG						0X80000007
#define EVENT_FAIL_CREATE_INI_VALUE							0X80000008
#define EVENT_FAIL_CREATE_NETWORK							0X80000009
#define EVENT_FAIL_CREATE_SHOP								0X8000000A
#define EVENT_FAIL_CREATE_CONTROL							0X8000000B
#define EVENT_FAIL_CREATE_SYSTEMLOG							0X8000000C
#define EVENT_FAIL_RUSSIA_INIT								0X8000000D
#define EVENT_FAIL_CREATE_FIELDSHOP							0X8000000E
#define EVENT_FAIL_CREATE_LOGIN								0X8000000F
#define EVENT_FAIL_CREATE_BUY_BUFFER						0X80000010
#define EVENT_FAIL_CREATE_LOGIN_BUFFER						0X80000011
#define EVENT_FAIL_CREATE_PCCAFE							0X80000012

#define EVENT_FAIL_RINGBUFFER_MAX							0X81000001
#define EVENT_FAIL_RINGBUFFER_OF							0X81000002
#define EVENT_FAIL_DATABASE_SQL								0X81000003
#define EVENT_FAIL_DATABASE_AUTH							0X81000004
#define EVENT_FAIL_DATABASE_CHECK							0X81000005
#define EVENT_FAIL_DATABASE_REPAIR							0X81000006
#define EVENT_FAIL_DATABASE_REPAIR_AUTHERROR				0X81000007

#define EVENT_FAIL_DIFFERENT_BUILD_VERSION					0X81000008
#define EVENT_FAIL_MAINSTREAMBUFFER_FULL					0X81000009
#define EVENT_FAIL_MAINSTREAMBUFFER_

// #define OLD_VER

#ifdef OLD_VER
#define LINK_RESULT_TYPE_E01_NO_SERVERLIST					0X82000001
#define LINK_RESULT_TYPE_E02_NO_ACTIONLIST					0X82000002
#define LINK_RESULT_TYPE_E03_USER_NOT_EXIST					0X82000003
#define LINK_RESULT_TYPE_E04_BALANCE_NOT_ENOUGH				0X82000004
#define LINK_RESULT_TYPE_E05_SYSTEM_ERROR					0X82000005
#define LINK_RESULT_TYPE_E06_SERVICE_NOT_AVAILABLE			0X82000006
#define LINK_RESULT_TYPE_E07_KEY_VALUE_NOT_MATCH			0X82000007
#define LINK_RESULT_TYPE_E08_PARAMETER_TOO_LONG				0X82000008
#define LINK_RESULT_TYPE_E09_PARAMETER_RANGE_NOT_CORRECT	0X82000009		
#define LINK_RESULT_TYPE_UNKNOWN_ERROR						0X82000010							// 알려지지 않은 에러
#define LINK_RESULT_TYPE_PACKET_BREAK						0X82000011

#else
#define LINK_RESULT_TYPE_E001_NOT_ALLOW_METHOD				0X82000001	
#define LINK_RESULT_TYPE_E010_NOT_MATCH_AUTH_KEY			0X82000002	
#define LINK_RESULT_TYPE_E020_UNKNOWN_SYSTEM_ERROR			0X82000003
#define LINK_RESULT_TYPE_E050_SYSTEM_ERROR					0X82000004	
#define LINK_RESULT_TYPE_E051_DATABASE_ERROR				0X82000005	
#define LINK_RESULT_TYPE_E100_NOT_FIND_PARAMETER0			0X82000006	
#define LINK_RESULT_TYPE_E200_NOT_ALLOW_FORMAT				0X82000007	
#define LINK_RESULT_TYPE_E201_NOT_MATCH_PRICE				0X82000008	
#define LINK_RESULT_TYPE_E202_EVENT_CODE_NOT_EXIST			0X82000009	
#define LINK_RESULT_TYPE_E203_EVENT_NAME_NOT_EXIST			0X82000010	
#define LINK_RESULT_TYPE_E204_TOTAL_PRICE_TOO_HIGH			0X82000011	
#define LINK_RESULT_TYPE_E300_INVALID_SHOP_CODE				0X82000012	
#define LINK_RESULT_TYPE_E301_SHOP_DISABLED					0X82000013	
#define LINK_RESULT_TYPE_E302_NOT_ALLOW_SHOP_IP				0X82000014	
#define LINK_RESULT_TYPE_E303_NOT_USE_SSL					0X82000015	
#define LINK_RESULT_TYPE_E304_LOWER_THAN_MIN_PRICE			0X82000016	
#define LINK_RESULT_TYPE_E305_NOT_USE_PAYMENT_GATEWAY		0X82000017	 
#define LINK_RESULT_TYPE_E400_NOT_FIND_BILLING_ACCOUNT		0X82000018	
#define LINK_RESULT_TYPE_E401_BLOCK_ACCOUNT					0X82000019	
#define LINK_RESULT_TYPE_E402_ROW_CASH_BALANCE				0X82000020
#define LINK_RESULT_TYPE_E403_PAYMENT_LIMITOVER				0X82000021	
#define LINK_RESULT_TYPE_E404_PG_LIMITOVER					0X82000022	
#define LINK_RESULT_TYPE_E405_USER_IP_INFO_REQUIRE			0X82000023	
#define LINK_RESULT_TYPE_E406_BLOCK_USER_IP					0X82000024	
#define LINK_RESULT_TYPE_E407_NOT_ALLOW_USER				0X82000025	
#define LINK_RESULT_TYPE_UNKNOWN_ERROR						0X82000026							// 알려지지 않은 에러
#define LINK_RESULT_TYPE_PACKET_BREAK						0X82000027							// 패킷 깨짐
#endif
#define EVENT_FAIL_SHOP_NOUID								0X83000010
#define EVENT_FAIL_SHOP_DB_OF								0X83000020
#define EVENT_FAIL_SHOP_LINK_OF								0X83000030
#define EVENT_FAIL_SHOP_GOODS								0X83000040
#define EVENT_FAIL_SHOP_GOODS_BUFFER_OF						0X83000041
#define EVENT_FAIL_SHOP_GOODS_NOHAVE						0X83000042
#define EVENT_FAIL_SHOP_GOODS_NOMONEY						0X83000043
#define EVENT_FAIL_SHOP_NO_BUY								0X83000044
#define EVENT_FAIL_SHOP_GOODS_HAVEITEM						0X83000045
#define EVENT_FAIL_SHOP_AUTH_ALREADY						0X83000046

/////////////////////////////////////////////////////////////////////////////////
//////	Taiwan Billing  /////////////////////////////////////////////////////////
#define EVENT_FAIL_BUY_FAIL									0X85020000			// 대만 빌링 구매 실패.
#define EVENT_FAIL_BUY_FAIL_UNKNOWN							0X85020001			// 알수 없는 이유로 구매 실패.
#define EVENT_FAIL_BUY_FAIL_NOT_ENOUGH_CASH					0X85020002			// 캐쉬 잔액이 부족하여 구매 실패.

//Count or Size Define 
#define PATH_STRING_COUNT				512
#define BUFFER_MAX_INT_COUNT			2000000000
#define IP_NAME_COUNT					32

#define	LOG_FILE_BUFFER_COUNT			250
#define	SERVICE_LOG_BUFFER_COUNT		250
#define DATABASE_BUFFER_COUNT			500

//#define WEBSHOP_MAX						10
#define WEBGIFT_MAX						10

#define WEBGIFT_INSERT_BEFORE			0
#define WEBGIFT_INSERT_COMPLETE			1
#define WEBGIFT_INSERT_FAIL				2

//Thread Priority 
#define THREAD_PRIORITY_TASK				THREAD_PRIORITY_ABOVE_NORMAL	//
#define THREAD_PRIORITY_NETWORK				THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_LOG					THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_SERVICE_LOG			THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_DATABASE			THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_LINK				THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_WEBSHOP				THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_CONTROL				THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_WEBGIFT				THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_PCCAFE				THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_ACCOUNTTRANS		THREAD_PRIORITY_NORMAL			//

//Thread Idx
#define THREAD_MAIN_IDX					-100
#define THREAD_TASK_IDX					-200
#define	THREAD_NETWORK_IDX				-300
#define THREAD_CONTROL_IDX				-400

#define	THREAD_S_LINK_IDX				0			// 0~99개 입니다. 
#define	THREAD_S_LOGIN_IDX				100			// 100~199개 입니다. 



//////////////////////////////////////////////////////////////////////////
#define SERVER_STRING_COUNT			512
#define NET_ID_SIZE					65
#define ITEM_ID_SIZE				11 // 스트링 변환
//#define NET_NICK_NAME_SIZE			MAX_USER_NICK				// 한글/영문 16자 (note. 다른 언어일 경우 16자 아닐 수 있음.)

#define RETURNCODE_MAX				8

#define NESIA_SHOP_ID			"PTBLK"
#define NESIA_SHOP_KEY			"PB8923KIOPEOOPP"
#define NESIA_PG_CODE			"P001"

#define	USE_SERVER_WARNING

#ifdef BUILDMODE_DLG
	#define MAX_CONNECT_USER_DIVISOR			31			//20 이상 소수
#else
	#define MAX_CONNECT_USER_DIVISOR			3001		//20 이상 소수
#endif

//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
extern UINT32			g_ui32StartTime;
//----------------------------------------------------------------------------------------------
//글로벌 변수	프로그램 시작시 설정됨 
extern char				g_strCCurrentPath[PATH_STRING_COUNT];			//파일 패스 
extern SERVER_STATE		g_eServerState;

extern i3LogFileW*		g_pLog;
// 컨트롤서버 빌드 버전 (New)

//----------------------------------------------------------------------------------------------
//글로벌 함수 
void				g_SetCurrentPathW(WCHAR16 * pPath); 
void				g_SetCurrentPath(char	* pPath);  
int					g_InitServer(void); 
void				g_UpdateServer(void); 
void				g_DestroyServer(INT32 i32DestroyReason);
char*				g_Int32ToStringIP( UINT32 ui32IP );

#define SIA_PACKET_HEADER_SIZE		6
#define I3_PACKET_HEADER_SIZE		4
#define NET_PACKET_BUFFER_SIZE		8912

//----------------------------------------------------------------------------------------------
enum TYPE_LOGIN
{
	TYPE_LOGIN_ID,
	TYPE_LOGIN_LOGOUT,
	TYPE_LOGIN_CASH_CHECK,
};

enum TYPE_PCCAFE_INPUT
{
	TYPE_PCCAFE_INPUT_FIRSTLOGIN,		// 로그인 요청			: 위치 바뀌면 안됨
	TYPE_PCCAFE_INPUT_LOGIN,			// 재로그인 요청		: 위치 바뀌면 안됨
	TYPE_PCCAFE_INPUT_CHECK_AMOUNT,		// 정량 보유여부 조회	: 위치 바뀌면 안됨
	TYPE_PCCAFE_INPUT_LOGOUT,			// 로그아웃 요청
};

enum TYPE_PCCAFE_OUTPUT
{
	TYPE_PCCAFE_OUTPUT_FIRSTLOGIN,		// 로그인 응답			: 위치 바뀌면 안됨
	TYPE_PCCAFE_OUTPUT_LOGIN,			// 재로그인 응답		: 위치 바뀌면 안됨
	TYPE_PCCAFE_OUTPUT_CHECK_AMOUNT,	// 정량 보유여부 응답	: 위치 바뀌면 안됨
	TYPE_PCCAFE_OUTPUT_SUBTRACTION,		// 차감정보 알림
	TYPE_PCCAFE_OUTPUT_CONNECT,			// 연동서버와 컨넥 알림
	TYPE_PCCAFE_OUTPUT_EMPTY_AMOUNT,	// 정량소진 알림
	TYPE_PCCAFE_OUTPUT_DISCONNECT,		// 연동서버와 끊어짐 알림
	TYPE_PCCAFE_OUTPUT_LOGOUT,			// 로그아웃 아무 처리 안함.
};

#pragma pack( push, 1 )
//////////////////////////////////////////////////////////////////////////////////////////
// Packet 정보
struct NET_PACKET_BUFFER
{
	UINT16				_ui16TotalSize;
	UINT16				_ui16DataSize;
	UINT16				_ui16Protocol; 	
	char				_Data[NET_PACKET_BUFFER_SIZE];

	NET_PACKET_BUFFER()
	{
		_ui16TotalSize		= SIA_PACKET_HEADER_SIZE;
		_ui16DataSize		= 0;
	}
	NET_PACKET_BUFFER( UINT16 ui16Protocol )
	{
		_ui16TotalSize		= SIA_PACKET_HEADER_SIZE;
		_ui16DataSize		= 0;
		_ui16Protocol		= ui16Protocol;
	}
	UINT32	GetDataMaxSize()
	{
		return NET_PACKET_BUFFER_SIZE;
	}
	void	Clear()
	{
		_ui16TotalSize		= 6;
		_ui16DataSize		= 0;
	}
	void	WriteData( void* pBuffer, UINT16 ui16BufferSize )
	{
		if( (_ui16TotalSize + ui16BufferSize) > NET_PACKET_BUFFER_SIZE )
		{
#ifdef _DEBUG
			assert( "SIA->Trans Write Packet OverFlow" );
#endif
			return;
		}

		if( 0 == ui16BufferSize ) return;

		i3mem::Copy( &_Data[_ui16DataSize], pBuffer, ui16BufferSize );
		_ui16DataSize	= _ui16DataSize + ui16BufferSize;
		_ui16TotalSize	= _ui16TotalSize + ui16BufferSize;
	};
	// 특정 위치에 데이터를 저장합니다. DataCount 는 증가하지 않습니다.
	void	WriteData( UINT16 ui16Pos, void* pBuffer, UINT16 ui16BufferSize )
	{
		if( 0 == ui16BufferSize ) return;
		i3mem::Copy( &_Data[ui16Pos], pBuffer, ui16BufferSize );
	}

	void	ReadData(void* pBuffer, UINT16 ui16Size )
	{
		i3mem::Copy( pBuffer, &_Data[ _ui16DataSize ], ui16Size );
		_ui16DataSize	= _ui16DataSize + ui16Size;
		_ui16TotalSize	= _ui16TotalSize + ui16Size;
	}
};

struct NET_I3_PACKET_BUFFER
{
	UINT16				_ui16DataSize;
	UINT16				_ui16Protocol; 	
	char				_Data[NET_PACKET_BUFFER_SIZE];
	UINT16				_ui16TotalSize;

	NET_I3_PACKET_BUFFER()
	{
		_ui16TotalSize		= 4;
		_ui16DataSize		= 0;
	}
	NET_I3_PACKET_BUFFER( UINT16 ui16Protocol )
	{
		_ui16TotalSize		= 4;
		_ui16DataSize		= 0;
		_ui16Protocol		= ui16Protocol;
	}
	void	Clear()
	{
		_ui16TotalSize		= 4;
		_ui16DataSize		= 0;
	}
	void	WriteData( void* pBuffer, UINT16 ui16BufferSize )
	{
#ifdef _DEBUG
		assert( (_ui16TotalSize + ui16BufferSize) < NET_PACKET_BUFFER_SIZE );
#endif
		if( 0 == ui16BufferSize ) return;

		i3mem::Copy( &_Data[_ui16DataSize], pBuffer, ui16BufferSize );
		_ui16DataSize	= _ui16DataSize + ui16BufferSize;
		_ui16TotalSize	= _ui16TotalSize + ui16BufferSize;
	}
	// 특정 위치에 데이터를 저장합니다. DataCount 는 증가하지 않습니다.
	void	WriteData( UINT16 ui16Pos, void* pBuffer, UINT16 ui16BufferSize )
	{
		if( 0 == ui16BufferSize ) return;
		i3mem::Copy( &_Data[ui16Pos], pBuffer, ui16BufferSize );
	}

	void	ReadData(void* pData, UINT16 ui16BufferSize, INT32* _i32CurrentReadPosition )
	{
		if( 0 == ui16BufferSize ) return;
		if(_ui16DataSize < (*_i32CurrentReadPosition + ui16BufferSize))	return;

		i3mem::Copy( pData, &_Data[*_i32CurrentReadPosition], ui16BufferSize);
		*_i32CurrentReadPosition	+= ui16BufferSize;		
	}
};

//////////////////////////////////////////////////////////////////////////////////////////

struct BILLING_BASE
{
	UINT32		m_ui32ServerIdx;						// Trans서버 Idx
	LINK_TYPE	m_eLinkType;							// 할거.
	T_UID		m_TUID;									// 유저 UID
	UINT32		m_ui32MainBufferIdx;					// Trans서버 MainBuffer Idx
	UINT8		m_ui8GoodsCount;						// 구매 상품 갯수.
	char		m_strCouponCode[GIFTCOUPON_LENGTH + 1];	// 쿠폰16자리 번호
};


//================================================================================================================================
// Input Structure
//================================================================================================================================
struct BILLING_INPUT	: public BILLING_BASE
{	
	UINT8					m_ui8Action;							//Command Type 
	char					m_strID[NET_ID_SIZE];					//ID
	char					m_strBillingID[ BILLING_ID_SIZE ];
	TTCHAR					m_strUserNick[ NET_NICK_NAME_SIZE ];	// User 닉네임. 북미 사용.
	UINT32					m_ui32Rank;								// User 랭크. 북미 사용
	UINT32  				m_ui32ClientIP;							// Client IP
	UINT64					m_ui64UserBillingID;					// User Billing ID for Russia
	UINT32					m_ui32BirthDay;							// User Birthday
	UINT32					m_ui32CreateDate;						// User CreateDate
	UINT8					m_ui8PcCafe;
	LINK_BUY				m_stLinkBuy[MAX_SHOP_GOODS_BUY_COUNT];	// 구매 상품 정보.
};
//================================================================================================================================
//================================================================================================================================


//================================================================================================================================
// Output Structure
//================================================================================================================================
struct BILLING_OUTPUT	: public BILLING_BASE
{
	UINT32					m_ui32CurrentUserCash;					//캐쉬 잔액
	UINT32					m_ui32CurrentUserCashType;				//캐쉬 잔액 유효성 TYPE_CURRENT_USER_CASH
	INT32					m_ai32Rv[MAX_SHOP_GOODS_BUY_COUNT];		//Return Code 
};
//================================================================================================================================
//================================================================================================================================

struct PCCAFE_BASE
{
	UINT32					m_ui32ServerIdx;						// Trans서버 Idx
	T_UID					m_TUID;									// 유저 UID
	UINT32					m_ui32MainBufferIdx;					// Trans서버 MainBuffer Idx
	INT32					m_i32LoginBufferIdx;					// SIA서버 메인로그인버퍼 Idx
	UINT32					m_ui32PCCafeGUID;
	UINT32					m_ui32PCCafeBillingUID;
};

//================================================================================================================================
// Input Structure
//================================================================================================================================
struct PCCAFE_INPUT	: public PCCAFE_BASE
{
	TYPE_PCCAFE_INPUT		m_eInputType;
	UINT32					m_ui32IP;
	UINT16					m_ui16GcaType;
	char					m_strID[NET_ID_SIZE];
};
//================================================================================================================================
//================================================================================================================================

//================================================================================================================================
// Output Structure
//================================================================================================================================
struct PCCAFE_OUTPUT	: public PCCAFE_BASE
{
	TYPE_PCCAFE_OUTPUT		m_eOutputType;
	UINT8					m_ui8PCCafe;
};
//================================================================================================================================
//================================================================================================================================

struct LOGIN_INPUT : public ST_PACKET_LOGIN_REQ
{
	T_UID					m_TUID;

	INT32					m_i32TransServerIdx;
	TYPE_LOGIN				m_eCommandType;
	INT32					m_i32MainLoginBufferIdx;

	UINT8					m_ui8LinkThreadIdx;

	LOGIN_INPUT()
	{
	}
	LOGIN_INPUT( ST_PACKET_LOGIN_REQ* pData )
	{
		i3mem::Copy( this, (void*)pData, sizeof( ST_PACKET_LOGIN_REQ ) );
	}
	void operator=( const ST_PACKET_LOGIN_REQ* pData )
	{
		i3mem::Copy( this, (void*)pData, sizeof( ST_PACKET_LOGIN_REQ ) );
	}
};

struct LOGIN_OUTPUT : public ST_PACKET_LOGIN_ACK
{
	INT32					m_i32TransServerIdx;
	TYPE_LOGIN				m_eCommandType;
	INT32					m_i32MainLoginBufferIdx;
	UINT16					m_ui16GcaType;
	T_UID					m_TExtUID;								// 외부연동 UID( ex - 스팀 )
};

struct PCCAFE_CALLBACK_RESULT
{
	PCCAFE_CALLBACK_RESULT( INT8 i8ID ) : m_i8ID( i8ID ) {}

	INT8			m_i8ID;
	char			m_Data[256];
};

enum WEBZEN_CALLBACK
{
	WEBZEN_CALLBACK_NETCONNECT,
	WEBZEN_CALLBACK_NETSEND,
	WEBZEN_CALLBACK_NETCLOSE,
	WEBZEN_CALLBACK_USERSTATUS,
	WEBZEN_CALLBACK_INQUIREMULTIUSER,
	WEBZEN_CALLBACK_INQUIREPCROOMPOINT,
	WEBZEN_CALLBACK_CHECKLOGINUSER,
	WEBZEN_CALLBACK_CHECKPCROOMIPADDR,
	WEBZEN_CALLBACK_PCROOMDEDUCTUPDATE,
};

#pragma pack( pop )


#endif
