#include "pch.h"
#include "ShopItemInfo.h"

#include "GlobalVariables.h"
#include "GlobalFunc.h"

#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/ColorString.h"

PURCHASE_AUTHORITY	PurchaseAuthorityToEnum(const i3::rc_wstring& Value)
{
	typedef	i3::pair<const wchar_t*, PURCHASE_AUTHORITY>		PurchaseAuthorityValue;
	static PurchaseAuthorityValue	Keywords[] =
	{
		PurchaseAuthorityValue(L"",				PURCHASE_AUTH_ANYONE),
		PurchaseAuthorityValue(L"Anyone",		PURCHASE_AUTH_ANYONE),
		PurchaseAuthorityValue(L"ClanMaster",	PURCHASE_AUTH_CLAN_MASTER),
	};

	INT32 Max = GET_ARRAY_COUNT(Keywords);
	for ( INT32 i = 0 ; i < Max ; ++i )
	{
		if ( i3::generic_is_iequal(Value, Keywords[i].first) )
		{
			return Keywords[i].second;
		}
	}

	return PURCHASE_AUTH_ANYONE;
};



USE_POSITION UsePositionToEnum(const i3::rc_wstring & Value)
{
	typedef	std::pair<const wchar_t *, USE_POSITION>		UsePositionValue;
	static UsePositionValue		Keywords[] =
	{
		UsePositionValue(L"",				USE_POS_ANYWHERE),
		UsePositionValue(L"Anywhere",		USE_POS_ANYWHERE),
		UsePositionValue(L"Game",			USE_POS_GAME),
		UsePositionValue(L"ClanManagement",	USE_POS_CLAN_MANAGEMENT),
		UsePositionValue(L"ReadyRoom",		USE_POS_READYROOM),
		UsePositionValue(L"Chatting",		USE_POS_CHATTING),
	};

	INT32 Max = GET_ARRAY_COUNT(Keywords);
	for ( INT32 i = 0 ; i < Max ; ++i )
	{
		if ( i3::generic_is_iequal(Value, Keywords[i].first) )
		{
			return Keywords[i].second;
		}
	}

	return USE_POS_ANYWHERE;
};



I3_CLASS_INSTANCE(ShopBaseInfo);//, i3NamedElement);
I3_CLASS_INSTANCE(ShopItemInfo);//, ShopBaseInfo);
I3_CLASS_INSTANCE(ShopPackageGoodsInfo);//, ShopBaseInfo);
I3_CLASS_INSTANCE(DomiShopItemInfo);
I3_CLASS_INSTANCE(DomiSpItemInfo);
I3_CLASS_INSTANCE(DomiCashItemInfo);
I3_CLASS_INSTANCE(CUITemplateInfo_Item);
I3_CLASS_INSTANCE(ShopItemInfoDataBase);//, i3GameNode);

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Capsule
{
	Capsule::PrizeInfo* read_info(i3RegKey* pKey)
	{
		INT32	PrizeCount = 0;
		FIND_REG_DATA( pKey, "_Count", &PrizeCount);
		if( PrizeCount == 0 ) return 0;

		Capsule::PrizeInfo* capsule_info = new Capsule::PrizeInfo;
		MEMDUMP_NEW( capsule_info, sizeof(Capsule::PrizeInfo));

		i3::rc_wstring No_Name_Capsule_Item(L"No Name : Capsule Item");

		for ( INT32 i = 0 ; i < PrizeCount ; ++i )
		{
			char KeyString[MAX_STRING_COUNT] = {};
			i3::snprintf(KeyString, MAX_STRING_COUNT, "_Name%d", i);

			i3RegData * pData = i3Registry::FindData( pKey, KeyString);
			if( pData != nullptr)
			{
				i3::rc_wstring	wstrPrizeItemName;
				GET_REG_DATA(pData, wstrPrizeItemName);
				capsule_info->prize_string_list.push_back(wstrPrizeItemName);
			}
			else
			{
				capsule_info->prize_string_list.push_back( No_Name_Capsule_Item );
			}
		}

		return capsule_info;
	}
}

