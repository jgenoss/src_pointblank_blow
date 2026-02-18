#ifndef __DESIGN_V2_H__
#define __DESIGN_V2_H__

#include "DesignBase.h"

class CDesignV2 : public CDesignBase
{
private:
	
protected:

public:
	 CDesignV2();
	virtual ~CDesignV2();

	virtual BOOL				OnCreate(  );
	void						OnDestroy();

	virtual INT32				GetActiveTeamCount(void) {	return 1; };
	virtual	BOOL				CheckItemPosition( T_ItemID TItemID, CHAR_EQUIPMENT eSlot );
	virtual	BOOL				CheckDuplicationParts(void)	{ return TRUE; } 
	virtual	T_RESULT			CheckBuyChara( UINT8 ui8CreateType, CUserSession* pSession, T_GoodsID TGoodsID )	;
	virtual BOOL				SetTeamSlot(CMultiSlot& MultiSlot, UINT8 ui8TeamSlot, T_ItemID TCharItemID );
	virtual UINT32				GetExpValue(INT32 i32Rank);
	virtual INT32				GetCharaSlotByTeam(INT32 i32Team, CUserSession* pSession ) ;
	virtual BOOL				CheckItemSlot( ITEM_INFO* aParts, INT32 i32SlotIdx );
	virtual void				EquipDefaultParts( CUserSession* pSession, INT32 i32SlotIdx, UINT32 ui32Check );
	virtual BOOL				CheckReleaseEquip( CHAR_EQUIPMENT eSlot );
	virtual BOOL				CheckBuyRestrictionMedal(void);
	virtual BOOL				CheckBuyRestrictionTitle(void);
	virtual BOOL				GetCharaBonus(INT32 i32TeamIdx, CMultiSlot* pMultiSlot, REAL32& r32AddExpItem, REAL32& r32AddPotItem);
	virtual BOOL				GetSkinBonus(CMultiSlot* pMultiSlot, REAL32& r32AddExpItem, REAL32& r32AddPotItem);

	// 팀 구분이 가능한 시점이면( 룸 ) 반드시 i32Team을 전달해줘야 합니다. 
	virtual BOOL				SetMainCharBaseInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_BASE_INFO* pCharBaseInfo, INT32 i32Team = TEAM_RED);
	virtual BOOL				GetMainCharBaseInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_BASE_INFO* pCharBaseInfo, INT32 i32Team = TEAM_RED);
	virtual BOOL				GetMainCharEquipInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_EQUIP_INFO* pCharEquipInfo, INT32 i32Team = TEAM_RED);
	virtual BOOL				GetMainCharSkillInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_SKILL_INFO* pCharSkillInfo, INT32 i32Team = TEAM_RED);
	virtual BOOL				GetMainCharEquipmentAll(CMultiSlot* pMultiSlot, ITEM_INFO* pItemInfo, INT32 i32Team = TEAM_RED);
	virtual BOOL				GetMainCharEquipment(CMultiSlot* pMultiSlot, INT32 i32Equipment, ITEM_INFO* pItemInfo, INT32 i32Team = TEAM_RED);

	virtual BOOL				DeleteChara(CMultiSlot& MultiSlot, INT32 i32DeleteSlotIdx, INT32& i32MainSlotIdx);
};

#endif