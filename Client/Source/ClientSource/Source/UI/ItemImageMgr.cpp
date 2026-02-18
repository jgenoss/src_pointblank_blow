#include "pch.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"

void ConvertItemIDIntoImageCategory(PBITEM_IMAGE_TYPE& out_main, T_ItemID itemID);

void ConvertWeaponItemIDIntoShapeIndex(INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, T_ItemID itemID);
void ConvertCharacItemIDIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, T_ItemID itemID);
void ConvertPartItemIDIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, T_ItemID itemID);
void ConvertItemItemIDIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, T_ItemID itemID);
void ConvertErrorItemIDIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, T_ItemID itemID);
void (*g_ConvertItemIDIntoShapeIndex[NUM_PBITEM + 1])(INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, T_ItemID itemID) = 
{
	ConvertWeaponItemIDIntoShapeIndex, ConvertCharacItemIDIntoShapeIndex,
	ConvertPartItemIDIntoShapeIndex,
	ConvertItemItemIDIntoShapeIndex,
	ConvertErrorItemIDIntoShapeIndex
};


void ConvertGoodsIntoImageCategory( PBITEM_IMAGE_TYPE& out_main, const SHOP_GOODS* goods );

void ConvertWeaponGoodsIntoShapeIndex(INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, const SHOP_GOODS* goods);
void ConvertCharacGoodsIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, const SHOP_GOODS* goods);
void ConvertPartsGoodsIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, const SHOP_GOODS* goods);
void ConvertItemGoodsIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, const SHOP_GOODS* goods);
void ConvertErrorGoodsIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, const SHOP_GOODS* goods);
void (*g_ConvertGoodsIntoShapeIndex[NUM_PBITEM + 1])(INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, const SHOP_GOODS* goods) = 
{
	ConvertWeaponGoodsIntoShapeIndex, ConvertCharacGoodsIntoShapeIndex,
	ConvertPartsGoodsIntoShapeIndex,
	ConvertItemGoodsIntoShapeIndex,
	ConvertErrorGoodsIntoShapeIndex
};

/****************************//****************************/
void ConvertItemIDIntoImageCategory(PBITEM_IMAGE_TYPE& out_main, T_ItemID itemID)
{
	INT32 item_class = GET_ITEM_TYPE(itemID);

	switch(item_class)
	{
	case ITEM_TYPE_PRIMARY:
	case ITEM_TYPE_SECONDARY:
	case ITEM_TYPE_MELEE:
	case ITEM_TYPE_THROWING1:
	case ITEM_TYPE_THROWING2:
		out_main = PBITEM_WEAPON;
		break;

	case ITEM_TYPE_CHARA:
	case ITEM_TYPE_DINO:
		out_main = PBITEM_CHARACTER;
		break;

	case ITEM_TYPE_HEADGEAR:
	case ITEM_TYPE_FACEGEAR:
	case ITEM_TYPE_UPPER:			// 상의
	case ITEM_TYPE_LOWER:			// 하의
	case ITEM_TYPE_GLOVE:			// 장갑
	case ITEM_TYPE_BELT:			// 벨트
	case ITEM_TYPE_HOLSTER:			// 권총집
	case ITEM_TYPE_SKIN:
	case ITEM_TYPE_BERET :			// 베레모
		out_main = PBITEM_PARTS;
		break;

	case ITEM_TYPE_POINT:
	case ITEM_TYPE_MAINTENANCE:
	case ITEM_TYPE_WRAP_PERIOD:
	case ITEM_TYPE_WRAP_COUNT:
	case ITEM_TYPE_INGAMEITEM:
	case ITEM_TYPE_DOMIITEM:
	case ITEM_TYPE_GACHA_TICKET:
		out_main = PBITEM_ITEM;
		break;

	default:
		out_main = NUM_PBITEM;			// 그냥 끝에다 붙인다...
		break;
	}
}