//------------------------------------////------------------------------------//
//ShopBaseInfo
ShopBaseInfo::ShopBaseInfo(void) : pCapsulePrizeInfo(0), m_isCouponType(false)
{
}
ShopBaseInfo::~ShopBaseInfo( void)
{
	I3_SAFE_DELETE( pCapsulePrizeInfo);
}

void ShopBaseInfo::ReadInfo( i3RegKey * pKey)
{
	if( pKey == nullptr ) return;
	
	const char * pszName = pKey->GetName();
	if(pszName == 0) return;

	SetName( pszName );

	GetKeyDataString( pKey, "_Name", m_Name);
	if(m_Name.empty() == true)
	{ m_Name = L"No Name"; ::ColorString(m_Name, 255, 0, 0); }

	GetKeyDataString( pKey, "Description", m_Description);
	if(m_Description.empty() == true)
	{ m_Description = L"No Description"; ::ColorString(m_Description, 255, 0, 0); }

	i3RegKey * pKeyPrizeItem = Config::FindKey("_PrizeItem", pKey);
	if( pKeyPrizeItem != nullptr )
	{
		pCapsulePrizeInfo = Capsule::read_info(pKeyPrizeItem); // 0값이 반환되어도 상관없다.
	}

	FIND_REG_DATA(pKey, "_IsCoupon", &m_isCouponType);

	ReadInfo_impl(pKey);
}

i3::rc_wstring ShopBaseInfo::GetCapsuleItemResultName(INT32 Idx) const
{
	if( pCapsulePrizeInfo == 0 ) return i3::rc_wstring();

	if ( Idx < 0 || Idx >= (INT32)pCapsulePrizeInfo->prize_string_list.size() )
		return L"Out of range : Capsule Item";

	return pCapsulePrizeInfo->prize_string_list[Idx];
}

//------------------------------------////------------------------------------//
//ShopItemInfo
ShopItemInfo::ShopItemInfo(void)
{
	m_Type = ITEM_TYPE_UNKNOWN;

	m_UITextureIndex	= -1;
	m_UIShapeIndex		= -1;
	m_PurchaseAuthority	= PURCHASE_AUTH_ANYONE;
	m_UsePosition		= USE_POS_ANYWHERE;
	m_ViewOtherUser		= 0;

	m_Catagory = 0;
	m_GroupIndex = 0;
}

ShopItemInfo::~ShopItemInfo( void)
{
}

void ShopItemInfo::ReadInfo_impl( i3RegKey * pKey)
{
	FIND_REG_DATA( pKey, "_Type",					(INT32*)&m_Type);
	FIND_REG_DATA( pKey, "_GroupIndex",				&m_GroupIndex);
	FIND_REG_DATA( pKey, "_UITexIndex",				&m_UITextureIndex);
	FIND_REG_DATA( pKey, "_UIShapeIndex",			&m_UIShapeIndex);

	GetKeyDataString( pKey, "ResultStatic", m_ResultStatic);
	GetKeyDataString( pKey, "ResultValue", m_ResultValue);
	GetKeyDataString( pKey, "ResultNotice",m_ResultNotice);

	GetKeyDataString( pKey, "_ResultMethod", m_ResultMethod);
	GetKeyDataString( pKey, "_UseMethod", m_UseMethod);

	i3::rc_wstring	Data;
	GetKeyDataString( pKey, "PurchaseAuthority", Data);
	m_PurchaseAuthority		= PurchaseAuthorityToEnum(Data);
	GetKeyDataString( pKey, "PurchaseAuthorityDesc", m_PurchaseAuthorityDesc);
	GetKeyDataString( pKey, "CertificationAuthDesc", m_CertificationAuthDesc);

	Data.clear();
	GetKeyDataString( pKey, "UsePosition", Data);

	m_UsePosition		= UsePositionToEnum(Data);
	GetKeyDataString( pKey, "UsePositionDesc",  m_UsePositionDesc);

	bool ViewOtherUser = 0;
	FIND_REG_DATA( pKey, "ViewOtherUser",			&ViewOtherUser);
	m_ViewOtherUser = ViewOtherUser ? true : false;

	FIND_REG_DATA(pKey, "_Catagory",	&m_Catagory);
}

