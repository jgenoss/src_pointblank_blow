#ifndef __PROTOCOL_BATTLE_STRUCT_H__
#define __PROTOCOL_BATTLE_STRUCT_H__

#pragma pack( push, 1)

typedef struct __tag_P_BattleServerStartAck
{
	INT32					m_i32Rv;
	INT32					_i32SlotIdx;	

	UINT8					m_ui8PosCharaSlot;						// 대표 캐릭터 Slot Idx
	CHARA_EQUIP_INFO		m_CharaEquipInfo;
	COMMON_PARTS_INFO		m_CommonPartsInfo;						// 공용파츠 1종
	CHARA_SLOT_SKILL		m_CharaSkill;							// 주특기 정보

	UINT8					_ui8InteruptEnter;
	UINT16					_TeamScore[TEAM_COUNT];					// 팀 스코어
	SCORE_INFO				_BattleUserKillDeath[SLOT_MAX_COUNT];	// 킬데스카운트

}P_BATTLE_SERVER_START_ACK;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_BATTLE_READYBATTLE_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_BATTLE_NEW_READYBATTLE_REQ
{
	bool		m_bIsObserverMode;	
	//UINT8		m_ui8CharaSlotIdx;			// 캐릭터 대표 3종 슬롯 번호( 0번배열:화면에 나오는 대표캐릭, 그외는 선택한 대표캐릭)
	T_ItemDBIdx	m_i64StartItemIdx;			// 배틀 시작하면서 소비할 아이템 인덱스
};

struct	PACKET_H_BATTLE_NEW_READYBATTLE_ACK
{
	INT32				m_i32Rv;
	UINT32				m_ui32StageID;
	COMMON_PARTS_INFO	m_CommonPartsInfo;
};

struct	PACKET_D_BATTLE_NEW_READYBATTLE_POS_CHARA
{
	CHARA_EQUIP_INFO			m_CharaEquipInfo;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_BATTLE_NEW_LOADING_COMPLATE_REQ
//////////////////////////////////////////////////////////////////////////
//struct	PACKET_BATTLE_NEW_LOADING_COMPLATE_REQ
//{
//	// PROTOCOL_BATTLE_NEW_READYBATTLE_ACK로 받은 유저의 캐릭 정보를 로드 완료시 해당 플래그를 전송한다.
//	// 0xFFFF : 16개의 슬롯에 대한 플래그값
//
//	UINT32	ui32LoadSlotFlag;								
//};
//
//struct	PACKET_BATTLE_NEW_LOADING_COMPLATE_ACK
//{
//	INT32	m_i32Rv;
//};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_BATTLE_NEW_SLOT_EQUIPMENT_REQ
//////////////////////////////////////////////////////////////////////////
// 배틀 중 실시간 아이템 장착 패킷
struct	PACKET_D_BATTLE_NEW_SLOT_EQUIPMENT_REQ
{
	ITEM_INFO	m_ItemInfo;

};
struct PACKET_D_SLOT_EQUIPMENT_NOITEM
{
	INT32						m_i32SlotIdx;										// 슬롯인덱스
	UINT16						m_ui16ChangeFlag;									// 변경된 장착 정보 비트 연산으로 보내집니다. - EQUIPMENT_IN_BATTLE	참조