void ConvertGoodsIntoImageCategory( PBITEM_IMAGE_TYPE& out_main, const SHOP_GOODS* goods )
{
	if (usf::IsSetItem(goods) )			// 세트 아이템의 경우 완전이 다르게 동작..
	{
		ShopPackageGoodsInfo* info = g_pShopItemInfoDataBase->GetPackageGoodsInfo(goods->_GoodsID);
		if (info == nullptr)
		{
			out_main = NUM_PBITEM; return;
		}

		const i3::rc_wstring& wstrTexType = info->GetType();

		if (i3::generic_is_equal(wstrTexType, L"Set") == true ||
			i3::generic_is_equal(wstrTexType, L"Count") == true ||
			i3::generic_is_equal(wstrTexType, L"Perido") == true) out_main = PBITEM_ITEM;
		else
		if (i3::generic_is_equal(wstrTexType, L"Chara") == true || 
			i3::generic_is_equal(wstrTexType, L"Head") == true ||
			i3::generic_is_equal(wstrTexType, L"Beret") == true ||
			i3::generic_is_equal(wstrTexType, L"Mask") == true ) out_main = PBITEM_CHARACTER;
		else
			out_main = NUM_PBITEM;

		return;
	}

	T_ItemID itemID = goods->_ItemID;
	ConvertItemIDIntoImageCategory(out_main, itemID);
}

/****************************//****************************/
void ConvertWeaponItemIDIntoShapeIndex(INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, T_ItemID itemID)
{
	out_sub = out_shape_id = out_tex_index	= -1;

	CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(itemID);
	if( pInfo == nullptr )	return;

	out_sub = 0;		// 무기 이미지셋에 서브카테고리가 없음..
	out_tex_index = 0;		// 이것은 아이템탭 전용으로 무기에서 쓰이는 것은 아니다..

	INT32 ordIdx = g_pFramework->GetWeaponOrdIdx( pInfo->GetTypeClass(), pInfo->GetNumber());

	INT32 oldTexIdx =  ordIdx / 88;   // 완전 엉터리 같은 코드로 원본이미지가 88장이 된다면 이코드를 삭제할수 있다....
	INT32 oldOrdIdx = ordIdx % 88; 
	out_shape_id = 90 * oldTexIdx + oldOrdIdx;
}

void ConvertCharacItemIDIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, T_ItemID itemID)
{
	//out_sub = NUM_PBITEM_CHARAC;
	out_sub = GetUITemplateIndex_Chara("UITEMPLATE_MAX");
	out_shape_id = -1;
	out_tex_index = 0;
	
	switch( CHARA::ItemID2Type(itemID) )
	{
	case ITEM_TYPE_CHARA	:	out_sub = GetUITemplateIndex_Chara("MAIN");		break;	//PBITEM_CHARAC_MAIN;		break;
	case ITEM_TYPE_DINO		:	out_sub = GetUITemplateIndex_Chara("DINO");		break;	//PBITEM_CHARAC_DINO;		break;
	case ITEM_TYPE_HEADGEAR	:	out_sub = GetUITemplateIndex_Parts("HEADGEAR"); break; //PBITEM_PARTS_HEADGEAR;	break;
	case ITEM_TYPE_FACEGEAR	:	out_sub = GetUITemplateIndex_Parts("FACEGEAR"); break; //PBITEM_PARTS_FACEGEAR;	break;
	case ITEM_TYPE_UPPER	:	out_sub = GetUITemplateIndex_Parts("UPPER");	break; //PBITEM_PARTS_UPPER;		break;
	case ITEM_TYPE_LOWER	:	out_sub = GetUITemplateIndex_Parts("LOWER");	break; //PBITEM_PARTS_LOWER;		break;
	case ITEM_TYPE_GLOVE	:	out_sub = GetUITemplateIndex_Parts("GLOVE");	break; //PBITEM_PARTS_GLOVE;		break;
	case ITEM_TYPE_BELT		:	out_sub = GetUITemplateIndex_Parts("BELT");		break; //PBITEM_PARTS_BELT;		break;
	case ITEM_TYPE_HOLSTER	:	out_sub = GetUITemplateIndex_Parts("HOLSTER");	break; //PBITEM_PARTS_HOLSTER;		break;
	case ITEM_TYPE_SKIN		:	out_sub = GetUITemplateIndex_Parts("SKIN");		break;
	case ITEM_TYPE_BERET	:	out_sub = GetUITemplateIndex_Parts("BERET");	break;
	default:
		I3PRINTLOG(I3LOG_FATAL,  "insert code please!!!");
		break;
	}

	const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo(itemID);
	if( pInfo == 0 ) return;

	out_shape_id = pInfo->GetUIShapeIndex();
}

void ConvertPartItemIDIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, T_ItemID itemID)
{
	ConvertCharacItemIDIntoShapeIndex(out_sub, out_shape_id, out_tex_index, itemID);
}

void ConvertItemItemIDIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, T_ItemID itemID)
{
	//out_sub = NUM_PBITEM_ITEM;				
	out_sub = GetUITemplateIndex_Item("UITEMPLATE_MAX");
	out_shape_id = -1;
	out_tex_index = 0;
	
	const ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(itemID);
	if( pInfo == 0 ) return;

	ITEM_TYPE type = pInfo->GetType();

	switch ( type )
	{
	case ITEM_TYPE_MAINTENANCE:
	case ITEM_TYPE_WRAP_COUNT:
	case ITEM_TYPE_DOMIITEM:
	case ITEM_TYPE_GACHA_TICKET:
		{
			//out_sub = PBITEM_ITEM_EXPENDABLE;	
			out_sub = GetUITemplateIndex_Item("EXPENDABLE");
			out_shape_id = pInfo->GetShapeIndex();
			out_tex_index = pInfo->GetTexIndex();
			break;
		}

	case ITEM_TYPE_WRAP_PERIOD:
		{
			//out_sub = PBITEM_ITEM_MAINTENANCE;	
			out_sub = GetUITemplateIndex_Item("MAINTENANCE");
			out_shape_id = pInfo->GetShapeIndex();
			out_tex_index = pInfo->GetTexIndex();
			break; 
		}
	default:		
		break;
	}
}

void ConvertErrorItemIDIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, T_ItemID itemID)
{
	out_sub = 0;	out_tex_index = 0; out_shape_id = -1;
}

/****************************//****************************/

void ConvertWeaponGoodsIntoShapeIndex(INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, const SHOP_GOODS* goods)
{
	ConvertWeaponItemIDIntoShapeIndex(out_sub, out_shape_id, out_tex_index, goods->_ItemID);
}

void ConvertCharacGoodsIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, const SHOP_GOODS* goods)
{
	//out_sub = NUM_PBITEM_CHARAC;
	out_sub = GetUITemplateIndex_Chara("UITEMPLATE_MAX");
	out_shape_id = -1;
	out_tex_index = 0;

	if (usf::IsSetItem(goods) )			// 세트아이템의 경우, 아이템 아이디로 처리가 안된다..
	{
		ShopPackageGoodsInfo *ItemInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(goods->_GoodsID);

		I3ASSERT_RETURN(ItemInfo);

		const i3::rc_wstring& wstrTexType = ItemInfo->GetType();
		
		if (i3::generic_is_equal(wstrTexType, L"Chara"))
		{
			//out_sub = PBITEM_CHARAC_MAIN;
			out_sub = GetUITemplateIndex_Chara("MAIN");
			out_shape_id= ItemInfo->GetTexIndex() * 18 + ItemInfo->GetImageIndex();
		}

		return;
	}

	ConvertCharacItemIDIntoShapeIndex(out_sub, out_shape_id, out_tex_index, goods->_ItemID);
}

void ConvertPartsGoodsIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, const SHOP_GOODS* goods)
{
	ConvertCharacGoodsIntoShapeIndex(out_sub, out_shape_id, out_tex_index, goods);
}

void ConvertItemGoodsIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, const SHOP_GOODS* goods)
{
	//out_sub = NUM_PBITEM_ITEM;				
	out_sub = GetUITemplateIndex_Item("UITEMPLATE_MAX");
	out_shape_id = -1;
	out_tex_index = 0;
	
	if (usf::IsSetItem(goods) )			// 세트아이템의 경우, 아이템 아이디로 처리가 안된다..
	{
		const ShopPackageGoodsInfo *ItemInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(goods->_GoodsID);
		if( ItemInfo == 0 ) return;

		const i3::rc_wstring& wstrTexType = ItemInfo->GetType();
		
		if (i3::generic_is_equal(wstrTexType, L"Set"))
		{
			out_sub = GetUITemplateIndex_Item("SET");
		}
		else
		if (i3::generic_is_equal(wstrTexType, L"Count"))
		{
			out_sub = GetUITemplateIndex_Item("EXPENDABLE");
		}
		else 
		if	(i3::generic_is_equal(wstrTexType, L"Perido"))
		{
			out_sub = GetUITemplateIndex_Item("MAINTENANCE");
		}
		 
		out_shape_id = ItemInfo->GetImageIndex();
		out_tex_index = ItemInfo->GetTexIndex();

		return;
	}

	ConvertItemItemIDIntoShapeIndex(out_sub, out_shape_id, out_tex_index, goods->_ItemID);
}