//------------------------------------////------------------------------------//
//ShopPackageGoodsInfo
ShopPackageGoodsInfo::ShopPackageGoodsInfo(void)
{
	m_GoodsID = 0;
	m_Catagory = 0;
	m_ShapeIndex = 0;
	m_TexIndex = 0;
}

ShopPackageGoodsInfo::~ShopPackageGoodsInfo(void)
{
}

void ShopPackageGoodsInfo::ReadInfo_impl(i3RegKey* pKey)
{
	GetKeyDataString( pKey, "_UITexType", m_TexType);

	FIND_REG_DATA(pKey, "_GoodsId",	&m_GoodsID);
	FIND_REG_DATA(pKey, "_UIShapeIndex",	&m_ShapeIndex);
	FIND_REG_DATA(pKey, "_UITexIndex",	&m_TexIndex);
	FIND_REG_DATA(pKey, "_Catagory",	&m_Catagory);
}


//------------------------------------////------------------------------------//
//DomiShopItemInfo
void DomiShopItemInfo::ReadInfo_impl( i3RegKey * pKey)
{
	ShopItemInfo::ReadInfo_impl( pKey);

	m_DisplayIndex = -1;
	FIND_REG_DATA(pKey, "DisplayIdx", &m_DisplayIndex);

	m_StartRound = 0;
	FIND_REG_DATA( pKey, "StartRound", &m_StartRound);

	m_ItemType = -1;
	FIND_REG_DATA( pKey, "ItemType", &m_ItemType);
	if( m_ItemType == Resurrect_ItemType )
	{
		g_pShopItemInfoDataBase->m_Insg_Domi_ItemInfo_List.push_back(this);
	}
}

//------------------------------------////------------------------------------//
//DomiSpItemInfo
void DomiSpItemInfo::ReadInfo_impl( i3RegKey * pKey)
{
	m_DisplayIndex = -1;
	FIND_REG_DATA(pKey, "DisplayIdx", &m_DisplayIndex);

	m_ShapeIndex = -1;
	FIND_REG_DATA(pKey, "_UIShapeIndex", &m_ShapeIndex);
}

//------------------------------------////------------------------------------//
//DomiCashItemInfo
void DomiCashItemInfo::ReadInfo_impl( i3RegKey * pKey)
{
	m_DisplayIndex = -1;
	FIND_REG_DATA(pKey, "DisplayIdx", &m_DisplayIndex);
	m_ShapeIndex = -1;
	FIND_REG_DATA(pKey, "_UIShapeIndex", &m_ShapeIndex);
	m_GroupIndex = -1;
	FIND_REG_DATA( pKey, "_GroupIndex", &m_GroupIndex);

	m_Sp_Increase = 0;
	FIND_REG_DATA( pKey, "SP_Increase", &m_Sp_Increase);
	m_MinRemain = 0;
	FIND_REG_DATA( pKey, "MinRemain", &m_MinRemain);

	enum { SpCash_ItemType = 0, };

	m_ItemType = -1;
	FIND_REG_DATA( pKey, "ItemType", &m_ItemType);
	if( m_ItemType == SpCash_ItemType )
	{
		g_pShopItemInfoDataBase->m_Insg_Domi_Cash_ItemInfo_List.push_back(this);
	}
}

//------------------------------------////------------------------------------//
CUITemplateInfo_Item::CUITemplateInfo_Item() : m_type(-1) {}
CUITemplateInfo_Item::~CUITemplateInfo_Item() {}

INT32 CUITemplateInfo_Item::GetType() const
{
	return m_type;
}

const i3::rc_wstring& CUITemplateInfo_Item::GetName() const
{
	return m_name;
}

void CUITemplateInfo_Item::ReadInfo_impl(i3RegKey* rk)
{
	FIND_REG_DATA(rk, "Type", &m_type);
	FIND_REG_DATA(rk, "Name", m_name);
}


//------------------------------------////------------------------------------//
ShopItemInfoDataBase::ShopItemInfoDataBase()
{
}

