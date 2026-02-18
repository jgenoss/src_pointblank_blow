#include "pch.h"
#include "Inventory.h"

CInventory::CInventory()
{
}

CInventory::~CInventory()
{
}

void CInventory::Reset()
{
	m_ui16InvenCount = 0;
	i3mem::FillZero( m_Inventory, sizeof(INVEN_BUFFER)*MAX_INVEN_COUNT);
}

void CInventory::InsertItem( INVEN_BUFFER* pInsertItem, INVEN_BUFFER_SET* pInvenSet)
{
	// 정상 구매 되지 않은 아이템은 인벤토리에 집어 넣지 않는다.
	if( ITEM_ATTR_TYPE_STAND_BY == pInsertItem->_ui8ItemType )				return;
	BOOL bNew = TRUE;
	INT32 i32BlankIdx = -1;
	for( INT32 i = 0 ; i < MAX_INVEN_COUNT ; i++ ) 
	{
		switch( pInsertItem->_ui8ItemType )
		{
		case ITEM_ATTR_TYPE_BUY:
		case ITEM_ATTR_TYPE_AUTH:
		case ITEM_ATTR_TYPE_BASIC:
		case ITEM_ATTR_TYPE_PC_1:
		case ITEM_ATTR_TYPE_PC_2:
		case ITEM_ATTR_TYPE_SPECIAL:
			{
				if( 0 == m_Inventory[ i ]._ui32ItemID ) 
				{	
					if( -1 == i32BlankIdx )
					{
						i32BlankIdx = i;
					}
				}
				else if( m_Inventory[ i ]._TItemWareDBIdx == pInsertItem->_TItemWareDBIdx )
				{
					i32BlankIdx = i;
					bNew = FALSE;
					break;
				}
			}
			break;
		}
	}


	if( -1 != i32BlankIdx )
	{
		if( pInvenSet )
		{
			i3mem::Copy( &pInvenSet->m_Inventory[ pInvenSet->m_ui16InvenCount ], pInsertItem, sizeof(INVEN_BUFFER)  );
			pInvenSet->m_ui16InvenCount++;
		}

		i3mem::Copy( &m_Inventory[ i32BlankIdx ], pInsertItem, sizeof(INVEN_BUFFER));
		if( bNew )
		{
			

			m_ui16InvenCount++;
		}
	}
}

UINT32 CInventory::DeleteItemByDBIdx( T_ItemDBIdx& TItemDBIdx, INVEN_BUFFER_SET* pInvenSet )
{
	UINT32 ui32DeleteItemID = 0;
	for( INT32 i = 0 ; i < m_ui16InvenCount ; i++ )
	{
		if( TItemDBIdx == m_Inventory[ i ]._TItemWareDBIdx )
		{
			ui32DeleteItemID = m_Inventory[ i ]._ui32ItemID;
			i3mem::Copy( &pInvenSet->m_Inventory[ pInvenSet->m_ui16InvenCount ], &m_Inventory[ i ], sizeof(INVEN_BUFFER)  );
			pInvenSet->m_Inventory[ pInvenSet->m_ui16InvenCount ]._ui32ItemID = 0;
			pInvenSet->m_ui16InvenCount++;

			m_ui16InvenCount--;

			// 바로 FillZero 시키지 않고 뒤에 있던 메모리를 복사해온다.
			i3mem::Copy( &m_Inventory[ i ], &m_Inventory[ m_ui16InvenCount ], sizeof( INVEN_BUFFER ) );
			i3mem::FillZero( &m_Inventory[ m_ui16InvenCount ], sizeof( INVEN_BUFFER ) );

			return ui32DeleteItemID;
		}
	}
	return ui32DeleteItemID;
}

UINT32 CInventory::DeleteItemByItemID( T_ItemID TItemID, INVEN_BUFFER_SET* pInvenSet )
{
	if( 0 == TItemID ) return 0;

	UINT32 ui32DeleteItemID = 0;
	for( INT32 i = 0 ; i < m_ui16InvenCount ; i++ )
	{
		if( TItemID == m_Inventory[ i ]._ui32ItemID )
		{
			ui32DeleteItemID = m_Inventory[ i ]._ui32ItemID;
			i3mem::Copy( &pInvenSet->m_Inventory[ pInvenSet->m_ui16InvenCount ], &m_Inventory[ i ], sizeof(INVEN_BUFFER)  );
			pInvenSet->m_Inventory[ pInvenSet->m_ui16InvenCount ]._ui32ItemID = 0;
			pInvenSet->m_ui16InvenCount++;

			m_ui16InvenCount--;

			// 바로 FillZero 시키지 않고 뒤에 있던 메모리를 복사해온다.
			i3mem::Copy( &m_Inventory[ i ], &m_Inventory[ m_ui16InvenCount ], sizeof( INVEN_BUFFER ) );
			i3mem::FillZero( &m_Inventory[ m_ui16InvenCount ], sizeof( INVEN_BUFFER ) );
			return ui32DeleteItemID;
		}
	}
	return ui32DeleteItemID;
}

