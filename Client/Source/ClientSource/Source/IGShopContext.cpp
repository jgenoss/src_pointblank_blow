#include "pch.h"
#include "IGShopContext.h"
#include "Shop.h"
#include "UI/UIPhaseShop.h"
#include "UI/UIShopFunction.h"
#include "ui/UIItemInfoUtil.h"
#include "MyRoomInfoContext.h"


I3_CLASS_INSTANCE(IGShopContext);//, i3GameNode);

IGShopContext::IGShopContext() :
m_bRequestOpen(false), m_bEntered(false)
{
	IGShopContext_Mode* mode = new IGShopContext_Mode;		// 기본 야전 상점을 위해 배치..
	MEMDUMP_NEW( mode, sizeof( IGShopContext_Mode));

	m_vecStorage.push_back(mode);
	::memset(m_apContextMode, 0, sizeof(IGShopContext_Mode*) * STAGE_MODE_MAX);
	m_apContextMode[0] = mode;
}

// 아직까지 어떠한 동적할당도 요구하지 않기때문에 소멸자처리는 특별이 없다.
IGShopContext::~IGShopContext()
{
	struct fn_del
	{
		void operator()(const IGShopContext_Mode* p)
		{
			I3ASSERT( p != nullptr);
	#if defined( I3_DEBUG)
			NetworkDump_Free( const_cast<IGShopContext_Mode*>(p), __FILE__, __LINE__);
	#endif
			delete p;
		}
	};
	for_each(m_vecStorage.begin(), m_vecStorage.end(), fn_del());
}



void	IGShopContext::Allocate_StageID(UINT32 ui32FieldShopDataFlag)	// 32개미만의 스테이지비트변수임...
{
	{
		const size_t num = m_vecStorage.size();
		for ( size_t i = 1 ; i < num ; ++i )		//1부터라는 것을 유의...
		{
			I3_SAFE_DELETE( m_vecStorage[i]);
		}
		m_vecStorage.resize(1);		// 원래것 빼고 나머지는 정리...
	}

	for (INT32 i = 1; i < STAGE_MODE_MAX ; ++i)	//1부터라는 것을 유의...
	{
		if( ( ui32FieldShopDataFlag & (0x01 << (i-1)) ) != 0 )	// 해당 모드 야전상점 데이터
		{
			IGShopContext_Mode* mode = new IGShopContext_Mode;
			MEMDUMP_NEW( mode, sizeof( IGShopContext_Mode));

			m_vecStorage.push_back(mode);
			m_apContextMode[i] = mode;
		}
		else							// 기본 야전상점 데이터
		{
			m_apContextMode[i] = m_apContextMode[0];		
		}
	}
}

void	IGShopContext::SetMainShopChange()
{
	const size_t num = m_vecStorage.size();
	for (size_t i = 0 ; i < num ; ++i)
		m_vecStorage[i]->SetMainShopChange();
}

bool	IGShopContext::IsMainShopChange() const
{
	return m_apContextMode[m_curr_stage_type]->IsMainShopChange();
}

void	IGShopContext::UpdateGoodsList()
{
	return m_apContextMode[m_curr_stage_type]->UpdateGoodsList();
}

void	IGShopContext::ReceiveAndUpdateGoodsList(const FieldShopInfo& fsgi)
{
	//IGShopContext_Mode* mode = m_apContextMode[fsgi.m_ui8StageType];
	IGShopContext_Mode* mode = m_apContextMode[0];
	mode->ReceiveAndUpdateGoodsList(fsgi);
}

void	IGShopContext::SetEvent_GetInfo()
{
	GameEventSender::i()->SetEvent( EVENT_GETINFO_IGSHOP );
}

void	IGShopContext::SetEventOnce_Open()
{
	m_bRequestOpen = true;		// 일단 앞에 놓는게 좋겠다.
	
#ifndef JuuL_Test_Code
	m_curr_stage_type = MyRoomInfoContext::i()->getStage()->GetStageMode();
	m_curr_stage_type = STAGE_MODE_NA; //테스트 코드
#endif

	UINT32 ver = m_apContextMode[m_curr_stage_type]->GetVersion();	  
	GameEventSender::i()->SetEventOnce(EVENT_OPEN_IGSHOP, &ver);		// 서버에는 실제로 3개데이터를 보내는데, 다른 하나는 gamecontext에 들어있다..
}

void	IGShopContext::SetEvent_EquipItem(const ITEM_INFO & item_info)
{
	GameEventSender::i()->SetEvent(EVENT_INGAMESHOP_APPLY_ITEM, &item_info);
}

void	IGShopContext::EnteredServer()
{
	m_bRequestOpen = false;
	m_bEntered = true;
}

