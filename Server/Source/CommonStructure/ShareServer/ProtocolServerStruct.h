#ifndef __PROTOCOL_SERVER_STRUCT_H__
#define __PROTOCOL_SERVER_STRUCT_H__

#pragma pack( push, 1)

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_MASK_SHOP_BUY_GOODS	- Trans SIA 통합버전 - Data 유지용 버퍼
//////////////////////////////////////////////////////////////////////////
struct GACHA_PURCHACE_BASE
{
	UINT8				m_ui8BuyType;	// 결제 유형. SIA_GOODS_BUY	
	INT32				m_i32ItemIdx;
	T_GoodsID			m_TGoodsID;
};

struct BUFFER_BUY_GACHA	: public GACHA_PURCHACE_BASE
{
	T_ItemDBIdx			m_TTicketDBIdx;		// DB에 INSERT된 뽑기 상점 이용권 DB Idx.
};

struct BUFFER_BUY_CHARA
{
	T_GoodsID			m_TGoodsID;			// 상품 ID
	T_ItemDBIdx			m_TItemDBIdx;
	UINT8				m_ui8Class;	
	UINT8				m_ui8CharSlot;
	TTCHAR				m_strCharName[NET_NICK_NAME_SIZE];
};

//S2MO_REPAIR_INFO 구조체와 동일하게 유지해야합니다.
struct REPAIR_ITEM
{
	INT32					m_i32Rv;				// 처리결과
	UINT8					m_ui8BuyType;			// 결제유형. SIA_GOODS_BUY
	T_ItemID				m_TItemID;				// ItemID.
	T_ItemDBIdx				m_TItemDBIdx;			// ItemDBIdx.
	UINT32					m_ui32AuthArg;			// 수리 요청 Arg - 수리 완료 Arg
	UINT32					m_ui32RepairPoint;		// 수리 Point
	UINT32					m_ui32RepairCash;		// 수리 Cash
};


struct BUFFER_BUY_REPAIR	 
{
	UINT8					m_ui8ItemCount;
	REPAIR_ITEM				m_stRepairItem[CHAR_EQUIPMENT_COUNT];
};


struct BUFFER_BUY_GIFT		
{
	T_UID			m_TReceiverUID;							// 선물 받는 사람 UID
	T_GiftDBIdx		m_aGiftDBIdx[MAX_SHOP_GOODS_BUY_COUNT]; // 선물 DBIdx
	UINT8			m_ui8MessageSize;						// 선물메시지 사이즈
	TTCHAR			m_strMessage[MAX_GIFT_MESSAGE_SIZE];	// 선물메시지
};

struct BUFFER_BUY_SHOP
{
	T_ItemDBIdx			m_TItemDBIdx;
	BUY_GOODS			m_stDBGoods[MAX_SHOP_GOODS_BUY_COUNT];
};

struct BUFFER_BUY_COUPON
{
	UINT8				m_ui8GoodsCount;	// 제거대상

	char				m_strCoupon[GIFTCOUPON_LENGTH + 1];
	DB_GOODS			m_aDBGoods[MAX_GIFTCOUPON_COUNT];
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_MASK_LOGIN	- Trans SIA 통합버전 - 로그인 패킷
//////////////////////////////////////////////////////////////////////////

struct ST_PACKET_LOGIN_REQ
{
	UINT8					m_ui8PBVer;
	INT32					m_i32ServerIdx;
	INT32					m_i32SessionIdx;
	UINT32					m_ui32AuthKey;
	UINT32					m_ui32IP;
	UINT64					m_ui64MacAddress;
	UINT8					m_ui8Country;

	char					m_strID[ NET_ID_SIZE ];
	char					m_strPW[ NET_PW_SIZE ];

	UINT64					m_ui64UniqueKey;						// 인증키
	UINT16					m_ui16SessionKeySize;
	UINT8					m_pSessionKey[ NET_SESSION_KEY_SIZE ];	// 웹 인증키 태국 - NET_ENCRYPTED_TOKEN_SIZE+1, NET_SESSION_KEY_SIZE

