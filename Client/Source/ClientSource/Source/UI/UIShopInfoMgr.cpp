#include "pch.h"
#include "UIShopInfoMgr.h"

#include "UIShopFunction.h"
#include "UIItemInfoUtil.h"

#include "UIMainFrame.h"
#include "UIFloatCharacterView.h"

//#include "UIBattleDef.h"

#include "IGShopContext.h"
#include "ShopContext.h"

#include "i3Base/string/ext/contain_string_head.h"

#include <functional>

#include "UI/UIUtil.h"

/************************//************************/
void shop_util::ShopPackInfoList::push( const ShopInfo & info )
{
	m_list.push_back( info );
}

void shop_util::ShopPackInfoList::push( const SHOP_GOODS_PACK* pack, UINT32 type, UINT32 usage, UINT8 sale_type, item_def::Category category)
{
	ShopInfo info;

	info.info = pack;
	info.type = type;
	info.usage = usage;
	info.sale_type = sale_type;
	info.category = category;

	m_list.push_back( info );
}

void shop_util::ShopPackInfoList::push(const shop_util::ShopPackInfoList& list)
{
	size_t size = list.size();
	for(size_t i = 0; i<size; ++i)
	{
		m_list.push_back( list[ i ] );
	}
}

void shop_util::ShopPackInfoList::erase(INT32 idx)
{
	m_list.erase(m_list.begin() + idx);
}

void shop_util::ShopPackInfoList::insert_front( const ShopInfo & info )
{
	m_list.insert(m_list.begin(), info);
}

void shop_util::ShopPackInfoList::SearchKeyword(const WCHAR* wstr)
{
	if (wstr[0] != 0)
	{
		i3::vector<ShopInfo>::iterator itr = m_list.begin();
		while (itr != m_list.end())
		{
			i3::rc_wstring name;
			usf::GetShopItemName(name, itr->info);
			if (i3::icontain_string(name, wstr) == -1)
			{
				itr = m_list.erase(itr);
			}
			else
				itr++;
		}
	}
}

/************************//************************/
/************************//************************/
namespace st	//shop type
{
	void get_weapon_attribute(SHOP_GOODS*, UINT32&, UINT32&);
	void get_character_attribute(SHOP_GOODS*, UINT32&, UINT32&);
	void get_item_attribute(SHOP_GOODS*, UINT32&, UINT32&);

	typedef void (*_pf_get_attribute)(SHOP_GOODS*, UINT32&, UINT32&);
	_pf_get_attribute pf_get_attribute[] = {	get_weapon_attribute, 	get_character_attribute, get_character_attribute, get_item_attribute, get_item_attribute	};
}

namespace fw	//filter weapon
{
	void filter_primary(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT8);
	void filter_assault(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT8);
	void filter_smg(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT8);
	void filter_sniper(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT8);
	void filter_machine(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT8);
	void filter_shotgun(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT8);
	typedef void (*_pf_filter)(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT8);
	_pf_filter pf_filter[] = {	filter_primary, 	filter_assault,	filter_smg, 	filter_sniper,	filter_machine,	filter_shotgun	};
}

namespace fg	//filter goods
{
	void filter_weapon(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT32, UINT8);
	void filter_chara(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT32, UINT8);
	void filter_parts(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT32, UINT8);
	void filter_skin(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT32, UINT8);
	void filter_item(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT32, UINT8);
	void filter_dino(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT32, UINT8);
	void filter_chara_V10(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT32, UINT8);

	void filter_all_mask(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT32, UINT8);
	void filter_parts_mask(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT32, UINT8);
	void filter_skin_mask(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT32, UINT8);

	typedef void (*_pf_filter)(shop_util::ShopPackInfoList&, const shop_util::ShopPackInfoList&, UINT32, UINT8);
	_pf_filter pf_filter[] = {	filter_weapon, 	filter_chara,	filter_parts, filter_skin,	filter_item, filter_all_mask };
	_pf_filter pf_filter_V10[] = { filter_weapon, 	filter_chara_V10,	filter_parts, filter_skin,	filter_item, filter_all_mask };
}

namespace filter_slot_type
{
	bool filter_shop(const shop_util::ShopInfo & data);
	bool filter_gift(const shop_util::ShopInfo & data);

	typedef bool (*_pf_filter)(const shop_util::ShopInfo & data);
	_pf_filter pf_filter[] = { filter_shop,		filter_gift };
}

namespace
{
	item_def::Category to_item_category(SHOP_GOODS_TYPE type)
	{
		switch(type)
		{
		case SHOP_WEAPON_GOODS:		return item_def::CTG_WEAPON;
		case SHOP_CHARACTER_GOODS:	return item_def::CTG_CHARA;
		case SHOP_CASH_ITEM_GOODS:	return item_def::CTG_ITEM;
		case SHOP_PACKAGE_GOODS:	return item_def::CTG_ITEM;
		case SHOP_SKIN_GOODS :		return item_def::CTG_SKIN;
		}
		return item_def::CTG_NA;
	}
}

namespace sort_f //sort function
{
	//% 용어 설명( 낮은게 앞으로 나타남 )
	//	1. 장착 상태 : 장착중 < 사용중 < 보유중 < 미 사용
	//	2. 무기 타입 : 주무기 < 보조무기 < 근접무기 < 수류탄(?) < 특수무기
	//	3. 사용 타입 : assault < smg < sniper < machine < shotgun
	//	4. 이름 : 0<1<11<8<9 <ㄱ<ㄴ~ ㅍ<ㅎ
	//	5. mark : new & hot 마크

	void mark_status_type_name(shop_util::ShopPackInfoList& shop_list); //weapon : mark < 장착 상태 < 무기 타입(사용 타입) < 이름순 정렬(오름 차순)
	void mark_status_name(shop_util::ShopPackInfoList& shop_list); //chara, part, item : mark < 장착 상태 < 이름순 정렬(오름 차순)
	//신규 정렬기능 
	void shop_sort_weapon(shop_util::ShopPackInfoList& shop_list, SHOP_SORT_TYPE type); //무기 부분
	void shop_sort_item(shop_util::ShopPackInfoList& shop_list, SHOP_SORT_TYPE type); //아이템부분
}



/*************************//*************************/
//ShopInfoList
void ShopInfoList::clear()
{
	for( size_t ctg =0; ctg < item_def::CTG_MAX; ++ctg )
	{
		m_list[ ctg ].clear();
	}
	m_current_list.clear();

	m_slot_type = 0;
}

//	상품의 Sale type를 오름차순으로 정렬합니다.
//bool _GoodsSaleTypeSort(const SHOP_GOODS* pGoods1, const SHOP_GOODS* pGoods2)
//{
//	return pGoods1->_SaleType < pGoods2->_SaleType;
//}

void ShopInfoList::initialize(UINT8 ui8SlotType/* = 0*/)
{
	//모든 상품에 대해서 정보를 수집한다. 수집시에 각각의 상품에 대한 type도 같이 수집한다.
	clear();

	m_slot_type = ui8SlotType;

	CShopContext * pShopContext = CShopContext::i();

	for( size_t ctg = SHOP_WEAPON_GOODS; ctg < SHOP_GOODS_MAX; ++ctg )
	{
		SHOP_GOODS_TYPE shop_type = (SHOP_GOODS_TYPE)ctg;
		item_def::Category category = to_item_category(shop_type);

		INT32 goodsPackCount = pShopContext->GetGoodsPackCount( shop_type );

		for(INT32 i=0; i<goodsPackCount; i++)
		{
			const SHOP_GOODS_PACK * pGoodsPack = pShopContext->GetGoodsPack(shop_type, i);
			if( pGoodsPack == nullptr) continue;
			//const_cast<SHOP_GOODS_PACK*> (pGoodsPack)->Sort(_GoodsSaleTypeSort);

			SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
			if( pGoods == nullptr) continue;

			// 뽑기상점에서 이벤트 아이템을 지정 가능합니다.
			// EVENT 아이템 자체도 GoodsPack에 포함되어 있습니다.
			// 다만 모든 아이템이 EVENT이면 상점에서 제외 합니다.
			INT32 j = 0;
			for(j = 0; j < pGoodsPack->GetGoodsCount(); j++)
			{
				if(pGoodsPack->GetGoods(j)->_SaleType != GOODS_SALE_EVENT)
					break;
			}

			if(j == pGoodsPack->GetGoodsCount())
				continue;

			if (shop_type == SHOP_WEAPON_GOODS && pGoods->_ItemID >= _I32_MAX) continue;	//이런 예외가 왜? 있어야 하는지 모르겟다.

			UINT32 type = 0, usage = 0;
			st::pf_get_attribute[shop_type](pGoods, type, usage);

			m_list[ category ].push(pGoodsPack, type, usage, pGoods->_SaleType, category);
			if (pGoods->GetSaleMark(usf::g_price_idx) == SHOP_ITEM_TYPE_HOT
				|| pGoods->GetSaleMark(usf::g_price_idx) == SHOP_ITEM_TYPE_NEW)
			{
				if (g_pEnvSet->IsV2Version() == true)
					m_list[item_def::CTG_NEWHOT].push(pGoodsPack, type, usage, pGoods->_SaleType, category);
				else
				{
					item_def::Category temp = item_def::get_category_tab(pGoods->_ItemID);
					if(temp != item_def::CTG_PARTS
						&& temp != item_def::CTG_SKIN)
					m_list[item_def::CTG_NEWHOT].push(pGoodsPack, type, usage, pGoods->_SaleType, category);
				}
			}
		}
	}

	arrange();
}

void ShopInfoList::list_up(item_def::Category category, UINT32 type)
{
	//미리 수집된 상품 정보를 필터링한다.
	m_current_list.clear();
	if (g_pEnvSet->IsV2Version() == true)
		fg::pf_filter[category](m_current_list, m_list[category], type, m_slot_type);
	else
		fg::pf_filter_V10[category](m_current_list, m_list[category], type, m_slot_type);

	if (category == item_def::CTG_ITEM)
	{
		switch (type)
		{
		case UINT_MAX: case item_def::IT_FEATURE: //공룡을 아이템에서 보여준다.(전체, 기능)
			shop_util::ShopPackInfoList dino_list;
			fg::filter_dino(dino_list, m_list[item_def::CTG_CHARA], 0, m_slot_type);	//캐릭터 정보에서 공룡 정보를 얻는다.
			m_current_list.push(dino_list);
			break;
		}
	}
	else if (category == item_def::CTG_NEWHOT)
	{
		switch (type)
		{
		case item_def::MASKNEWHOT_CHARA:
			shop_util::ShopPackInfoList skin_mask_list;
			fg::filter_skin_mask(skin_mask_list, m_list[item_def::CTG_NEWHOT], UINT_MAX, m_slot_type);
			m_current_list.push(skin_mask_list);

			break;
		}
	}
}

void ShopInfoList::list_up(item_def::WeaponComboType type)
{
	m_current_list.clear();

	//각 항목에 대해서 미리 얻은 데이터로 필터링 한다. ㅋㅋ
	fw::pf_filter[type](m_current_list, m_list[item_def::CTG_WEAPON], m_slot_type);
}

void ShopInfoList::list_up_all()
{
	//미리 수집된 상품 정보를 필터링한다.
	m_current_list.clear();
	fg::filter_all_mask(m_current_list, m_list[item_def::CTG_WEAPON], UINT_MAX, m_slot_type);
	fg::filter_all_mask(m_current_list, m_list[item_def::CTG_CHARA], UINT_MAX, m_slot_type);
	fg::filter_parts_mask(m_current_list, m_list[item_def::CTG_PARTS], UINT_MAX, m_slot_type);
	fg::filter_skin_mask(m_current_list, m_list[item_def::CTG_SKIN], UINT_MAX, m_slot_type);
	fg::filter_all_mask(m_current_list, m_list[item_def::CTG_ITEM], UINT_MAX, m_slot_type);
}
void ShopInfoList::list_up_weapon()
{
	//미리 수집된 상품 정보를 필터링한다.
	m_current_list.clear();
	fg::filter_all_mask(m_current_list, m_list[item_def::CTG_WEAPON], UINT_MAX, m_slot_type);
}
void ShopInfoList::list_up_chara()
{
	//미리 수집된 상품 정보를 필터링한다.
	m_current_list.clear();
	fg::filter_all_mask(m_current_list, m_list[item_def::CTG_CHARA], UINT_MAX, m_slot_type);
}
void ShopInfoList::list_up_item()
{
	m_current_list.clear();
	//각 항목에 대해서 미리 얻은 데이터로 필터링 한다. ㅋㅋ
	fg::filter_all_mask(m_current_list, m_list[item_def::CTG_ITEM], UINT_MAX, m_slot_type);
}

void ShopInfoList::arrange()
{
	//item_def::CTG_CHARA 리스트에는 item_def::CTG_CHARA, item_def::CTG_PARTS가 같이 있기에 분리한다.
	shop_util::ShopPackInfoList list[2];

	shop_util::ShopPackInfoList& db = m_list[ item_def::CTG_CHARA ];
	size_t size = db.size();
	for(size_t i=0;i<size;++i)
	{
		if (db[i].type == ITEM_TYPE_DINO)
			continue;

		SHOP_GOODS* pGoods = db[i].info->GetGoods(0);
		
		bool ctg = item_def::get_category_tab(pGoods->_ItemID) == item_def::CTG_PARTS;
		list[ctg].push(db[i]);
	}

	std::swap( m_list[ item_def::CTG_CHARA ], list[0] );
	std::swap( m_list[ item_def::CTG_PARTS ], list[1] );
}

