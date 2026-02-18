#include "pch.h"			
#include "ItemTypeCharaClass.h"

ENUM_USE_CHARA_TYPE	GetCharaType(T_ItemID CharaItemID)
{
	for(INT32 Loop = 0; Loop < ENUM_USE_MAN_COUNT; Loop++)
	{
		if(g_UseManChara[Loop] == CharaItemID)
		{
			return ENUM_USE_MAN;
		}
	}
	for(INT32 Loop = 0; Loop < ENUM_USE_MAN_COUNT; Loop++)
	{
		if(g_UseWoManChara[Loop] == CharaItemID)
		{
			return ENUM_USE_WOMAN;
		}
	}

	return ENUM_USE_NONE;
}

bool CmpUsePartItemID( INT32 ItemCnt, const T_ItemID CmpItemID[][MAX_USE_PARTS_TYPE], T_ItemID CharaItemID, T_ItemID ItemID )
{
	for(INT32 Loop = 0; Loop < ItemCnt; Loop++)
	{
		if( ( CmpItemID[Loop][ENUM_ITEMID]) == ItemID )
		{
			UINT32 Idx = GET_ITEM_NUMBER(CharaItemID);
			if( CmpItemID[Loop][ENUM_CHARA_RES_ID] == Idx )	{ return true; }
			else if(CHARA_RES_ID_HUMAN_S == CmpItemID[Loop][ENUM_CHARA_RES_ID])
			{
				switch( CmpItemID[Loop][ENUM_CHARA_TYPE] )
				{
				case ENUM_USE_MAN:		{ for(INT32 Loop2 = 0; Loop2 < ENUM_USE_MAN_COUNT; Loop2++)	{ if( g_UseManChara[Loop2]	== CharaItemID )	{ return true; } } } break;
				case ENUM_USE_WOMAN:	{ for(INT32 Loop2 = 0; Loop2 < ENUM_USE_WOMAN_COUNT; Loop2++)	{ if( g_UseWoManChara[Loop2]	== CharaItemID )	{ return true; } } } break;
				case ENUM_USE_NONE:		{ return true; } break;
				default:				{ return false; } break;
				}
			}
			return false;
		}
	}

	return true;
}

bool CmpUsePartGoodsID( INT32 ItemCnt, const T_ItemID CmpItemID[][MAX_USE_PARTS_TYPE], T_ItemID CharaItemID, TGoodsID GoodsID )
{
	for(INT32 Loop = 0; Loop < ItemCnt; Loop++)
	{
		if( CmpItemID[Loop][ENUM_GOODSID] == GET_GOODS_FLAG_MATCHING(GoodsID) )
		{
			UINT32 Idx = GET_ITEM_NUMBER(CharaItemID);
			if( CmpItemID[Loop][ENUM_CHARA_RES_ID] == Idx )	{ return true; }
			else if(CHARA_RES_ID_HUMAN_S == CmpItemID[Loop][ENUM_CHARA_RES_ID])
			{
				switch( CmpItemID[Loop][ENUM_CHARA_TYPE] )
				{
				case ENUM_USE_MAN:		{ for(INT32 Loop2 = 0; Loop2 < ENUM_USE_MAN_COUNT; Loop2++)	{ if( g_UseManChara[Loop2]	== CharaItemID )	{ return true; } } } break;
				case ENUM_USE_WOMAN:	{ for(INT32 Loop2 = 0; Loop2 < ENUM_USE_WOMAN_COUNT; Loop2++)	{ if( g_UseWoManChara[Loop2]	== CharaItemID )	{ return true; } } } break;
				case ENUM_USE_NONE:		{ return true; } break;
				default:				{ return false; } break;
				}
			}
			return false;
		}
	}

	return true;
}

bool g_CheckUsePartItemID(T_ItemID CharaItemID, T_ItemID ItemID)
{
	ITEM_TYPE ItemType = (ITEM_TYPE) GET_ITEM_TYPE(ItemID);

	switch(ItemType)
	{
	case ITEM_TYPE_HEADGEAR:	{ return CmpUsePartItemID( ENUM_HEAD_GEAR_COUNT,	g_UseHeadGear,	CharaItemID, ItemID ); }	break;
	case ITEM_TYPE_FACEGEAR:	{ return CmpUsePartItemID( ENUM_FACE_GEAR_COUNT,	g_UseFaceGear,	CharaItemID, ItemID ); }	break;
	case ITEM_TYPE_UPPER:		{ return CmpUsePartItemID( ENUM_UPPER_COUNT,		g_UseUpper,		CharaItemID, ItemID ); }	break;
	case ITEM_TYPE_LOWER:		{ return CmpUsePartItemID( ENUM_LOWER_COUNT,		g_UseLower,		CharaItemID, ItemID ); }	break;
	case ITEM_TYPE_GLOVE:		{ return CmpUsePartItemID( ENUM_GLOVE_COUNT,		g_UseGlove,		CharaItemID, ItemID ); }	break;
	case ITEM_TYPE_BELT:		{ return CmpUsePartItemID( ENUM_BELT_COUNT,			g_UseBelt,		CharaItemID, ItemID ); }	break;
	case ITEM_TYPE_HOLSTER:		{ return CmpUsePartItemID( ENUM_HOLSTER_COUNT,		g_UseHolster,	CharaItemID, ItemID ); }	break;
	default:					{ return true; } break;
	}
}

bool g_CheckUsePartGoodsID(T_ItemID CharaItemID, TGoodsID GoodsID)
{
	ITEM_TYPE ItemType = (ITEM_TYPE) GET_GOODS_FLAG_TYPE(GoodsID);

	switch(ItemType)
	{
	case ITEM_TYPE_HEADGEAR:		{ return CmpUsePartGoodsID( ENUM_HEAD_GEAR_COUNT,	g_UseHeadGear,	CharaItemID, GoodsID ); }	break;
	case ITEM_TYPE_FACEGEAR:		{ return CmpUsePartGoodsID( ENUM_FACE_GEAR_COUNT,	g_UseFaceGear,	CharaItemID, GoodsID ); }	break;
	case ITEM_TYPE_UPPER:			{ return CmpUsePartGoodsID( ENUM_UPPER_COUNT,		g_UseUpper,		CharaItemID, GoodsID ); }	break;
	case ITEM_TYPE_LOWER:			{ return CmpUsePartGoodsID( ENUM_LOWER_COUNT,		g_UseLower,		CharaItemID, GoodsID ); }	break;
	case ITEM_TYPE_GLOVE:			{ return CmpUsePartGoodsID( ENUM_GLOVE_COUNT,		g_UseGlove,		CharaItemID, GoodsID ); }	break;
	case ITEM_TYPE_BELT:			{ return CmpUsePartGoodsID( ENUM_BELT_COUNT,		g_UseBelt,		CharaItemID, GoodsID ); }	break;
	case ITEM_TYPE_HOLSTER:			{ return CmpUsePartGoodsID( ENUM_HOLSTER_COUNT,		g_UseHolster,	CharaItemID, GoodsID ); }	break;
	default:						{ return true; } break;
	}								
}

INT32 GetCharaTeamType(INT32 i32ResNum)
{
	switch(i32ResNum)
	{
	case CHARA_RES_ID_ACID_POLE:
	case CHARA_RES_ID_SWAT_FEMALE:
	case CHARA_RES_ID_LEOPARD:
	case CHARA_RES_ID_HIDE:
	case CHARA_RES_ID_RF_LEOPARD:
	case CHARA_RES_ID_RF_HIDE:
	case CHARA_RES_ID_RFC_LEOPARD:
	case CHARA_RES_ID_RFC_HIDE:
	case CHARA_RES_ID_RF_ACID_POLE:
	case CHARA_RES_ID_RF_SWAT_FEMALE:
	case CHARA_RES_ID_EXP20_LEOPARD:
	case CHARA_RES_ID_POINT30_HIDE:
	case CHARA_RES_ID_CHOU:
	case CHARA_RES_ID_RF_CHOU:
		return 2;
	default : 
		return 1;
	}
}


WEAPON_SLOT_TYPE	GetWeaponSlot_WeaponItemIDFunc(T_ItemID WeaponItemID)
{
	switch( GET_ITEM_TYPE( WeaponItemID ) )
	{
	case ITEM_TYPE_PRIMARY:			return WEAPON_SLOT_PRIMARY;
	case ITEM_TYPE_SECONDARY:		return WEAPON_SLOT_SECONDARY;	
	case ITEM_TYPE_MELEE:			return WEAPON_SLOT_MELEE;
	case ITEM_TYPE_THROWING1:		return WEAPON_SLOT_THROWING1;
	case ITEM_TYPE_THROWING2:		return WEAPON_SLOT_THROWING2;
	case ITEM_TYPE_MISSION:			return WEAPON_SLOT_MISSION;
	default:						return WEAPON_SLOT_UNKNOWN;
	}
}

ITEM_TYPE	GetItemType_WeaponSlotTypeFunc(WEAPON_SLOT_TYPE WeaponSlotType)
{
	switch( WeaponSlotType )
	{
	case WEAPON_SLOT_PRIMARY:		return ITEM_TYPE_PRIMARY;
	case WEAPON_SLOT_SECONDARY:		return ITEM_TYPE_SECONDARY;
	case WEAPON_SLOT_MELEE:			return ITEM_TYPE_MELEE;
	case WEAPON_SLOT_THROWING1:		return ITEM_TYPE_THROWING1;
	case WEAPON_SLOT_THROWING2:		return ITEM_TYPE_THROWING2;
	case WEAPON_SLOT_MISSION:		return ITEM_TYPE_MISSION;
	default:						return ITEM_TYPE_UNKNOWN;
	}
}