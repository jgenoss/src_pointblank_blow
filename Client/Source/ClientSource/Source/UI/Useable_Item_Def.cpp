#include "pch.h"
#include "Useable_Item_Def.h"

namespace inner
{
	const char* method_Keywords[MAX_USE_METHOD] =
	{
		_T("DefaultAuth"),
		_T("DisguiseNick"),
		_T("DisguiseRank"),
		_T("ColorNick"),
		_T("ColorClan"),
		_T("ColorCrossHair"),
		_T("ChangeNick"),
		_T("ChangeClanName"),
		_T("ChangeClanMark"),
		_T("BattleFieldMove"),
		_T("ColorFireMuzzle"),
	};

	const wchar_t * result_Keywords[MAX_RESULT_METHOD] =
	{
		L"ColorNick",
		L"ColorClan",
		L"Nick",
		L"Rank",
		L"ColorCrossHair",
		L"ClanName",
		L"ClanMark",
		L"ClanPersonPlus",
		L"GrabBag",
		L"Point",
		L"ColorFireMuzzle",
	};
}

ITEM_USE_METHOD use_item::UseMethodStringToEnum(T_ItemID item_id)
{
	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(item_id);
	if( pInfo == 0 ) return USE_METHOD_UNKNOWN;

	const i3::rc_wstring& wstrString = pInfo->GetUseMethod();

	for (INT32 i = 0 ; i < MAX_USE_METHOD ; ++i )
	{
		if ( i3::generic_is_iequal( inner::method_Keywords[i], wstrString) )
			return static_cast<ITEM_USE_METHOD>(i);
	}

	return USE_METHOD_UNKNOWN;
}

ITEM_RESULT_METHOD	use_item::ResultMethodStringToEnum( T_ItemID item_id)
{
	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(item_id);
	if( pInfo == 0 ) return RESULT_METHOD_UNKNOWN;

	const i3::rc_wstring& wstrString = pInfo->GetResultMethod();

	for (INT32 i = 0 ; i < MAX_RESULT_METHOD ; ++i )
	{
		if ( i3::generic_is_iequal(inner::result_Keywords[i], wstrString) )
			return static_cast<ITEM_RESULT_METHOD>(i);
	}

	return RESULT_METHOD_UNKNOWN;
}