	UINT8					m_ui8LocaleCode;						// 접속 국가
	UINT8					m_ui8LinkMethod;						// 인증 방법
};

struct ST_PACKET_LOGIN_ACK
{
	UINT8					m_ui8PBVer;
	INT32					m_i32Result; 
	INT32					m_i32ServerIdx;
	INT32					m_i32SessionIdx;
	UINT32					m_ui32AuthKey;
	UINT32					m_ui32IP;

	char					m_strID[ NET_ID_SIZE ];

	UINT64					m_ui64UniqueKey;						// 인증키	

	char					m_strBillingID[ BILLING_ID_SIZE ];		// Billing 사용시 전달 되는 키값 ( 터키, 중남미, 필리핀은 ID 아닌 다른 값 사용 )
	INT8					m_i8UIDCreateType;						// UID 생성 타입( TYPE_UID_CREATE 값 사용 )
	INT8					m_i8LogOutSendType;						// LogOut 시 퍼블리셔에 통보 여부( TYPE_LOGOUT_ACK 값 사용 )
	T_UID					m_TUID;
	INT32					m_i32Cash;
	UINT8					m_ui8PCCafe;
	UINT32					m_ui32Birthday;	
	UINT32					m_ui32LoginTime;
	UINT8					m_ui8LocaleCode;						// 접속 국가
	UINT8					m_ui8LinkThreadIdx;	
	UINT64					m_ui64BillingIdx;						// 러시아 빌링IDX
	UINT8					m_aui8ShutDownTime[SHUTDOWN_TIME_SIZE];	// 셧다운 타임 설정값

	UINT32					m_ui32PCCafeGUID;						// 웹젠PC방 GUID
	UINT32					m_ui32PCCafeBillingUID;					// 웹젠빌링UID (로그아웃시 사용)
	UINT8					m_ui8Country;

