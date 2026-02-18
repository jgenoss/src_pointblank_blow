#ifndef __COMMONDEF_CHAR_H__
#define __COMMONDEF_CHAR_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Enum
//enum DEFAULT_CHAR
//{
//	DEFAULT_CHAR_RED_BULLS,						// 레드불스
//	DEFAULT_CHAR_ACID_POOL,						// 액시드폴
//	//DEFAULT_CHAR_TARANTULA,						// 타란튤라
//	//DEFAULT_CHAR_KEEN_EYES,						// 킨아이즈	
//	//DEFAULT_CHAR_VIPERRED,						// 바이퍼레드
//	//DEFAULT_CHAR_D_FOX,							// 디폭스
//	//DEFAULT_CHAR_LEOPARD,						// 레오파드
//	//DEFAULT_CHAR_HIDE,							// 하이드
//
//	DEFAULT_CHAR_COUNT,
//};


enum MULTI_SLOT_STATE
{
	MULTI_SLOT_STATE_NONE		= 0,	// 삭제.
	MULTI_SLOT_STATE_WAIT		= 10,	// 결제대기.
	MULTI_SLOT_STATE_NORMAL		= 20,	// 결제완료. 사용중.
	MULTI_SLOT_STATE_UNUSE		= 30,	// 결제완료. 미사용중. 언제든지 활성화 시킬 수 있음.
};


enum MULTI_SLOT_CHECK
{
	MULTI_SLOT_CHECK_EXIST	= 0,	// 소지여부
	MULTI_SLOT_CHECK_USING,			// 사용중인지
};

//enum CharBranch
//{
//
//};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Struct
#pragma pack( push, 1)


struct ITEM_INFO	//: public ITEM_INFO_OTHER						// 캐릭터 장착 슬롯의 정보
{
	T_ItemID		m_TItemID = 0;				// 아이템 ID(아이템 자체)
	T_ItemDBIdx		m_TItemDBIdx = 0;			// Idx(아이템의 고유값 체크)

	bool		IsSame( ITEM_INFO * pDest) 
	{
		if( pDest->m_TItemDBIdx != m_TItemDBIdx)
			return false;

		if( pDest->m_TItemID != m_TItemID)
			return false;

		return true;
	}

	void		CopyTo( ITEM_INFO * pDest)
	{
		pDest->m_TItemDBIdx = m_TItemDBIdx;
		pDest->m_TItemID = m_TItemID;
	}
};
inline bool operator==(const ITEM_INFO& lhs, const ITEM_INFO& rhs)
{
	return( (lhs.m_TItemDBIdx == rhs.m_TItemDBIdx) && (lhs.m_TItemID == rhs.m_TItemID) );
}

inline bool operator!=(const ITEM_INFO& lhs, const ITEM_INFO& rhs)
{
	return( (lhs.m_TItemDBIdx != rhs.m_TItemDBIdx) || (lhs.m_TItemID != rhs.m_TItemID) );
}

union USER_INFO_EQUIPMENT_WEAPON
{
	struct
	{
		UINT32				_noprim;
		UINT32				_nosub;
		UINT32				_nomelee;
		UINT32				_nothrow;
		UINT32				_noitem;
	};

	T_ItemID				m_TItemID[CHAR_EQUIPMENT_WEAPON_COUNT] = { 0 };

	void	Reset() 
	{
		memset( m_TItemID, 0, sizeof(T_ItemID)*CHAR_EQUIPMENT_WEAPON_COUNT );
	}
};

class USER_INFO_EQUIPMENT_CHARA
{
private:
	T_ItemID				m_TItemID[CHAR_EQUIPMENT_PARTS_COUNT] = { 0 };

public:
	UINT32	GetEquipNum( CHAR_EQUIPMENT_PARTS eParts )						const	
	{
		return m_TItemID[ eParts ];
	}
	void	SetEquipNum( CHAR_EQUIPMENT_PARTS eParts, T_ItemID TItemID )
	{
		m_TItemID[ eParts ] = TItemID;
	}

	void	Reset()
	{
		memset( m_TItemID, 0, sizeof(T_ItemID)*CHAR_EQUIPMENT_PARTS_COUNT );
	}

	void	CopyTo( USER_INFO_EQUIPMENT_CHARA * pDest ) const
	{
		memcpy( pDest->m_TItemID, m_TItemID, sizeof(T_ItemID)*CHAR_EQUIPMENT_PARTS_COUNT );
	}

	bool IsSame(USER_INFO_EQUIPMENT_CHARA& temp)
	{
		for( INT32 i = 0 ; i < CHAR_EQUIPMENT_PARTS_COUNT ; i++ )
		{
			if( m_TItemID[i] != temp.GetEquipNum((CHAR_EQUIPMENT_PARTS)i) )	return false;
		}
		return true;
	}
};

#pragma pack( pop )
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif