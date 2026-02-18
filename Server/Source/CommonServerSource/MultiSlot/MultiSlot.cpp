#include "pch.h"
#include "MultiSlot.h"

CMultiSlot::CMultiSlot()
{
	m_i32CharSlotCount	= 0;
}

CMultiSlot::~CMultiSlot()
{
}

BOOL CMultiSlot::Create()
{
	for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
	{
		if( FALSE == m_aCharacters[i].Create() )	return FALSE;
	}

	// v1.0 - 팀별 슬롯에 기본캐릭터 지급
	m_i32TeamSlot[TEAM_RED] = 0;
	m_i32TeamSlot[TEAM_BLUE] = 1;

	return TRUE;
}

void CMultiSlot::Destroy()
{
}

void CMultiSlot::Reset()
{
	m_i32CharSlotCount	= 0;
	m_i32TeamSlot[TEAM_RED] = 0;
	m_i32TeamSlot[TEAM_BLUE] = 1;
	for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
	{
		m_aCharacters[i].Reset();
	}
	i3mem::FillZero( m_aCommonEquips, sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COMMON_COUNT );
}

BOOL CMultiSlot::IsActiveChar( INT32 i32SlotIdx, MULTI_SLOT_CHECK eCheck )
{
	if( 0 > i32SlotIdx )						return FALSE;
	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )	return FALSE;

	return m_aCharacters[ i32SlotIdx ].IsActive(eCheck);
}

BOOL CMultiSlot::SetActiveChar( INT32 i32SlotIdx,  MULTI_SLOT_STATE	eSlotState)
{
	if( 0 > i32SlotIdx )						return FALSE;
	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )	return FALSE;

	m_aCharacters[ i32SlotIdx ].SetActive(eSlotState);
	return TRUE;
}

MULTI_SLOT_STATE CMultiSlot::GetActive( INT32 i32SlotIdx)
{
	if( 0 > i32SlotIdx )						return MULTI_SLOT_STATE_NONE;
	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )	return MULTI_SLOT_STATE_NONE;

	return m_aCharacters[ i32SlotIdx ].GetActive();
}

void CMultiSlot::GetPacketData( S2_MULTI_SLOT_INFO* pMultiInfo )
{
	pMultiInfo->m_ui8TeamSlot[TEAM_RED]		=  (UINT8)m_i32TeamSlot[TEAM_RED];
	pMultiInfo->m_ui8TeamSlot[TEAM_BLUE]	=  (UINT8)m_i32TeamSlot[TEAM_BLUE];
	////pMultiInfo->m_i8CharCount	= (INT8)m_i32CharSlotCount;
	//INT32 i32SlotIdx = 0;
	//for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
	//{
	//	if( FALSE == m_aCharacters[i].IsActive() )	continue;
	//	pMultiInfo->m_aCharInfo[i32SlotIdx].m_ui8SlotIdx = (UINT8)i;
	//	pMultiInfo->m_aCharInfo[i32SlotIdx].m_ui8SlotState = (UINT8)m_aCharacters[i].GetActive();
	//	//m_aCharacters[i].GetPacketData( &pMultiInfo->m_aCharInfo[i32SlotIdx] );	
	//	i32SlotIdx++;
	//}
	i3mem::Copy( pMultiInfo->m_aCommonEquips, m_aCommonEquips, sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COMMON_COUNT );
}

void	CMultiSlot::GetPacketData( INT32 i32SlotIdx,  S2MO_CHAR_INFO* pCharaInfo )
{
	if( 0 > i32SlotIdx )						return;
	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )	return;

	if( FALSE == m_aCharacters[i32SlotIdx].IsActive() )	return;

	m_aCharacters[i32SlotIdx].GetPacketData( pCharaInfo );	

	return;
}


INT32 CMultiSlot::GetInsertSlot()
{
	for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT - S2MO_MULTI_SLOT_PCCAFE_CHARA_COUNT; i++ )
	{
		if( FALSE == m_aCharacters[i].IsActive() )	return i;
	}
	return CHAR_SLOT_NONE;
}