	char					m_strExtUID[EXT_UID_SIZE];				// 외부연동 UID(암호화)
	char					m_strWebLoginKey[WEB_LOGINKEY_SIZE];	// 웹로그인 연동키(북미스팀만 사용중)
	UINT8					m_ui8LinkMethod;						// 인증 방법
};

struct ST_PACKET_LOGOUT_REQ
{
	char					m_strID[ NET_ID_SIZE ];
	UINT32					m_ui32PCCafeBillingUID;
};

struct PACKET_LOGOUT_REQ
{
	T_UID					m_TUID;			// 중남미 사용.
	char					m_strID[ NET_ID_SIZE ];
	UINT32					m_ui32IP;
	INT64					m_i64UniqueKey;		// 북미 사용.
};

struct PACKET_PCCAFE_FIRSTLOGIN_REQ
{
	T_UID					m_TUID;
	UINT32					m_ui32IP;
};

struct PACKET_PCCAFE_LOGOUT_REQ
{
	INT32					m_i32BillingGUID;
};

struct PACKET_PCCAFE_LOGIN_REQ
{
	T_UID					m_TUID;
	UINT32					m_ui32IP;
	UINT32					m_ui32PCCafeGUID;
};

struct PACKET_PCCAFE_CHECK_AMOUNT_REQ
{
	T_UID					m_TUID;
	UINT32					m_ui32IP;
	UINT32					m_ui32PCCafeGUID;
};

struct PACKET_PCCAFE_FIRSTLOGIN_ACK
{
	T_UID					m_TUID;
	UINT32					m_ui32PCCafeGUID;
};

struct PACKET_PCCAFE_SUBTRACTION_ACK
{
	T_UID					m_TUID;
	INT32					m_i32BillingGUID;
};

struct PACKET_PCCAFE_LOGIN_ACK
{
	T_UID					m_TUID;
	UINT32					m_ui32PCCafeGUID;
	UINT32					m_ui32BillingUID;
	UINT8					m_ui8PCCafe;
};

struct PACKET_PCCAFE_EMPTY_AMOUNT_ACK
{
	T_UID					m_TUID;
};

struct PACKET_PCCAFE_CHECK_AMOUNT_ACK
{
	T_UID					m_TUID;
	UINT32					m_ui32PCCafeGUID;
	UINT32					m_ui32BillingUID;
	UINT8					m_ui8PCCafe;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_MASK_SHOP_BUY_GOODS	- Trans SIA 통합버전.
//////////////////////////////////////////////////////////////////////////
struct PACKET_BUY_GOODS_BASE_REQ
{
	UINT8				m_ui8Action;							// 작업종류( 일반구입, 선물, 뽑기상점, 수리, 캐릭터구입)
	T_UID				m_TUID;									// UID.
	char				m_strBillingID[ BILLING_ID_SIZE ];		// 구매 ID
	TTCHAR				m_strUserNick[ NET_NICK_NAME_SIZE ];	// User 닉네임. 북미 사용.
	UINT32				m_ui32Rank;								// User 랭크. 북미 사용					
	UINT32				m_ui32ClientIP;							// Client IP
	UINT64				m_ui64UserBillingIdx;					// User Billing DB Index(For Russia)
	UINT8				m_ui8LinkMethod;						// 사용할 Billing Method
	UINT8				m_ui8GoodsCount;						// 구매 상품 갯수.
};

struct PACKET_BUY_GOODS_REQ : public PACKET_BUY_GOODS_BASE_REQ
{
	LINK_BUY			m_stLinkBuy[MAX_SHOP_GOODS_BUY_COUNT];	// 구매 상품 정보.
};

struct PACKET_BUY_GOODS_BASE_ACK
{
	T_UID				m_TUID;						// UID.
	UINT8				m_ui8Action;				// 작업종류( 일반구입, 선물, 뽑기상점, 수리, 캐릭터구입)
	UINT32				m_ui32CurrentUserCash;		//캐쉬 잔액
	UINT32				m_ui32CurrentUserCashType;	//캐쉬 잔액 유효성(0: 사용안함, 1: 사용)	
	UINT8				m_ui8GoodsCount;					// 구매 상품 갯수.
};

struct PACKET_BUY_GOODS_ACK : public PACKET_BUY_GOODS_BASE_ACK
{
	INT32				m_i32Rv[MAX_SHOP_GOODS_BUY_COUNT];	// 결과.
	T_GoodsID			m_aGoodsID[MAX_SHOP_GOODS_BUY_COUNT];
};



//////////////////////////////////////////////////////////////////////////
// PROTOCOL_MASK_GET_CASH - Trans SIA 통합버전.
//////////////////////////////////////////////////////////////////////////
struct PACKET_GET_CASH_REQ
{
	T_UID				m_TUID;								// UID.
	char				m_strBillingID[ BILLING_ID_SIZE ];
	UINT64				m_ui64UserBillingIdx;					// Billing Idx
	UINT8				m_ui8LinkMethod;
};

struct PACKET_GET_CASH_ACK
{
	T_UID				m_TUID;								// UID.
	INT32				m_i32Result;							// 결과값.
	UINT32				m_ui32Cash;								// 캐쉬잔액.
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_MASK_USE_GIFTCOUPON
//////////////////////////////////////////////////////////////////////////
struct PACKET_USE_GIFTCOUPON_REQ
{
	T_UID				m_TUID;								// UID
	char				m_strBillingID[ BILLING_ID_SIZE ];	// 빌링ID
	UINT64				m_ui64UserBillingIdx;				// Billing Idx
	char				m_strCouponCode[GIFTCOUPON_LENGTH + 1];	// 쿠폰번호
	TTCHAR				m_strUserNick[NET_NICK_NAME_SIZE];	// 닉네임
	UINT32				m_ui32BirthDay;						// 생일
	UINT32				m_ui32IP;							// IP
	UINT32				m_ui32CreateDate;					// 계정생성일
	char				m_strID[NET_ID_SIZE];
	UINT8				m_ui8PcCafe;
	UINT8				m_ui8LinkMethod;
};

struct PACKET_USE_GIFTCOUPON_ACK
{
	T_UID				m_TUID;
	INT32				m_i32Result;						// 결과값
	char				m_strCouponCode[GIFTCOUPON_LENGTH + 1];	// 쿠폰번호
	UINT32				m_ui32IncCash;						// 쿠폰지급캐쉬
	INT8				m_i8GoodsCount;						// 지급받는 Goods수
	INT32				m_ai32GoodsID[MAX_GIFTCOUPON_COUNT];// 지급받는 GoodsID
};

//////////////////////////////////////////////////////////////////////////

//BUY BUFFER 관리용 통합 버퍼
struct MAIN_STREAM_BUFFER_BUY
{
	UINT32					m_ui32ServerST;
	UINT32					m_ui32ServerIdx;	
	BOOL					m_bIsEmpty;
	PACKET_BUY_GOODS_ACK	m_stBuyGoodsAck;
	UINT8					m_ui8RecvCount;

