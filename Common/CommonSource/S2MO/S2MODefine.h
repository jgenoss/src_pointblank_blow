#ifndef __S2_MO_DEFINE_H__
#define __S2_MO_DEFINE_H__

#define S2MO_MULTI_SLOT_COUNT				64				// 멀티슬롯 최대 카운트
#define S2MO_MULTI_SLOT_PACKET_COUNT		16				// 멀티슬롯 패킷 하나당 보낼 캐릭터 수
#define S2MO_MULTI_SLOT_PCCAFE_CHARA_COUNT	4				// 멀티슬롯 기본/PC방 캐릭터용 슬롯 갯수.
#define S2MO_MAX_NOTIFY_MADAL_COUNT			4				// 알리미 등록 메달 갯수.
#define S2MO_MAX_QUICK_JOIN_INFO_COUNT		3				// 빠른 입장 설정 정보 갯수.

//#define GET_DEFAULT_CHARA_SLOT( DBIDX )	( DBIDX &= ( 0x000000FF >> S2MO_DEFAULT_CHARA_BIT ) )

enum S2MO_ROOM_ENTER			// 방 입장 종류
{
	S2MO_ROOM_ENTER_NORMAL		= 0,		// 일반 입장.
	S2MO_ROOM_ENTER_QUICK_JOIN				// 빠른 입장.
};

enum S2MO_REPAIR_TYPE			// 전체 수리 타입
{
	S2MO_REPAIR_TYPE_ONE		= 0,		// 단일 수리.
	S2MO_REPAIR_TYPE_WEAPON,				// 무기
	S2MO_REPAIR_TYPE_ALL,					// 무기 + 파츠
};

enum REPAIR_PRICE_TYPE			// 수리 비용 타입
{
	REPAIR_PRICE_TYPE_POINT		= 1,		// 포인트
	REPAIR_PRICE_TYPE_CASH,					// 캐쉬
};

#pragma pack( push, 1)

struct S2MO_CHAR_BASE_INFO														// 캐릭터 장착 슬롯의 정보
{
	INT32					m_i32WeaponDBIdx;	// PBUser DB - Weapon 테이블

	UINT32					m_ui32CreateDate;									// 케릭터 생성 날짜
	UINT32					m_ui32BattleTime;									// 배틀 누적 시간
	UINT32					m_ui32SkillRelTime;									// 메인스킬 개방용 누적시간
	TTCHAR					m_strCharName[ NET_NICK_NAME_SIZE ];			// 케릭터 이름

	void					Reset();
};

struct S2MO_CHAR_EQUIP_INFO
{
	ITEM_INFO				m_aCharEquipment[ CHAR_EQUIPMENT_COUNT ];
	
	void					Reset();
	INT32					GetEquipmentCount();
	void					GetCharEquipmentAll( ITEM_INFO* pItemInfo );
	void					GetCharEquipmentInfo( INT32 i32Equipment, ITEM_INFO* pItemInfo );
	void					SetCharEquipmentAll( ITEM_INFO* pItemInfo );
	void					SetCharEquipment( INT32 i32Equipment, T_ItemDBIdx TItemDBIdx, T_ItemID TItemID );
};

struct S2MO_CHAR_SKILL_INFO
{
	UINT8					m_ui8Class;											// 클래스

	UINT8					m_ui8CommonSkill;									// 공통 스킬 ( COMMON_SKILL_TYPE )
	UINT8					m_ui8MainSkill;										// 메인 스킬 ( MAIN_SKILL_TYPE )
	UINT8					m_ui8AssistSkill;									// 도움 스킬 ( ASSIST_SKILL_TYPE )

	// 카테고리별 장착 스킬 레벨 정보
	UINT8					m_ui8CommonSkillLv;
	UINT8					m_ui8MainSkillLv;
	UINT8					m_ui8AssistSkillLv;

	void					Reset();
};

struct S2MO_CHAR_INFO
{
	UINT8					m_ui8SlotIdx;										// SlotIdx
	UINT8					m_ui8SlotState;										// MULTI_SLOT_STATE
	S2MO_CHAR_BASE_INFO		m_BasicInfo;					 					// 기본정보
	S2MO_CHAR_EQUIP_INFO	m_EquipInfo;										// 장착 정보
	S2MO_CHAR_SKILL_INFO	m_SkillInfo;										// SkillInfo
};

