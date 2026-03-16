#include "pch.h"
#include "S2MODefine.h"

void S2MO_CHAR_BASE_INFO::Reset()
{
	m_i32WeaponDBIdx		=-1;
	//m_TItemID				= 0;
	m_ui32CreateDate		= 0;
	m_ui32BattleTime		= 0;
	m_ui32SkillRelTime		= 0;
	m_strCharName[0]		= '\0';
}

void S2MO_CHAR_EQUIP_INFO::Reset()
{
	memset( m_aCharEquipment, 0, sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COUNT );
}
INT32 S2MO_CHAR_EQUIP_INFO::GetEquipmentCount()
{
	return CHAR_EQUIPMENT_COUNT;
}
void S2MO_CHAR_EQUIP_INFO::GetCharEquipmentAll( ITEM_INFO* pItemInfo )
{
	i3mem::Copy( pItemInfo, m_aCharEquipment, sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COUNT );
}
void S2MO_CHAR_EQUIP_INFO::GetCharEquipmentInfo( INT32 i32Equipment, ITEM_INFO* pItemInfo )
{
	i3mem::Copy( pItemInfo, &m_aCharEquipment[ i32Equipment ], sizeof(ITEM_INFO) );
}
void S2MO_CHAR_EQUIP_INFO::SetCharEquipmentAll( ITEM_INFO* pItemInfo )
{
	i3mem::Copy( m_aCharEquipment, pItemInfo, sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COUNT );
}
void S2MO_CHAR_EQUIP_INFO::SetCharEquipment( INT32 i32Equipment, T_ItemDBIdx TItemDBIdx, T_ItemID TItemID )
{
	m_aCharEquipment[ i32Equipment ].m_TItemDBIdx	= TItemDBIdx;
	m_aCharEquipment[ i32Equipment ].m_TItemID		= TItemID;
}

void S2MO_CHAR_SKILL_INFO::Reset()
{
	m_ui8Class			= CHARA_CLASS_NONE;
	m_ui8CommonSkill	= (UINT8) COMMON_SKILL_NONE;
	m_ui8MainSkill		= (UINT8) MAIN_SKILL_NONE;
	m_ui8AssistSkill	= (UINT8) ASSIST_SKILL_NONE;
	m_ui8CommonSkillLv	= 0;
	m_ui8MainSkillLv	= 0;
	m_ui8AssistSkillLv	= 0;
}

void S2_SLOT_TOTAL_INFO::Reset( UINT8 ui8State )
{
	SLOT_INFO::Reset( ui8State );
	m_ui8SlotIdx		= 0;
	m_ui8NickColor		= 0;
	m_strNickName[0]	= '\0';
	m_ui16MuzzleFlashColor = 0;
	m_ui8MerClass		= INIT_UINT8;
	m_ui8MerRank		= INIT_UINT8;
};