	union
	{
		BUFFER_BUY_GACHA	m_stGachaBuffer;
		BUFFER_BUY_CHARA	m_stCharaBuffer;
		BUFFER_BUY_REPAIR	m_stRepairBuffer;
		BUFFER_BUY_GIFT		m_stGiftBuffer;
		BUFFER_BUY_SHOP		m_stShopBuffer;
	};
};

enum MAIN_STREAM_BUFFER_STATUS
{
	MAIN_STREAM_BUFFER_STATUS_EMPTY = 0,				// 비어있음
	MAIN_STREAM_BUFFER_STATUS_LOGINWORKER_SEND,			// LoginWorker 응답 기다리는 상태
	MAIN_STREAM_BUFFER_STATUS_PCCAFEWORKER_SEND,		// PCCafeWorker 응답 기다리는 상태
};

#define TIME_OUT_SECOND_LOGINWORKER					15
#define	TIME_OUT_SECOND_PCCAFEWORKER				5
#define MAX_COUNT_PROCESS_LOGIN_BUFFER_IN_FRAME		300



struct MAIN_STREAM_BUFFER_LOGIN
{
	UINT32					m_ui32ServerIdx;
	UINT32					m_ui32ServerST;
	INT8					m_i8Status;
	ST_PACKET_LOGIN_ACK		m_LoginAck;
};

struct WORKER_STREAM_BUFFER_PCCAFE
{
	T_UID					m_i64UID;
	bool					m_bEmpty;
	INT8					m_i8Type;
	UINT32					m_ui32IP;
	UINT32					m_ui32PCGUID;
	UINT32					m_ui32ServerTime;
};


struct DB_GACHA	: public GACHA_PURCHACE_BASE
{
	INT32				m_i32Rv;
	T_UID				m_TUID;
	UINT8				m_ui8Action;		// DB Action. ENUM_TYPE_SHOP
	UINT32				m_ui32GachaGoodsID;	
	UINT32				m_ui32GachaPrice;	// 결제 금액.	
	T_ItemDBIdx			m_TTicketDBIdx;		// DB에 INSERT된 뽑기 상점 이용권 DB Idx.
	
	// 당첨 아이템 ( GoodsID )
	INT32				m_i32ResultCount;
	PACKET_GACHA_RESULT_DATA stResult[MAX_GACHA_GAME_COUNT];

	// 당첨 아이템 ( DB 지급. ItemID )
	DB_ITEM				m_stDBItem;
};

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////
// 항상 아래에 둘것
//////////////////////////////////////////////////////////////////////////

#ifndef GAMEPORTAL_BUILD	// ZLog,SIA를 빌드 중이라면 하단의 내용은 전부 무시		2011.10.06

#include "ProtocolServerCommunityStruct.h"
#include "ProtocolServerClanBattleStruct.h"

#endif

//////////////////////////////////////////////////////////////////////////
#endif