struct S2MO_CHAR_DETAIL_INFO
{
	UINT8					m_ui8SlotIdx;										// SlotIdx
	T_ItemID				m_TItemID;											// ItemID
	UINT32					m_ui32CreateDate;									// 케릭터 생성 날짜
	UINT32					m_ui32BattleTime;									// 배틀 누적 시간
	UINT32					m_ui32SkillRelTime;									// 메인스킬 개방용 누적시간
	TTCHAR					m_strCharName[ NET_NICK_NAME_SIZE ];			// 케릭터 이름
	S2MO_CHAR_SKILL_INFO	m_SkillInfo;										// SkillInfo
};

struct S2_MULTI_SLOT_INFO
{
	UINT8					m_ui8TeamSlot[TEAM_COUNT];
	//INT8					m_i8CharCount;										// 멀티 케릭터 개수
	//S2MO_CHAR_INFO			m_aCharInfo[ S2MO_MULTI_SLOT_COUNT ];				// 케릭터 장착 정보
	ITEM_INFO				m_aCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];			// 공용 파츠
};

struct S2_USER_DETAIL_INFO
{
	T_UID					m_TUID;
	TTCHAR					m_strNickName[ NET_NICK_NAME_SIZE ];
	TTCHAR					m_strClanName[ NET_CLAN_NAME_SIZE ];
	UINT32					m_ui32ClanMark;	
	UINT8					m_ui8Rank;
	UINT32					m_ui32Exp;
	UINT32					m_ui32Match;
	UINT32					m_ui32Win;
	UINT32					m_ui32Draw;
	UINT32					m_ui32Lose;
	UINT32					m_ui32DisConnect;
	UINT32					m_ui32Kill;
	UINT32					m_ui32Death;
	UINT32					m_ui32HeadShot;
	UINT32					m_ui32State;
	UINT32					m_ui32ServerIdx;
	UINT32					m_ui32ChannelNum;
	UINT32					m_ui32RoomIdx;
	USER_INFO_LAST_GAME		m_LastGame;
	USER_INFO_INVITEM_DATA	m_InvenData;
	INT8					m_i8CharCount;										// 멀티 케릭터 개수
	S2MO_CHAR_DETAIL_INFO	m_aCharInfo[S2MO_MULTI_SLOT_COUNT];				// 케릭터 장착 정보
	UINT32					m_ui32Assist;
};

struct S2_CHAR_BATTLE_INFO
{
	UINT8					m_ui8SlotIdx;
	T_ItemID				m_TCharItemID;
	T_ItemID				m_TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_COUNT ];
	T_ItemID				m_TBattleEquipParts[ CHAR_EQUIPMENT_PARTS_COUNT ];

	// vv3 - job. 공용파츠 정보 추가

	UINT8					m_ui8UseWeaponEndurance[ CHAR_EQUIPMENT_WEAPON_COUNT ];
	UINT8					m_ui8EquipUserTitle[ MAX_EQUIP_USER_TITLE ];
	S2MO_CHAR_SKILL_INFO	m_SkillInfo;
};

struct S2_CHAR_CHANGE_EQUIP_INFO
{
	UINT8					m_ui8SlotIdx;								// 캐릭터 슬롯 번호
	ITEM_INFO				m_aCharEquipment[ CHAR_EQUIPMENT_COUNT ];
};

//// 알리미 등록 메달.
//struct S2_MEDAL_NOTIFY
//{
//	UINT8					m_ui8MedalType;		// MEDAL_TYPE. 메달 타입.
//	UINT16					m_ui16Idx;			// 메달 고유 인덱스
//	UINT16					m_ui16Count;		// 진행 횟수. 
//};

//// 빠른 입장 설정 정보
//struct S2_QUICKJOIN_INFO		
//{
//	UINT8					m_ui8Stage;			// 스테이지.		STAGE_UID
//	UINT8					m_ui8StageType;		// 스테이지 타입.	STAGE_MODE
//	UINT32					m_ui32StageOption;	// 스테이지 옵션.	STAGE_OPTION_TYPE
//};

