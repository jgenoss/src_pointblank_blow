#ifndef	__SIA_DEFINE_H__
#define	__SIA_DEFINE_H__

#define		SHOP_FILE_NAME				L".\\Shop.dat"
#define		SHOP_FILE_NAME_ASCII		".\\Shop.dat"
#define		SHOP_FILE_VERSION_DEF		0

#define LIMIT_ITEM_COUNT		2100000000		// 아이템 최대 소지가능 갯수
#define LIMIT_ITEM_DATE			4212312359		// 아이템 최대 소지가능 만료 날짜

#define CALCULATE_ITEM_COUNT(OrgArg, AddArg)	( MIN( ((UINT64)OrgArg) + ((UINT64)AddArg), LIMIT_ITEM_COUNT ) ) 		

// 매크로 함수
#define GET_GOODS_FLAG_CLASS(Goodsflag)					( (Goodsflag) / 1000000000 )
#define GET_GOODS_FLAG_MATCHING(Goodsflag)				( ((Goodsflag) / 100) *100 )
#define GET_GOODS_FLAG_TYPE(Goodsflag)					( ((Goodsflag) % 1000000000 ) / 10000000 )
#define GET_GOODS_FLAG_NUMBER(Goodsflag)				(((Goodsflag) % 10000000) / 100)
#define MAKE_GOODS_FLAG(type,number)					(((type)*10000000)+((number)*100))

#define GET_GOODS_FLAG_RS_TAB_IDX(Goodsflag)			(((Goodsflag) % 10000000) / 100)	// 굿즈 아이디로 뽑기 상점 탭번호 추출. 

//Shop Define
#define	MAX_SHOP_GOODS_COUNT			8192				// 총 상품 카운트
#define	MAX_SHOP_ITEM_COUNT				8192				// 총 아이템 카운트
#define	MAX_SHOP_MATCHING_COUNT			8192				// 매칭 카운트
#define	MAX_SHOP_COUPON_COUNT			1024				// 할인쿠폰 카운트
#define MAX_SHOP_GOODS_ITEM_COUNT		5					// 상점 상품에 포함되어있는 아이템 카운트
#define MAX_SHOP_GOODS_BUY_COUNT		8					// 한번에 구매할 수 있는 상품 카운트. 8보다 커지면 패킷 사이체크해야합니다.
#define MAX_SHOP_ITEM_AUTH_COUNT		5					// 한번에 인증 할수 있는 아이템 카운트
#define MAX_INVEN_COUNT		450					// 유저가 들고 있는 아이템 카운트
#define MAX_SHOP_DB_ITEM_COUNT			20					// 아이템 지급 최대 카운트. DB기준.
#define	MAX_SHOP_PRICE_COUNT			MAX_SHOP_GOODS_COUNT*PRICE_TYPE_MAX		// 총 상품 가격 카운트

// Gift
//#define MAX_SHOP_GOODS_BUY_COUNT		10					// 선물 인증(지급/삭제) 최대 갯수
#define MAX_SHOP_AUTH_GIFT_ITEM_COUNT	MAX_SHOP_GOODS_BUY_COUNT * 	MAX_SHOP_GOODS_ITEM_COUNT	// 선물 지급시 아이템 최대 갯수
#define MAX_SHOP_USER_GIFT_COUNT		100					// 선물받은 아이템 카운트
#define MAX_SHOP_USER_GIFT_COUNT_TEMP	MAX_SHOP_USER_GIFT_COUNT * 2	// 임시 선물 카운트
#define MAX_GIFT_MESSAGE_SIZE			126					// 선물 메시지 길이
#define MAX_AUTH_GIFT_EXPIRE_DAY		15					// 선물 인증(지급)시 추가되는 유효기간. Day
#define MAX_GET_GIFT_EXPIRE_DAY			30					// 선물 받았을때 유효기간. Day
#define	MAX_GIFT_RANKING_COUNT			6 					// 선물 랭킹 카운트. 
#define	MAX_TOTAL_GIFT_RANKING_COUNT	MAX_GIFT_RANKING_COUNT * 2		// 선물 랭킹 정보 최대값. 지난주, 지지난주.

#define MAX_COUPON_NUM					20					// 쿠폰 입력 길이(MAX_UNIQUE_ID_LEN(33)을 넘지 않음)
#define MAX_COUPON_INVEN_COUNT			50

#define MAX_ITEM_NAME					32		// 상품, 아이템 이름
#define SECOND_PER_DAY					86400	// 1일을 1초로 변환한수.
#define MINUTE_PER_DAY					1440

#define	MAX_MINI_MATCHING_COUNT			256

typedef INT32	TWebGiftDxIdx;

enum TYPE_ITEM_RESTRICTION				// 아이템 구매 제약 타입.
{
	TYPE_ITEM_RESTRICTION_NONE,			// 없음.
	TYPE_ITEM_RESTRICTION_MEDAL,		// 특정 메달의 레벨을 달성해야 구매가능.
	TYPE_ITEM_RESTRICTION_TITLE,		// 특정 호칭을 구입해야 구매 가능
	TYPE_ITEM_RESTRICTION_MAX
};

enum TYPE_UID_CREATE				// UID 생성 방법
{
	TYPE_UID_CREATE_INTER,			// PB 시스템에서 자체적으로 생성
	TYPE_UID_CREATE_EXTER,			// 퍼블리셔에서 받아옴
};

enum TYPE_LOGOUT_ACK				// 로그아웃 시 퍼블리셔에 통보 여부
{
	TYPE_LOGOUT_ACK_NOT,			// 로그아웃 시 통보 하지 않음
	TYPE_LOGOUT_ACK_SEND,			// 로그아웃 시 통보		( 터키, 북미, 중남미는 전달 )
};


enum ENUM_TYPE_SHOP
{
	// *주의* 신규 타입 추가시 TYPE_SHOP_MAX 위에 추가해주세요!!!

	// DB_SHOP_GOODS
	TYPE_SHOP_GOODS_BUY = 0,	// 캐쉬구매 - Link 결제전 DB 작업, 아이템 기간연장 ( 캐쉬 결제 )
	TYPE_SHOP_GOODS_BUY_LINK,	// 캐쉬구매 - Link 결제후 DB 작업. 
	TYPE_SHOP_ITEM_EXTEND,		// 아이템 기간연장.( 포인트 결제 )
	TYPE_SHOP_GOODS_INSERT,		// 포인트 구매.  

	// DB_SHOP_IN / DB_SHOP_OUT
	TYPE_SHOP_ITEM_INSERT,
	TYPE_SHOP_ITEM_DELETE,
	TYPE_SHOP_ITEM_DELETE_NOACK,
	TYPE_SHOP_ITEM_AUTH,
	TYPE_SHOP_ITEM_AUTH_NOACK,
	TYPE_SHOP_COUPON_CHECK,
	TYPE_GACHA_PURCHASE,
	TYPE_GACHA_PROVIDE,

	// DB_REPAIR
	TYPE_SHOP_REPAIR,

	// SEND_GIFT 
	TYPE_SHOP_GIFT_BUY,

	// DB_GIFT_IN / DB_GIFT_OUT
	TYPE_SHOP_GIFT_LIST,
	TYPE_SHOP_GIFT_READ,
	TYPE_SHOP_GIFT_AUTH,
	TYPE_SHOP_GIFT_DELETE,

	// DB_ITEM
	TYPE_SHOP_WEB_GIFT,

	TYPE_SHOP_CHARA_BUY,
	TYPE_SHOP_TYPE_GET_CASH,

	TYPE_SHOP_USE_GIFTCOUPON,

	// DB_SHOP_IN
	TYPE_GACHA_TICKET_DELETE,	// 링크 결제 성공시 뽑상 티켓 인벤DB에서 삭제.