ShopItemInfoDataBase::~ShopItemInfoDataBase()
{
	i3::cu::for_each_safe_release_clear(m_InfoList[DBT_ShopItem], &ShopItemInfo::Release);
	i3::cu::for_each_safe_release_clear(m_InfoList[DBT_SetItem], &ShopPackageGoodsInfo::Release);
	i3::cu::for_each_safe_release_clear(m_InfoList[DBT_Domi_Item], &DomiShopItemInfo::Release);
	i3::cu::for_each_safe_release_clear(m_InfoList[DBT_Domi_Cashltem], &DomiCashItemInfo::Release);
	i3::cu::for_each_safe_release_clear(m_InfoList[DBT_Domi_Spltem], &DomiSpItemInfo::Release);
	i3::cu::for_each_safe_release_clear(m_InfoList[DBT_UITemplate], &CUITemplateInfo_Item::Release);
}

namespace inner
{
	template <typename db_class_name>
	bool read_item_info(const char* pef_name, i3::vector<ShopBaseInfo*> & con)
	{
		i3RegKey * pRoot = Config::_GetRegRoot(pef_name);
		if( pRoot == nullptr) return false;

		INT32 count = pRoot->getChildCount();
		for( INT32 i = 0; i < count; ++i)
		{
			i3RegKey * pKey = ( i3RegKey*)pRoot->getChild( i);

			db_class_name* pInfo = db_class_name::new_object();
			pInfo->ReadInfo( pKey );

			con.push_back( pInfo);
		}
		return true;
	}
}

bool ShopItemInfoDataBase::Create()
{
	bool is_loaded;

	m_InfoList[DBT_ShopItem].clear();
	is_loaded = inner::read_item_info<ShopItemInfo>("Goods/ShopItem", m_InfoList[DBT_ShopItem]);
	if( is_loaded == false) return false;

	m_InfoList[DBT_SetItem].clear();
	is_loaded = inner::read_item_info<ShopPackageGoodsInfo>("Goods/SetGoods", m_InfoList[DBT_SetItem]);
	if( is_loaded == false) return false;

	m_InfoList[DBT_Domi_Item].clear();
	is_loaded = inner::read_item_info<DomiShopItemInfo>("Goods/DomiItem", m_InfoList[DBT_Domi_Item]);
	if( is_loaded == false) return false;

	m_InfoList[DBT_Domi_Cashltem].clear();
	is_loaded = inner::read_item_info<DomiCashItemInfo>("Goods/DomiCashItem", m_InfoList[DBT_Domi_Cashltem]);
	if( is_loaded == false) return false;

	m_InfoList[DBT_Domi_Spltem].clear();
	is_loaded = inner::read_item_info<DomiSpItemInfo>("Goods/DomiSpItem", m_InfoList[DBT_Domi_Spltem]);
	if( is_loaded == false) return false;

	m_InfoList[DBT_UITemplate].clear();
	is_loaded = inner::read_item_info<CUITemplateInfo_Item>("Goods/UITemplate", m_InfoList[DBT_UITemplate]);
	if (is_loaded == false) return false;

	ColorNickPalette::i()->Load();
	GachaItem::i()->Load();
	ColorFireMuzzleFlash::i()->Load();

	return true;
}

void ShopItemInfoDataBase::change_condition_for_finding_in_pef_data( T_ItemID item_id, ITEM_TYPE & type, INT32 &group_index)
{
	type = (ITEM_TYPE) GET_ITEM_TYPE(item_id);
	group_index	= GET_ITEM_NUMBER(item_id);

	switch( type )
	{
	case ITEM_TYPE_POINT:
		group_index	= 900;
		type = ITEM_TYPE_WRAP_COUNT;
		break;
	case ITEM_TYPE_MAINTENANCE:
		type = ITEM_TYPE_WRAP_PERIOD;
		break;
	case ITEM_TYPE_INGAMEITEM:
		type = ITEM_TYPE_WRAP_COUNT;
		break;
	case ITEM_TYPE_DOMIITEM:
		type = ITEM_TYPE_WRAP_COUNT;
		break;
	case ITEM_TYPE_WRAP_COUNT:
		group_index = item_id % 100000;
		break;
	}
}