void	IGShopContext::LeaveServer()
{
	m_bEntered = false;	
	m_bRequestOpen = false;			// 이런 경우도 가정한다..(진입시도자체에서 리브시도...)
	// 실제로 닫는 과정에 대한 네트웍 패킷이 존재하지 않기 때문에 이것으로 종료..

}

INT32				IGShopContext::GetNumCategory() const
{
	return m_apContextMode[m_curr_stage_type]->GetNumCategory();
}

const TTCHAR*			IGShopContext::GetCategoryString(INT32 idx) const
{
	return m_apContextMode[m_curr_stage_type]->GetCategoryString(idx);
}

INT32				IGShopContext::GetNumShopGoodsPack(INT32 cat) const
{
	return m_apContextMode[m_curr_stage_type]->GetNumShopGoodsPack(cat);
}

SHOP_GOODS_PACK*	IGShopContext::GetShopGoodsPack( INT32 cat, INT32 idx) const
{
	return m_apContextMode[m_curr_stage_type]->GetShopGoodsPack(cat, idx);
}


////////////////////

IGShopContext_Mode::IGShopContext_Mode() : m_num_category(0), m_bChangeMainShopContext(false) 
{	
	::memset(m_num_items, 0, sizeof(INT32)* MAX_FIELDSHOP_CATEGORYCOUNT);
	::memset(m_pstrCategoryName, 0, sizeof(TTCHAR*) * MAX_FIELDSHOP_CATEGORYCOUNT);
	::memset(m_aapGoodsPack, 0, sizeof(SHOP_GOODS_PACK*) * MAX_FIELDSHOP_CATEGORYCOUNT * MAX_FIELDSHOP_GOODSLISTCOUNT );
}

IGShopContext_Mode::~IGShopContext_Mode()		// 가상처리를 하지 않는다..
{
	IGShopContext_Mode::ClearInfo();
}

void	IGShopContext_Mode::ClearInfo()
{
	const INT32 num_cat = m_num_category;

	for (INT32 i = 0 ; i < num_cat ; ++i )
	{
		const INT32 num_pack = m_num_items[i];
		
		for (INT32 j = 0 ; j < num_pack; ++j )
		{
			SHOP_GOODS_PACK*& pack = m_aapGoodsPack[i][j];
			I3_SAFE_DELETE( pack);
		}

		m_pstrCategoryName[i] = nullptr;
		m_num_items[i] = 0;
	}
	
	m_num_category = 0;
}


void	IGShopContext_Mode::ReceiveAndUpdateGoodsList(const FieldShopInfo& fsgi)
{
	m_info_from_server = fsgi;
	IGShopContext_Mode::UpdateGoodsList();
}

void	IGShopContext_Mode::UpdateGoodsList()
{
	IGShopContext_Mode::ClearInfo();

	for (INT32 i = 0; i < MAX_FIELDSHOP_CATEGORYCOUNT ; ++i)				// 유효한것만 받아들인다...
	{
		Category* cat = m_info_from_server.m_CategoryList + i;

		for (INT32 j = 0 ; j < MAX_FIELDSHOP_GOODSLISTCOUNT ; ++j)
		{
			IGShopContext_Mode::BuildShopGoodsPack( cat->m_ui32GoodsIDList[j] );
		}

		const INT32 num_curr_items = m_num_items[ m_num_category ];

		if ( num_curr_items > 0)		// 유효하다고 판단...이 경우에만 카테고리를 유효하게 가정...
		{
			m_pstrCategoryName[m_num_category] = cat->m_strCategoryName;
			++m_num_category;
		}

	}
	
	m_bChangeMainShopContext = false;	//

}