	POS_MULTI_SLOT				m_PosMultiSlot;
	// 무기 내구도
	UINT8						m_ui8WeaponEndurance[ WEAPON_SLOT_COUNT ];								// 장착중인 무기의 열화 상태
};

struct PACKET_H_PROTOCOL_SLOT_EQUIPMENT_ACK
{
	PACKET_D_SLOT_EQUIPMENT_NOITEM		m_Slot_Equipment;
	UINT32								m_EquipItem;										// 장착중인 아이템 리스트 - SLOT_ITEM_BULLET_PROOF_VEST_GM 검색
};

struct PACKET_PROTOCOL_BASE_EQUIPMENT_ACK
{
	CHARA_EQUIP_INFO					m_EquipInfo;
};
//////////////////////////////////////////////////////////////////////////
//
//struct	PACKET_BATTLE_NEW_SLOT_EQUIPMENT_ACK
//{
//	INT32						m_i32SlotIdx;										// 슬롯인덱스
//	UINT16						m_ui16ChangeFlag;									// 변경된 장착 정보 비트 연산으로 보내집니다. - EQUIPMENT_IN_BATTLE	참조
//	// 무기 내구도
//	UINT8						m_ui8WeaponEndurance;								// 장착중인 무기의 열화 상태
//
//	UINT32						m_EquipItem;										// 장착중인 아이템 리스트 - SLOT_ITEM_BULLET_PROOF_VEST_GM 검색(ammo up....)
//
//
//	//	아래는 POS_MULTI_SLOT 구조체의 내용과 같음. POS_MULTI_SLOT 의 경우 대표캐릭터 슬롯 3개에 대한 정보이지만
//	//	PACKET_BATTLE_NEW_SLOT_EQUIPMENT_ACK 의 경우 슬롯 하나의 정보만 있으면 되어 배열을 사용하지 않기 위해 POS_MULTI_SLOT 를 사용하지 않음.
//
//	UINT8						m_ui8PlayMultiSlot;		// 캐릭터 Slot Idx( 0 ~ 9 ) -> 배열인덱스로 대체가능하면 삭제할것
//	CHARA_EQUIP_INFO			m_CharaEquipInfo;		// 캐릭터 장착정보
//	COMMON_PARTS_INFO			m_CommonPartsInfo;		// 공용파츠 1종
//	POS_CHARA_SKILL_INFO		m_PosCharaSkill;		// 대표캐릭터 주특기 정보
//
//	void	Reset(T_ItemID	ItemID) 
//	{
//		m_ui8PlayMultiSlot =  MAX_POS_MULTI_SLOT;
//		m_CharaEquipInfo.Reset(ItemID);
//		m_CommonPartsInfo.Reset(ItemID);
//	}
//};

//////////////////////////////////////////////////////////////////////////
//	PROTOCOL_BATTLE_NEW_LOAD_EQUIPMENT_INFO_ACK
//////////////////////////////////////////////////////////////////////////
struct PACKET_H_BATTLE_LOAD_EQUIPMENT_INFO_ACK
{
	//////////////////////////////////////////////////////////////////////////
	// Alloc Count
	UINT8				m_ui8PosCharaCount;	// 0개이면 레뒤상태인 유저가 전혀 없음을 의미한다.
};

struct PACKET_D_BATTLE_LOAD_EQUIPMENT_INFO
{
	INT32						m_i32SlotIdx;
	POS_MULTI_SLOT				m_PosMultiSlot;
	UINT8						m_ui8WeaponEndurance[ WEAPON_SLOT_COUNT ];
};

//////////////////////////////////////////////////////////////////////////
//	PROTOCOL_BATTLE_NEW_LOAD_EQUIPMENT_INFO_OTHER_ACK
//////////////////////////////////////////////////////////////////////////
struct PACKET_BATTLE_LOAD_EQUIPMENT_INFO_OTHER_ACK
{
	INT32						m_i32SlotIdx;		
	POS_MULTI_SLOT				m_PosMultiSlot;
	UINT8						m_ui8WeaponEndurance[ WEAPON_SLOT_COUNT ];	
};

//////////////////////////////////////////////////////////////////////////
//	PROTOCOL_BATTLE_NEW_LOAD_INVEN_INFO_ACK
//////////////////////////////////////////////////////////////////////////
//#define USE_CHANGE_PARTS
enum eBATTLE_LOAD_INVEN_TYPE
{
	BATTLE_LOAD_INVEN_TYPE_NONE		= -1,	// 배틀에 없는 상태

