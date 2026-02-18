#ifndef __DESIGN_BASE_H__
#define __DESIGN_BASE_H__
 
class CDesignBase
{
private:
	

protected:

public:
	 
	CDesignBase();
	virtual ~CDesignBase();

	virtual BOOL				OnCreate(  );
	void						OnDestroy();

	virtual INT32				GetActiveTeamCount(void) = 0;
	virtual	BOOL				CheckItemPosition( T_ItemID TItemID, CHAR_EQUIPMENT eSlot )	{ return TRUE; }
	virtual	BOOL				CheckDuplicationParts(void)	 = 0;
	virtual	T_RESULT			CheckBuyChara( UINT8 ui8CreateType, CUserSession* pSession, T_GoodsID TGoodsID ) = 0;
	virtual BOOL				SetTeamSlot(CMultiSlot& MultiSlot, UINT8 ui8TeamSlot, T_ItemID TCharItemID ) = 0;
	virtual UINT32				GetExpValue(INT32 i32Rank) = 0;
	virtual INT32				GetCharaSlotByTeam(INT32 i32Team, CUserSession* pSession ) = 0;
	virtual BOOL				CheckItemSlot( ITEM_INFO* aParts, INT32 i32SlotIdx ) = 0;
	virtual void				EquipDefaultParts( CUserSession* pSession, INT32 i32SlotIdx, UINT32 ui32Check ) = 0;
	virtual BOOL				CheckReleaseEquip( CHAR_EQUIPMENT eSlot ) = 0;
	virtual BOOL				CheckBuyRestrictionMedal(void) = 0;
	virtual BOOL				CheckBuyRestrictionTitle(void) = 0;
	virtual BOOL				GetCharaBonus(INT32 i32TeamIdx, CMultiSlot* pMultiSlot, REAL32& r32AddExpItem, REAL32& r32AddPotItem) = 0;
	virtual BOOL				GetSkinBonus( CMultiSlot* pMultiSlot, REAL32& r32AddExpItem, REAL32& r32AddPotItem) = 0;

	// 팀 구분이 가능한 시점이면( 룸 ) 반드시 i32Team을 전달해줘야 합니다. 
	virtual BOOL				SetMainCharBaseInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_BASE_INFO* pCharBaseInfo, INT32 i32Team = TEAM_RED) = 0;
	virtual BOOL				GetMainCharBaseInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_BASE_INFO* pCharBaseInfo, INT32 i32Team = TEAM_RED) = 0;
	virtual BOOL				GetMainCharEquipInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_EQUIP_INFO* pCharEquipInfo, INT32 i32Team = TEAM_RED) = 0;
	virtual BOOL				GetMainCharSkillInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_SKILL_INFO* pCharSkillInfo, INT32 i32Team = TEAM_RED) = 0;
	virtual BOOL				GetMainCharEquipmentAll(CMultiSlot* pMultiSlot, ITEM_INFO* pItemInfo, INT32 i32Team = TEAM_RED) = 0;
	virtual BOOL				GetMainCharEquipment(CMultiSlot* pMultiSlot, INT32 i32Equipment, ITEM_INFO* pItemInfo, INT32 i32Team = TEAM_RED) = 0;

	virtual BOOL				DeleteChara(CMultiSlot& MultiSlot, INT32 i32DeleteSlotIdx, INT32& i32MainSlotIdx) = 0;
};

#endif