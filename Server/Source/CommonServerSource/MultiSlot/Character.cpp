#include "pch.h"
#include "Character.h"

CCharacter::CCharacter()
{
}

CCharacter::~CCharacter()
{
}

BOOL CCharacter::Create()
{
	return TRUE;
}

void CCharacter::Destroy()
{
}

void CCharacter::Reset()
{
	//m_i32SlotIdx			= CHAR_SLOT_NONE;
	m_eSlotState			= MULTI_SLOT_STATE_NONE;
	m_BaseInfo.Reset();
	m_EquipInfo.Reset();
	m_SkillInfo.Reset();
}

BOOL CCharacter::InsertCharInfoAll( MULTI_SLOT_STATE eSlotState, S2MO_CHAR_BASE_INFO* pCharBaseInfo )
{
	// i32SlotIdx 의 싱크는 DB 에서 해결해 주리라 믿습니다.

	switch( m_eSlotState )
	{	// 사용중인 슬롯
	case MULTI_SLOT_STATE_NORMAL:
	case MULTI_SLOT_STATE_UNUSE:
		return FALSE;
	}

	m_eSlotState			= eSlotState;

	i3mem::Copy( &m_BaseInfo, pCharBaseInfo, sizeof(S2MO_CHAR_BASE_INFO) );

	return TRUE;
}

BOOL CCharacter::DeleteCharInfoAll( void )
{
	// i32SlotIdx 의 싱크는 DB 에서 해결해 주리라 믿습니다.
	if( MULTI_SLOT_STATE_NONE == m_eSlotState )	return FALSE;

	m_eSlotState			= MULTI_SLOT_STATE_NONE;

	i3mem::FillZero( &m_BaseInfo,	sizeof(S2MO_CHAR_BASE_INFO) );
	i3mem::FillZero( &m_EquipInfo,  sizeof(S2MO_CHAR_EQUIP_INFO) );
	i3mem::FillZero( &m_SkillInfo,  sizeof(S2MO_CHAR_SKILL_INFO) );

	return TRUE;
}

void CCharacter::GetPacketData( S2MO_CHAR_INFO* pCharInfo )
{
	pCharInfo->m_ui8SlotState		= (UINT8)m_eSlotState;
	i3mem::Copy( &pCharInfo->m_BasicInfo, &m_BaseInfo,	sizeof(S2MO_CHAR_BASE_INFO) );
	i3mem::Copy( &pCharInfo->m_EquipInfo, &m_EquipInfo, sizeof(S2MO_CHAR_EQUIP_INFO) );
	i3mem::Copy( &pCharInfo->m_SkillInfo, &m_SkillInfo, sizeof(S2MO_CHAR_SKILL_INFO) );
}

BOOL CCharacter::GetCharBaseInfo( S2MO_CHAR_BASE_INFO* pCharBaseInfo )
{
	i3mem::Copy( pCharBaseInfo, &m_BaseInfo, sizeof(S2MO_CHAR_BASE_INFO) );

	return TRUE;
}

BOOL CCharacter::GetCharEquipInfo( S2MO_CHAR_EQUIP_INFO* pCharEquipInfo )
{
	i3mem::Copy( pCharEquipInfo, &m_EquipInfo, sizeof(S2MO_CHAR_EQUIP_INFO) );

	return TRUE;
}

BOOL CCharacter::GetCharSkillInfo( S2MO_CHAR_SKILL_INFO* pCharSkillInfo )
{
	i3mem::Copy( pCharSkillInfo, &m_SkillInfo, sizeof(S2MO_CHAR_SKILL_INFO) );

	return TRUE;
}

BOOL CCharacter::GetCharEquipmentAll( ITEM_INFO* pItemInfo )
{
	m_EquipInfo.GetCharEquipmentAll( pItemInfo );
	return TRUE;
}

BOOL CCharacter::GetCharEquipment( INT32 i32Equipment, ITEM_INFO* pItemInfo )
{
	//if( CHAR_EQUIPMENT_NONE >= i32Equipment )	return FALSE;
	//if( CHAR_EQUIPMENT_COUNT <= i32Equipment )	return FALSE;

	m_EquipInfo.GetCharEquipmentInfo( i32Equipment, pItemInfo );
	return TRUE;
}

BOOL CCharacter::SetCharNick( TTCHAR* strNick )
{
	if( MULTI_SLOT_STATE_NONE == m_eSlotState )	return FALSE;

	i3String::Copy( m_BaseInfo.m_strCharName, strNick, NET_NICK_NAME_SIZE );
	return TRUE;
}

BOOL CCharacter::SetCharBaseInfo( S2MO_CHAR_BASE_INFO* pCharBaseInfo )
{
	if( MULTI_SLOT_STATE_NONE == m_eSlotState )	return FALSE;

	i3mem::Copy( &m_BaseInfo, pCharBaseInfo, sizeof(S2MO_CHAR_BASE_INFO) );

	return TRUE;
}

BOOL CCharacter::SetCharSkillInfo( S2MO_CHAR_SKILL_INFO* pCharSkillInfo )
{
	if( MULTI_SLOT_STATE_NONE == m_eSlotState )	return FALSE;

	i3mem::Copy( &m_SkillInfo, pCharSkillInfo, sizeof(S2MO_CHAR_SKILL_INFO) );

	return TRUE;
}

BOOL CCharacter::SetCharEquipmentAll( ITEM_INFO* pItemInfo )
{
	if( MULTI_SLOT_STATE_NONE == m_eSlotState )	return FALSE;

	m_EquipInfo.SetCharEquipmentAll( pItemInfo );

	return TRUE;
}

BOOL CCharacter::SetCharEquipment( INT32 i32Equipment, T_ItemDBIdx TItemDBIdx, T_ItemID TItemID )
{
	if( MULTI_SLOT_STATE_NONE == m_eSlotState )	return FALSE;
	//if( CHAR_EQUIPMENT_NONE >= eEquipment )		return FALSE;
	//if( CHAR_EQUIPMENT_COUNT <= eEquipment )	return FALSE;

	m_EquipInfo.SetCharEquipment( i32Equipment, TItemDBIdx, TItemID );

	return TRUE;
}

BOOL	CCharacter::IsActive( MULTI_SLOT_CHECK eCheck )								
{	
	switch( eCheck )
	{
	case MULTI_SLOT_CHECK_EXIST:
		{
			switch( m_eSlotState ) 
			{
			case MULTI_SLOT_STATE_NORMAL:
			case MULTI_SLOT_STATE_UNUSE:
				return TRUE; 
			}
		}
		break;
	case MULTI_SLOT_CHECK_USING:
		{
			switch( m_eSlotState ) 
			{
			case MULTI_SLOT_STATE_NORMAL:  
				return TRUE; 
			}
		}
		break;
	}

	return FALSE;
}