	// LOOP 
	TYPE_SHOP_ITEM_LOOP_AUTH,	// 한 번의 인증으로 Loop 개수 만큼 처리하기 위한 타입

	TYPE_SHOP_MAX,
};

enum ENUM_TYPE_PCCAFE
{
	TYPE_PCCAFE_SUBTRACTION = 0,	// 차감알림
	TYPE_PCCAFE_CONNECT,			// 컨넥
	TYPE_PCCAFE_LOGIN,				// 재로그인결과
	TYPE_PCCAFE_EMPTY_AMOUNT,		// 정량소진알림
	TYPE_PCCAFE_CHECK_AMOUNT,		// 정량확인결과 알림
	TYPE_PCCAFE_DISCONNECT,			// 디스컨넥
};

// 굿즈 아이디 0번째 ( 가장 앞자리 숫자 )
enum GOODS_CLASS
{
	GOODS_CLASS_NORMAL = 0,	// 일반 상품
	GOODS_CLASS_GIFT,			// 선물용 상품
};

enum GIFT_TYPE		// 선물 타입
{
	GIFT_TYPE_NONE = 0,
	GIFT_TYPE_USER,			// 유저 선물( 구매 ) 
	GIFT_TYPE_EVENT,		// 이벤트 보상
	GIFT_TYPE_GM,			// GM( 웹툴 - Send Gift )
	GIFT_TYPE_COUPON,		// 페이레터쿠폰

	GIFT_TYPE_MAX,

};

enum TYPE_CURRENT_USER_CASH
{
	TYPE_CURRENT_USER_CASH_NONE = 0,
	TYPE_CURRENT_USER_CASH_USE = 1,

	TYPE_CURRENT_USER_CASH_MAX
};

// 선물 지급 
enum ENUM_TYPE_GIFT_DELIVER
{
	TYPE_GIFT_DELIVER_STAND_BY = 0,	// 결제 대기
	TYPE_GIFT_DELIVER_NORMAL,				// 결제 완료
	TYPE_GIFT_DELIVER_READ,					// 메시지 확인 완료.
	TYPE_GIFT_DELIVER_RECEIVED,				// 인벤토리 지급 완료. 
	TYPE_GIFT_DELIVER_EXPIRE,				// 기간 만료로 인한 자동지급.	( ZLog 용 )
	TYPE_GIFT_DELIVER_DELETE,				// 유저가 삭제.( ZLog 용 )
	TYPE_GIFT_DELIVER_OVERFLOW,				// 갯수 초과(100개)로 인한 자동지급.	( ZLog 용 )
	TYPE_GIFT_DELIVER_MAX,
};

// 구입물품 인벤 저장 결과
enum ENUM_TYPE_BUY_INVEN_RETURN
{
	TYPE_BUY_INVEN_RETURN_NONE = 0,	// 인벤에 물품 저장 전
	TYPE_BUY_INVEN_RETURN_SUCCESS = 1,	// 인벤에 물품 저장 성공
	TYPE_BUY_INVEN_RETURN_FAIL = 2,	// 인벤에 물품 저장 실패

	TYPE_BUY_INVEN_RETURN_MAX,
};

// 사용여부
enum SIA_GOODS_USE
{
	GOODS_USE_ENABLE = 1,					// 사용
	GOODS_USE_DISABLE = 2,					// 미사용
};
// 판매유형
enum SIA_GOODS_SALE
{
	GOODS_SALE_ALL = 1,					// 모두가능
	GOODS_SALE_BUY = 2,					// 구입만가능
	GOODS_SALE_GIFT = 3,					// 선물만가능
	GOODS_SALE_EVENT = 4,					// 이벤트용 Goods. 상점에 표시 안된다.
	GOODS_SALE_HIDDEN = 5,					// 클라이언트에 전달하지 않는 상품.
};

