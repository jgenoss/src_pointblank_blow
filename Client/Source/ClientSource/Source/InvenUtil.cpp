#include "pch.h"
#include "InvenUtil.h"
#include "InvenDB.h"
#include "InvenList.h"
#include "ui/UIItemInfoUtil.h"

bool inven::GetItemTypeArg( T_ItemID ItemID, UINT8* attr, UINT32* arg)
{
	return GetItemTypeArg(ItemID, 0, attr, arg);
}

bool inven::GetItemTypeArg( T_ItemID ItemID, T_ItemDBIdx ware_idx, UINT8* attr, UINT32* arg)
{
	bool ret = false;

	switch( item_def::get_category_tab(ItemID) )
	{
	case item_def::CTG_WEAPON:	ret = CInvenList::i()->GetItemTypeByDBIdx( INVEN::DB_WEAPON, ware_idx, attr, arg );		break;
	case item_def::CTG_CHARA:	
		{
			// 공룡은 Cash DB List에서 검사합니다.
			if( item_def::get_type(ItemID) != ITEM_TYPE_DINO )
				ret = CInvenList::i()->GetItemTypeByDBIdx( INVEN::DB_CHARA, ware_idx, attr, arg );
			else
				ret = CInvenList::i()->GetItemTypeByDBIdx( INVEN::DB_CASHITEM, ware_idx, attr, arg );
		}
		break;
	case item_def::CTG_ITEM:	ret = CInvenList::i()->GetItemTypeByDBIdx( INVEN::DB_CASHITEM, ware_idx, attr, arg );		break;
	case item_def::CTG_PARTS: 
		if(ware_idx == 0) return false;
		ret = CInvenList::i()->GetItemTypeByDBIdx( INVEN::DB_PARTS, ware_idx, attr, arg );	
		break;
	case item_def::CTG_SKIN :
		if( ware_idx == 0) return false;
		ret = CInvenList::i()->GetItemTypeByDBIdx( INVEN::DB_SKIN, ware_idx, attr, arg );
		break;
	default:
		if (attr) *attr = 0;
		if (arg) *arg = 0;
		break;
	}

	return ret;
}

bool inven::is_valid_item( T_ItemID ItemID)
{
	bool ret = false;

	switch( item_def::get_category(ItemID) )
	{
	case item_def::CTG_WEAPON:	ret = CInvenList::i()->IsValidItem( INVEN::DB_WEAPON, ItemID);		break;
	case item_def::CTG_CHARA:	ret = CInvenList::i()->IsValidItem( INVEN::DB_CHARA, ItemID);		break;
	case item_def::CTG_ITEM:	ret = CInvenList::i()->IsValidItem( INVEN::DB_CASHITEM, ItemID);	break;
	case item_def::CTG_PARTS:	ret = CInvenList::i()->IsValidItem( INVEN::DB_PARTS, ItemID );		break;
	case item_def::CTG_SKIN :	ret = CInvenList::i()->IsValidItem( INVEN::DB_SKIN, ItemID);		break;
	default :
		I3ASSERT_0;	// 코드 작성 필요
		break;
	}

	return ret;
}

bool inven::is_valid_item( T_ItemID ItemID, T_ItemDBIdx ware_idx)
{
	bool ret = false;

	switch( item_def::get_category(ItemID) )
	{
	case item_def::CTG_WEAPON:	ret = CInvenList::i()->IsValidItem( INVEN::DB_WEAPON, ware_idx);	break;
	case item_def::CTG_CHARA:	ret = CInvenList::i()->IsValidItem( INVEN::DB_CHARA, ware_idx);	break;
	case item_def::CTG_ITEM:	ret = CInvenList::i()->IsValidItem( INVEN::DB_CASHITEM, ware_idx);	break;
	case item_def::CTG_PARTS:	ret = CInvenList::i()->IsValidItem( INVEN::DB_PARTS, ware_idx );	break;	
	case item_def::CTG_SKIN :	ret = CInvenList::i()->IsValidItem( INVEN::DB_SKIN, ware_idx );	break;
	default :
		I3ASSERT_0;	// 코드 작성 필요
		break;
	}

	return ret;
}

