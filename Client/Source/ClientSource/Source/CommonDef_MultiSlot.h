#ifndef __COMMONDEF_MULTI_SLOT_H
#define	__COMMONDEF_MULTI_SLOT_H

#include "ItemID/ItemTypeCharaClass.h"

#define MULTI_SLOT_COUNT	9

//#define	USE_MULTI_SLOT // 배틀에서 캐릭터 여러개 사용
//////////////////////////////////////////////////////////////////////////
// 멀티슬롯
//////////////////////////////////////////////////////////////////////////
//#define	CHECK_CHARA_EQUIP	10				// 인벤토리 아이템 체크시 확인할 파츠 개수
//#define INIT_PLAY_MULTI_SLOT		-1
//#define MAX_PLAY_MULTI_SLOT		3
enum	eCharaState
{
	CHARA_STATE_NONE	= 0,	// 슬롯에 캐릭 없음
	CHARA_STATE_CREATED,		// 슬롯에 캐릭 장착중		// USE_MULTI_SLOT system이 아니라면 PLAY_POS와 동일하게 작동한다.
	CHARA_STATE_POS,			// 슬롯에 캐릭을 대표 캐릭으로 설정
	CHARA_STATE_PLAY_POS,		// 해당 슬롯이 대표 캐릭 중 화면에 표시될 캐릭

	MAX_CHARA_STATE
}; // 멀티슬롯의 상태값

//enum	ePOS_MULTI_SLOT
//{
//	POS_MULTI_SLOT_FIRST = 0,
//#ifdef USE_MULTI_SLOT
//	POS_MULTI_SLOT_SECOND,
//	POS_MULTI_SLOT_THIRD,
//#endif
//	MAX_POS_MULTI_SLOT
//}; // 대표슬롯에서 인게임중 대표로 사용할 배열의 인덱스


#define	DEF_WEAPON_SLOT_MISSION_EXCEPTION_COUNT		WEAPON_SLOT_MISSION

#define MAX_SLOT_WEAPON		WEAPON_SLOT_MISSION


//////////////////////////////////////////////////////////////////////////
// 내구도 시스템
//////////////////////////////////////////////////////////////////////////
// 무기별 가중치
enum WEAPON_ENDURANCE_WEIGHT_TYPE
{
	WEAPON_ENDURANCE_WEIGHT_NOTUSE		= 0,
	WEAPON_ENDURANCE_WEIGHT_PRIMARY,
	WEAPON_ENDURANCE_WEIGHT_SECONDARY,
	WEAPON_ENDURANCE_WEIGHT_MELEE,
	
	MAX_WEAPON_ENDURANCE_WEIGHT
};

const REAL32	g_WeaponEnduranceWeight[MAX_WEAPON_ENDURANCE_WEIGHT] = { 1.0f, 0.7f, 0.2f, 0.1f };

// 배틀 인원에 대한 가중치
enum USERCOUNT_ENDURANCE_WEIGHT_TYPE
{	
	USERCOUNT_ENDURANCE_WEIGHT_100	= 0,
	USERCOUNT_ENDURANCE_WEIGHT_120,
	USERCOUNT_ENDURANCE_WEIGHT_140,
	USERCOUNT_ENDURANCE_WEIGHT_150,

	MAX_USERCOUNT_ENDURANCE_WEIGHT
};

const REAL32	g_UserEnduranceWeight[MAX_USERCOUNT_ENDURANCE_WEIGHT] = { 1.0f, 1.2f, 1.4f, 1.5f };

// 내구도 슬롯 Bit값
enum WEAPON_ENDURANCE_TYPE
{
	WEAPON_ENDURANCE_UNKNOWN		= 0x00,	
	WEAPON_ENDURANCE_PRIMARY		= 0x01,
	WEAPON_ENDURANCE_SECONDARY		= 0x02,
	WEAPON_ENDURANCE_MELEE			= 0x04,
	WEAPON_ENDURANCE_THROWING1		= 0x08,
	WEAPON_ENDURANCE_THROWING2		= 0x10,
	WEAPON_ENDURANCE_MISSION		= 0x20,

	WEAPON_ENDURANCE_USER			= 0x1F,
	WEAPON_ENDURANCE_ALL			= 0x3F,

