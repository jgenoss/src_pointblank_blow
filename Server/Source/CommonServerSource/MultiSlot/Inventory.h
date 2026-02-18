#ifndef __INVENTORY_H__
#define __INVENTORY_H__

class CInventory
{
	// Inven 정보
	UINT16					m_ui16InvenCount;
	INVEN_BUFFER			m_Inventory[MAX_INVEN_COUNT];		// Inventory Item

public:
	CInventory();
	~CInventory();

	void					Reset();

	void					InsertItem( INVEN_BUFFER* pInvenItem, INVEN_BUFFER_SET* pInvenSet );
	UINT32					DeleteItemByDBIdx( T_ItemDBIdx& TItemDBIdx, INVEN_BUFFER_SET* pInvenSet );
	UINT32					DeleteItemByItemID( T_ItemID TItemID, INVEN_BUFFER_SET* pInvenSet );

	void					SetItemArg( T_ItemDBIdx& TItemDBIdx, UINT32 ui32ItemArg, INVEN_BUFFER_SET* pInvenSet );

	void					GetInventory( UINT16* pui16InvenCount, INVEN_BUFFER* pInventory );
	void					GetInventoryItemItemID( UINT16* pui16InvenCount, T_ItemID* TItemID );	// 기간제 강화 아이템 리스트

	INVEN_BUFFER*			GetItemByWareDBIdx( T_ItemDBIdx& TWareDBIdx );																			// DBIdx 로 인벤 얻어오기
	INVEN_BUFFER*			GetItemByItemID( T_ItemID TItemID );																					// ItemID로 인벤 얻어오기

	void					GetItemBonusInRoom( INT32 i32Hour, REAL32& r32AddExpItem, REAL32& r32AddPotItem, REAL32& r32AddSkillItem, UINT8& ui8ResultIcon );		// 방에서 사용되는 보너스에 대한 처리
	UINT32					GetUseItemInRoom();																										// 방에서 사용되는 아이템에 대한 처리	
	UINT16					GetInventoryCount() { return m_ui16InvenCount; }
	BOOL					CheckExistItemInvenByItemID(UINT32 ui32ItemID);

};

#endif