void CInventory::SetItemArg( T_ItemDBIdx& TItemDBIdx, UINT32 ui32ItemArg, INVEN_BUFFER_SET* pInvenSet )
{
	for( INT32 i = 0 ; i < m_ui16InvenCount ; i++ )
	{
		if( TItemDBIdx == m_Inventory[ i ]._TItemWareDBIdx )
		{
			m_Inventory[ i ]._ui32ItemArg = ui32ItemArg;	// 내구도 MAX로 변경.
			i3mem::Copy( &pInvenSet->m_Inventory[ pInvenSet->m_ui16InvenCount ], &m_Inventory[ i ], sizeof(INVEN_BUFFER)  );
			pInvenSet->m_ui16InvenCount++;
		}
	}
}

void CInventory::GetInventory( UINT16* pui16InvenCount, INVEN_BUFFER* pInventory )
{
	*pui16InvenCount = m_ui16InvenCount;
	i3mem::Copy( pInventory, m_Inventory, sizeof(INVEN_BUFFER)*m_ui16InvenCount );
}

void CInventory::GetInventoryItemItemID( UINT16* pui16InvenCount, T_ItemID* TItemID )
{
	*pui16InvenCount = 0;

	for( INT32 i = 0 ; i < m_ui16InvenCount ; i++ )
	{
		if ( ITEM_TYPE_MAINTENANCE != GET_ITEM_TYPE(m_Inventory[i]._ui32ItemID)) continue;
		
		TItemID[ *pui16InvenCount ] = m_Inventory[i]._ui32ItemID;
		(*pui16InvenCount)++; 
	}
}

INVEN_BUFFER* CInventory::GetItemByWareDBIdx( T_ItemDBIdx& TWareDBIdx )
{
	for( INT32 i = 0 ; i < m_ui16InvenCount ; i++ )
	{
		if( TWareDBIdx == m_Inventory[i]._TItemWareDBIdx )
		{
			return &m_Inventory[i];
		}
	}
	
	return NULL;
}

INVEN_BUFFER* CInventory::GetItemByItemID( T_ItemID TItemID )
{
	for( INT32 i = 0 ; i < m_ui16InvenCount ; i++ )
	{
		if( TItemID == m_Inventory[i]._ui32ItemID )
		{
			return &m_Inventory[i];
		}
	}
	
	return NULL;
}

void CInventory::GetItemBonusInRoom( INT32 i32Hour, REAL32& r32AddExpItem, REAL32& r32AddPotItem, REAL32& r32AddSkillItem, UINT8& ui8ResultIcon )
{
	// 사용중인 증가권 확인
	for(INT32 itemIdx = 0; itemIdx < m_ui16InvenCount ; itemIdx++)
	{
		if (m_Inventory[itemIdx]._ui8ItemType != ITEM_ATTR_TYPE_AUTH)	continue;
		switch( m_Inventory[itemIdx]._ui32ItemID )
		{
		case CASHITEM_ITEMID_XP_PLUS_130:
		case CASHITEM_ITEMID_XP_PLUS_130_EV:
			r32AddExpItem		+= 0.30001f;
			ui8ResultIcon		|= RESULT_ICON_ITEM;
			break;
		case CASHITEM_ITEMID_PT_PLUS_130:
		case CASHITEM_ITEMID_PT_PLUS_130_EV:
			r32AddPotItem		+= 0.30001f;
			ui8ResultIcon		|= RESULT_ICON_ITEM;
			break;
		case CASHITEM_ITEMID_XP_PLUS_110:
			r32AddExpItem		+= 0.10001f;
			ui8ResultIcon		|= RESULT_ICON_ITEM;
			break;
		case CASHITEM_ITEMID_XP_PLUS_150:
			r32AddExpItem		+= 0.50001f;				
			ui8ResultIcon		|= RESULT_ICON_ITEM;
			break;
		case CASHITEM_ITEMID_XP_PLUS_200:
			r32AddExpItem		+= 1.00001f;				
			ui8ResultIcon		|= RESULT_ICON_ITEM;
			break;
		case CASHITEM_ITEMID_PT_PLUS_200:
			r32AddPotItem		+= 1.00001f;				
			ui8ResultIcon		|= RESULT_ICON_ITEM;
			break;
		case CASHITEM_ITEMID_PT_PLUS_150:
			r32AddPotItem		+= 0.50001f;
			ui8ResultIcon		|= RESULT_ICON_ITEM;
			break;
		case CASHITEM_ITEMID_CAPTAIN_ARMBAND:
			r32AddExpItem		+= 0.30001f;
			r32AddPotItem		+= 0.30001f;
			ui8ResultIcon		|= RESULT_ICON_ITEM;
			break;
		case CASHITEM_ITEMID_MAIN_SKILL_BOOSTER:
			r32AddSkillItem		+= 1.00001f;
			break;
			// 시간대별 보너스
			// 12 ~ 15
		case CASHITEM_ITEMID_XP_PLUS_X2_12PM:			// 12PM~3PM 경험치x2 
			if(  12 <= i32Hour && 15 > i32Hour )
			{
				r32AddExpItem	+= 1.00001f;				
				ui8ResultIcon	|= RESULT_ICON_ITEM;
			}
			break;
			// 15 ~ 18
		case CASHITEM_ITEMID_XP_PLUS_X2_3PM:			// 3PM~6PM 경험치x2 
			if( 15 <= i32Hour && 18 > i32Hour )
			{
				r32AddExpItem	+= 1.00001f;				
				ui8ResultIcon	|= RESULT_ICON_ITEM;
			}
			break;
			// 18 ~ 21
		case CASHITEM_ITEMID_XP_PLUS_X2_6PM:			// 6PM~9PM 경험치x2
			if( 18 <= i32Hour && 21 > i32Hour )
			{
				r32AddExpItem	+= 1.00001f;				
				ui8ResultIcon	|= RESULT_ICON_ITEM;
			}
			break;
			// 21 ~ 0
		case CASHITEM_ITEMID_XP_PLUS_X2_9PM:			// 9PM~12PM 경험치x2
			if( 21 <= i32Hour && 23 >= i32Hour )
			{
				r32AddExpItem	+= 1.00001f;				
				ui8ResultIcon	|= RESULT_ICON_ITEM;
			}
			break;
			// 12 ~ 15
		case CASHITEM_ITEMID_PT_PLUS_X2_12PM:			// 12PM~3PM 포인트x2
			if( 12 <= i32Hour && 15 > i32Hour )
			{
				r32AddPotItem	+= 1.00001f;				
				ui8ResultIcon	|= RESULT_ICON_ITEM;
			}
			break;
			// 15 ~ 18
		case CASHITEM_ITEMID_PT_PLUS_X2_3PM:			// 3PM~6PM 포인트x2
			if( 15 <= i32Hour && 18 > i32Hour )
			{
				r32AddPotItem	+= 1.00001f;				
				ui8ResultIcon	|= RESULT_ICON_ITEM;
			}
			break;
			// 18 ~ 21
		case CASHITEM_ITEMID_PT_PLUS_X2_6PM:			// 6PM~9PM 포인트x2
			if( 18 <= i32Hour && 21 > i32Hour )
			{
				r32AddPotItem	+= 1.00001f;				
				ui8ResultIcon	|= RESULT_ICON_ITEM;
			}
			break;
			// 21 ~ 0
		case CASHITEM_ITEMID_PT_PLUS_X2_9PM:			// 9PM~12PM 포인트x2
			if( 21 <= i32Hour && 23 >= i32Hour )
			{
				r32AddPotItem	+= 1.00001f;				
				ui8ResultIcon	|= RESULT_ICON_ITEM;
			}
			break;
			// 0 ~ 6
		case CASHITEM_ITEMID_XP_PLUS_200_0AM_6AM:		// 0AM~6AM 경험치x2
			if( 0 <= i32Hour && 6 > i32Hour )
			{
				r32AddExpItem	+= 1.00001f;
				ui8ResultIcon	|= RESULT_ICON_ITEM;
			}
			break;
			// 6 ~ 12
		case CASHITEM_ITEMID_XP_PLUS_200_6AM_12PM:		// 6AM~12PM 경험치x2
			if( 6 <= i32Hour && 12 > i32Hour )
			{
				r32AddExpItem	+= 1.00001f;
				ui8ResultIcon	|= RESULT_ICON_ITEM;
			}
			break;
		}
	}
}