	WEAPON_ENDURANCE_SKIP			= 0x80,
};	// 무기 사용 여부 설정(내구도 처리)

extern WEAPON_SLOT_TYPE		GetWeaponSlot_WeaponItemIDFunc(T_ItemID WeaponItemID);
extern ITEM_TYPE			GetItemType_WeaponSlotTypeFunc(WEAPON_SLOT_TYPE WeaponSlotType);

// 캐릭 소유권 검사(CheckMultiChara)
enum ENUM_CHECK_MULTI_CHARA
{
	CHECK_MULTI_SLOT_NONE = 0,
	CHECK_MULTI_SLOT_FILL,
	CHECK_MULTI_SLOT_EMPTY,

	MAX_CHECK_MULTI_SLOT
};

//////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1)
//////////////////////////////////////////////////////////////////////////

enum ENUM_CREATE_CHARA_SUPPLY_ITEM
{
	ENUM_CREATE_CHARA_SUPPLY_ITEM_ASSERT	= 0,
	ENUM_CREATE_CHARA_SUPPLY_ITEM_SMG,
	ENUM_CREATE_CHARA_SUPPLY_ITEM_SNIPER,
	ENUM_CREATE_CHARA_SUPPLY_ITEM_MG,
	ENUM_CREATE_CHARA_SUPPLY_ITEM_SHOTGUN,
	ENUM_CREATE_CHARA_SUPPLY_ITEM_HANDGUN,
	ENUM_CREATE_CHARA_SUPPLY_ITEM_DINO,

	MAX_CREATE_CHARA_SUPPLY_ITEM
};

// 지급품의 종류를 바꿀순 있지만, 속성을 바꿀수는 없다.
// 예를 들어 AK를 K2로 바꿀순 있지만, AK를 SMG로 바꿀수는 없다.
struct CREATE_CHARA_SUPPLY_ITEM				// 캐릭 생성시 기본 지급품
{
	INT32		m_Cnt = 0;	// 설정된 지급품 개수
	ITEM_INFO	m_CreateCharaSupplyItem[MAX_CREATE_CHARA_SUPPLY_ITEM];
	UINT32		m_ItemArg[MAX_CREATE_CHARA_SUPPLY_ITEM] = { 0 };

	void		Reset()
	{
		m_Cnt = 0;
		ZeroMemory(m_CreateCharaSupplyItem, sizeof(ITEM_INFO)*MAX_CREATE_CHARA_SUPPLY_ITEM);
		ZeroMemory(m_ItemArg,				sizeof(ITEM_INFO)*MAX_CREATE_CHARA_SUPPLY_ITEM);
	}
	
	T_ItemDBIdx	GetItemDBIdx(INT32 Idx)	{ return m_CreateCharaSupplyItem[Idx].m_TItemDBIdx; }
	T_ItemID	GetItemID(INT32 Idx)	{ return m_CreateCharaSupplyItem[Idx].m_TItemID; }
	UINT32		GetItemArg(INT32 Idx)	{ return m_ItemArg[Idx]; }

	void		SetItem(INT32 Idx, T_ItemDBIdx InvenIdx, T_ItemID ItemID, UINT32 ItemArg) 
	{ 
		m_CreateCharaSupplyItem[Idx].m_TItemDBIdx	= InvenIdx;
		m_CreateCharaSupplyItem[Idx].m_TItemID	= ItemID; 
		m_ItemArg[Idx]								= ItemArg; 
	}
	void		SetItemDBIdx(INT32 Idx, T_ItemDBIdx InvenIdx) 
	{
		m_CreateCharaSupplyItem[Idx].m_TItemDBIdx	= InvenIdx;
	}
	void		SetItemID(INT32 Idx, T_ItemID ItemID) 
	{
		m_CreateCharaSupplyItem[Idx].m_TItemID	= ItemID;
	}
	void		SetItemArg(INT32 Idx, UINT32 ItemArg) 
	{
		m_ItemArg[Idx]								= ItemArg; 
	}

	INT32		GetCount() { return m_Cnt; }
	void		SetCnt(INT32 Idx)
	{
		if(MAX_CREATE_CHARA_SUPPLY_ITEM >= Idx)
		{
			m_Cnt	= Idx;
		}		
	}
};

// 캐릭터 무기 정보
struct CHARA_WEAPON_INFO
{ 
	ITEM_INFO		m_Info[ CHAR_EQUIPMENT_WEAPON_COUNT ];