void ShopInfoList::sort(item_def::Category category)
{
	//필터링
	switch(category)
	{
	case item_def::CTG_WEAPON:	
		sort_f::mark_status_type_name(m_current_list);
		break;

	case item_def::CTG_CHARA:
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN :
	case item_def::CTG_ITEM:
	case item_def::CTG_NEWHOT:
		sort_f::mark_status_name(m_current_list);
		break;
	}
}

void ShopInfoList::SortType(item_def::Category category, SHOP_SORT_TYPE type)
{
	//필터링
	if(category == item_def::CTG_WEAPON)
		sort_f::shop_sort_weapon(m_current_list, type);
	else
		sort_f::shop_sort_item(m_current_list, type);
}

void ShopInfoList::SearchKeyword(const WCHAR* wstr)
{
	m_current_list.SearchKeyword(wstr);
}

bool ShopInfoList::set_list_front_item(UINT32 ui32ItemID)
{
	for(size_t i = 0 ; i < m_current_list.size() ; ++i)
	{
		if(m_current_list[i].info->GetGoods(0)->_ItemID == ui32ItemID)
		{
			shop_util::ShopInfo info;
			i3mem::Copy(&info, &m_current_list[i], sizeof(shop_util::ShopInfo));
			m_current_list.erase(i);
			m_current_list.insert_front(info);
			return true;
		}
	}

	return false;
}

const shop_util::ShopInfo&  ShopInfoList::get(UINT32 idx) const
{
	return m_current_list[ idx ];
}

shop_util::ShopInfo& ShopInfoList::get(UINT32 idx)
{
	return m_current_list[ idx ];
}


/*************************//*************************/
//igShopInfoList
void igShopInfoList::list_up(UINT32 ctg)
{
	shop_util::ShopPackInfoList ig_list;

	IGShopContext* ig_cont = g_pFramework->GetIGShopContext();
	INT32 size = ig_cont->GetNumShopGoodsPack(ctg);
	for(INT32 i=0;i<size;++i)
	{
		ig_list.push(ig_cont->GetShopGoodsPack(ctg, i), 0, 0, 0, item_def::CTG_NA);
	}

	//필터 처리하자.
	m_list.clear();

	//해당 캐릭터가 착용 가능한 파츠만 선택한다.
	const CHARA_INFO* info = CCharaInfoContext::i()->GetMainCharaInfo();
	// vv3 - check
	T_ItemID charaID = info->getPartsInfo()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA); //현재 선택된 캐릭터

	size_t ig_size = ig_list.size();
	for(size_t i=0;i<ig_size;++i)
	{
		switch( item_def::get_category_goodsID(ig_list[i].info->GetGoods(0)->_GoodsID) )
		{
		case item_def::CTG_PARTS:
		case item_def::CTG_SKIN:
			for( INT32 j = 0; j < ig_list[i].info->GetGoodsCount(); j++)
			{
				SHOP_GOODS * pGoods = ig_list[i].info->GetGoods( j);
				if( pGoods->_ItemID == 0) continue;

				if( item_def::is_shared_parts_item(pGoods->_ItemID, charaID) == true )
				{
					m_list.push( ig_list[i] );
					break;
				}
			}
			break;
		case item_def::CTG_CHARA:
			for( INT32 j = 0; j < ig_list[i].info->GetGoodsCount(); j++)
			{
				SHOP_GOODS * pGoods = ig_list[i].info->GetGoods( j);
				if( pGoods->_ItemID == 0) continue;

				if( pGoods->_ItemID != charaID )
				{
					m_list.push( ig_list[i] );
					break;
				}
			}
			break;
		}
	}
}

/*************************//*************************/
//ig_domi_ShopInfoList
namespace inner
{
	shop_util::ShopInfo* find_shop_info(shop_util::ShopPackInfoList& shop_item_info_list, INT32 group_idx)
	{
		size_t ig_size = shop_item_info_list.size();
		for(size_t i=0;i<ig_size;++i)
		{
			SHOP_GOODS * pGoods = shop_item_info_list[i].info->GetGoods(0);
			if( pGoods->_ItemID == 0) continue;

			if( (INT32) GET_ITEM_NUMBER(pGoods->_ItemID) == group_idx ) return &shop_item_info_list[i];
		}

		return 0;
	}
}

//void ig_domi_ShopInfoList::list_up()
//{
//	ShopInfoList shop_info_list;
//	shop_info_list.initialize();
//	shop_info_list.list_up(item_def::CTG_ITEM, UINT_MAX);
//
//	shop_util::ShopPackInfoList& shop_item_info_list = shop_info_list.get_list();
//	
//	m_domi_cash_item_list.clear();
//
//	m_domi_item_list.clear();
//	const i3::vector<DomiShopItemInfo*> & domi_item_list = g_pShopItemInfoDataBase->get_insg_domi_item_info_list();
//	size_t item_size = domi_item_list.size();
//	for(size_t i=0;i<item_size;++i)
//	{
//		shop_util::ShopInfo* shop_info = inner::find_shop_info(shop_item_info_list, domi_item_list[i]->GetGroupIndex() );
//		if( shop_info != 0 )
//		{
//			m_domi_item_list.push( *shop_info );
//		}
//	}
//
//	m_domi_cash_item_list.clear();
//	const i3::vector<DomiCashItemInfo*> & domi_cash_item_list = g_pShopItemInfoDataBase->get_insg_domi_cash_item_info_list();
//	item_size = domi_cash_item_list.size();
//	for(size_t i=0;i<item_size;++i)
//	{
//		shop_util::ShopInfo* shop_info = inner::find_shop_info(shop_item_info_list, domi_cash_item_list[i]->GetGroupIndex() );
//		if( shop_info != 0 )
//		{
//			m_domi_cash_item_list.push( *shop_info );
//		}
//	}
//}
//
//const SHOP_GOODS_PACK* ig_domi_ShopInfoList::find_domi_item_pack(CASHITEM_GROUP_TYPE buy_group_type)
//{
//	shop_util::ShopInfo* shop_info = inner::find_shop_info(m_domi_item_list, buy_group_type);
//	if( shop_info == 0 ) return 0;
//
//	return shop_info->info;	
//}
//
//const SHOP_GOODS_PACK* ig_domi_ShopInfoList::find_domi_cash_item_pack(CASHITEM_GROUP_TYPE buy_group_type)
//{
//	shop_util::ShopInfo* shop_info = inner::find_shop_info(m_domi_cash_item_list, buy_group_type);
//	if( shop_info == 0 ) return 0;
//
//	return shop_info->info;
//}