// 상품 결제 수단. 순서 변경하지 마세요.CHARA_RING_TYPE 랑 맞춰놓음
enum SIA_GOODS_BUY
{
	GOODS_BUY_NONE = 0,
	GOODS_BUY_POINT = 1,					// 포인트로 구매
	GOODS_BUY_CASH,											// 캐쉬로 구매
	GOODS_BUY_CHAR_FREE,									// 케릭터 무료 구매
	GOODS_BUY_COIN,											// 코인으로 구매
	GOODS_BUY_MAX,
};
// 상품 마크 적용 여부
enum SIA_GOODS_MARK
{
	GOODS_MARK_NO = 0,					// 마크 없음
	GOODS_MARK_NEW,											// New 마크
	GOODS_MARK_HOT,											// Hot 마크
	GOODS_MARK_EVENT,										// Event 마크
	GOODS_MARK_PCROOM,										// PC방 마크
	GOODS_MARK_SALE,										// Sale 마크
};
// 크리스마스 이벤트 마크 적용 여부 (스테이지에서만 사용 2011.11.16 - 양승천)
#define GOODS_MARK_XMAS					2					// 크리스마스 마크

// 아이템 종류
enum SIA_ITEM_TYPE
{
	ITEM_TYPE_COUNT = 1,					// 개수제
	ITEM_TYPE_TIME,											// 기간제
	ITEM_TYPE_ETERNITY,										// 영구제
};
// 만료후처리
enum SIA_ITEM_EXPIRE
{
	ITEM_EXPIRE_DELETE = 1,					// 만료후 삭제
	ITEM_EXPIRE_LEAVE = 2,					// 만료후 유지(내구도)
};
// 인증 종류
enum SIA_ITEM_AUTH
{
	ITEM_AUTH_AUTO = 1,					// 자동 인증(기간제만 사용)
	ITEM_AUTH_HAND = 2,					// 수동 인증
	ITEM_AUTH_NOT = 3,					// 인증 못함
};
// 아이템 상태
enum SIA_INVENTORY_STATE
{
	INVENTORY_STATE_BUY = 1,					// 구매상태
	INVENTORY_STATE_AUTH = 2,					// 인증 상태
	INVENTORY_STATE_ETERNITY = 3,					// 영구 상태
};

enum SALE_TYPE
{
	SALE_TYPE_NONE = 0,
	SALE_TYPE_PER,											// 퍼센트로 할인
	SALE_TYPE_CASH											// 지정된 금액 할인
};
enum DAY_TYPE
{
	DAY_TYPE_NONE = 0,
	DAY_TYPE_DATE,											// 정해진 기간 ( 절대적 ) 20111031 ~ 20111105
	DAY_TYPE_ARG											// 인증시 정해진 값만큼 기간 설정 ( 상대적 )	86400
};
enum LIMIT_TYPE
{
	LIMIT_TYPE_NONE = 0,
	LIMIT_TYPE_UP,											// 얼마 이상 시에만 사용가능
	LIMIT_TYPE_DOWN 										// 얼마 이하 시에만 사용가능
};

// 유저 인벤토리 DB 에서 ItemState 값과도 동일한 의미를 가집니다.
enum ITEM_ATTR_TYPE
{
	ITEM_ATTR_TYPE_BUY = 1,			//미인증 아이템
	ITEM_ATTR_TYPE_AUTH,					//인증한 아이템
	ITEM_ATTR_TYPE_BASIC,					//기본 지급 아이템
	ITEM_ATTR_TYPE_PC_1,					//PC방 아이템 1
	ITEM_ATTR_TYPE_PC_2,					//PC방 아이템 2
	ITEM_ATTR_TYPE_SPECIAL,					//특별 이벤트
	ITEM_ATTR_TYPE_STAND_BY,				//대기 상태. 캐쉬 결제시. 인벤토리 정보에서 제외 시킵니다.
};