namespace inner
{
	ShopBaseInfo* find_shop_info( const i3::vector<ShopBaseInfo*> & list, ITEM_TYPE type, INT32 group_index)
	{
		struct fn
		{
			fn(ITEM_TYPE type, INT32 group_index) : type(type), group_index(group_index) {}
			bool operator()(ShopBaseInfo* item_info) const
			{
				ShopItemInfo* shop_item_info = (ShopItemInfo*)item_info;
				if( shop_item_info->GetType() == type && shop_item_info->GetGroupIndex() == group_index)
					return true;

				return false;
			}

			ITEM_TYPE type; 
			INT32 group_index;
		};

		fn fn_compare(type, group_index);
		i3::vector<ShopBaseInfo*>::const_iterator it = std::find_if(list.begin(), list.end(), fn_compare);
		if( it == list.end() ) return 0;

		return *it;
	}
	
	template< typename db_class_name >
	ShopBaseInfo* find_shop_info( const i3::vector<ShopBaseInfo*> & list, INT32 display_idx)
	{
		struct fn
		{
			fn(INT32 display_idx) : display_idx(display_idx) {}
			bool operator()(ShopBaseInfo* item_info) const
			{
				db_class_name* shop_item_info = (db_class_name*)item_info;
				if( shop_item_info->GetDisplayIndex() == display_idx )
					return true;

				return false;
			}
			INT32 display_idx;
		};

		fn fn_compare(display_idx);
		i3::vector<ShopBaseInfo*>::const_iterator it = std::find_if(list.begin(), list.end(), fn_compare);
		if( it == list.end() ) return 0;

		return *it;
	}
}



ShopItemInfo* ShopItemInfoDataBase::getInfo( T_ItemID item_id)
{
	INT32 group_index; ITEM_TYPE type;
	change_condition_for_finding_in_pef_data(item_id, type, group_index);

	ShopBaseInfo* shop_info = 0;

	shop_info = inner::find_shop_info(m_InfoList[DBT_ShopItem], type, group_index);
	if(shop_info != 0) return (ShopItemInfo*)shop_info;
	
	shop_info = inner::find_shop_info(m_InfoList[DBT_Domi_Item], type, group_index);
	if(shop_info != 0) return (ShopItemInfo*)shop_info;

	return 0;
}

DomiCashItemInfo* ShopItemInfoDataBase::find_domi_cash_item_info( INT32 display_idx)
{
	return (DomiCashItemInfo*)inner::find_shop_info<DomiCashItemInfo>(m_InfoList[DBT_Domi_Cashltem], display_idx);
}

DomiSpItemInfo* ShopItemInfoDataBase::find_domi_sp_item_info( INT32 display_idx)
{
	return (DomiSpItemInfo*)inner::find_shop_info<DomiSpItemInfo>(m_InfoList[DBT_Domi_Spltem], display_idx);
}

ShopPackageGoodsInfo * ShopItemInfoDataBase::GetPackageGoodsInfo(INT32 GoodsID)
{
	INT32 srvGoodsId = (INT32)(GoodsID / 100);
	INT32 cliGoodsId = 0;

	for(size_t i = 0; i < m_InfoList[DBT_SetItem].size(); i++)
	{
		ShopPackageGoodsInfo* pInfo = (ShopPackageGoodsInfo*)m_InfoList[DBT_SetItem][i];

		if( pInfo->m_GoodsID > 9999999 )		//날짜를 포함한 GoodsID 일 경우
			cliGoodsId = (INT32)(pInfo->m_GoodsID / 100);

		if(srvGoodsId == cliGoodsId )
			return pInfo;
	}

	return nullptr;
}

