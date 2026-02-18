#include "pch.h"
#include "TestAutomationInterface.h"

#if defined( TEST_AUTOMATION)

#include "UI/UIDefine.h"
#include "UI/UIMainFrame.h"

#include "UI/UIShopInfoMgr.h"

#include "IGShopContext.h"
#include "ShopContext.h"

#include "i3Base/string/ext/contain_string_head.h"

#include "ClientStageInfoUtil.h"
#include "ClientStageInfoContext.h"

vector<wstring *> *	TestAutomationInterface::GetStageList(STAGE_MODE stageType, int count)
{
	/*
	모드 리뉴얼로 해당 부분으로는 스테이지 정보를 받을 수 없어 아래 함수로 대체 합니다.
	const STAGEID_INFO_LIST::StageIdInfoList& list =
		s_StageInfoTableList.GetStageIdInfoList();
	*/
	const i3::vector<CSI_STAGE>* stagelist = ClientStageInfoContext::i()->GetALLStageList();

	//char name[256]; 안써서 삭제 합니다.
	int cnt;
	
	//StageInfoList에 등록된 내용만 수정한다.
	//모드리뉴얼로 인해 정보 받는 부분이 바뀌어 수정합니다.
	//STAGEID_INFO_LIST::StageIdInfoList::const_iterator it = list.begin();
	//STAGEID_INFO_LIST::StageIdInfoList::const_iterator end = list.end();
	//it++;	//맨 처음은 사용하지 않는 맵 (STAGE_ORD_NA) 이다.

	i3::vector<CSI_STAGE>::const_iterator it	= stagelist->begin();
	i3::vector<CSI_STAGE>::const_iterator end	= stagelist->end();

	vector<wstring *> * pList = new vector<wstring *>;

	for ( cnt = 0;(it != end); it++)
	{
		//모드리뉴얼로 인해 정보 받는 부분이 바뀌어 수정합니다.
		//STAGEID_INFO & info = (STAGEID_INFO &)it->second;
		const CSI_STAGE & info = *it;

		//액티브 스테이즈는 없어지고 서버에서 받은 모든 스테이지들은 활성화 되어있다고 가정합니다.
		//STAGE_ID id = info.GetStageID();
		//if (isActiveStage( id) == false)
		//	continue;

		//모드가 없어지고 상위 개념인 룰이 생겼습니다.
		//허나 이 함수내에선 모드로 판별했엄으로, 모드로 골라 스테이지 이름(맵이름)을 넣는 것으로 바꿉니다.
		/*
		if (stageType != STAGE_MODE_NA)
		{
			STAGE_MODE type = getStageType(id);

			if (type != stageType)
				continue;
		}

		info.GetStageDisplayName(name);

		wchar_t wszName[256];

		MultiByteToWideChar(CP_ACP, 0, name, -1, wszName, _countof(wszName) - 1);
		*/

		if ( StageID::GetStageMode(info.m_ui32StageID) != stageType)
			continue;

		wstring * pName = new wstring( CStageInfoUtil::GetMapData(info.m_ui32StageID)->getDisplayName().c_str() );

		pList->push_back(pName);

		cnt++;
		if((count > 0) && (cnt >= count))
			break;
	}

	return pList;
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

vector<wstring *> *		TestAutomationInterface::EnumStageValues(const wchar_t * pszValue, int count)
{
	if (wcsicmp(pszValue, L"모든 스테이지") == 0)
	{
		return GetStageList(STAGE_MODE_NA, count);
	}
	else if (wcsicmp(pszValue, L"모든 단체전") == 0)
	{
		return GetStageList(STAGE_MODE_DEATHMATCH, count);
	}
	else if (wcsicmp(pszValue, L"모든 폭파미션") == 0)
	{
		return GetStageList(STAGE_MODE_BOMB, count);
	}
	else if (wcsicmp(pszValue, L"모든 파괴미션") == 0)
	{
		return GetStageList(STAGE_MODE_DESTROY, count);
	}
	else if (wcsicmp(pszValue, L"모든 섬멸전") == 0)
	{
		return GetStageList(STAGE_MODE_ANNIHILATION, count);
	}
	else if (wcsicmp(pszValue, L"모든 방어전") == 0)
	{
		return GetStageList(STAGE_MODE_DEFENCE, count);
	}
	
	return nullptr;
}

static void _addList(vector<wstring *> & list, const wchar_t * pszStr)
{
	size_t count = list.size();

	// 동일한 항목이 있는지 먼저 확인한다.
	for (size_t i = 0; i < count; i++)
	{
		wstring * pTemp = list[i];

		if (*pTemp == pszStr)
		{
			// 이미 있다. 추가하지 않는다.
			return;
		}
	}

	wstring * pItem = new wstring;

	*pItem = pszStr;

	list.push_back(pItem);
}

vector<wstring *> *		TestAutomationInterface::GetGoodsList( item_def::Category cate, int count, WEAPON_CLASS_TYPE weaponType)
{
	vector<wstring *> * pList = new vector<wstring *>;
	i3::rc_wstring name;
	int cnt = 0;

	CShopContext * pCtx = CShopContext::i();

	for( size_t ctg = SHOP_WEAPON_GOODS; ctg < SHOP_GOODS_MAX; ++ctg )
	{
		SHOP_GOODS_TYPE shop_type = (SHOP_GOODS_TYPE)ctg;
		item_def::Category temp = to_item_category(shop_type);

		if((cate != item_def::CTG_NA) && (cate != temp))
			continue;

		INT32 cntPack = pCtx->GetGoodsPackCount( shop_type );
		for(INT32 i = 0; i < cntPack; i++)
		{
			const SHOP_GOODS_PACK * pPack = pCtx->GetGoodsPack(shop_type, i);
			if( pPack == nullptr) continue;

			SHOP_GOODS * pGoods = pPack->GetGoods(0);
			if( pGoods == nullptr) continue;

			// 뽑기상점에서 이벤트 아이템을 지정 가능합니다.
			// EVENT 아이템 자체도 GoodsPack에 포함되어 있습니다.
			// 다만 모든 아이템이 EVENT이면 상점에서 제외 합니다.
			for( INT32 j = 0; j < pPack->GetGoodsCount(); j++)
			{
				pGoods = pPack->GetGoods( j);				

				switch( cate)
				{
					case item_def::CTG_WEAPON:
						{
							WEAPON_CLASS_TYPE weapon_class = WEAPON::ItemID2Class( pGoods->_ItemID);

							if((weaponType != WEAPON_CLASS_UNKNOWN) && (weaponType != weapon_class))
								continue;

							CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo(pGoods->_ItemID);

							if( pInfo != nullptr)
							{
								if((pGoods->_PriceInfo->_SaleMark == 0) && (pGoods->_SaleDesignation == 0))
								{
									pCtx->GetGoodsName(name, pGoods->_GoodsID, pGoods->_ItemID);

									if (wcsicmp(name.c_str(), L"") == 0)
										continue;

									_addList(*pList, name.c_str());

									cnt++;
								}
							}
							else
							{
								wchar_t conv[256];

								swprintf_s( conv, _countof(conv), L"Invalid ItemID : %d", pGoods->_ItemID);

								//_addList(*pList, conv);
							}
						}
						break;

					default:
						{
							pCtx->GetGoodsName( name, pGoods->_GoodsID, pGoods->_ItemID);

							_addList(*pList, name.c_str());

							cnt++;
						}
						break;
				}

				if((count > 0) && (cnt >= count))
				{
					return pList;
				}
			}
		}
	}

	return pList;
}


vector<wstring *> *		TestAutomationInterface::EnumGoodsValues(const wchar_t * pszValue, int count)
{
	if (wcsicmp(pszValue, L"모든 상품") == 0)
	{
		return GetGoodsList( item_def::CTG_NA, count);
	}
	else if (wcsicmp(pszValue, L"모든 무기") == 0)
	{
		return GetGoodsList( item_def::CTG_WEAPON, count);
	}
	else if (wcsicmp(pszValue, L"모든 돌격소총") == 0)
	{
		return GetGoodsList( item_def::CTG_WEAPON, count, WEAPON_CLASS_ASSAULT);
	}
	else if (wcsicmp(pszValue, L"모든 기관단총") == 0)
	{
		return GetGoodsList( item_def::CTG_WEAPON, count, WEAPON_CLASS_SMG);
	}
	else if (wcsicmp(pszValue, L"모든 산탄총") == 0)
	{
		return GetGoodsList( item_def::CTG_WEAPON, count, WEAPON_CLASS_SHOTGUN);
	}
	else if (wcsicmp(pszValue, L"모든 저격총") == 0)
	{
		return GetGoodsList( item_def::CTG_WEAPON, count, WEAPON_CLASS_SNIPER);
	}
	else if (wcsicmp(pszValue, L"모든 캐릭터") == 0)
	{
		return GetGoodsList( item_def::CTG_CHARA, count);
	}
	else if (wcsicmp(pszValue, L"모든 파츠") == 0)
	{
		return GetGoodsList( item_def::CTG_PARTS, count);
	}
	else if (wcsicmp(pszValue, L"모든 스킨") == 0)
	{
		return GetGoodsList( item_def::CTG_SKIN, count);
	}
	else if (wcsicmp(pszValue, L"모든 아이템") == 0)
	{
		return GetGoodsList( item_def::CTG_ITEM, count);
	}

	
	return nullptr;
}

vector<wstring *> *		TestAutomationInterface::EnumInvenValues(const wchar_t * pszValue, int count)
{
	return nullptr;
}

vector<std::wstring *> * TestAutomationInterface::EnumVariableValue( const wchar_t * pszVar, const wchar_t * pszValue, int count)
{
	if (wcsicmp(pszVar, L"Stage") == 0)
	{
		return EnumStageValues(pszValue, count);
	}
	else if (wcsicmp(pszVar, L"Goods") == 0)
	{
		return EnumGoodsValues(pszValue, count);
	}
	else if (wcsicmp(pszVar, L"Inven") == 0)
	{
		return EnumInvenValues(pszValue, count);
	}

	return nullptr;
}

#endif