	void	Delete()
	{	
		// 캐릭 삭제시에만 사용
		i3mem::FillZero( m_Info, sizeof( ITEM_INFO ) * CHAR_EQUIPMENT_WEAPON_COUNT );
	}

	void	Reset()
	{
		Delete();
		m_Info[CHAR_EQUIPMENT_WEAPON_PRIMARY].m_TItemID		= DEFAULT_WEAPON_ASSAULT;
		m_Info[CHAR_EQUIPMENT_WEAPON_SECONDARY].m_TItemID	= DEFAULT_WEAPON_HANDGUN;
		m_Info[CHAR_EQUIPMENT_WEAPON_MELEE].m_TItemID		= DEFAULT_WEAPON_KNIFE;
		m_Info[CHAR_EQUIPMENT_WEAPON_THROWING1].m_TItemID	= DEFAULT_WEAPON_THROWING1;
		m_Info[CHAR_EQUIPMENT_WEAPON_THROWING2].m_TItemID	= DEFAULT_WEAPON_THROWING2;
	}

	void	CopyTo( CHARA_WEAPON_INFO * pDest)
	{
		memcpy( pDest->m_Info, m_Info, sizeof(ITEM_INFO)*CHAR_EQUIPMENT_WEAPON_COUNT );
	}
};

// 캐릭터 파츠 정보
struct CHARA_PARTS_INFO						
{
	ITEM_INFO			m_Info[ CHAR_EQUIPMENT_PARTS_COUNT];
	
	CHARA_PARTS_INFO()
	{
		Reset();
	}

	bool		IsSame( const CHARA_PARTS_INFO * pDest) const
	{
		for( INT32 i = 0 ; i < CHAR_EQUIPMENT_PARTS_COUNT ; ++i )
		{
			if ( pDest->m_Info[i].m_TItemID != m_Info[i].m_TItemID ) return false;
			if ( pDest->m_Info[i].m_TItemDBIdx != m_Info[i].m_TItemDBIdx ) return false;			
		}

		return true;
	}

	bool	IsSameBody( const CHARA_PARTS_INFO * pDest) const
	{
		for( INT32 i = 0 ; i < CHAR_EQUIPMENT_PARTS_COUNT ; ++i )
		{
			switch( i )
			{
			case CHAR_EQUIPMENT_PARTS_HEAD :
			case CHAR_EQUIPMENT_PARTS_FACE :
			case CHAR_EQUIPMENT_PARTS_BELT :
			case CHAR_EQUIPMENT_PARTS_HOLSTER :
			case CHAR_EQUIPMENT_PARTS_BERET :
				continue;
			}

			if ( pDest->m_Info[i].m_TItemID != m_Info[i].m_TItemID ) return false;
			if ( pDest->m_Info[i].m_TItemDBIdx != m_Info[i].m_TItemDBIdx ) return false;
			
		} 

		return true;
	}

	bool	IsSameEquip( CHARA_PARTS_INFO * pDest) const
	{
		for( INT32 i = 0 ; i < CHAR_EQUIPMENT_PARTS_COUNT ; ++i )
		{
			switch( i )
			{
			case CHAR_EQUIPMENT_PARTS_CHARA :
			case CHAR_EQUIPMENT_PARTS_UPPER :
			case CHAR_EQUIPMENT_PARTS_LOWER :
			case CHAR_EQUIPMENT_PARTS_GLOVE :
			case CHAR_EQUIPMENT_PARTS_SKIN :
				continue;
			}

			if ( pDest->m_Info[i].m_TItemID != m_Info[i].m_TItemID ) return false;
			if ( pDest->m_Info[i].m_TItemDBIdx != m_Info[i].m_TItemDBIdx ) return false;
			
		} 

		return true; 
	}

	void		SetItemInfo( CHAR_EQUIPMENT_PARTS part, const ITEM_INFO & Info)
	{
		if (part < 0 || part >= CHAR_EQUIPMENT_PARTS_COUNT)
		{
			I3ASSERT(part >= 0 && part < CHAR_EQUIPMENT_PARTS_COUNT);
			return;
		}
		m_Info[ part] = Info;
	}
 
