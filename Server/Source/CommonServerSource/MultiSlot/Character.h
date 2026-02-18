#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#define CHAR_SLOT_NONE		0xFF

class CCharacter
{
	//INT32					m_i32SlotIdx;														// SlotIdx
	MULTI_SLOT_STATE		m_eSlotState;														// 케릭터 상태

	S2MO_CHAR_BASE_INFO		m_BaseInfo;
	S2MO_CHAR_EQUIP_INFO	m_EquipInfo;														// 장착 정보
	S2MO_CHAR_SKILL_INFO	m_SkillInfo;														// 스킬 정보
	//CCharaSkill				m_CharaSkills[ SKILL_CLASS_COUNT ];									// 장착 스킬

public:
	CCharacter();
	~CCharacter();

	//CharBranch				GetCharacterBranch()			{ return static_cast<CharBranch>(m_SkillInfo.m_ui8Class); }

	BOOL					Create();
	void					Destroy();

	void					Reset();

	BOOL					IsActive( MULTI_SLOT_CHECK eCheck = MULTI_SLOT_CHECK_EXIST );						
	
	void					SetActive( MULTI_SLOT_STATE	 eSlotState) {	m_eSlotState = eSlotState;	}
	MULTI_SLOT_STATE		GetActive( void ) {	return m_eSlotState;	}

	BOOL					InsertCharInfoAll( MULTI_SLOT_STATE eSlotState, S2MO_CHAR_BASE_INFO* pCharBaseInfo );
	BOOL					DeleteCharInfoAll( void );
	
	void					GetPacketData( S2MO_CHAR_INFO* pCharInfo );

	BOOL					GetCharBaseInfo( S2MO_CHAR_BASE_INFO* pCharBaseInfo );
	BOOL					GetCharEquipInfo( S2MO_CHAR_EQUIP_INFO* pCharEquipInfo );
	BOOL					GetCharSkillInfo( S2MO_CHAR_SKILL_INFO* pCharSkillInfo );

	BOOL					GetCharEquipmentAll( ITEM_INFO* pItemInfo );
	BOOL					GetCharEquipment( INT32 i32Equipment, ITEM_INFO* pItemInfo );
	
	BOOL					SetCharNick( TTCHAR* strNick );
	BOOL					SetCharBaseInfo( S2MO_CHAR_BASE_INFO* pCharBaseInfo );
	BOOL					SetCharSkillInfo( S2MO_CHAR_SKILL_INFO* pCharSkillInfo );
	BOOL					SetCharEquipmentAll( ITEM_INFO* pItemInfo );
	BOOL					SetCharEquipment( INT32 i32Equipment, T_ItemDBIdx TItemDBIdx, T_ItemID TItemID );
};

#endif