INT32 CMultiSlot::GetMakeDefCharInfo(INT32* pSlotIdx, BOOL *pMakeTeam )
{
	INT32		i32HaveDefChara = 0;		// 소지중인 기본 캐릭터 수
	INT32		i32EmptySlotCount = 0;		// 빈 슬롯 갯수
	ITEM_INFO	CharaInfo;

	for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
	{
		if( FALSE == m_aCharacters[i].IsActive() )
		{
			if( i32EmptySlotCount < TEAM_COUNT )
			{
				pSlotIdx[i32EmptySlotCount] = i;		// 빈 슬롯 저장
				++i32EmptySlotCount;
			}
			continue;
		}

		m_aCharacters[i].GetCharEquipment( CHAR_EQUIPMENT_CHARA, &CharaInfo );

		if( CharaInfo.m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN )
		{	// 기본 캐릭터 소지
			switch(GET_ITEM_SUBTYPE(GET_DEFAULT_ITEM_ID(CharaInfo.m_TItemDBIdx)))
			{
			case CHARACTER_TEAM_RED:
				pMakeTeam[TEAM_RED] = FALSE;
				break;
			case CHARACTER_TEAM_BLUE:
				pMakeTeam[TEAM_BLUE] = FALSE;
				break;
			}
			++i32HaveDefChara;
		}
	}

	return TEAM_COUNT - i32HaveDefChara;
}

BOOL CMultiSlot::GetCommonEquipmentAll( ITEM_INFO* pCommonEquips )
{
	i3mem::Copy( pCommonEquips, m_aCommonEquips, sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COMMON_COUNT );
	return TRUE;
}

BOOL CMultiSlot::GetCommonEquipment( CHAR_EQUIPMENT_COMMON eEquipment, ITEM_INFO* pCommonEquips )
{
	if( CHAR_EQUIPMENT_COMMON_NONE >= eEquipment )	return FALSE;
	if( CHAR_EQUIPMENT_COMMON_COUNT <= eEquipment )	return FALSE;

	i3mem::Copy( pCommonEquips, &m_aCommonEquips[ eEquipment ], sizeof(ITEM_INFO) );
	return TRUE;
}

BOOL CMultiSlot::GetCharBaseInfo( INT32 i32SlotIdx, S2MO_CHAR_BASE_INFO* pCharBaseInfo )
{
	if( 0 > i32SlotIdx )						return FALSE;
	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )	return FALSE;

	return m_aCharacters[ i32SlotIdx ].GetCharBaseInfo( pCharBaseInfo );
}

BOOL CMultiSlot::GetCharEquipInfo( INT32 i32SlotIdx, S2MO_CHAR_EQUIP_INFO* pCharEquipInfo )
{
	if( 0 > i32SlotIdx )						return FALSE;
	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )	return FALSE;

	return m_aCharacters[ i32SlotIdx ].GetCharEquipInfo( pCharEquipInfo );
}

BOOL CMultiSlot::GetCharSkillInfo( INT32 i32SlotIdx, S2MO_CHAR_SKILL_INFO* pCharSkillInfo )
{
	if( 0 > i32SlotIdx )						return FALSE;
	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )	return FALSE;

	return m_aCharacters[ i32SlotIdx ].GetCharSkillInfo( pCharSkillInfo );
}

BOOL CMultiSlot::GetCharEquipmentAll( INT32 i32SlotIdx, ITEM_INFO* pItemInfo )
{
	if( 0 > i32SlotIdx )						return FALSE;
	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )	return FALSE;

	return m_aCharacters[ i32SlotIdx ].GetCharEquipmentAll( pItemInfo );
}

BOOL CMultiSlot::GetCharEquipment( INT32 i32SlotIdx, INT32 i32Equipment, ITEM_INFO* pItemInfo )
{
	if( 0 > i32SlotIdx )						return FALSE;
	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )	return FALSE;
	return m_aCharacters[ i32SlotIdx ].GetCharEquipment( i32Equipment, pItemInfo );
}

//BOOL CMultiSlot::SetMainSlot( INT32 i32SlotIdx )
//{
//	if( 0 > i32SlotIdx )										return FALSE;
//	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )					return FALSE;
//	if( FALSE == m_aCharacters[ i32SlotIdx ].IsActive(MULTI_SLOT_CHECK_USING) )		return FALSE;
//		
//	m_i32TeamSlot[TEAM_RED] = i32SlotIdx;
//
//	return TRUE;
//}

BOOL CMultiSlot::InsertCharInfoAll( INT32 i32SlotIdx, MULTI_SLOT_STATE eSlotState, S2MO_CHAR_BASE_INFO* pCharBaseInfo )
{
	if( S2MO_MULTI_SLOT_COUNT < m_i32CharSlotCount+1 )								return FALSE;
	if( 0 > i32SlotIdx )															return FALSE;
	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )										return FALSE;

	if( FALSE == m_aCharacters[ i32SlotIdx ].InsertCharInfoAll( eSlotState, pCharBaseInfo ) )	return FALSE;

	// 최초 케릭터 생성일 경우 메인으로 잡아 줍니다.
	//if( 0 == m_i32CharSlotCount )	SetTeamSlot( TEAM_RED, i32SlotIdx );

	m_i32CharSlotCount++;
	return TRUE;
}

// v1.5
BOOL CMultiSlot::DeleteCharInfo(INT32 i32DeleteSlotIdx, INT32& i32MainSlotIdx)
{
	if( m_i32CharSlotCount <= 1 )													return FALSE;
	if( 0 > i32DeleteSlotIdx)														return FALSE;
	if( S2MO_MULTI_SLOT_COUNT <= i32DeleteSlotIdx)									return FALSE;

	if( FALSE == m_aCharacters[i32DeleteSlotIdx].DeleteCharInfoAll() )				return FALSE;
	m_i32CharSlotCount--;

	i32MainSlotIdx = CheckMainSlot(TEAM_RED);
	
	return TRUE;
}

// v1.0/1.1
BOOL CMultiSlot::DeleteCharInfoTeam(INT32 i32DeleteSlotIdx, INT32& i32MainSlotIdx)
{
	if (m_i32CharSlotCount <= 1)												return FALSE;
	if (0 > i32DeleteSlotIdx)													return FALSE;
	if (S2MO_MULTI_SLOT_COUNT <= i32DeleteSlotIdx)								return FALSE;

	// 삭제할 캐릭터의 팀 검색
	ITEM_INFO	CharaInfo;
	m_aCharacters[i32DeleteSlotIdx].GetCharEquipment(CHAR_EQUIPMENT_CHARA, &CharaInfo);

	TEAM_TYPE eChangeTeam = TEAM_RED;

	switch ( GET_ITEM_SUBTYPE(CharaInfo.m_TItemID))
	{
	case CHARACTER_TEAM_BLUE:
		eChangeTeam = TEAM_BLUE;
		break;
	}


	if (FALSE == m_aCharacters[i32DeleteSlotIdx].DeleteCharInfoAll())			return FALSE;
	m_i32CharSlotCount--;

	i32MainSlotIdx = CheckTeamSlot( eChangeTeam);

	return TRUE;
}

INT32 CMultiSlot::CheckMainSlot(TEAM_TYPE eChangeTeam)
{
	INT32 i32MainSlot = m_i32TeamSlot[eChangeTeam];

	if (m_aCharacters[i32MainSlot].IsActive(MULTI_SLOT_CHECK_USING))	return i32MainSlot;

	// 메인슬롯이 삭제된 경우 재설정.
	for (INT32 i = 0; i < S2MO_MULTI_SLOT_COUNT; i++)
	{
		if (FALSE == m_aCharacters[i].IsActive(MULTI_SLOT_CHECK_USING))	continue;
		m_i32TeamSlot[eChangeTeam] = i;
		return i;
	}
	return -1;
}

INT32 CMultiSlot::CheckTeamSlot(TEAM_TYPE eChangeTeam)
{
	INT32 i32MainSlot = m_i32TeamSlot[eChangeTeam];

	if (m_aCharacters[i32MainSlot].IsActive(MULTI_SLOT_CHECK_USING))	return i32MainSlot;

	// 메인슬롯이 삭제된 경우 재설정.
	ITEM_INFO	CharaInfo;
	for (INT32 i = 0; i < S2MO_MULTI_SLOT_COUNT; i++)
	{
		if (FALSE == m_aCharacters[i].IsActive(MULTI_SLOT_CHECK_USING))	continue;

		m_aCharacters[i].GetCharEquipment(CHAR_EQUIPMENT_CHARA, &CharaInfo);

		if (0 == CharaInfo.m_TItemID) continue;
		if ( eChangeTeam  != (TEAM_TYPE)GET_ITEM_SUBTYPE(CharaInfo.m_TItemID) - 1 ) continue;

		m_i32TeamSlot[eChangeTeam] = i;
		return i;
	}

	INT32 i32MainSlotIdx = -1;
	switch (eChangeTeam)
	{ 
	case TEAM_RED:	i32MainSlotIdx = 0;	break;
	case TEAM_BLUE:	i32MainSlotIdx = 1;	break; 
	}
	m_i32TeamSlot[eChangeTeam] = i32MainSlotIdx;
	
	return i32MainSlotIdx;
}

//BOOL CMultiSlot::SetChar( INT32 i32SlotIdx )
//{
//	if( 0 > i32SlotIdx )												return FALSE;
//	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )							return FALSE;
//	if( S2MO_MULTI_SLOT_COUNT <= m_i32CharSlotCount+1 )					return FALSE;
//
//	if( FALSE == m_aCharacters[ i32SlotIdx ].SetChar( eSlotState ) )	return FALSE;
//
//	m_i32CharSlotCount++;
//
//	return TRUE;
//}

BOOL CMultiSlot::SetCharNick( INT32 i32SlotIdx, TTCHAR* strNick )
{
	return m_aCharacters[ i32SlotIdx ].SetCharNick( strNick );
}

BOOL CMultiSlot::SetCharBaseInfo( INT32 i32SlotIdx, S2MO_CHAR_BASE_INFO* pCharBaseInfo )
{
	if( 0 > i32SlotIdx )						return FALSE;
	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )	return FALSE;
	return m_aCharacters[ i32SlotIdx ].SetCharBaseInfo( pCharBaseInfo );
}

BOOL CMultiSlot::SetCharSkillInfo( INT32 i32SlotIdx, S2MO_CHAR_SKILL_INFO* pCharSkillInfo )
{
	if( 0 > i32SlotIdx )						return FALSE;
	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )	return FALSE;
	return m_aCharacters[ i32SlotIdx ].SetCharSkillInfo( pCharSkillInfo );
}

BOOL CMultiSlot::SetCommonEquipmentAll( ITEM_INFO* pItemInfo )
{
	i3mem::Copy( m_aCommonEquips, pItemInfo, sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COMMON_COUNT );
	return TRUE;
}

BOOL CMultiSlot::SetCommonEquipment( INT32 i32Equipment, T_ItemDBIdx TItemDBIdx, T_ItemID TItemID )
{
	//if( CHAR_EQUIPMENT_COMMON_NONE >= i32Equipment )	return FALSE;
	//if( CHAR_EQUIPMENT_COMMON_COUNT <= i32Equipment )	return FALSE;

	m_aCommonEquips[ i32Equipment ].m_TItemDBIdx	= TItemDBIdx;
	m_aCommonEquips[ i32Equipment ].m_TItemID		= TItemID;
	return TRUE;
}

BOOL CMultiSlot::SetCharEquipmentAll( INT32 i32SlotIdx, ITEM_INFO* pItemInfo )
{
	if( 0 > i32SlotIdx )						return FALSE;
	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )	return FALSE;
	return m_aCharacters[ i32SlotIdx ].SetCharEquipmentAll( pItemInfo );
}

BOOL CMultiSlot::SetCharEquipment( INT32 i32SlotIdx, INT32 i32Equipment, T_ItemDBIdx TItemDBIdx, T_ItemID TItemID )
{
	if( 0 > i32SlotIdx )						return FALSE;
	if( S2MO_MULTI_SLOT_COUNT <= i32SlotIdx )	return FALSE;
	return m_aCharacters[ i32SlotIdx ].SetCharEquipment( i32Equipment, TItemDBIdx, TItemID );
}

BOOL CMultiSlot::SetTeamSlot(INT32 i32Slot, TEAM_TYPE eTeam )
{	
	if( 0 > i32Slot )						return FALSE;
	if( S2MO_MULTI_SLOT_COUNT <= i32Slot )	return FALSE;
	if( eTeam <= TEAM_NONE ) return FALSE;
	if( eTeam >= TEAM_COUNT ) return FALSE;
	if( FALSE == m_aCharacters[ i32Slot ].IsActive(MULTI_SLOT_CHECK_USING) ) return FALSE;
	m_i32TeamSlot[eTeam] = i32Slot;
	return TRUE;
}

INT32 CMultiSlot::GetTeamSlot(TEAM_TYPE eTeam)
{	
	if( eTeam <= TEAM_NONE ) return 0;
	if( eTeam >= TEAM_COUNT ) return 0;
	return m_i32TeamSlot[eTeam];	
}

void CMultiSlot::RefreshSlotCount( void )
{
	m_i32CharSlotCount = 0;

	for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
	{
		if( FALSE == m_aCharacters[ i ].IsActive() )	continue;
		++m_i32CharSlotCount;
		
	}
	return ;
}

INT32 CMultiSlot::FindSlotIdxByCharaDBIdx(T_ItemDBIdx TCharaDBIdx)
{
	ITEM_INFO ItemInfo;

	for (INT32 i = 0; i < S2MO_MULTI_SLOT_COUNT; ++i)
	{
		if (FALSE == GetCharEquipment(i, CHAR_EQUIPMENT_CHARA, &ItemInfo) ) continue;
		if (ItemInfo.m_TItemDBIdx != TCharaDBIdx) continue;

		return i;
	}
	return -1;
}

INT32 CMultiSlot::FindSlotIdxPCCafeChara(void)
{
	for (INT32 i = S2MO_MULTI_SLOT_COUNT - S2MO_MULTI_SLOT_PCCAFE_CHARA_COUNT; i < S2MO_MULTI_SLOT_COUNT; ++i)
	{
		if ( TRUE == m_aCharacters[i].IsActive())	continue;
		return i;
	}
	return -1;
}