	ITEM_INFO *			GetPartsInfo( CHAR_EQUIPMENT_PARTS eParts )						{ return &m_Info[ eParts ];	}	
	const ITEM_INFO *	GetPartsInfo( CHAR_EQUIPMENT_PARTS eParts )	const				{ return &m_Info[ eParts ];	}

	void		SetPartsInfo( CHAR_EQUIPMENT_PARTS eParts, ITEM_INFO* pPartsInfo )		
	{ 
		if (eParts >= CHAR_EQUIPMENT_PARTS_COUNT)
		{
			I3ASSERT(eParts < CHAR_EQUIPMENT_PARTS_COUNT);	return;
		}
		i3mem::Copy( &m_Info[ eParts ], pPartsInfo, sizeof(ITEM_INFO) ); 
	}

	void		SetPartsIdx( CHAR_EQUIPMENT_PARTS eParts, T_ItemDBIdx i64ItemIdx )		{ m_Info[ eParts ].m_TItemDBIdx = i64ItemIdx;	}
	T_ItemDBIdx	GetPartsIdx( CHAR_EQUIPMENT_PARTS eParts )		const					{ return m_Info[ eParts ].m_TItemDBIdx;			}
	void		SetPartsItemID( CHAR_EQUIPMENT_PARTS eParts, T_ItemID ui32ItemID )		
	{ 
		if (eParts < 0 || eParts >= CHAR_EQUIPMENT_PARTS_COUNT)
		{
			I3ASSERT(eParts < CHAR_EQUIPMENT_PARTS_COUNT);
			return;
		}
		if( eParts == CHAR_EQUIPMENT_PARTS_CHARA )
			Reset();
		
		m_Info[ eParts ].m_TItemID = ui32ItemID;	
	}

	T_ItemID	GetPartsItemID( CHAR_EQUIPMENT_PARTS eParts ) const;

	INT32		GetPartsResID(CHAR_EQUIPMENT_PARTS eParts) const		{ return GET_ITEM_NUMBER( GetPartsItemID(eParts) ); }

	void		SetItemID( T_ItemID itemID);

	void	Reset( void);

	void	CopyTo( CHARA_PARTS_INFO * pDest ) //const
	{
		i3mem::Copy( pDest->m_Info, m_Info , sizeof(ITEM_INFO) * CHAR_EQUIPMENT_PARTS_COUNT );
	}

	void	CopyTo( CHARA_PARTS_INFO * pDest ) const
	{
		i3mem::Copy( pDest->m_Info, m_Info , sizeof(ITEM_INFO) * CHAR_EQUIPMENT_PARTS_COUNT );
	}
 
 	bool IsValid( void) const
	{
		for( INT32 i = 0; i < CHAR_EQUIPMENT_PARTS_COUNT; ++i )
		{
			T_ItemID ItemID = GetPartsItemID( static_cast<CHAR_EQUIPMENT_PARTS>(i) );

			if( ItemID == 0 )
			{
				if( IsSkipValidCheck(i) )	continue;
				else						return false;
			}

			switch( i )
			{
			case CHAR_EQUIPMENT_PARTS_BERET :		
				if( ITEM_TYPE_BERET != GET_ITEM_TYPE(ItemID) ) return false;
				break;
			case CHAR_EQUIPMENT_PARTS_CHARA : 
				if( !is_valid_type(ITEM_TYPE_CHARA, ItemID) && 
					!is_valid_type(ITEM_TYPE_DINO, ItemID) )	return false;
				break;
			default :	
				ITEM_TYPE item_type = static_cast<ITEM_TYPE>(ITEM_TYPE_CHARA + i);
				if( !is_valid_type(item_type, ItemID) ) return false;
				break;
			}
		}
		return true;
	}

	bool IsSkipValidCheck( INT32 idx ) const
	{
		switch( idx) 
		{
		case CHAR_EQUIPMENT_PARTS_HEAD	:
		case CHAR_EQUIPMENT_PARTS_SKIN	:
		case CHAR_EQUIPMENT_PARTS_BERET	: return true;
		}
		
		return false;
	}

	bool is_valid_type(ITEM_TYPE chk_type, T_ItemID item_id ) const
	{
		ITEM_TYPE item_type = static_cast<ITEM_TYPE>(GET_ITEM_TYPE(item_id));

		if( chk_type == ITEM_TYPE_HEADGEAR && item_type == ITEM_TYPE_BERET )
			item_type = ITEM_TYPE_HEADGEAR;

		return (chk_type == item_type);
	}
};