void ConvertErrorGoodsIntoShapeIndex( INT32& out_sub, INT32& out_shape_id, INT32& out_tex_index, const SHOP_GOODS* goods)
{
	ConvertErrorItemIDIntoShapeIndex( out_sub, out_shape_id, out_tex_index, 0);
}



ItemImageMgr_Init::MainCategory::~MainCategory()
{
	i3::cu::for_each_delete(m_sub_category_list);
}



ItemImageMgr_Init::ItemImageMgr_Init()
{
	// 무기의 경우 서브카테고리가 별도로 없음..
	MainCategory* weapon_category = &m_main_categorys[PBITEM_WEAPON];
	SubCategory * pSub = new SubCategory;
	MEMDUMP_NEW( pSub, sizeof( SubCategory));
	weapon_category->m_sub_category_list.push_back( pSub);
}

ItemImageMgr_Init::~ItemImageMgr_Init()
{

}

void	ItemImageMgr_Init::AddWeaponTemplate( const char* szTemplate)
{
	i3UITemplate* tpl = i3UI::getUILibrary()->FindByName(szTemplate);
	MainCategory* main_category = &m_main_categorys[PBITEM_WEAPON];
	SubCategory* sub_category = main_category->m_sub_category_list[0];

	sub_category->m_tpl_list.push_back(tpl);		// 널값이 들어올수도 있다.. (이 부분도 처리를 고민)
}

void	ItemImageMgr_Init::AddCharacterTemplate(INT32 sub_category, INT32 looplood, const char* szTemplate)
{
	if (looplood == 1)
	{
		bool bloop = 1;
		INT32 i = 1;
		while (bloop)
		{
			i3::string str;
			i3::sprintf(str, "%s%02d", szTemplate, i);
			i3UITemplate* tpl = i3UI::getUILibrary()->FindByName(str.c_str());
			if (tpl != nullptr)
			{
				MainCategory* main_category = &m_main_categorys[PBITEM_CHARACTER];
				ItemImageMgr_Init::_AddTemplate(main_category, sub_category, tpl);
				i++;
			}
			else
				bloop = false;

		}
	}
	else
	{
		i3UITemplate* tpl = i3UI::getUILibrary()->FindByName(szTemplate);
		MainCategory* main_category = &m_main_categorys[PBITEM_CHARACTER];
		ItemImageMgr_Init::_AddTemplate(main_category, sub_category, tpl);
	}
}

void	ItemImageMgr_Init::AddItemTemplate( INT32 sub_category, const char* szTemplate )
{
	i3UITemplate* tpl = i3UI::getUILibrary()->FindByName(szTemplate);
	MainCategory* main_category = &m_main_categorys[PBITEM_ITEM];
	ItemImageMgr_Init::_AddTemplate(main_category, sub_category, tpl);		
}

void	ItemImageMgr_Init::AddPartsTemplate( INT32 sub_category, INT32 looplood, const char* szTemplate )
{
	if (looplood == 1)
	{
		bool bloop = 1;
		INT32 i = 1;
		while (bloop)
		{
			i3::string str;
			i3::sprintf(str, "%s%d", szTemplate, i);
			i3UITemplate* tpl = i3UI::getUILibrary()->FindByName(str.c_str());
			if (tpl != nullptr)
			{
				MainCategory* main_category = &m_main_categorys[PBITEM_PARTS];
				ItemImageMgr_Init::_AddTemplate(main_category, sub_category, tpl);
				i++;
			}
			else
				bloop = false;

		}
	}
	else
	{
		i3UITemplate* tpl = i3UI::getUILibrary()->FindByName(szTemplate);
		MainCategory* main_category = &m_main_categorys[PBITEM_PARTS];
		ItemImageMgr_Init::_AddTemplate(main_category, sub_category, tpl);
	}
}