bool ShopItemInfoDataBase::CheckPackageGoodsGroupInfo( INT32 GoodsID)
{
	UINT32 nGroupGoodIdFromArg = MAKE_ITEM_FLAG( 
		ITEM_EVENT_TYPE_NONE, 
		GET_ITEM_TYPE( GoodsID),
		GET_ITEM_SUBTYPE( GoodsID),
		0);		// 상품 종류를 비교하기위해 날짜만 0으로 리셋

	// 셋트 아이템 GoodID 를 날짜를 제외하고 나머지 분류 인덱스로 Info가 존재하는지 확인한다.
	// 결과적으로 같은 상품이지만 날짜만 다른 GoodsID의 특성을 고려하고 날짜만 다를지라도 같은 상품 계열일 경우 같은 상품으로 간주한다.
	for(size_t i = 0; i < m_InfoList[DBT_SetItem].size(); i++)
	{
		ShopPackageGoodsInfo* pInfo = (ShopPackageGoodsInfo*)m_InfoList[DBT_SetItem][i];

		UINT32 nGroupGoodIdFromInfo = MAKE_ITEM_FLAG( 
										ITEM_EVENT_TYPE_NONE,
										GET_ITEM_TYPE( pInfo->m_GoodsID),
										GET_ITEM_SUBTYPE( pInfo->m_GoodsID),
										0);		// 상품 종류를 비교하기위해 날짜만 0으로 리셋

		if (nGroupGoodIdFromInfo == nGroupGoodIdFromArg)
			return true;
	}

	return false;
}

namespace inner
{
	void to_not_find_item_string( T_ItemID item_id, i3::rc_wstring& outName)
	{
		i3::sprintf(outName, L"%s (%d)", INVALID_TEXTW, item_id);
		::ColorString(outName, 255, 0, 0);
	}
}

//상점 Item, Domi Cash 아이템 전용
void	ShopItemInfoDataBase::GetItemName( T_ItemID item_id, i3::rc_wstring& outName)
{
	ShopItemInfo* pInfo = getInfo(item_id);
	if( pInfo == 0 )
	{
		inner::to_not_find_item_string(item_id, outName);
		return;
	}

	if( GET_ITEM_TYPE(item_id) == ITEM_TYPE_POINT )
	{
		i3::sprintf(outName, pInfo->GetScriptName(), GAME_RCSTRING("STBL_IDX_SHOP_MONEY"), GET_POINTITEM(item_id));
		return;
	}

	outName = pInfo->GetScriptName();
}
void	ShopItemInfoDataBase::GetItemDescription(T_ItemID item_id, i3::rc_wstring& outName)
{
	ShopItemInfo* pInfo = getInfo(item_id);
	if( pInfo == 0 )
	{
		inner::to_not_find_item_string(item_id, outName);
		return;
	}

	outName = pInfo->GetDescription();
}

size_t ShopItemInfoDataBase::GetUITemplateCount() const
{
	return m_InfoList[DBT_UITemplate].size();
}

ShopBaseInfo* ShopItemInfoDataBase::GetUITemplateInfo(size_t i) const
{
	return m_InfoList[DBT_UITemplate].at(i);
}

#if !defined( I3_NO_PROFILE)
void ShopItemInfoDataBase::OnReloadProperty()
{
	i3RegKey * pRoot = Config::_GetRegRoot("Goods/ShopItem");
	I3ASSERT( pRoot != nullptr);

	const size_t count = pRoot->getChildCount();
	for( size_t i = 0; i < count; ++i)
	{
		i3RegKey * pKey = (i3RegKey*)pRoot->getChild( i);

		ShopItemInfo* pInfo = (ShopItemInfo*)m_InfoList[DBT_ShopItem][i];

		//INT32	nNumber = -1;
		INT32	nType = (INT32) ITEM_TYPE_UNKNOWN;

		FIND_REG_DATA( pKey, "_Type",					(INT32*)&nType);

		pInfo->ReadInfo( pKey);
	}

	pRoot = Config::_GetRegRoot("Goods/UITemplate");
	I3ASSERT( pRoot != nullptr);
	for ( INT32 i = 0; i < pRoot->getChildCount(); ++i)
	{
		i3RegKey * pKey = (i3RegKey*)pRoot->getChild(i);

		CUITemplateInfo_Item* pInfo = (CUITemplateInfo_Item*)m_InfoList[DBT_UITemplate][i];
		pInfo->ReadInfo(pKey);
	}

	ColorNickPalette::i()->Load();
	GachaItem::i()->Load();
	ColorFireMuzzleFlash::i()->Load();
}
#endif