// 캐릭터 정보
struct CHARA_BASIC_INFO														
{
	UINT8					m_ui8CharaState = CHARA_STATE_NONE;								// 캐릭터 상태( 0 : 없음, 1: 생성)
	UINT8					m_ui8CharaSlotIdx = INIT_UINT8;								// 캐릭터 Slot Idx( 0 ~ 9 ) -> 배열인덱스로 대체가능하면 삭제할것
	TTCHAR					m_strCharaName[NET_CHARA_NICK_NAME_SIZE] = { 0 };		// 캐릭터 별명
	UINT32					m_ui32CreateDate = 0;								// 생성일

	void	Reset() 
	{
		m_ui8CharaState		= CHARA_STATE_NONE;
		m_ui8CharaSlotIdx	= INIT_UINT8;		
		ZeroMemory(m_strCharaName,	sizeof(char)*NET_CHARA_NICK_NAME_SIZE);
		m_ui32CreateDate	= 0;
	}
};

// 공용 파츠 정보
struct COMMON_PARTS_INFO					
{
	ITEM_INFO		m_Info[CHAR_EQUIPMENT_COMMON_COUNT];					 

	void	Reset( void);

	ITEM_INFO *		getParts( CHAR_EQUIPMENT_COMMON eParts = CHAR_EQUIPMENT_COMMON_DINO)				{ return &m_Info[eParts];}
	const ITEM_INFO * getParts( CHAR_EQUIPMENT_COMMON eParts = CHAR_EQUIPMENT_COMMON_DINO ) const		{ return &m_Info[eParts];}

	void			SetParts( ITEM_INFO *pItemInfo, CHAR_EQUIPMENT_COMMON eParts = CHAR_EQUIPMENT_COMMON_DINO )				
	{ 
		i3mem::Copy( &m_Info[eParts], pItemInfo, sizeof(ITEM_INFO) );
	}

	void	CopyTo( COMMON_PARTS_INFO * pDest)
	{
		i3mem::Copy( pDest->m_Info, m_Info, sizeof(ITEM_INFO) * CHAR_EQUIPMENT_COMMON_COUNT );
	}

	void		SetPartsItemID( T_ItemID TItemID , CHAR_EQUIPMENT_COMMON eParts = CHAR_EQUIPMENT_COMMON_DINO )	{ m_Info[eParts].m_TItemID = TItemID; }
	T_ItemID	GetPartsItemID( CHAR_EQUIPMENT_COMMON eParts = CHAR_EQUIPMENT_COMMON_DINO )						{ return m_Info[eParts].m_TItemID; }

	void		SetPartsIdx( T_ItemDBIdx TItemDBIdx, CHAR_EQUIPMENT_COMMON eParts = CHAR_EQUIPMENT_COMMON_DINO  ){ m_Info[eParts].m_TItemDBIdx = TItemDBIdx; }
	T_ItemDBIdx	GetPartsIdx( CHAR_EQUIPMENT_COMMON eParts = CHAR_EQUIPMENT_COMMON_DINO )						{ return m_Info[eParts].m_TItemDBIdx; }
};

// 캐릭터 장착 정보
struct CHARA_EQUIP_INFO												
{
	CHARA_WEAPON_INFO		m_CharWeaponInfo;						// 무기 정보
	CHARA_PARTS_INFO		m_CharPartsInfo;						// 파츠 정보

	void	Reset( void) 
	{
		m_CharWeaponInfo.Reset();
		m_CharPartsInfo.Reset();		
	}

	void	CopyTo( CHARA_EQUIP_INFO * pDest)
	{
		m_CharWeaponInfo.CopyTo( &pDest->m_CharWeaponInfo);
		m_CharPartsInfo.CopyTo( &pDest->m_CharPartsInfo);
	}
};

struct POS_SLOT_INFO
{	
	UINT8						m_ui8CharaSlotCount = 0;						// 생성한 캐릭터 개수(2개이하로는 삭제불가)
	UINT32						m_ui32OwnCharaSlotFlag = 0;						// 캐릭터 설정유무 판단 Flag	0xFF000000 : eMULTI_SLOT
																			// 캐릭슬롯Flag(0x00000003 이상의 값이면 캐릭을 생성한 경우임). 캐릭 생성시 서버에서 해당 SlotIdx의 플래그를 활성화함.
	UINT8						m_ui8PosCharaSlot = INIT_UINT8;							// 대표캐릭터 슬롯번호(9 : None, 0~8 : CharaSlotNum)		
	UINT8						m_ui8OwnCharaSlotCount = 0;						// 소유한 슬롯갯수(구매시 1씩 증가,감소없음)	
	UINT8						m_ui8CashCharaSlotCount = 0;

	void Reset()
	{
		m_ui8CharaSlotCount		= 0;
		m_ui32OwnCharaSlotFlag	= 0;

		m_ui8PosCharaSlot		= INIT_UINT8;
		m_ui8OwnCharaSlotCount	= 0;
		m_ui8CashCharaSlotCount	= 0;
	}
};

// 캐릭터 정보
struct CHARA_INFO			
{
	MULTI_SLOT_STATE		m_eState = MULTI_SLOT_STATE_NONE;
	CHARA_BASIC_INFO		m_CharaBasicInfo;						// 캐릭터정보 // 주특기
	CHARA_SLOT_SKILL		m_CharaSkillInfo;						// 주특기 정보
	CHARA_EQUIP_INFO		m_CharaEquipInfo;						// 개인무기 3종 // 몸체 1종 // 파츠 8종
	UINT32					m_ui32BattleTime = 0;						// 배틀 참여 시간 ( 누적 )
	UINT32					m_ui32SkillRelTime = 0;						// 스킬 개방권 사용 시간

	void	Reset( void) 
	{
		m_CharaBasicInfo.Reset();
		m_CharaSkillInfo.Reset();
		m_CharaEquipInfo.Reset();
		m_ui32BattleTime = 0;
		m_ui32SkillRelTime = 0;
	}

	CHARA_BASIC_INFO *		getBasicInfo( void)			{ return &m_CharaBasicInfo; }
	CHARA_SLOT_SKILL *		getSkillInfo( void)			{ return &m_CharaSkillInfo; }
	CHARA_EQUIP_INFO *		getEquipInfo( void)			{ return &m_CharaEquipInfo; }

	const CHARA_PARTS_INFO * getPartsInfo( void) const	{ return &m_CharaEquipInfo.m_CharPartsInfo; }
};

// 캐릭터 무기 정보
struct CHARA_WEAPON_INFO_OTHER					
{
	T_ItemID	m_Info[CHAR_EQUIPMENT_WEAPON_COUNT] = { DEFAULT_WEAPON_ASSAULT , DEFAULT_WEAPON_HANDGUN , DEFAULT_WEAPON_KNIFE , DEFAULT_WEAPON_THROWING1 , DEFAULT_WEAPON_THROWING2 };
	
	void	Reset()
	{		
		m_Info[CHAR_EQUIPMENT_WEAPON_PRIMARY]		= DEFAULT_WEAPON_ASSAULT;
		m_Info[CHAR_EQUIPMENT_WEAPON_SECONDARY]		= DEFAULT_WEAPON_HANDGUN;
		m_Info[CHAR_EQUIPMENT_WEAPON_MELEE]			= DEFAULT_WEAPON_KNIFE;
		m_Info[CHAR_EQUIPMENT_WEAPON_THROWING1]		= DEFAULT_WEAPON_THROWING1;
		m_Info[CHAR_EQUIPMENT_WEAPON_THROWING2]		= DEFAULT_WEAPON_THROWING2;
	}

	void	SetData(CHARA_WEAPON_INFO_OTHER* pData) 
	{
		i3mem::Copy( m_Info, pData->m_Info, sizeof(T_ItemID) * CHAR_EQUIPMENT_WEAPON_COUNT);
	}

	void	SetDataNoIdx(CHARA_WEAPON_INFO*	pData) 
	{
		for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT; ++i )
		{
			m_Info[i] = pData->m_Info[i].m_TItemID;
		}
	}

	void	CopyTo(CHARA_WEAPON_INFO*	pData) 
	{
		for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT; ++i )
		{
			pData->m_Info[i].m_TItemID = m_Info[i];
		}
	}

	T_ItemID	GetItemID( INT32 type) const
	{
		I3_BOUNDCHK( type, CHAR_EQUIPMENT_WEAPON_COUNT);
		return m_Info[type];
	}
};

// 캐릭터 파츠 정보
struct CHARA_PARTS_INFO_OTHER					
{
public:
	 
	T_ItemID			m_Info[ CHAR_EQUIPMENT_PARTS_COUNT];		// [initialize at constructor]
 
	CHARA_PARTS_INFO_OTHER()
	{
		Reset(0);
	}

	bool		IsSame( CHARA_PARTS_INFO_OTHER * pDest)
	{
		for( INT32 i = 0 ; i < CHAR_EQUIPMENT_PARTS_COUNT ; ++i )
		{
			if ( pDest->m_Info[i] != m_Info[i] ) return false;
		}

		return true;
	}

	void	SetItemID( CHAR_EQUIPMENT_PARTS eParts, T_ItemID ui32ItemID)			
	{ 
		m_Info[ eParts ] = ui32ItemID; 
	}

	void	Reset(T_ItemID ui32ItemID);

	void	SetData(CHARA_PARTS_INFO_OTHER* pData) 
	{
		i3mem::Copy( m_Info, pData->m_Info, sizeof(T_ItemID) * CHAR_EQUIPMENT_PARTS_COUNT );
	}

	void	SetDataNoIdx(CHARA_PARTS_INFO*	pData) 
	{
		for( INT32 i = 0 ; i < CHAR_EQUIPMENT_PARTS_COUNT ; ++i )
		{
			m_Info[i] = pData->m_Info[i].m_TItemID;
		}
	}

	void	CopyTo(CHARA_PARTS_INFO*	pData) const
	{
		for( INT32 i = 0 ; i < CHAR_EQUIPMENT_PARTS_COUNT ; ++i )
		{
			pData->m_Info[i].m_TItemID = m_Info[i];
		}
	}
 
	bool	IsValid( void)
	{
		for( INT32 i = 0; i < CHAR_EQUIPMENT_PARTS_COUNT; ++i )
		{
			T_ItemID ItemID = GetPartsItemID( static_cast<CHAR_EQUIPMENT_PARTS>(i) );

			if( ItemID == 0 )
			{
				if( IsSkipValidCheck(i) )	continue;
				else						return false;
			}

			switch( i )
			{
			case CHAR_EQUIPMENT_PARTS_BERET :		
				if( ITEM_TYPE_BERET != GET_ITEM_TYPE(ItemID) ) return false;
				break;
			case CHAR_EQUIPMENT_PARTS_CHARA : 
				if( !is_valid_type(ITEM_TYPE_CHARA, ItemID) && 
					!is_valid_type(ITEM_TYPE_DINO, ItemID) )	return false;
				break;
			default :	
				ITEM_TYPE item_type = static_cast<ITEM_TYPE>(ITEM_TYPE_CHARA + i);
				if( !is_valid_type(item_type, ItemID) ) return false;
				break;
			}
		}

		return true;
	}

	bool IsSkipValidCheck( INT32 idx ) const
	{
		switch( idx) 
		{
		case CHAR_EQUIPMENT_PARTS_HEAD	:
		case CHAR_EQUIPMENT_PARTS_SKIN	:
		case CHAR_EQUIPMENT_PARTS_BERET	: return true;
		}

		return false;
	}

	bool is_valid_type(ITEM_TYPE chk_type, T_ItemID item_id)  const
	{
		ITEM_TYPE item_type = static_cast<ITEM_TYPE>(GET_ITEM_TYPE(item_id));

		if( chk_type == ITEM_TYPE_HEADGEAR && item_type == ITEM_TYPE_BERET )
			item_type = ITEM_TYPE_HEADGEAR;

		return (chk_type == item_type);
	}

	T_ItemID	GetPartsItemID( CHAR_EQUIPMENT_PARTS eParts ) const;						
};

// 공용 파츠 정보
struct COMMON_PARTS_INFO_OTHER
{
	T_ItemID		m_Info[CHAR_EQUIPMENT_COMMON_COUNT] = { DEFAULT_DINO };

	void	Reset()
	{
		i3mem::FillZero( m_Info, sizeof(T_ItemID) * CHAR_EQUIPMENT_COMMON_COUNT);
		m_Info[CHAR_EQUIPMENT_COMMON_DINO]		= DEFAULT_DINO;
	}