void	IGShopContext_Mode::BuildShopGoodsPack(UINT32* goodsIDs)
{
	CShop* shop = CShop::i(); 
	
	for ( INT32 i = 0 ; i < MAX_FIELDSHOP_GOODSPERIODCOUNT; ++i)
	{
		UINT32 goodsID = goodsIDs[i];

		SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(goodsID);		// 개별항목..
		if( pGoodsList == nullptr ) continue;

		//pGoodsList->_ui8ItemCount == 1 일 경우만 유효하다.
		if( pGoodsList->_ui8ItemCount == 0 ) continue; //이럴 경우는 없겠지만...
		if( pGoodsList->_ui8ItemCount > 1) // 세트 아이템을 제거합니다..
		{
			I3TRACE("[INGAME SHOP] Can't Insert Set Item Because Of Shop Convention [Set Item] (GOODS_ID: %d)\n", goodsID);
			continue;
		}

		SHOP_GOODS_TYPE t = SHOP_GOODS_UNKNOWN;

		T_ItemID itemId =  pGoodsList->_ppItemData[0]->_ui32ItemID;
		switch( item_def::get_category(itemId) )
		{
		case item_def::CTG_WEAPON: t = SHOP_WEAPON_GOODS; break;
		case item_def::CTG_CHARA: t = SHOP_CHARACTER_GOODS; break;
		case item_def::CTG_PARTS: t = SHOP_CHARACTER_GOODS; break;
		case item_def::CTG_SKIN : t = SHOP_CHARACTER_GOODS; break;
		case item_def::CTG_ITEM:
			{
				UIShop_Item_Category cat = usf::GetItemItemCategory(itemId);
				if (cat != UISHOP_ITEM_ENHANCED && cat != UISHOP_ITEM_COMPENSATION)
				{
					I3TRACE("[INGAME SHOP] Can't Insert Item Because Of Shop Convention (ITEM_ID: %d, GOODS_ID: %d)\n", itemId, goodsID);
					continue;
				}

				t = SHOP_CASH_ITEM_GOODS; 
			}
		break;
		}

		IGShopContext_Mode::BuildShopGoodsPack( t, goodsID, ( i > 0) );
	}
}


void	IGShopContext_Mode::BuildShopGoodsPack(SHOP_GOODS_TYPE type, UINT32 AddGoodsID, bool bHint)
{
	if (type == SHOP_GOODS_UNKNOWN)
		return;
	
	SHOP_GOODS_PACK * pFindGoodsPack = nullptr;

	SHOP_GOODS_PACK** ppPacks = m_aapGoodsPack[m_num_category];

	const INT32	 curr_num_item = m_num_items[m_num_category];
	
	if ( bHint && curr_num_item > 0)
	{
		SHOP_GOODS_PACK* pGoodsPack = ppPacks[ curr_num_item - 1];			// 가장 최근 것..
		
		if (IGShopContext_Mode::IsExistedPack(type, pGoodsPack, AddGoodsID) )
		{
			pFindGoodsPack = pGoodsPack;
		}
	}
	
	if (pFindGoodsPack == nullptr)			// 기간인덱스쪽이 상이한데도 겹치는 경우의 처리...
	{
		for (INT32 i = 0 ; i < curr_num_item ; ++i )
		{
			SHOP_GOODS_PACK* pGoodsPack = ppPacks[i];
			
			if ( IGShopContext_Mode::IsExistedPack(type, pGoodsPack, AddGoodsID) )
			{
				pFindGoodsPack = pGoodsPack;
				break;
			}
		}
	}

	if ( pFindGoodsPack == nullptr)
	{
		if ( curr_num_item == MAX_FIELDSHOP_GOODSLISTCOUNT)		// 이 경우 넘치기 때문에 중단해야한다..
			return;
		
		pFindGoodsPack = new SHOP_GOODS_PACK; 
		MEMDUMP_NEW( pFindGoodsPack, sizeof( SHOP_GOODS_PACK));

		ppPacks[curr_num_item] = pFindGoodsPack;

		++m_num_items[m_num_category];
	}

	UINT32  nAddGameItemId = 0xFFFFFFFF;
	UINT8	nAuthType = 0;
	UINT32	nAuthArg = 0;		//  _AuthType의 1일 경우 초, 2일 경우 개수 
	UINT8	nSaleMark = GOODS_MARK_NO;	// GOODS_MARK_NO, GOODS_MARK_NEW, GOODS_MARK_HOT
	UINT8	nSaleDesignation = DESIGNATION_TYPE_NONE;
	UINT8	nSaleType = GOODS_SALE_EVENT;

	CShop* shop = CShop::i(); 
	SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(AddGoodsID);

	if (pGoodsList)
	{
		nAddGameItemId =  pGoodsList->_ppItemData[0]->_ui32ItemID;
		nAuthType = pGoodsList->_ppItemData[0]->_ui8AuthType;
		nAuthArg = pGoodsList->_pui32ItemArg[0];
		nSaleMark = pGoodsList->_pGoodsData->GetMark();
		nSaleDesignation = pGoodsList->_ppItemData[0]->_ui8Restriction;
		nSaleType = pGoodsList->_pGoodsData->_ui8SaleType;
	}

	//	가격
	GOODS_PRICE_INFO PriceInfo[PRICE_TYPE_MAX];
	i3mem::FillZero(PriceInfo, sizeof(PriceInfo));
	shop->GetGoodsPrice(AddGoodsID, PriceInfo);

	//	상품 목록에 채워 넣는다.
	SHOP_GOODS * pNewGoods = (SHOP_GOODS*) i3MemAlloc(sizeof(SHOP_GOODS));
	i3mem::FillZero(pNewGoods, sizeof(SHOP_GOODS));

	pNewGoods->_GoodsID		= AddGoodsID;
	pNewGoods->_ItemID		= nAddGameItemId;
	pNewGoods->_AuthType	= nAuthType;
	pNewGoods->_AuthArg		= nAuthArg;
	pNewGoods->_SaleDesignation = nSaleDesignation;
	pNewGoods->_SaleType	= nSaleType;
	i3mem::Copy(pNewGoods->_PriceInfo, PriceInfo, sizeof(PriceInfo));

	//	상품 Pack에 상품 추가	
	if ( ! pFindGoodsPack->AddGoods(pNewGoods) )
	{
		I3PRINTLOG(I3LOG_NOTICE,"Failed to AddGoods. %d", pNewGoods->_GoodsID);
	}		
}

bool	IGShopContext_Mode::IsExistedPack( SHOP_GOODS_TYPE type, SHOP_GOODS_PACK* test_pack,	UINT32 AddGoodsID)
{
	if (test_pack == nullptr) return false;			// 실제로 이런 일은 발생하지 않을 것이다...

	SHOP_GOODS* pGoods = test_pack->GetGoods(0);
	if (pGoods == nullptr) return false;

	UINT32 nFindGameItemId = pGoods->_ItemID;
	UINT32 FindGoodsID = pGoods->_GoodsID;
	
	UINT32  nAddGameItemId = 0xFFFFFFFF;

	CShop* shop = CShop::i(); 

	SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(AddGoodsID);
	if (pGoodsList)
	{
		nAddGameItemId =  pGoodsList->_ppItemData[0]->_ui32ItemID;
	}

	switch (type)
	{
	case SHOP_CASH_ITEM_GOODS:
		{
			//굿즈 타입, 캐쉬 타입이 같은걸 합친다.
			INT32 nGroupOnFindGoods = nFindGameItemId - (GET_ITEM_SUBTYPE(nFindGameItemId) * 1000);
			INT32 nGroupOnAddGoods = nAddGameItemId - (GET_ITEM_SUBTYPE(nAddGameItemId) * 1000);;

			if ( nGroupOnFindGoods != 0 && nGroupOnAddGoods != 0 && nGroupOnFindGoods == nGroupOnAddGoods)
				return true;
		}
		break;

	case SHOP_PACKAGE_GOODS:
		{
			SHOP_GOODS_LIST * pFindGoodsList = shop->GetGoodsListData(FindGoodsID);
			SHOP_GOODS_LIST * pAddGoodsList = pGoodsList;			//shop->GetGoodsListData(AddGoodsID);	// 중복..

			if (pFindGoodsList && pAddGoodsList)
			{
				if (pFindGoodsList->_ui8ItemCount == pAddGoodsList->_ui8ItemCount)
				{						
					bool isSame = true;

					INT32 ItemCount = pFindGoodsList->_ui8ItemCount;
					for( INT32 k = 0; k < ItemCount ; k++)
					{
						INT32 findItemId = pFindGoodsList->_ppItemData[k]->_ui32ItemID;
						INT32 addItemId = pAddGoodsList->_ppItemData[k]->_ui32ItemID;
						if (findItemId == addItemId) continue;

						ITEM_TYPE find_item_type = (ITEM_TYPE)GET_ITEM_TYPE(findItemId);
						ITEM_TYPE add_item_type = (ITEM_TYPE)GET_ITEM_TYPE(addItemId);

						if ((ITEM_TYPE_MAINTENANCE == find_item_type || ITEM_TYPE_WRAP_PERIOD == find_item_type || ITEM_TYPE_WRAP_COUNT == find_item_type) &&
							(ITEM_TYPE_MAINTENANCE == add_item_type || ITEM_TYPE_WRAP_PERIOD == add_item_type || ITEM_TYPE_WRAP_COUNT == add_item_type))
						{
							INT32 nGroupOnFindGoods = findItemId - (GET_ITEM_SUBTYPE(findItemId) * 1000);
							INT32 nGroupOnAddGoods = addItemId - (GET_ITEM_SUBTYPE(addItemId) * 1000);

							//	캐시 아이템의 경우 그룹 idx를 상품 pack의 기준으로 한다.
							if( nGroupOnFindGoods == 0 || nGroupOnAddGoods == 0 ||
								nGroupOnFindGoods != nGroupOnAddGoods)
							{
								isSame = false;
								break;
							}
						}
						else
						{
							isSame = false;
							break;
						}
					}

					if (isSame)
						return true;
				}
			}
		}
		break;

	default:
		if (nFindGameItemId == nAddGameItemId)
			return true;
		break;
	}

	return false;
			
}
