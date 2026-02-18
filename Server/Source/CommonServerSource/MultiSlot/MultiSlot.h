#ifndef __MULTI_SLOT_H__
#define __MULTI_SLOT_H__

#include "Character.h"

class CMultiSlot
{
	INT32					m_i32CharSlotCount;
	INT32					m_i32TeamSlot[TEAM_COUNT];												// 팀별로 장착중인 캐릭터 슬롯. For v1.0
	CCharacter				m_aCharacters[ S2MO_MULTI_SLOT_COUNT ];									// 케릭터 슬롯
	ITEM_INFO				m_aCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];							// 공용 파츠

public:
	CMultiSlot();
	~CMultiSlot();

	BOOL					Create();
	void					Destroy();

	void					GetPacketData( S2_MULTI_SLOT_INFO* pMultiInfo );
	void					GetPacketData( INT32 i32SlotIdx,  S2MO_CHAR_INFO* pCharaInfo );

	void					Reset();

	BOOL					IsActiveChar( INT32 i32SlotIdx, MULTI_SLOT_CHECK eCheck = MULTI_SLOT_CHECK_EXIST  );	// 소지중인 캐릭터 확인시 사용
	
	BOOL					SetActiveChar( INT32 i32SlotIdx,  MULTI_SLOT_STATE	eSlotState);
	MULTI_SLOT_STATE		GetActive( INT32 i32SlotIdx );

	

	BOOL					InsertChar( INT32 i32SlotIdx );
	BOOL					InsertCharInfoAll( INT32 i32SlotIdx, MULTI_SLOT_STATE eSlotState, S2MO_CHAR_BASE_INFO* pCharBaseInfo ); 
	BOOL					DeleteCharInfo(INT32 i32DeleteSlotIdx, INT32& i32MainSlotIdx);
	BOOL					DeleteCharInfoTeam(INT32 i32DeleteSlotIdx, INT32& i32MainSlotIdx);
	INT32					CheckMainSlot(TEAM_TYPE eChangeTeam);
	INT32					CheckTeamSlot(TEAM_TYPE eChangeTeam);

	//INT32					GetMainSlot()							{	return m_i32TeamSlot[TEAM_RED];	}
	INT32					GetCharSlotCount()						{	return m_i32CharSlotCount;	}
	INT32					GetMakeDefCharInfo(INT32* pSlotIdx, BOOL *pMakeTeam );
	INT32					GetInsertSlot();
	BOOL					GetCommonEquipmentAll( ITEM_INFO* pCommonEquips );
	BOOL					GetCommonEquipment( CHAR_EQUIPMENT_COMMON eEquipment, ITEM_INFO* pCommonEquips );

	// DesignV1, DesignV2 로 옮깁니다. 2015.10.01 어깨깡패
	/*BOOL					GetMainCharBaseInfo( S2MO_CHAR_BASE_INFO* pCharBaseInfo )									{	return GetCharBaseInfo( m_i32TeamSlot[TEAM_RED], pCharBaseInfo );				}
	BOOL					GetMainCharEquipInfo( S2MO_CHAR_EQUIP_INFO* pCharEquipInfo )								{	return GetCharEquipInfo( m_i32TeamSlot[TEAM_RED], pCharEquipInfo );			}
	BOOL					GetMainCharSkillInfo( S2MO_CHAR_SKILL_INFO* pCharSkillInfo )								{	return GetCharSkillInfo( m_i32TeamSlot[TEAM_RED], pCharSkillInfo );			}
	BOOL					GetMainCharEquipmentAll( ITEM_INFO* pItemInfo )												{	return GetCharEquipmentAll( m_i32TeamSlot[TEAM_RED], pItemInfo );				}
	BOOL					GetMainCharEquipment( INT32 i32Equipment, ITEM_INFO* pItemInfo )							{	return GetCharEquipment( m_i32TeamSlot[TEAM_RED], i32Equipment, pItemInfo );	}*/
	//BOOL					SetMainCharBaseInfo(S2MO_CHAR_BASE_INFO* pCharBaseInfo) { return SetCharBaseInfo(m_i32TeamSlot[TEAM_RED], pCharBaseInfo); }

	BOOL					GetCharBaseInfo( INT32 i32SlotIdx, S2MO_CHAR_BASE_INFO* pCharBaseInfo );
	BOOL					GetCharEquipInfo( INT32 i32SlotIdx, S2MO_CHAR_EQUIP_INFO* pCharEquipInfo );
	BOOL					GetCharSkillInfo( INT32 i32SlotIdx, S2MO_CHAR_SKILL_INFO* pCharSkillInfo );

	BOOL					GetCharEquipmentAll( INT32 i32SlotIdx, ITEM_INFO* pItemInfo );
	BOOL					GetCharEquipment( INT32 i32SlotIdx, INT32 i32Equipment, ITEM_INFO* pItemInfo );	

	

	//BOOL					SetMainSlot( INT32 i32SlotIdx );
	BOOL					SetCharNick( INT32 i32SlotIdx, TTCHAR* strNick );

	BOOL					SetCharBaseInfo( INT32 i32SlotIdx, S2MO_CHAR_BASE_INFO* pCharBaseInfo );
	BOOL					SetCharSkillInfo( INT32 i32SlotIdx, S2MO_CHAR_SKILL_INFO* pCharSkillInfo );

	BOOL					SetCommonEquipmentAll( ITEM_INFO* pItemInfo );
	BOOL					SetCommonEquipment( INT32 i32Equipment, T_ItemDBIdx TItemDBIdx, T_ItemID TItemID );
	BOOL					SetCharEquipmentAll( INT32 i32SlotIdx, ITEM_INFO* pItemInfo );
	BOOL					SetCharEquipment( INT32 i32SlotIdx, INT32 i32Equipment, T_ItemDBIdx TItemDBIdx, T_ItemID TItemID );

	BOOL					SetTeamSlot(INT32 i32Slot, TEAM_TYPE eTeam = TEAM_RED );							
	INT32					GetTeamSlot(TEAM_TYPE eTeam = TEAM_RED);	

	void					RefreshSlotCount(void);

	INT32					FindSlotIdxByCharaDBIdx(T_ItemDBIdx TCharaDBIdx);

	INT32					FindSlotIdxPCCafeChara(void);
	
};
#endif