UINT32 CInventory::GetUseItemInRoom()
{
	UINT32	ui32UseItem = 0;
	UINT32 ui32ValentineChocolateID		= 0xFFFFFFF0;
	UINT32 ui32ketupatID				= 0xFFFFFFF0;
	for( INT32 i = 0 ; i < m_ui16InvenCount ; i++ )
	{
		ui32ValentineChocolateID	= 1800101;
		ui32ketupatID				= 1800091;

		// 발렌타인 이벤트. Vallentine Chocolate
		if( m_Inventory[i]._ui32ItemID == ui32ValentineChocolateID )
		{	// Vallentine Chocolate만 예외 처리.
			ui32UseItem	|= SLOT_ITEM_VALENTINE_CHOCOLATE;
		}

		// Ketupat 아이템. 발렌타인 초콜렛과 중복 사용할 수 없다.
		if( m_Inventory[i]._ui32ItemID == ui32ketupatID )
		{
			ui32UseItem	|= SLOT_ITEM_KETUPAT;
		}
		
		if( ITEM_TYPE_MAINTENANCE != GET_ITEM_TYPE( m_Inventory[i]._ui32ItemID ) ) continue;

		switch( m_Inventory[i]._ui32ItemID )
		{
		case CASHITEM_ITEMID_AMMO_UP:
			ui32UseItem		|= SLOT_ITEM_AMMO_UP;
			break;
		case CASHITEM_ITEMID_GET_DROPPED_WEAPON:
		case CASHITEM_ITEMID_GET_DROPPED_WEAPON_EV:
			ui32UseItem		|= SLOT_ITEM_PICKUP_WEAPON;
			break;
		case CASHITEM_ITEMID_QUICK_CHANGE_WEAPON:
		case CASHITEM_ITEMID_QUICK_CHANGE_WEAPON_EV:
			ui32UseItem		|= SLOT_ITEM_QUICK_CHANGE_WEAPON;
			break;
		case CASHITEM_ITEMID_QUICK_CHANGE_MAGAZINE:
		case CASHITEM_ITEMID_QUICK_CHANGE_MAGAZINE_EV:
			ui32UseItem		|= SLOT_ITEM_QUICK_CHANGE_MAGAZINE;
			break;
		case CASHITEM_ITEMID_INCREASE_INVINCIBLE_TIME:
		case CASHITEM_ITEMID_INCREASE_INVINCIBLE_TIME_EV:
			ui32UseItem		|= SLOT_ITEM_INCREASE_INVINCIBLE_TIME;
			break;		
		case CASHITEM_ITEMID_FULLMETALJACKETAMMO:
		case CASHITEM_ITEMID_FULLMETALJACKETAMMO_EV:
			ui32UseItem		|= SLOT_ITEM_FULLMETALJACKETAMMO;
			break;
		case CASHITEM_ITEMID_HOLLOW_POINT_AMMO:
		case CASHITEM_ITEMID_HOLLOW_POINT_AMMO_EV:
			ui32UseItem		|= SLOT_ITEM_HOLLOW_POINT_AMMO;
			break;
		case CASHITEM_ITEMID_C4_SPEED_KIT:
			ui32UseItem		|= SLOT_ITEM_C4_SPEED_KIT;
			break;
		case CASHITEM_ITEMID_INCREASE_GRENADE_SLOT:
			ui32UseItem		|= SLOT_ITEM_INCREASE_GRENADE_SLOT;
			break;
		case CASHITEM_ITEMID_INCREASE_THR2_SLOT:
			ui32UseItem		|= SLOT_ITEM_INCREASE_THR2_SLOT;
			break;
		case CASHITEM_ITEMID_JACKETED_HELLOW_POINT_AMMO:
			ui32UseItem		|= SLOT_ITEM_JACKETED_HELLOW_POINT_AMMO;
			break;
		case CASHITEM_ITEMID_MEGA_HP5:
			ui32UseItem		|= SLOT_ITEM_MEGA_HP5;
			break;
		case CASHITEM_ITEMID_MEGA_HP10:
			ui32UseItem		|= SLOT_ITEM_MEGA_HP10;
			break;
		case CASHITEM_ITEMID_BULLET_PROOF_VEST:
			ui32UseItem		|= SLOT_ITEM_BULLET_PROOF_VEST;
			break;
		case CASHITEM_ITEMID_BULLET_PROOF_VEST_PLUS:
			ui32UseItem		|= SLOT_ITEM_BULLET_PROOF_VEST_PLUS;
			break;
		case CASHITEM_ITEMID_HOLLOW_POINT_AMMO_PLUS:
			ui32UseItem		|= SLOT_ITEM_HOLLOW_POINT_AMMO_PLUS;
			break;
		case CASHITEM_ITEMID_BULLET_PROOF_VEST_METAL:
			ui32UseItem		|= SLOT_ITEM_BULLET_PROOF_VEST_METAL;
			break;	
		case CASHITEM_ITEMID_BULLET_PROOF_VEST_GM:
			ui32UseItem		|= SLOT_ITEM_BULLET_PROOF_VEST_GM;
			break;
		case CASHITEM_ITEMID_SHORT_RESPAWN_20:
		case CASHITEM_ITEMID_SHORT_RESPAWN_20_EV:
			ui32UseItem		|= SLOT_ITEM_SHORT_RESPAWN_20;
			break;
		case CASHITEM_ITEMID_SHORT_RESPAWN	:
		case CASHITEM_ITEMID_SHORT_RESPAWN_30_EV:
			ui32UseItem		|= SLOT_ITEM_SHORT_RESPAWN_30;
			break;
		case CASHITEM_ITEMID_SHORT_RESPAWN_50:
			ui32UseItem		|= SLOT_ITEM_SHORT_RESPAWN_50;
			break;
		case CASHITEM_ITEMID_SHORT_RESPAWN_100:
			ui32UseItem		|= SLOT_ITEM_SHORT_RESPAWN_100;
			break;
		case CASHITEM_ITEMID_UPGRADE_REINFORCED:
			ui32UseItem		|= SLOT_ITEM_UPGRADE_REINFORCED;
			break;
		case CASHITEM_ITEMID_UPGRADE_DAMAGE:
			ui32UseItem		|= SLOT_ITEM_UPGRADE_DAMAGE;
			break;
		case CASHITEM_ITEMID_UPGRADE_DEFENSE:
			ui32UseItem		|= SLOT_ITEM_UPGRADE_DEFENSE;
			break;
		} 
	}

	if( SLOT_ITEM_MEGA_HP10 & ui32UseItem )					ui32UseItem &= ~SLOT_ITEM_MEGA_HP5;
	if( SLOT_ITEM_BULLET_PROOF_VEST_PLUS & ui32UseItem )	ui32UseItem &= ~SLOT_ITEM_BULLET_PROOF_VEST;
	if( SLOT_ITEM_HOLLOW_POINT_AMMO_PLUS & ui32UseItem )	ui32UseItem &= ~SLOT_ITEM_HOLLOW_POINT_AMMO;
	if( SLOT_ITEM_BULLET_PROOF_VEST_GM	 & ui32UseItem )	ui32UseItem &= ~(SLOT_ITEM_BULLET_PROOF_VEST_PLUS|SLOT_ITEM_BULLET_PROOF_VEST);
	if( SLOT_ITEM_VALENTINE_CHOCOLATE & ui32UseItem )		ui32UseItem &= ~SLOT_ITEM_KETUPAT;

	if( ui32UseItem & SLOT_ITEM_SHORT_RESPAWN_100 )			ui32UseItem &= ~ (SLOT_ITEM_SHORT_RESPAWN_50|SLOT_ITEM_SHORT_RESPAWN_30|SLOT_ITEM_SHORT_RESPAWN_20);
	else if( ui32UseItem & SLOT_ITEM_SHORT_RESPAWN_50 )		ui32UseItem &= ~ (SLOT_ITEM_SHORT_RESPAWN_30|SLOT_ITEM_SHORT_RESPAWN_20);
	else if( ui32UseItem & SLOT_ITEM_SHORT_RESPAWN_30 )		ui32UseItem &= ~ (SLOT_ITEM_SHORT_RESPAWN_20);
	return ui32UseItem;
}

BOOL		CInventory::CheckExistItemInvenByItemID(UINT32 ui32ItemID)
{
	//인벤토리 확인
	if (m_ui16InvenCount < MAX_INVEN_COUNT) return TRUE;

	BOOL bExistFlag = FALSE;
	for (INT32 i = 0; i < MAX_INVEN_COUNT; i++)
	{
		if (ui32ItemID != m_Inventory[i]._ui32ItemID) continue;

		bExistFlag = TRUE;
		break;
	}

	if (!bExistFlag) return FALSE; //인벤토리에 없는 아이템을 INSERT하는것이기에 실패리턴

	return TRUE;
}