void	ItemImageMgr_Init::_AddTemplate(MainCategory* main_category, INT32 sub_category, i3UITemplate* tpl)	 // 공통루틴..
{
	i3::vector<SubCategory*>& sub_list = main_category->m_sub_category_list;

	const INT32 count = (INT32)sub_list.size();

	if ( sub_category >= count )
	{
		for ( INT32 i = count ; i < sub_category ; ++i)
		{
			SubCategory * p = new SubCategory;
			MEMDUMP_NEW(p, sizeof( SubCategory));
			sub_list.push_back( p);				// 빈공간없이 모두 중간클래스 객체를 생성..
		}

		SubCategory* pSub =  new SubCategory;
		MEMDUMP_NEW( pSub, sizeof( SubCategory));

		sub_list.push_back(pSub);
		pSub->m_tpl_list.push_back(tpl);
	}
	else
	{
		SubCategory* pSub = sub_list[sub_category];
		pSub->m_tpl_list.push_back(tpl);
	}
}

ItemImageMgr::ItemImageMgr()
{

}

ItemImageMgr::~ItemImageMgr()
{

}

void	ItemImageMgr::Initialize(const ItemImageMgr_Init& init)
{
	// 매핑 실행..
	INT32	acc_index = 0;
	INT32	base_tpl_count = 0;

	for (INT32 i = 0 ; i < NUM_PBITEM ; ++i )
	{
		MainCategoryIdx* main_idx_list = &m_idx_offset_list[i];
		const ItemImageMgr_Init::MainCategory* main_category = init.GetMainCategory(i);

		i3::vector<SubCategoryIdx*>& sub_offset_list = main_idx_list->m_sub_category_idx_list;
		const i3::vector<ItemImageMgr_Init::SubCategory*>& sub_category_list = main_category->m_sub_category_list;
		const size_t num_sub_category = sub_category_list.size();

		for ( size_t j = 0 ; j < num_sub_category; ++j )
		{
			acc_index = 0;										// 각 서브카테고리별로 셰이프 인덱스가 0부터 시작하도록 조정한다..

			SubCategoryIdx* pSubCategoryIdx = new SubCategoryIdx;
			MEMDUMP_NEW( pSubCategoryIdx, sizeof(SubCategoryIdx));
			sub_offset_list.push_back(pSubCategoryIdx);				// 리스트를 새로 채움..
			
			pSubCategoryIdx->m_base_tpl_idx = base_tpl_count;

			ItemImageMgr_Init::SubCategory* sub_category = sub_category_list[j];

			const i3::vector<i3UITemplate*>& tpl_list = sub_category->m_tpl_list;
			const size_t num_tpl = tpl_list.size();

			for ( size_t k = 0 ; k < num_tpl ; ++k )					// 
			{
				i3UITemplate* tpl =	tpl_list[k];			// 만약 이것이 널값이라면 조금도 등록하지 않도록 처리..
				if (tpl)
				{
					acc_index += tpl->getShapeCount();
					pSubCategoryIdx->m_num_idx_list.push_back(acc_index);
					m_tpl_group.AddTemplate(tpl);
					++base_tpl_count;
				}
			}
		}
	}
	
}

ItemImageMgr::MainCategoryIdx::~MainCategoryIdx()
{
	i3::cu::for_each_delete(m_sub_category_idx_list );
}

void	ItemImageMgr::CalcIndexes(  INT32& outTplIdx, INT32& outImageIdx, T_ItemID itemID) const
{
	PBITEM_IMAGE_TYPE image_type;
	ConvertItemIDIntoImageCategory(image_type, itemID);
	
	INT32 image_sub_type;	INT32 shape_idx;	INT32 tex_index;
	g_ConvertItemIDIntoShapeIndex[image_type](image_sub_type, shape_idx, tex_index, itemID);
	
	ItemImageMgr::CalcIndexes_Impl(outTplIdx, outImageIdx, image_type, image_sub_type, shape_idx, tex_index);
}