	void	SetData(COMMON_PARTS_INFO_OTHER* pData) 
	{
		for( INT32 i = 0 ; i < CHAR_EQUIPMENT_COMMON_COUNT; ++ i)
		{
			m_Info[i] = pData->m_Info[i];
		}
	}

	void	SetDataNoIdx(COMMON_PARTS_INFO*	pData) 
	{
		for( INT32 i = 0 ; i < CHAR_EQUIPMENT_COMMON_COUNT; ++ i)
		{
			m_Info[i] = pData->m_Info[i].m_TItemID;
		}
	}
	void	CopyTo(COMMON_PARTS_INFO *pData)
	{
		for( INT32 i = 0 ; i < CHAR_EQUIPMENT_COMMON_COUNT; ++ i)
		{
			pData->m_Info[i].m_TItemDBIdx = 0;
			pData->m_Info[i].m_TItemID = m_Info[i];
		}
	}

	void		SetPartsItemID( T_ItemID TItemID, CHAR_EQUIPMENT_COMMON eParts = CHAR_EQUIPMENT_COMMON_DINO )	{ m_Info[eParts] = TItemID; }
	T_ItemID	GetPartsItemID( CHAR_EQUIPMENT_COMMON eParts  = CHAR_EQUIPMENT_COMMON_DINO)	{ return m_Info[eParts]; }
};

// 캐릭터 장착 정보
struct CHARA_EQUIP_INFO_OTHER
{
	CHARA_WEAPON_INFO_OTHER		m_CharWeaponInfo;						// 무기 정보
	CHARA_PARTS_INFO_OTHER		m_CharPartsInfo;						// 파츠 정보

	void	Reset(T_ItemID ItemID) 
	{
		m_CharWeaponInfo.Reset();
		m_CharPartsInfo.Reset(ItemID);		
	}

	void	SetData(CHARA_EQUIP_INFO_OTHER* pData) 
	{
		m_CharWeaponInfo.SetData(&pData->m_CharWeaponInfo);
		m_CharPartsInfo.SetData(&pData->m_CharPartsInfo);
	}
	void	SetDataNoIdx(CHARA_EQUIP_INFO*	pData) 
	{
		m_CharWeaponInfo.SetDataNoIdx(&pData->m_CharWeaponInfo);
		m_CharPartsInfo.SetDataNoIdx(&pData->m_CharPartsInfo);
	}
};

struct POS_MULTI_SLOT
{
	UINT8				m_ui8PlayMultiSlot = INIT_UINT8;						// 캐릭터 Slot Idx( 0 ~ 9 ) -> 배열인덱스로 대체가능하면 삭제할것

	CHARA_EQUIP_INFO_OTHER		m_CharaEquipInfo;
	COMMON_PARTS_INFO_OTHER		m_CommonPartsInfo;
	CHARA_SLOT_SKILL			m_PosCharaSkill;						// 대표캐릭터 주특기 정보

	void	Reset(T_ItemID ItemID) 
	{
		m_ui8PlayMultiSlot		= INIT_UINT8;
		m_CharaEquipInfo.Reset(ItemID);
		m_PosCharaSkill.Reset();
		m_CommonPartsInfo.Reset();
	}
};

struct OWN_CHARA_INFO
{
	UINT32	ui32CharaFlag = 0;		// 고유 인덱스 번호
	UINT8	m_ui8ManCount = 0;		// 생성한 남자   캐릭터 개수
	UINT8	m_ui8WomonCount = 0;	// 생성한 여자   캐릭터 개수
	UINT8	m_ui8SkinCount = 0;		// 생성한 스키니 캐릭터 개수
};

struct MULTI_SLOT
{
 
	CHARA_INFO				m_CharaInfo[MULTI_SLOT_COUNT];			// 캐릭터 정보
	COMMON_PARTS_INFO		m_CommonPartsInfo;						// 공용파츠 1종

	void	Reset(T_ItemID ItemID) 
	{
		for(INT32 Loop = 0; Loop < MULTI_SLOT_COUNT; Loop++)
		{
			m_CharaInfo[Loop].Reset();
		}

		m_CommonPartsInfo.Reset();
	}

};

#pragma pack( pop )

#endif