//------------------------------------////------------------------------------//
namespace
{
	i3RegKey* FindRowKey(i3RegKey* rootkey, const i3::rc_string rowName)
	{
		for (INT32 i=0; i<rootkey->getChildCount(); i++)
		{
			i3RegKey* key = (i3RegKey*)rootkey->getChild(i);
			if (key->GetNameString() == rowName)
				return key;
		}

		return nullptr;
	}
}

ColorNickPalette::ColorNickPalette() : m_numOfRow(16), m_numOfCol(16)
{
}

ColorNickPalette::~ColorNickPalette() 
{
	m_colors.clear();
}

void ColorNickPalette::Load()
{
	if (!m_colors.empty())
		m_colors.clear();

	i3::stack_string rowName;
	i3::stack_string colName;

	i3RegKey* rootkey = Config::_GetRegRoot("Goods/ColorNickPalette");
	if (rootkey == nullptr) return;

	for (size_t r=0; r<m_numOfRow; r++)
	{
		i3::sprintf(rowName, "row%d", r);

		i3RegKey* rowkey = FindRowKey(rootkey, rowName);

		if (rowkey == nullptr) 
		{
			I3ASSERT("[ColorNickPalette] row key not found\n");
			m_colors.clear();
			return;
		}

		for (size_t c=0; c<m_numOfCol; c++)
		{
			i3::sprintf(colName, "col%d", c);

			I3COLOR color;
			FIND_REG_DATA(rowkey, colName.c_str(), &color);

			if (rowkey == nullptr)
			{
				I3ASSERT("[ColorNickPalette] col key not found\n");
				m_colors.clear();
				return;
			}

			m_colors.push_back(color);

			colName.clear();
		}

		rowName.clear();
	}
}

const I3COLOR& ColorNickPalette::GetColor(const size_t row, const size_t col) const
{
	I3ASSERT( row < m_numOfRow && col < m_numOfCol);
	
	size_t pos = (row * m_numOfCol) + col;
	return m_colors[pos];
}

const I3COLOR& ColorNickPalette::GetColor(const UINT8 colorIndex) const
{
	const static I3COLOR white = I3COLOR(0xff,0xff,0xff,0xff);

	if(m_colors.size() > colorIndex)
		return m_colors[colorIndex];

	return white;
}

ColorFireMuzzleFlash::ColorFireMuzzleFlash() : m_numOfRow(1), m_numOfCol(7)
{
}

ColorFireMuzzleFlash::~ColorFireMuzzleFlash() 
{
	m_colors.clear();
}

void ColorFireMuzzleFlash::Load()
{
	if (!m_colors.empty())
		m_colors.clear();

	i3::stack_string rowName;
	i3::stack_string colName;

	i3RegKey* rootkey = Config::_GetRegRoot("Goods/ColorFireMuzzleFlash");
	if (rootkey == nullptr) return;

	m_numOfRow = rootkey->getChildCount();

	for (size_t r=0; r<m_numOfRow; r++)
	{
		i3::sprintf(rowName, "Type%d", r);

		i3RegKey* rowkey = FindRowKey(rootkey, rowName);

		if (rowkey == nullptr) 
		{
			I3ASSERT("[ColorFireMuzzleFlash] row key not found\n");
			m_colors.clear();
			return;
		}

		for (size_t c=0; c<m_numOfCol; c++)
		{
			i3::sprintf(colName, "Color%d", c);

			I3COLOR color;
			FIND_REG_DATA(rowkey, colName.c_str(), &color);

			if (rowkey == nullptr)
			{
				I3ASSERT("[ColorFireMuzzleFlash] col key not found\n");
				m_colors.clear();
				return;
			}

			m_colors.push_back(color);

			colName.clear();
		}

		rowName.clear();
	}
}

const I3COLOR& ColorFireMuzzleFlash::GetColor(const size_t type, const size_t Index) const
{
	I3ASSERT(type < m_numOfRow && Index < m_numOfCol);

	size_t pos = (type * m_numOfCol) + Index;
	return m_colors[pos];
}

const I3COLOR& ColorFireMuzzleFlash::GetColor(const UINT16 colorIndex) const
{
	const static I3COLOR white = I3COLOR(0xff,0xff,0xff,0xff);

	if(m_colors.size() > colorIndex)
		return m_colors[colorIndex];

	return white;
}

GachaItem::GachaItem()
{
}

GachaItem::~GachaItem()
{
	i3::cu::for_each_delete_clear(m_itemSprInfoAry);
}

void GachaItem::Load()
{
	i3::cu::for_each_delete_clear(m_itemSprInfoAry);

	i3RegKey* keyRoot = Config::_GetRegRoot("Goods/Gacha");
	if(keyRoot == nullptr) return;

	i3RegKey* keyLayout = Config::FindKey("Layout", keyRoot);
	if(!keyLayout) return;

	i3RegKey* keySprViewScale = Config::FindKey("ItemSpriteViewScale", keyLayout);
	if(!keySprViewScale) return;

	const char* cateName[EUISPRITE_SIZ_MAX] = { "1024", "512" };

	for(INT32 i=0; i<EUISPRITE_SIZ_MAX; i++)
	{
		m_itemSprInfoAry.push_back(new ItemSprInfo);

		i3RegKey* cur = Config::FindKey(cateName[i], keySprViewScale);
		ItemSprInfo* dstInfo = m_itemSprInfoAry[i];

		if(cur)
		{
			i3::string scaleName;
			i3::string rotName;

			for(INT32 k=0; k<GACHA_POS_MAX; k++)
			{
				i3::sprintf(scaleName, "Scale%d", k);
				i3::sprintf(rotName, "Rotation%d", k);

				FIND_REG_DATA(cur, scaleName.c_str(), &dstInfo->m_scaleGachaView[k]);
				FIND_REG_DATA(cur, rotName.c_str(),	 &dstInfo->m_rotGachaView[k]);
			}
		}
		else
		{
			for(INT32 k=0; k<GACHA_POS_MAX; k++)
			{
				dstInfo->m_scaleGachaView[k] = VEC2D(1.0f, 0.35f);
				dstInfo->m_rotGachaView[k] = VEC3D(0.0f, 0.0f, 0.0f);
			}
		}
	}

	//for(INT32 i=0; i<EUISPRITE_SIZ_MAX; i++)
	//{
	//	i3RegDat* dat =  rootKey->FindDataByName(cateName[i]);

	//	const char* name = dat->GetName();


	//	//i3RegKey* cateKey = (i3RegKey*)rootkey->getChild(i);

	//	//for(INT32 i=0; i<rootkey->getChildCount(); i++)
	//	//{
	//	//	i3RegKey* key = (i3RegKey*)rootkey->getChild(i);

	//	//	Item* item = new Item;
	//	//	FIND_REG_DATA(key, "Scale", &item->scale);
	//	//	FIND_REG_DATA(key, "Rotation", &item->rotation);
	//	//	m_itemList.push_back(item);
	//	//}
	//}
}

const VEC2D& GachaItem::GetItemSprScale(const size_t idx, T_ItemID id) const
{
	const static VEC2D defaultScale = VEC2D(1.0f, 0.35f);

	size_t aryIndex = ResolveItemSprSizType(id);
	if(idx < GACHA_POS_MAX)
		return m_itemSprInfoAry[aryIndex]->m_scaleGachaView[idx];

	return defaultScale;
}

const VEC3D& GachaItem::GetItemSprRotation(const size_t idx, T_ItemID id) const
{
	const static VEC3D defaultRot = VEC3D(0.0f, 0.0f, 0.0f);

	size_t aryIndex = ResolveItemSprSizType(id);
	if( idx < GACHA_POS_MAX)
		return m_itemSprInfoAry[aryIndex]->m_rotGachaView[idx];

	return defaultRot;
}

GachaItem::EUISPRITE_SIZ GachaItem::ResolveItemSprSizType(T_ItemID id)
{
	ITEM_TYPE type = (ITEM_TYPE) GET_ITEM_TYPE(id);

	if(ITEM_TYPE_CHARA <= type && type <= ITEM_TYPE_SKIN)
		return EUISPRITE_SIZ_1024;  // Avatar
	else if(ITEM_TYPE_PRIMARY <= type && type <= ITEM_TYPE_THROWING2)
		return EUISPRITE_SIZ_1024;  // Weapon

	return EUISPRITE_SIZ_512;
}