struct S2_SLOT_TOTAL_INFO : public SLOT_INFO
{
	UINT8					m_ui8SlotIdx;
	TTCHAR					m_strNickName[ NET_NICK_NAME_SIZE ];
	UINT8					m_ui8NickColor;
	UINT16					m_ui16MuzzleFlashColor;
	UINT8					m_ui8MerClass;		// 용병 신청시 주무기 병과. INIT_UINT8 : 용병아님. WEAPON_CLASS_TYPE. 
	UINT8					m_ui8MerRank;		// 용병 랭크. INIT_UINT8 : 용병아님.

	void					Reset( UINT8 ui8State );
};

// REPAIR_ITEM 구조체와 동일하게 유지해야 합니다.
struct S2MO_REPAIR_ITEM
{
	INT32					m_i32Rv;				// 처리결과
	UINT8					m_ui8BuyType;			// 결제유형. SIA_GOODS_BUY
	T_ItemID				m_TItemID;				// ItemID.
	T_ItemDBIdx				m_TItemDBIdx;			// ItemDBIdx.
	UINT32					m_ui32AuthArg;			// 수리 요청 Arg - 수리 완료 Arg
	UINT32					m_ui32RepairPoint;		// 수리 Point
	UINT32					m_ui32RepairCash;		// 수리 Cash
};


struct S2MO_REPAIR_INFO  
{
	UINT8					m_ui8ItemCount;			// 수리할 아이템 갯수.
	S2MO_REPAIR_ITEM		m_stRepairItem[CHAR_EQUIPMENT_COUNT];
};


struct S2MO_SHOP_BUY_RESULT	
{
	INT32				m_i32Rv;		// 결과
	T_GoodsID			m_TGoodsID;		// 상품 ID
	UINT8				m_ui8Action;	// ENUM_TYPE_SHOP. 선물하기시에는 SIA_GOODS_BUY 로사용.
};

struct S2MO_SHOP_REPAIR_RESULT	
{
	INT32				m_i32Rv;		// 결과
	T_ItemID			m_TItemID;		// ItemID.
};



//////////////////////////////////////////////////////////////////////////////////////
//////////////	GIFT	//////////////////////////////////////////////////////////////

//  선물 랭킹 변동 타입
enum S2MO_GIFT_CHANGE_RANK_TYPE
{	
	S2MO_GIFT_CHANGE_RANK_TYPE_NONE  = 1,	// 변동 없음.
	S2MO_GIFT_CHANGE_RANK_TYPE_NEW,		// 신규 진입
	S2MO_GIFT_CHANGE_RANK_TYPE_UP,		// 순위 상승
	S2MO_GIFT_CHANGE_RANK_TYPE_DOWN,		// 순위 하락
};

//  선물 정보
struct S2MO_GIFT		// DB 기준. PB_GT_SUserGift. 174 byte
{	
	T_GiftDBIdx		m_TDBIdx;								// 선물 DB Idx (인증시 사용)
	T_GoodsID		m_TGoodsID;								// 선물 Goods ID
	UINT8			m_ui8Deliver;							// 선물 전달 상태. ENUM_TYPE_GIFT_DELIVER
	UINT32			m_ui32Expire;							// 선물 만료날짜.
	UINT8			m_ui8SenderNickSize;					// 선물한 유저 닉네임 사이즈.
	TTCHAR			m_strSenderNick[ NET_NICK_NAME_SIZE ];	// 선물한 유저 닉네임
	UINT8			m_ui8MessageSize;						// 메시지 사이즈.
	TTCHAR			m_strMessage[ MAX_GIFT_MESSAGE_SIZE ];	// 메시지
};

// TRANS SERVER - DB BUFFER OUT
struct S2MO_RECV_GIFT	: public BUY_BASKET// 실시간 선물 받기 정보.
{
	INT32			m_i32Rv;		// 결과
	T_GiftDBIdx		m_TGiftDBIdx;	// 선물 DB Idx
};

#pragma pack( pop )


#endif	// __S2_MO_DEFINE_H__