// 상품 가격 유형
enum PRICE_TYPE
{
	PRICE_TYPE_SHOP_REAL = 0,	// 상점 실제판매가격.( 할인 된 가격 )
	PRICE_TYPE_SHOP_DC,			// 상점 할인전 가격( 가로줄 그어지는가격 )
	PRICE_TYPE_GIFT_REAL,		// 선물샵 실제판매가격.( 할인 된 가격 )
	PRICE_TYPE_GIFT_DC,			// 선물샵 할인전 가격( 가로줄 그어지는가격 )
	PRICE_TYPE_MAX,
};

#pragma pack( push, 1 )


//////////////////////////////////////////////////ㄴ////////////////////////////////////////
//Goods 기본정보
struct SHOP_GOODS_BASE
{
	UINT32					_ui32GoodsID;					// GoodsID
	UINT8					_ui8Enable;						// SIA_GOODS_USE
	UINT8					_ui8SaleType;					// SIA_GOODS_SALE
};

struct  SHOP_GOODS_PRICE
{
	UINT32 			m_ui32Point;
	UINT32 			m_ui32Cash;
	UINT32 			m_ui32Coin;
	UINT8			m_ui8Mark;			// SIA_GOODS_MARK
};

// Client, Game 사용
struct SHOP_GOODS_EXPANSION : public  SHOP_GOODS_BASE
{
	SHOP_GOODS_PRICE	m_Price[PRICE_TYPE_MAX];

	UINT32 GetCash(PRICE_TYPE ePriceType = PRICE_TYPE_SHOP_REAL) { return m_Price[ePriceType].m_ui32Cash; }
	UINT32 GetPoint(PRICE_TYPE ePriceType = PRICE_TYPE_SHOP_REAL) { return m_Price[ePriceType].m_ui32Point; }
	UINT8	GetMark(PRICE_TYPE ePriceType = PRICE_TYPE_SHOP_REAL) { return m_Price[ePriceType].m_ui8Mark; }
	UINT32 GetCoin(PRICE_TYPE ePriceType = PRICE_TYPE_SHOP_REAL) { return m_Price[ePriceType].m_ui32Coin; }

	UINT32 GetPrice(SIA_GOODS_BUY eBuyType, PRICE_TYPE ePriceType = PRICE_TYPE_SHOP_REAL)
	{	// 상점 REAL 가격만
		switch (eBuyType)
		{
		case GOODS_BUY_POINT: return m_Price[ePriceType].m_ui32Point;
		case GOODS_BUY_CASH: return m_Price[ePriceType].m_ui32Cash;
		case GOODS_BUY_COIN: return m_Price[ePriceType].m_ui32Coin;
		default: return 0;
		}
	}
};

// Item 기본정보
struct SHOP_ITEM_BASE
{
	UINT32					_ui32ItemID;					// ItemID
	UINT8					_ui8AuthType;					// SIA_ITEM_TYPE
	UINT8					_ui8AuthExpireType;				// SIA_ITEM_EXPIRE
	UINT8					_ui8AutoStart;					// SIA_ITEM_AUTH
	UINT8					_ui8Restriction;				// 구입 제한. PBv1.0은 호칭 Number로 사용.
	UINT8					_ui8RestrictionType;			// 구입 제한 타입.	TYPE_ITEM_RESTRICTION
	UINT16					_ui16Condition;					// 구입 제한 조건값.
};
struct SHOP_ITEM_EXPANSION
{
	char					_strName[MAX_ITEM_NAME];		// Item 이름
};

// Matching 기본정보
struct SHOP_MATCHING_DATA
{
	UINT32					_ui32GoodsID;					// GoodsID
	UINT32					_ui32ItemID;					// ItemID
	UINT32					_ui32ItemArg;					// ItemArg(개수제=개수,기간제=기간)
	UINT32					_ui32BonusArg;					// BounsArg(개수제=개수,기간제=기간)
};

// ItemRepair 기본정보
struct SHOP_ITEM_REPAIR_DATA
{
	UINT32					_ui32ItemID;					// ItemID
	UINT32					_ui32PricePoint;				// 수리가격
	UINT32					_ui32PriceCash;					// 수리가격
	UINT32					_ui32Durability;				// 내구도 맥스
};

struct ITEM_INSERT_DATA
{
	UINT32					_ui32ItemID;					// 추가할 아이템 ID
	UINT32					_ui32ItemSec;					// 추가할 아이템 기간(초단의로 입력) (기간제 아이템일때 )
};

struct ITEM_AUTH_DATA
{
	T_ItemDBIdx				_TItemIdx;					// 인증할 아이템 Idx
	INT32					_i32ItemArg;					// 인증할 아이템 기간(초단의로 입력) (기간제 아이템일때 )
};

struct ITEM_REPAIR_DATA
{
	T_ItemDBIdx				_TItemIdx;						// 수리할 아이템 Idx
	UINT32					_ui32RepairCount;				// 수리할 아이템 수
	UINT32					_ui32Point;						// 수리 포인트 가격
};


//////////////////////////////////////////////////////////////////////////////////////////
// REQ Buffer
struct INVEN_BUFFER
{
	T_ItemDBIdx			_TItemWareDBIdx;					// 아이템 창고 DBIdx( 인증시 이 값을 사용합니다 )
	T_ItemID			_ui32ItemID;						// 구매한 아이템 ID
	UINT8				_ui8ItemType;						// 구매한 아이템 속성(0:미사용중, 1:사용중, 2:영구사용)
	T_ItemArg			_ui32ItemArg;						// (개수제(초)= Type 0:인증시간, Type 1:만료시간), (기간제=개수)

	void		Reset()
	{
		_TItemWareDBIdx = 0;
		_ui32ItemID = 0;
		_ui8ItemType = 0;
		_ui32ItemArg = 0;
	}
};

struct INVEN_BUFFER_DB : public INVEN_BUFFER
{
	T_ItemArg			_ui32AddItemArg;					// 최초 요청되었던 개수.
};


struct GIFT_BUFFER
{
	T_GiftDBIdx				_i32GiftWareDBIdx;						// DB IDX (삭제할때 사용되어짐)
	UINT32					_ui32GoodsID;							// 선물 Goods ID
	ENUM_TYPE_GIFT_DELIVER	_eDeliver;								// 지급 여부
	UINT32					_ui32Expire;							// 선물 만료날짜.
	TTCHAR					_strSenderNick[MAX_USER_NICK];		// 선물한 유저 닉네임
	TTCHAR					_strMessage[MAX_GIFT_MESSAGE_SIZE];	// 메시지


	void	Reset()
	{
		_i32GiftWareDBIdx = 0;
		_ui32GoodsID = 0;
		_eDeliver = TYPE_GIFT_DELIVER_STAND_BY;
		_ui32Expire = 0;
		_strSenderNick[0] = '\0';
		_strMessage[0] = '\0';
	}
};

struct SALE_COUPON
{
	char	m_strCouponName[32]; // 쿠폰 이름
	UINT32	m_ui32CouponID;		// 쿠폰 ID
	UINT8	m_ui8CouponType;	// 쿠폰의 기간 타입 ( 절대적이냐 상대적이냐 )
	UINT32	m_ui32PeriodArg;	// 상대적 기간
	UINT32	m_ui32StartDate;	// 언제부터
	UINT32	m_ui32EndDate;		// 언제까지 - 쿠폰 유효기간
	UINT8	m_ui8SaleType;		// 할인 방법 - % 할인이냐 아니면 정가 할인이냐 ( 퍼센트일 경우 1 ~ 50% )
	UINT32	m_ui32DisCount;		// 할인값
	UINT8	m_ui8ItemClass;		// 쿠폰 적용 가능한 아이템 클래스. 해당 아이템 클래스를 구입할때만 쿠폰 사용 가능함.
	UINT8	m_ui8LimitType;		// 금액 제한 타입 - 얼마 이상짜리를 살때만 , 얼마 이하짜리를 살때만
	UINT32	m_ui32Limit;		// 금액 제한값
};

struct COUPON_INVEN_OUT
{
	T_ItemDBIdx		m_TCouponIdx;
	T_UID			m_UID;
	SALE_COUPON		m_Coupon[MAX_COUPON_INVEN_COUNT];
};

struct LINK_BUY
{
	T_GoodsID			m_TGoodsID;								// 구매 상품 ID.
	UINT32				m_ui32Price;							// 구매 상품 가격.
	char				m_strGoodsName[MAX_ITEM_NAME];		// 구매 상품명.
};

// 메달보상기준 DBItem
struct DB_ITEM
{
	UINT8				m_ui8InsertType;	// 지급 타입. ITEM_INSERT_REQUEST_TYPE
	UINT8				m_ui8ItemCount;		// 지급 갯수.
	INVEN_BUFFER_DB		m_aItem[MAX_SHOP_DB_ITEM_COUNT]; // 아이템 정보
};

// Goods기준 DBItem
struct DB_ITEM_IN_GOODSID
{
	UINT8				m_ui8InsertType;	// 지급 타입. ITEM_INSERT_REQUEST_TYPE
	UINT8				m_ui8ItemCount;		// 지급 갯수.
	INVEN_BUFFER_DB		m_aItem[MAX_SHOP_GOODS_ITEM_COUNT]; // 아이템 정보
};

struct DB_GOODS
{
	INT32				m_i32Rv;			// 성공 여부. 
	T_GoodsID			m_TGoodsID;			// 상품 ID
	UINT8				m_ui8BuyType;		// 결제 유형.	SIA_GOODS_BUY

	DB_ITEM_IN_GOODSID	m_stDBItem;
};

// SIA 서버 전달용 구매 Goods 정보
struct BUY_GOODS
{
	UINT8				m_ui8ItemCount;		// 매칭된 아이템 갯수
	T_ItemDBIdx			m_aItemDBIdx[MAX_SHOP_GOODS_ITEM_COUNT]; // ITEM_ATTR_TYPE_STAND_BY 상태로 INSERT 된 인벤토리 DB Idx
};

// 선물 인증
struct AUTH_GIFT
{
	INT32			m_i32Rv;		// 결과
	T_GiftDBIdx		m_TDBIdx;		// 선물 DB Idx 
	UINT8			m_ui8Deliver;	// 선물 전달 상태. ENUM_TYPE_GIFT_DELIVER
	UINT32			m_ui32Expire;	// 선물 만료날짜.
};


// 선물 랭킹 정보
struct GIFT_RANKING
{
	UINT32			m_ui32Date;		// 집계 날짜. 그 주의 월요일입니다.
	UINT8			m_ui8Rank;		// 한 주간 선물 판매 순위. 
	UINT32			m_ui32GiftID;	// GoodsID / 100 => ItemID. (단, 셋트 상품 제외)
};

// 선물 판매 랭킹. ~1주일전, ~2주일전.( ex : 오늘이 5월29일 목요일이면, 서비스시작 ~ 5/18과 서비스시작 ~ 5/25의 선물판매 랭킹 )
struct GIFT_BUY_RANKING
{
	UINT8			m_ui8GiftCount;	// 순위권 내에 진입한 선물 갯수.
	GIFT_RANKING	m_aGiftRanking[MAX_TOTAL_GIFT_RANKING_COUNT];	// 선물 랭킹 정보
};

struct MINI_MATCHING
{
	UINT8				m_ui8Count;
	SHOP_MATCHING_DATA	m_aMatching[MAX_MINI_MATCHING_COUNT];
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

#pragma pack( pop )

#endif