void	ItemImageMgr::CalcIndexes( INT32& outTplIdx, INT32& outImageIdx, const SHOP_GOODS* goods) const
{
	// itemID를 갖고 기본카테고리->서브카테고리->내부리스트 모두 참조할수 있도록 처리할것...
	PBITEM_IMAGE_TYPE image_type;
	ConvertGoodsIntoImageCategory(image_type, goods);									// 기본 카테고리값 획득
	
	INT32 image_sub_type;	INT32 shape_idx;	INT32 tex_index;
	g_ConvertGoodsIntoShapeIndex[image_type]( image_sub_type, shape_idx, tex_index, goods);		// 서브 카테고리값과 기존 셰이프 인덱스를 얻음..
	
	ItemImageMgr::CalcIndexes_Impl(outTplIdx, outImageIdx, image_type, image_sub_type, shape_idx, tex_index);
}

void	ItemImageMgr::CalcIndexes_Impl(INT32& outTplIdx, INT32& outImageIdx, PBITEM_IMAGE_TYPE image_type, INT32 image_sub_type, INT32 shape_idx, INT32 tex_index) const
{
	outTplIdx = -1;
	outImageIdx = -1;

	if (shape_idx == -1)
		return;
	
	const MainCategoryIdx* main_category = &m_idx_offset_list[image_type];
	const INT32 num_sub_category =	(INT32)main_category->m_sub_category_idx_list.size();

	if ( image_sub_type >= num_sub_category )	// 주어진 카테고리타입리소스보다 더 적은 리스트갯수를 가진다면...없는 것으로 간주..
		return;
	
	SubCategoryIdx* sub_category = main_category->m_sub_category_idx_list[image_sub_type];
	i3::vector<INT32>& accum_list = sub_category->m_num_idx_list;

	if (tex_index > 0)			// 텍스쳐 인덱스가 주어지는 경우 ( 아이템탭의 경우..shape_idx를 강제로 늘려주어야한다..)
	{
		I3ASSERT( tex_index < (INT32)accum_list.size());
		INT32 tex_offset = accum_list[ tex_index - 1];
		shape_idx += tex_offset;
	}
	
	i3::vector<INT32>::iterator it = i3::upper_bound(accum_list.begin(), accum_list.end(), shape_idx);
	
	if ( it != accum_list.end() )
	{
		outTplIdx = (INT32) i3::distance(accum_list.begin(), it);
	}
	else
	{
		return;
	}
	
	INT32 offset = 0;
	if (outTplIdx > 0)
		offset = accum_list[ outTplIdx-1 ];

	outImageIdx = shape_idx - offset;
	outTplIdx += sub_category->m_base_tpl_idx;

	I3ASSERT(outImageIdx > -1);
}


void	ItemImageMgr::SelectImage( UIImgSetCtrl* ctrl, const SHOP_GOODS* goods) const
{
	INT32 tpl_idx;	INT32 image_idx;
	ItemImageMgr::CalcIndexes(tpl_idx, image_idx, goods);
	ctrl->SetImage(tpl_idx, image_idx);
}

void	ItemImageMgr::SelectImage( UIImgSetCtrl* ctrl, T_ItemID itemID) const		// itemID로도 처리가 되도록 다시 함수추가.
{
	INT32 tpl_idx;	INT32 image_idx;
	ItemImageMgr::CalcIndexes(tpl_idx, image_idx, itemID);
	ctrl->SetImage(tpl_idx, image_idx);
}

void	ItemImageMgr::SelectImage_UseTemplate( UIImgSetCtrl* ctrl, T_ItemID itemID, INT32 chara_sub_type) const
{
	INT32 shape_idx;	INT32 tex_index; INT32 image_sub_type;
	g_ConvertItemIDIntoShapeIndex[PBITEM_CHARACTER](image_sub_type, shape_idx, tex_index, itemID);
	image_sub_type = chara_sub_type;

	INT32 tpl_idx;	INT32 image_idx; 
	ItemImageMgr::CalcIndexes_Impl(tpl_idx, image_idx, PBITEM_CHARACTER, image_sub_type, shape_idx, tex_index);

	ctrl->SetImage(tpl_idx, image_idx);
}

UIImgSetCtrl*	ItemImageMgr::CreateImgSetCtrl(i3UIButtonImageSet* bound_rsc) const
{
	return m_tpl_group.CreateImgSetCtrl(bound_rsc);
}

UIImgSetCtrl*	ItemImageMgr::CreateImgSetCtrl(i3UIScene* scn, const char* bound_rsc) const
{
	return m_tpl_group.CreateImgSetCtrl(scn, bound_rsc);
}