/*************************//*************************/
//inner
namespace inner
{
	struct Fn_comape_pef : std::unary_function<shop_util::ShopInfo, bool>
	{
		bool operator()(const shop_util::ShopInfo& sinfo) const
		{
			SHOP_GOODS* pGoods = sinfo.info->GetGoods(0);
			return (usf::CheckValidGoods_in_Pef( pGoods->_ItemID ) == false);
		}
	};

	struct Fn_comape_no_name : std::unary_function<shop_util::ShopInfo, bool>
	{
		bool operator()(const shop_util::ShopInfo& sinfo) const
		{
			i3::rc_wstring name;
			usf::GetShopItemName( name, sinfo.info );
			return i3::contain_string_head(name, L"No Name") ;
		}
	};
}


void shop_util::only_valid_item(shop_util::ShopPackInfoList& spi_list)
{
	i3::vector< ShopInfo > & info_list = const_cast< i3::vector< ShopInfo >& >( spi_list.get_list() );
	i3::vector< ShopInfo >::iterator it = std::remove_if(info_list.begin(), info_list.end(), inner::Fn_comape_pef() );
	info_list.resize( it - info_list.begin() );
}
void shop_util::only_invalid_item(shop_util::ShopPackInfoList& spi_list)
{
	i3::vector< ShopInfo > & info_list = const_cast< i3::vector< ShopInfo >& >( spi_list.get_list() );
	i3::vector< ShopInfo >::iterator it = std::remove_if(info_list.begin(), info_list.end(), std::not1( inner::Fn_comape_pef() ) );
	info_list.resize( it - info_list.begin() );
}
void shop_util::only_name_item(shop_util::ShopPackInfoList& spi_list)
{
	i3::vector< ShopInfo > & info_list = const_cast< i3::vector< ShopInfo >& >( spi_list.get_list() );

	i3::vector< ShopInfo >::iterator it = std::remove_if(info_list.begin(), info_list.end(), inner::Fn_comape_pef() );
	info_list.resize( it - info_list.begin() );

	it = std::remove_if(info_list.begin(), info_list.end(), inner::Fn_comape_no_name() );
	info_list.resize( it - info_list.begin() );
}
void shop_util::only_noname_item(shop_util::ShopPackInfoList& spi_list)
{
	i3::vector< ShopInfo > & info_list = const_cast< i3::vector< ShopInfo >& >( spi_list.get_list() );

	i3::vector< ShopInfo >::iterator it = std::remove_if(info_list.begin(), info_list.end(), inner::Fn_comape_pef() );
	info_list.resize( it - info_list.begin() );

	it = std::remove_if(info_list.begin(), info_list.end(), std::not1( inner::Fn_comape_no_name() ) );
	info_list.resize( it - info_list.begin() );
}








//- (shop type) ----------------------//
void st::get_weapon_attribute(SHOP_GOODS * pGoods, UINT32 & type, UINT32 & usage)
{
	type = UINT_MAX;	usage = UINT_MAX;

	CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(pGoods->_ItemID);
	if(pInfo == nullptr)	return;

	//weapon은 usage가 대분류로 가네? 그런가봐.. ㅋㅋ
	usage = pInfo->GetTypeClass();
	type = pInfo->GetTypeUsage();
}
void st::get_character_attribute(SHOP_GOODS * pGoods, UINT32 & type, UINT32 & usage)
{
	type = UINT_MAX;	usage = UINT_MAX;

	type = GET_ITEM_TYPE(pGoods->_ItemID);
	usage = GET_ITEM_SUBTYPE(pGoods->_ItemID);
}
void st::get_item_attribute(SHOP_GOODS * pGoods, UINT32 & type, UINT32 & usage)
{
	type = UINT_MAX;	usage = UINT_MAX;

	//세트 아이템은 단품이 묶인거다. 하지만, ShopPackageGoodsInfo 여기서 셋트 아이템을 찾지 못한다고 세트 아이템이 아니라고 말 할수 없다.
	if( usf::IsSetItem(pGoods) == true )
	{
		ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(pGoods->_GoodsID);
		if(pInfo != nullptr)
		{
			type = pInfo->GetCatagory(); 
			//usage = pInfo->GetType();
			return;
		}
	}

	ShopItemInfo* pInfo = g_pShopItemInfoDataBase->getInfo(pGoods->_ItemID);
	if(pInfo == nullptr) return;

	type = pInfo->GetCatagory();
	usage = pInfo->GetType();
}

//- (filter weapon) ----------------------//
void fw::filter_primary(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT8 slot_type)
{
	shop_list.clear();

	size_t size = src_list.size();
	for(size_t i=0;i<size;++i)
	{
		if( !filter_slot_type::pf_filter[slot_type](src_list[i]) )
			continue;

		if( src_list[i].type == WEAPON_SLOT_PRIMARY)
			shop_list.push( src_list[i] );
	}
}

void fw::filter_assault(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT8 slot_type)
{
	shop_list.clear();

	size_t size = src_list.size();
	for(size_t i=0;i<size;++i)
	{
		if( !filter_slot_type::pf_filter[slot_type](src_list[i]) )
			continue;

		if( src_list[i].usage == WEAPON_CLASS_ASSAULT )
			shop_list.push( src_list[i] );
	}
}
void fw::filter_smg(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT8 slot_type)
{
	shop_list.clear();

	size_t size = src_list.size();
	for(size_t i=0;i<size;++i)
	{
		if( !filter_slot_type::pf_filter[slot_type](src_list[i]) )
			continue;

		if( src_list[i].usage == WEAPON_CLASS_SMG || src_list[i].usage == WEAPON_CLASS_DUALSMG )
			shop_list.push( src_list[i] );
	}
}
void fw::filter_sniper(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT8 slot_type)
{
	shop_list.clear();

	size_t size = src_list.size();
	for(size_t i=0;i<size;++i)
	{
		if( !filter_slot_type::pf_filter[slot_type](src_list[i]) )
			continue;

		if( src_list[i].usage == WEAPON_CLASS_SNIPER )
			shop_list.push( src_list[i] );
	}
}
void fw::filter_machine(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT8 slot_type)
{
	shop_list.clear();

	size_t size = src_list.size();
	for(size_t i=0;i<size;++i)
	{
		if( !filter_slot_type::pf_filter[slot_type](src_list[i]) )
			continue;

		if( src_list[i].usage == WEAPON_CLASS_MG )
			shop_list.push( src_list[i] );
	}
}
void fw::filter_shotgun(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT8 slot_type)
{
	shop_list.clear();

	size_t size = src_list.size();
	for(size_t i=0;i<size;++i)
	{
		if( !filter_slot_type::pf_filter[slot_type](src_list[i]) )
			continue;

		if( src_list[i].usage == WEAPON_CLASS_SHOTGUN || src_list[i].usage == WEAPON_CLASS_DUALSHOTGUN )
			shop_list.push( src_list[i] );
	}
}

//- (filter goods) ----------------------//
void fg::filter_weapon(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT32 filter_type, UINT8 slot_type)
{
	shop_list.clear();

	size_t size = src_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (!filter_slot_type::pf_filter[slot_type](src_list[i]))
			continue;

		if (filter_type == UINT_MAX || src_list[i].type == filter_type)
			shop_list.push(src_list[i]);
	}
}

//- (filter goods) ----------------------//
void fg::filter_all_mask(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT32 filter_type, UINT8 slot_type)
{
	shop_list.clear();

	size_t size = src_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (!filter_slot_type::pf_filter[slot_type](src_list[i]))
			continue;

		switch (filter_type)
		{
		case item_def::MASKNEWHOT_WEAPON:
			if( src_list[i].category == item_def::CTG_WEAPON)
			{
				shop_list.push(src_list[i]);
			}

			break;
		case item_def::MASKNEWHOT_CHARA:
			if (src_list[i].category == item_def::CTG_CHARA
				&& src_list[i].type == ITEM_TYPE_CHARA)
			{
				shop_list.push(src_list[i]);
			}
			break;
		case item_def::MASKNEWHOT_SKIN:
			if (src_list[i].type == ITEM_TYPE_SKIN)
			{
				shop_list.push(src_list[i]);
			}
			break;
		case item_def::MASKNEWHOT_PARTS:
			if (src_list[i].type == ITEM_TYPE_HEADGEAR
				|| src_list[i].type == ITEM_TYPE_FACEGEAR
				|| src_list[i].type == ITEM_TYPE_UPPER
				|| src_list[i].type == ITEM_TYPE_LOWER
				|| src_list[i].type == ITEM_TYPE_GLOVE
				|| src_list[i].type == ITEM_TYPE_BELT
				|| src_list[i].type == ITEM_TYPE_HOLSTER
				|| src_list[i].type == ITEM_TYPE_BERET)
			{
				shop_list.push(src_list[i]);
			}
			break;
		case item_def::MASKNEWHOT_ITEM:
			if (src_list[i].category == item_def::CTG_ITEM)
			{
				shop_list.push(src_list[i]);
			}
			break;
		default:
			shop_list.push(src_list[i]);
			break;
		}

	}
}

//- (filter goods) ----------------------//
void fg::filter_parts_mask(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT32 filter_type, UINT8 slot_type)
{
	const CHARA_INFO* info = CCharaInfoContext::i()->GetMainCharaInfo();
	// vv3 - check
	T_ItemID charaID = info->getPartsInfo()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA); //현재 선택된 캐릭터

	size_t size = src_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (!filter_slot_type::pf_filter[slot_type](src_list[i]))
			continue;

		if (filter_type == UINT_MAX || src_list[i].type == filter_type)
		{
			if (src_list[i].info->GetGoods(0)->GetSaleMark(usf::g_price_idx) == SHOP_ITEM_TYPE_HOT
				|| src_list[i].info->GetGoods(0)->GetSaleMark(usf::g_price_idx) == SHOP_ITEM_TYPE_NEW)
			{

				// 공유 파츠 체크
				for (INT32 j = 0; j < src_list[i].info->GetGoodsCount(); j++)
				{
					SHOP_GOODS * pGoods = src_list[i].info->GetGoods(j);
					if (pGoods->_ItemID == 0) continue;

					if (item_def::is_shared_parts_item(pGoods->_ItemID, charaID))
					{
						shop_list.push(src_list[i]);
						break;
					}
				}

			}
		}
	}
}

void fg::filter_skin_mask(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT32 filter_type, UINT8 slot_type)
{
	const CHARA_INFO* pInfo = CCharaInfoContext::i()->GetMainCharaInfo();
	// vv3 - check
	T_ItemID charaID = pInfo->getPartsInfo()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA); //현재 선택된 캐릭터

																						  // 스킨은 현재 설정된 캐릭터 타입에 맞춰 필터링한다.
	size_t size = src_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (!filter_slot_type::pf_filter[slot_type](src_list[i]))
			continue;

		if (src_list[i].info->GetGoods(0)->GetSaleMark(usf::g_price_idx) == SHOP_ITEM_TYPE_HOT
			|| src_list[i].info->GetGoods(0)->GetSaleMark(usf::g_price_idx) == SHOP_ITEM_TYPE_NEW)
		{
			// 공유 스킨 체크
			for (INT32 j = 0; j < src_list[i].info->GetGoodsCount(); j++)
			{
				SHOP_GOODS * pGoods = src_list[i].info->GetGoods(j);
				if (pGoods->_ItemID == 0) continue;

				if (item_def::is_shared_parts_item(pGoods->_ItemID, charaID))
				{
					shop_list.push(src_list[i]);
					break;
				}
			}
		}

	}
}

void fg::filter_chara(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT32 filter_type, UINT8 slot_type)
{
	shop_list.clear();

	size_t size = src_list.size();
	for(size_t i=0;i<size;++i)
	{
		//if( src_list[i].type != ITEM_TYPE_CHARA)		continue;

		switch(filter_type)
		{
		case UINT_MAX: //전체 아이템
			shop_list.push( src_list[i] );
			break;

		case item_def::CT_MAN:	case item_def::CT_WOMAN:
			{
				T_ItemID item_id = src_list[i].info->GetGoods(0)->_ItemID;
				const CCharaInfo* pInfo = g_pCharaInfoDataBase->GetCharaDBInfo( item_id);

				if( pInfo != nullptr)
				{
					if( (pInfo->GetCharaSexTypeDISPLAY()) == (INT32)filter_type )
						shop_list.push( src_list[i] );
				}
			}
			break;
		default:
			{
				if (src_list[i].type == filter_type)
					shop_list.push(src_list[i]);
			}
			break;
		}
	}
}

void fg::filter_chara_V10(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT32 filter_type, UINT8 slot_type)
{
	shop_list.clear();

	size_t size = src_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		//if( src_list[i].type != ITEM_TYPE_CHARA)		continue;

		switch (filter_type)
		{
			case UINT_MAX: //전체 아이템
				shop_list.push(src_list[i]);
				break;

			case FILTER_CHARA_TYPE_RED:	case FILTER_CHARA_TYPE_BLUE:
			{
				T_ItemID item_id = src_list[i].info->GetGoods(0)->_ItemID;
				const CCharaInfo* pInfo = g_pCharaInfoDataBase->GetCharaDBInfo(item_id);

				if (pInfo != nullptr && pInfo->GetHP() > 100)
				{
					CHARACTER_TEAM_TYPE team = CHARA::ItemID2Team(item_id) + 1;
					if (team == (INT32)filter_type)
						shop_list.push(src_list[i]);
				}
			}
			break;
			case FILTER_CHARA_TYPE_NORMAL:
			{
				T_ItemID item_id = src_list[i].info->GetGoods(0)->_ItemID;
				const CCharaInfo* pInfo = g_pCharaInfoDataBase->GetCharaDBInfo(item_id);

				if (pInfo != nullptr && pInfo->GetHP() == 100)
				{
					shop_list.push(src_list[i]);
				}
			}
			break;
			case FILTER_CHARA_TYPE_HEADGEAR:
			{
				if (src_list[i].type == ITEM_TYPE_BERET
					|| src_list[i].type == ITEM_TYPE_HEADGEAR)
					shop_list.push(src_list[i]);
			}
			break;
			default:
			{
				if (src_list[i].type == filter_type)
					shop_list.push(src_list[i]);
			}
		}
	}
}

void fg::filter_parts(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT32 filter_type, UINT8 slot_type)
{
	filter_type = (UINT)item_def::convert_parts_type( (item_def::PartsType)filter_type );

	shop_list.clear();

	const CHARA_INFO* info = CCharaInfoContext::i()->GetMainCharaInfo();
	// vv3 - check
	T_ItemID charaID = info->getPartsInfo()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA); //현재 선택된 캐릭터

	size_t size = src_list.size();
	for(size_t i=0;i<size;++i)
	{
		if( src_list[i].type == ITEM_TYPE_CHARA || src_list[i].type == ITEM_TYPE_DINO )	continue;

		if( !filter_slot_type::pf_filter[slot_type](src_list[i]) )
			continue;

		// 공유 파츠 체크
		for( INT32 j = 0; j < src_list[i].info->GetGoodsCount(); j++)
		{
			SHOP_GOODS * pGoods = src_list[i].info->GetGoods( j);
			if( pGoods->_ItemID == 0) continue;

			if( item_def::is_shared_parts_item(pGoods->_ItemID, charaID) )
			{
				item_def::PartsType parts_type = item_def::convert_parts_type( item_def::get_parts_type(pGoods->_ItemID) );
				UINT32 class_type = (UINT32)item_def::convert_parts_type(parts_type);

				if( class_type == filter_type )
				{
					shop_list.push( src_list[i] );
					break;
				}
			}	
		}
	}
}

void fg::filter_item(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT32 filter_type, UINT8 slot_type)
{
	shop_list.clear();

	size_t size = src_list.size();
	for(size_t i=0;i<size;++i)
	{
		if( !filter_slot_type::pf_filter[slot_type](src_list[i]) )
			continue;

		switch(filter_type)
		{
		case UINT_MAX: //전체 아이템
			shop_list.push( src_list[i] );
			break;
		case item_def::IT_ENHANCE: case item_def::IT_FEATURE: case item_def::IT_REWARD:
		case item_def::IT_BOX: case item_def::IT_SET:
			if( src_list[i].type == filter_type )
				shop_list.push( src_list[i] );
			break;
		}
	}
}

void fg::filter_skin( shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT32 filter_type, UINT8 slot_type)
{
	shop_list.clear();

	const CHARA_INFO* pInfo = CCharaInfoContext::i()->GetMainCharaInfo();
	// vv3 - check
	T_ItemID charaID = pInfo->getPartsInfo()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA); //현재 선택된 캐릭터

	// 스킨은 현재 설정된 캐릭터 타입에 맞춰 필터링한다.
	size_t size = src_list.size();
	for(size_t i=0;i<size;++i)
	{
		if( !filter_slot_type::pf_filter[slot_type](src_list[i]) )
			continue;

		// 공유 스킨 체크
		for( INT32 j = 0; j < src_list[i].info->GetGoodsCount(); j++)
		{
			SHOP_GOODS * pGoods = src_list[i].info->GetGoods( j);
			if( pGoods->_ItemID == 0) continue;

			if( item_def::is_shared_parts_item(pGoods->_ItemID, charaID) )
			{
				shop_list.push( src_list[i] );
				break;
			}
		}
	}
}

void fg::filter_dino(shop_util::ShopPackInfoList & shop_list, const shop_util::ShopPackInfoList& src_list, UINT32 filter_type, UINT8 slot_type)
{
	shop_list.clear();

	size_t size = src_list.size();
	for(size_t i=0;i<size;++i)
	{
		if( !filter_slot_type::pf_filter[slot_type](src_list[i]) )
			continue;

		if( src_list[i].type == ITEM_TYPE_DINO )
			shop_list.push( src_list[i] );
	}
}

bool filter_slot_type::filter_shop(const shop_util::ShopInfo & data)
{
	return (data.sale_type == GOODS_SALE_ALL) || (data.sale_type == GOODS_SALE_BUY);
}

bool filter_slot_type::filter_gift(const shop_util::ShopInfo & data)
{
	return (data.sale_type == GOODS_SALE_ALL) || (data.sale_type == GOODS_SALE_GIFT);
}


#include "i3Base/string/compare/generic_iless.h"

namespace sort_f	
{
	namespace inner
	{
		struct fn_name
		{
			bool operator()(const shop_util::ShopInfo & lhs, const shop_util::ShopInfo & rhs) const
			{
				i3::rc_wstring lhs_name; usf::GetShopItemName(lhs_name, lhs.info);
				i3::rc_wstring rhs_name; usf::GetShopItemName(rhs_name, rhs.info);
				
				return i3::generic_iless()(lhs_name, rhs_name); 
			}
		};

		void re_status(ItemStatus & status)
		{
			if(status == ITEM_STATUS_NOHAVE) status = NUM_ITEM_STATUS;
		}

		struct fn_status_name
		{
			bool operator()(const shop_util::ShopInfo & lhs, const shop_util::ShopInfo & rhs) const
			{
				ItemStatus lhs_status = usf::GetItemStatus( lhs.info ); sort_f::inner::re_status(lhs_status);
				ItemStatus rhs_status = usf::GetItemStatus( rhs.info ); sort_f::inner::re_status(rhs_status);

				if(lhs_status == rhs_status ) return sort_f::inner::fn_name()(lhs, rhs);
				return lhs_status < rhs_status;
			}
		};

		struct fn_weapon_usage_name
		{
			void re_usage(WEAPON_CLASS_TYPE & usage) const
			{
				switch( usage )
				{
				case  WEAPON_CLASS_ASSAULT		:	usage = static_cast<WEAPON_CLASS_TYPE>(0); break;
				case  WEAPON_CLASS_SMG			:	
				case  WEAPON_CLASS_DUALSMG		:	usage = static_cast<WEAPON_CLASS_TYPE>(1); break;
				case  WEAPON_CLASS_SNIPER		:	usage = static_cast<WEAPON_CLASS_TYPE>(2); break;
				case  WEAPON_CLASS_MG			:	usage = static_cast<WEAPON_CLASS_TYPE>(3); break;
				case  WEAPON_CLASS_SHOTGUN		:	
				case  WEAPON_CLASS_DUALSHOTGUN	:	usage = static_cast<WEAPON_CLASS_TYPE>(4); break;
				}
			}

			bool operator()(const shop_util::ShopInfo & lhs, const shop_util::ShopInfo & rhs) const
			{
				WEAPON_CLASS_TYPE lhs_usage = (WEAPON_CLASS_TYPE)lhs.usage; re_usage(lhs_usage);
				WEAPON_CLASS_TYPE rhs_usage = (WEAPON_CLASS_TYPE)rhs.usage; re_usage(rhs_usage);

				if( lhs_usage == rhs_usage ) return sort_f::inner::fn_name()(lhs, rhs);
				return lhs_usage < rhs_usage;
			}
		};

		struct fn_weapon_slot_name
		{
			bool operator()(const shop_util::ShopInfo & lhs, const shop_util::ShopInfo & rhs) const
			{
				WEAPON_SLOT_TYPE lhs_slot = (WEAPON_SLOT_TYPE)lhs.type;
				WEAPON_SLOT_TYPE rhs_slot = (WEAPON_SLOT_TYPE)rhs.type;

				if(lhs_slot == WEAPON_SLOT_PRIMARY && rhs_slot == WEAPON_SLOT_PRIMARY) return sort_f::inner::fn_weapon_usage_name()(lhs, rhs);
				if( lhs_slot == rhs_slot ) return sort_f::inner::fn_name()(lhs, rhs);
				return lhs_slot < rhs_slot;
			}
		};

		struct fn_weapon_damage_name
		{
			bool operator()(const shop_util::ShopInfo & lhs, const shop_util::ShopInfo & rhs) const
			{
				SHOP_GOODS * pGoodslhs = lhs.info->GetGoods(0);
				SHOP_GOODS * pGoodsrhs = rhs.info->GetGoods(0);
				i3::stack_wstring wstrRange;
				i3::stack_wstring wstrBulletCount;
				INT32 damagelhs = 0, damagerhs = 0, rapid = 0, accuracy = 0, recoil_v = 0, recoil_h = 0, speed = 0;
				GameUI::GetNormalizedWeaponInfo(pGoodslhs->_ItemID,
					damagelhs, rapid, accuracy,
					recoil_v, recoil_h, speed,
					wstrRange, wstrBulletCount);

				GameUI::GetNormalizedWeaponInfo(pGoodsrhs->_ItemID,
					damagerhs, rapid, accuracy,
					recoil_v, recoil_h, speed,
					wstrRange, wstrBulletCount);

				if (damagelhs != damagerhs) return damagerhs < damagelhs;

				return sort_f::inner::fn_name()(lhs, rhs);
			}
		};

		struct fn_weapon_rapid_name
		{
			bool operator()(const shop_util::ShopInfo & lhs, const shop_util::ShopInfo & rhs) const
			{
				SHOP_GOODS * pGoodslhs = lhs.info->GetGoods(0);
				SHOP_GOODS * pGoodsrhs = rhs.info->GetGoods(0);
				i3::stack_wstring wstrRange;
				i3::stack_wstring wstrBulletCount;
				INT32 rapidlhs = 0, rapidrhs = 0, damage = 0, accuracy = 0, recoil_v = 0, recoil_h = 0, speed = 0;
				GameUI::GetNormalizedWeaponInfo(pGoodslhs->_ItemID,
					damage, rapidlhs, accuracy,
					recoil_v, recoil_h, speed,
					wstrRange, wstrBulletCount);

				GameUI::GetNormalizedWeaponInfo(pGoodsrhs->_ItemID,
					damage, rapidrhs, accuracy,
					recoil_v, recoil_h, speed,
					wstrRange, wstrBulletCount);

				if (rapidlhs != rapidrhs) return rapidrhs < rapidlhs;

				return sort_f::inner::fn_name()(lhs, rhs);
			}
		};

		struct fn_weapon_speed_name
		{
			bool operator()(const shop_util::ShopInfo & lhs, const shop_util::ShopInfo & rhs) const
			{
				SHOP_GOODS * pGoodslhs = lhs.info->GetGoods(0);
				SHOP_GOODS * pGoodsrhs = rhs.info->GetGoods(0);
				i3::stack_wstring wstrRange;
				i3::stack_wstring wstrBulletCount;
				INT32 speedlhs = 0, speedrhs = 0, damage = 0, accuracy = 0, recoil_v = 0, recoil_h = 0, rapid = 0;
				GameUI::GetNormalizedWeaponInfo(pGoodslhs->_ItemID,
					damage, rapid, accuracy,
					recoil_v, recoil_h, speedlhs,
					wstrRange, wstrBulletCount);

				GameUI::GetNormalizedWeaponInfo(pGoodsrhs->_ItemID,
					damage, rapid, accuracy,
					recoil_v, recoil_h, speedrhs,
					wstrRange, wstrBulletCount);

				if (speedlhs != speedrhs) return speedrhs < speedlhs;

				return sort_f::inner::fn_name()(lhs, rhs);
			}
		};

		struct fn_weapon_point_name
		{
			bool operator()(const shop_util::ShopInfo & lhs, const shop_util::ShopInfo & rhs) const
			{
				GOODS_PRICE_INFO infolhs, inforhs;
				usf::GetShopGoodInfo(lhs.info->GetGoods(0), infolhs);
				usf::GetShopGoodInfo(rhs.info->GetGoods(0), inforhs);

				if (infolhs._Point != inforhs._Point) return inforhs._Point < infolhs._Point;

				return sort_f::inner::fn_name()(lhs, rhs);
			}
		};

		struct fn_weapon_cash_name
		{
			bool operator()(const shop_util::ShopInfo & lhs, const shop_util::ShopInfo & rhs) const
			{
				GOODS_PRICE_INFO infolhs, inforhs;
				usf::GetShopGoodInfo(lhs.info->GetGoods(0), infolhs);
				usf::GetShopGoodInfo(rhs.info->GetGoods(0), inforhs);

				if (infolhs._Cash != inforhs._Cash) return inforhs._Cash < infolhs._Cash;

				return sort_f::inner::fn_name()(lhs, rhs);
			}
		};

		struct fn_mark_status_category_name
		{
			bool operator()(const shop_util::ShopInfo & lhs, const shop_util::ShopInfo & rhs) const
			{
				UINT32 lhs_type = GET_ITEM_TYPE(lhs.info->GetGoods(0)->_ItemID);
				UINT32 rhs_type = GET_ITEM_TYPE(rhs.info->GetGoods(0)->_ItemID);

				if (lhs_type != rhs_type) return lhs_type < rhs_type;

				return sort_f::inner::fn_status_name()(lhs, rhs);
			}
		};

		struct fn_mark_status_name
		{
			bool operator()(const shop_util::ShopInfo & lhs, const shop_util::ShopInfo & rhs) const
			{
				UINT8 lhs_mark = lhs.info->GetGoods(0)->GetSaleMark(usf::g_price_idx); 
				UINT8 rhs_mark = rhs.info->GetGoods(0)->GetSaleMark(usf::g_price_idx);

				if(lhs_mark == rhs_mark) return sort_f::inner::fn_mark_status_category_name()(lhs, rhs);

				const UINT8 l_priority = CShopContext::i()->GetTagPriority((SHOP_ITEM_TYPE)lhs_mark);
				const UINT8 r_priority = CShopContext::i()->GetTagPriority((SHOP_ITEM_TYPE)rhs_mark);

				return l_priority < r_priority;
			}
		};

		struct fn_status_type_name
		{
			bool operator()(const shop_util::ShopInfo & lhs, const shop_util::ShopInfo & rhs) const
			{
				ItemStatus lhs_status = usf::GetItemStatus( lhs.info ); sort_f::inner::re_status(lhs_status);
				ItemStatus rhs_status = usf::GetItemStatus( rhs.info ); sort_f::inner::re_status(rhs_status);

				if( lhs_status == rhs_status ) return sort_f::inner::fn_weapon_slot_name()(lhs, rhs);
				return lhs_status < rhs_status;
			}
		};

		struct fn_mark_status_weapon_default
		{
			bool operator()(const shop_util::ShopInfo & lhs, const shop_util::ShopInfo & rhs) const
			{
				UINT8 lhs_mark = lhs.info->GetGoods(0)->GetSaleMark(usf::g_price_idx); 
				UINT8 rhs_mark = rhs.info->GetGoods(0)->GetSaleMark(usf::g_price_idx);
				
				if( lhs_mark == rhs_mark ) return sort_f::inner::fn_status_type_name()(lhs, rhs);

				const UINT8 l_priority = CShopContext::i()->GetTagPriority((SHOP_ITEM_TYPE)lhs_mark);
				const UINT8 r_priority = CShopContext::i()->GetTagPriority((SHOP_ITEM_TYPE)rhs_mark);

				return l_priority < r_priority; 	
			}
		};
	}

	//-------------------------------////-------------------------------//
	void mark_status_name(shop_util::ShopPackInfoList& shop_list)
	{
		i3::vector< shop_util::ShopInfo >& list = const_cast< i3::vector< shop_util::ShopInfo >& >( shop_list.get_list() );
		std::sort( list.begin(), list.end(), sort_f::inner::fn_mark_status_name() );
	}
	void mark_status_type_name(shop_util::ShopPackInfoList& shop_list)
	{
		i3::vector< shop_util::ShopInfo >& list = const_cast< i3::vector< shop_util::ShopInfo >& >( shop_list.get_list() );
		std::sort( list.begin(), list.end(), sort_f::inner::fn_mark_status_weapon_default() );
	}

	void shop_sort_weapon(shop_util::ShopPackInfoList& shop_list, SHOP_SORT_TYPE type)
	{
		i3::vector< shop_util::ShopInfo >& list = const_cast<i3::vector< shop_util::ShopInfo >&>(shop_list.get_list());

		switch (type)
		{
		case SHOP_SORT_TYPE_NORMAL:
			CShopContext::i()->SetTagPriorityType(SHOP_ITEM_TYPE_NORMAL);
			std::sort(list.begin(), list.end(), sort_f::inner::fn_mark_status_weapon_default());
			break;
		case SHOP_SORT_TYPE_NEW:
			CShopContext::i()->SetTagPriorityType(SHOP_ITEM_TYPE_NEW);
			std::sort(list.begin(), list.end(), sort_f::inner::fn_mark_status_weapon_default());
			break;
		case SHOP_SORT_TYPE_HOT:
			CShopContext::i()->SetTagPriorityType(SHOP_ITEM_TYPE_HOT);
			std::sort(list.begin(), list.end(), sort_f::inner::fn_mark_status_weapon_default());
			break;
		case SHOP_SORT_TYPE_NAME:
			std::sort(list.begin(), list.end(), sort_f::inner::fn_name());
			break;
		case SHOP_SORT_TYPE_DAMAGE:
			std::sort(list.begin(), list.end(), sort_f::inner::fn_weapon_damage_name());
			break;
		case SHOP_SORT_TYPE_RAPID:
			std::sort(list.begin(), list.end(), sort_f::inner::fn_weapon_rapid_name());
			break;
		case SHOP_SORT_TYPE_SPEED:
			std::sort(list.begin(), list.end(), sort_f::inner::fn_weapon_speed_name());
			break;
		case SHOP_SORT_TYPE_POINT:
			std::sort(list.begin(), list.end(), sort_f::inner::fn_weapon_point_name());
			break;
		case SHOP_SORT_TYPE_CASH:
			std::sort(list.begin(), list.end(), sort_f::inner::fn_weapon_cash_name());
			break;
		default:
			break;
		}
	}
	void shop_sort_item(shop_util::ShopPackInfoList& shop_list, SHOP_SORT_TYPE type)
	{
		i3::vector< shop_util::ShopInfo >& list = const_cast<i3::vector< shop_util::ShopInfo >&>(shop_list.get_list());

		switch (type)
		{
		case SHOP_SORT_TYPE_NORMAL:
			CShopContext::i()->SetTagPriorityType(SHOP_ITEM_TYPE_NORMAL);
			std::sort(list.begin(), list.end(), sort_f::inner::fn_mark_status_name());
			break;
		case SHOP_SORT_TYPE_NEW:
			CShopContext::i()->SetTagPriorityType(SHOP_ITEM_TYPE_NEW);
			std::sort(list.begin(), list.end(), sort_f::inner::fn_mark_status_name());
			break;
		case SHOP_SORT_TYPE_HOT:
			CShopContext::i()->SetTagPriorityType(SHOP_ITEM_TYPE_HOT);
			std::sort(list.begin(), list.end(), sort_f::inner::fn_mark_status_name());
			break;
		case SHOP_SORT_TYPE_NAME:
			std::sort(list.begin(), list.end(), sort_f::inner::fn_name());
			break;
		case SHOP_SORT_TYPE_DAMAGE:
			std::sort(list.begin(), list.end(), sort_f::inner::fn_weapon_damage_name());
			break;
		case SHOP_SORT_TYPE_RAPID:
			std::sort(list.begin(), list.end(), sort_f::inner::fn_weapon_rapid_name());
			break;
		case SHOP_SORT_TYPE_SPEED:
			std::sort(list.begin(), list.end(), sort_f::inner::fn_weapon_speed_name());
			break;
		case SHOP_SORT_TYPE_POINT:
			std::sort(list.begin(), list.end(), sort_f::inner::fn_weapon_point_name());
			break;
		case SHOP_SORT_TYPE_CASH:
			std::sort(list.begin(), list.end(), sort_f::inner::fn_weapon_cash_name());
			break;
		default:
			break;
		}

	}


}