	BATTLE_LOAD_INVEN_TYPE_WEAPON_0	=  0,	// 무기정보 슬롯 0~99
	BATTLE_LOAD_INVEN_TYPE_WEAPON_100,		// 무기정보 슬롯 100~199
	BATTLE_LOAD_INVEN_TYPE_WEAPON_200,		// 무기정보 슬롯 200~299
#ifdef USE_CHANGE_PARTS
	BATTLE_LOAD_INVEN_TYPE_PARTS_0,			// 무기정보 슬롯 0~99
	BATTLE_LOAD_INVEN_TYPE_PARTS_100,		// 무기정보 슬롯 100~199
	BATTLE_LOAD_INVEN_TYPE_PARTS_200,		// 무기정보 슬롯 200~299
#endif

	MAX_BATTLE_LOAD_INVEN_TYPE
};

struct PACKET_H_BATTLE_LOAD_INVEN_INFO_ACK
{
	INT32		m_i32SlotIdx;		// 슬롯인덱스(SERVER_VALUE_UNKNOWN 값이면 배틀에 없는 경우)
	INT32		m_i32LoadInvenType;	// 인벤정보의 분할 전달 번호(BATTLE_LOAD_INVEN_TYPE_NONE 이면 배틀에 없는 상태입니다.)
	//////////////////////////////////////////////////////////////////////////
	// Alloc Count
	TItemCnt	m_i16InvenCount;	// 최대 100개의 정보 전달

};

struct PACKET_D_BATTLE_LOAD_INVEN_INFO
{
	T_ItemID		m_TItemID;		// 배틀에서 사용 가능한 아이템ID
};

struct PACKET_D_BATTLE_START_BATTLE_STAGE_ANNIHILATION
{
	UINT16				m_ui16TotalRoundCount[ TEAM_COUNT ];
	UINT16				m_ui16RountStartUser;
};

struct PACKET_D_BATTLE_START_BATTLE_STAGE_BOMB
{
	UINT16				m_ui16TotalRoundCount[ TEAM_COUNT ];
	UINT16				m_ui16RountStartUser;
};

struct PACKET_D_BATTLE_START_BATTLE_STAGE_DEFENCE
{
	UINT16				m_ui16TotalRoundCount[ TEAM_COUNT ];
	//DEFENCE_INFO		m_MiDefenceInfo;
};


struct PACKET_D_BATTLE_START_BATTLE_STAGE_DESTROY
{
	UINT16				m_ui16TotalRoundCount[ TEAM_COUNT ];
	//DESTRUCTION_INFO	m_MiDestructionInfo;
};

struct PACKET_D_BATTLE_START_BATTLE_STAGE_ESCAPE
{
	UINT16				m_ui16TotalRoundCount[ TEAM_COUNT ];
	UINT16				m_ui16RountStartUser;
	UINT8				m_ui8NowRoundCount;
	UINT8				m_ui8BlockBonusRemain;

	DINO_ROOM_INFO		m_MiDinoInfo;
};

struct PACKET_D_BATTLE_START_BATTLE_STAGE_SUDDEN_DEATH
{
	UINT16				m_ui16TotalRoundCount[ TEAM_COUNT ];
	UINT16				m_ui16RountStartUser;
};


struct PACKET_D_BATTLE_START_BATTLE_STAGE_HEAD_KILLER
{// HEAD_HUNTER
	UINT16				m_ui16TotalRoundCount[ TEAM_COUNT ];
	UINT16				m_ui16RountStartUser;
};

struct PACKET_D_BATTLE_START_BATTLE_STAGE_CROSS_COUNT
{
	UINT16				m_ui16TotalRoundCount[ TEAM_COUNT ];
	UINT16				m_ui16RountStartUser;
	UINT8				m_ui8NowRoundCount;

	DINO_ROOM_INFO		m_MiDinoInfo;
};

//struct PACKET_D_BATTLE_START_BATTLE_STAGE_HEAD_HUNTER
//{
//};

//struct PACKET_D_BATTLE_START_BATTLE_STAGE_TUTORIAL
//{
//	
//};
//
//struct PACKET_D_BATTLE_START_BATTLE_STAGE_DOMINATION
//{
//	
//};
//

//////////////////////////////////////////////////////////////////////////
//	PROTOCOL_BATTLE_EQUIPMENT_INFO_ACK
//////////////////////////////////////////////////////////////////////////
struct PACKET_H_BATTLE_EQUIPMENT_INFO_ACK
{
	//////////////////////////////////////////////////////////////////////////
	// Alloc Count
	UINT8				m_ui8PosCharaCount;	// 0개이면 레뒤상태인 유저가 전혀 없음을 의미한다.
};

struct PACKET_D_BATTLE_EQUIPMENT_INFO
{
	INT32						m_i32SlotIdx;
	POS_MULTI_SLOT				m_PosMultiSlot;
	// 무기 내구도
	UINT8						m_ui8WeaponEndurance[ WEAPON_SLOT_COUNT ];
};

//////////////////////////////////////////////////////////////////////////
//	PROTOCOL_BATTLE_NEW_EQUIPMENT_INFO_OTHER_ACK
//////////////////////////////////////////////////////////////////////////
struct PACKET_BATTLE_EQUIPMENT_INFO_OTHER_ACK
{
	INT32						m_i32SlotIdx;		// 슬롯인덱스	
	POS_MULTI_SLOT				m_PosMultiSlot;
	// 무기 내구도
	UINT8						m_ui8WeaponEndurance[ WEAPON_SLOT_COUNT ];	
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_BATTLE_NEW_RESPAWN_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_BATTLE_NEW_RESPAWN_REQ
{
	ITEM_INFO					TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_COUNT ];
	ITEM_INFO					TBattleEquipParts[ CHAR_EQUIPMENT_PARTS_COUNT ];
	UINT8						m_ui8WeaponFlag;		// 무기 제한 플래그
};

typedef struct __tag_P_BattleRespawn_Ack
{
	//BATTLE_ROOM_INFO	_RoomInfo;					// 룸정보
	//UINT32			_GroupIdx;					//
	UINT8				_cSlotIdx;					// 슬롯인덱스
	INT32				_i32TotalRespawnIdx;		// 리스폰 포인트 로테이션 인덱스
	// v1.5 Check
	POS_MULTI_SLOT		m_MyPosMultiSlot;			// 선택한 대표의 장비정보

	UINT8				_ui8UseWeaponEndurance[ WEAPON_SLOT_COUNT ];		// 내구도

	//DINO_ROOM_INFO		_MiDinoInfo;

}P_BATTLE_RESPAWN_ACK;

struct PACKET_BATTLE_NEW_RESPAWN_ACK
{
	UINT32					m_ui8SlotIdx;
	INT32					m_i32TotalRespawnIdx;
	INT32					m_i32RespawnCount;

	T_ItemID				m_TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_COUNT ];
	UINT8					m_aUseWeaponEndurance[ CHAR_EQUIPMENT_WEAPON_COUNT ];
	T_ItemID				m_TBattleEquipParts[ CHAR_EQUIPMENT_PARTS_COUNT ];
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_BATTLE_GIVEUPBATTLE_ACK
//////////////////////////////////////////////////////////////////////////
struct PACKET_PROTOCOL_BATTLE_GIVEUPBATTLE_ACK
{
	INT32					m_i32SlotIdx;				// 룸슬롯번호
	UINT32					m_ui32Exp;					// 경험치
	UINT32					m_ui32Rank;					// 계급
	UINT32					m_ui32Point;				// 포인트
	INT32					m_i32CurrentDomiScore;		// 제압모드 획득점수

	// User Record
	UINT32					m_ui32Dis;					// Total  강제종료횟수
	UINT32					m_ui32SDis;					// 시즌당 강제종료횟수
	
	// User Daily Record
	UINT32					m_ui32DGPlaytime;	
	UINT8					m_ui8GetItem;
};

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif