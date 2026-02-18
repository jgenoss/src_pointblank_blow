#include "pch.h"
#include "Shop.h"
#include "ModuleDBShop.h"
#include "TaskProcessor.h"

I3_CLASS_INSTANCE(CShop, i3ElementBase);

CShop::CShop()
{
	m_ui32Version		= 0;
	m_ui32VerionIdx		= 0;
	m_hFile				= 0;
	
	m_strShopMD5Key[NET_MD5_KEY_SIZE] = 0;

	i3mem::FillZero(&m_ShopData,		sizeof(SHOP_DATA)* MAX_SHOP_DATA_BUF);
	i3mem::FillZero(&m_FieldShop,		sizeof(FieldShopInfo));

	m_pUseShopData		= &m_ShopData[0];		
	m_pNotUseShopData	= &m_ShopData[1];		

}

CShop::~CShop()
{
	Destory();
}

void	CShop::Destory()
{
	for( UINT32 k = 0 ; k < MAX_SHOP_DATA_BUF ; k++ )
	{	
		for( UINT32 i = 0 ; i < m_ShopData[k].m_ui32GoodsCount ; i++ )
		{
			I3MEM_SAFE_FREE( m_ShopData[k].m_pGoodsList[i]._ppItemData );
			I3MEM_SAFE_FREE( m_ShopData[k].m_pGoodsList[i]._pui32ItemArg );
		}
	}
	return; 
}

UINT32 CShop::GetFieldShopVersion()
{
	return m_FieldShop.m_ui32Version;
}

SHOP_GOODS_DATA*		CShop::GetGoodsData( UINT32 ui32GoodsID  )
{
	for( UINT32 i = 0 ; i < m_pUseShopData->m_ui32GoodsCount ; i++ )
	{
		if( m_pUseShopData->m_pGoods[ i ]._ui32GoodsID == ui32GoodsID ) return &m_pUseShopData->m_pGoods[ i ];
	}	

	return NULL;
}

SHOP_ITEM_DATA*			CShop::GetItemData( UINT32 ui32ItemID  )
{
	for( UINT32 i = 0 ; i < m_pUseShopData->m_ui32ItemCount ; i++ )
	{
		if( m_pUseShopData->m_pItem[ i ]._ui32ItemID == ui32ItemID ) return &m_pUseShopData->m_pItem[ i ];
	}	

	return NULL;
}

SHOP_MATCHING_DATA*		CShop::GetMatchingData( UINT32 ui32GoodsID  )
{
	for( UINT32 i = 0 ; i < m_pUseShopData->m_ui32MatchingCount ; i++ )
	{
		if( m_pUseShopData->m_pMatching[ i ]._ui32GoodsID == ui32GoodsID ) return &m_pUseShopData->m_pMatching[ i ];
	}	

	return NULL;
}


void				CShop::SetShopMD5Key(char* strShopMD5Key)	
{  
	i3String::Copy( m_strShopMD5Key, strShopMD5Key, NET_MD5_KEY_SIZE+1); 
	m_strShopMD5Key[NET_MD5_KEY_SIZE] = 0;
};

UINT32	CShop::_GetFindGoodsIdx( SHOP_DATA* pShopData, UINT32 ui32GoodsID  )
{
	for( UINT32 i = 0 ; i < pShopData->m_ui32GoodsCount ; i++ )
	{
		if( pShopData->m_pGoods[ i ]._ui32GoodsID == ui32GoodsID ) return i;
	}

	return 0xFFFFFFFF;
}

UINT32	CShop::_GetFindItemIdx( SHOP_DATA* pShopData, UINT32 ui32ItemID )
{
	for( UINT32 i = 0 ; i < pShopData->m_ui32ItemCount ; i++ )
	{
		if( pShopData->m_pItem[ i ]._ui32ItemID == ui32ItemID ) return i;
	}

	return 0xFFFFFFFF;
}

SHOP_GOODS_LIST*	CShop::GetGoodsByID( UINT32 ui32GoodsID )
{
	UINT32 ui32GoodsIdx = _GetFindGoodsIdx( m_pUseShopData, ui32GoodsID );

	if( m_pUseShopData->m_ui32GoodsCount <= ui32GoodsIdx )	return NULL;
	if (MAX_SHOP_GOODS_COUNT <= ui32GoodsIdx)				return NULL;

	return &m_pUseShopData->m_pGoodsList[ ui32GoodsIdx ];
	
}

SHOP_ITEM_DATA* CShop::GetItemByID( UINT32 ui32ItemID )
{
	UINT32 ui32ItemIdx = _GetFindItemIdx( m_pUseShopData, ui32ItemID );
	
	if( m_pUseShopData->m_ui32ItemCount <= ui32ItemIdx )	return NULL; 
	if (MAX_SHOP_ITEM_COUNT <= ui32ItemIdx)					return NULL;

	return &m_pUseShopData->m_pItem[ ui32ItemIdx ];
	
}

void	CShop::ChangeShopVersion(UINT32 ui32NewShopVersion)
{
	m_ui32Version = ui32NewShopVersion; 

	// 현재 사용중인 상점 버퍼를 사용아지 않는걸로.
	m_pNotUseShopData = &m_ShopData[m_ui32VerionIdx];

	m_ui32VerionIdx++;
	if( m_ui32VerionIdx >= MAX_SHOP_DATA_BUF ) m_ui32VerionIdx = 0;

	// 현재 사용하지 않는 상점 버퍼를 사용중인 걸로.
	m_pUseShopData = &m_ShopData[m_ui32VerionIdx];

	// 새로운 ShopFile을 만든다.
	SaveShopFile();

	return;

}

void		CShop::MakeShopMD5Key(void)
{
	// ShopFile로 MD5Key를 생성합니다.
	unsigned char md5Key[NET_MD5_HASH_SIZE] = {};
	UINT128 ui128Temp;
	
	i3MD5::Getmd5_file(SHOP_FILE_NAME_ASCII, md5Key);
	memcpy(&ui128Temp, md5Key, sizeof(char)*NET_MD5_HASH_SIZE);
	i3MD5::GetMD5String(m_strShopMD5Key, ui128Temp);

	return;
}


BOOL CShop::SaveShopFile()
{
	// 파일이 있으면 지운다.
	DeleteShopFile();

	INT32			   i32ReadLength = 0;
 
	if( CreateShopFile() == FALSE)	return FALSE;

	::WriteFile(m_hFile, &m_ui32Version,		sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);

	// 사용하지 않는 
	::WriteFile(m_hFile, &m_pUseShopData->m_ui32GoodsCount,		sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);
	::WriteFile(m_hFile, &m_pUseShopData->m_ui32ItemCount,		sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);
	::WriteFile(m_hFile, &m_pUseShopData->m_ui32RepairCount,	sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);
	::WriteFile(m_hFile, &m_pUseShopData->m_ui32MatchingCount,	sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);

	// Goods는 클라이언트와 맞추기 위해 SHOP_GOODS_BASE 기준으로 Write합니다.
	for( UINT32 i = 0 ; i < m_pUseShopData->m_ui32GoodsCount ; i++ )
	{
		::WriteFile(m_hFile, &m_pUseShopData->m_pGoods[i],		sizeof(SHOP_GOODS_EXPANSION),		(LPDWORD)&i32ReadLength, NULL);
	}

	// Item은 클라이언트와 맞추기 위해 SHOP_ITEM_BASE 기준으로만 Write합니다.
	for( UINT32 i = 0 ; i < m_pUseShopData->m_ui32ItemCount ; i++ )
	{
		::WriteFile(m_hFile, &m_pUseShopData->m_pItem[i],		sizeof(SHOP_ITEM_BASE) ,		(LPDWORD)&i32ReadLength, NULL);
	}
	::WriteFile(m_hFile, &m_pUseShopData->m_pRepair,			sizeof(SHOP_ITEM_REPAIR_DATA)	* m_pUseShopData->m_ui32RepairCount,	(LPDWORD)&i32ReadLength, NULL);
	::WriteFile(m_hFile, &m_pUseShopData->m_pMatching,			sizeof(SHOP_MATCHING_DATA)		* m_pUseShopData->m_ui32MatchingCount,	(LPDWORD)&i32ReadLength, NULL);
	
	if( INVALID_HANDLE_VALUE == m_hFile )
	{			
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}

	// Shop.dat 파일에 대한 MD5Ket를 생성한다.
	MakeShopMD5Key();
 
	return TRUE;
}

BOOL CShop::CreateShopFile()
{
	char	wstrFileName[MAX_PATH];

	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}
	
	i3String::Copy(wstrFileName, SHOP_FILE_NAME_ASCII, MAX_PATH);
	
	m_hFile = ::CreateFile(wstrFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( INVALID_HANDLE_VALUE == m_hFile )
	{			
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
		return FALSE; 
	}

	SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);

	return TRUE;
}


BOOL CShop::DeleteShopFile()
{
	char	wstrFileName[MAX_PATH];

	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}

	i3String::Copy(wstrFileName, SHOP_FILE_NAME_ASCII, MAX_PATH);

	if( !DeleteFile(wstrFileName) ) return FALSE;

	return TRUE;
}



INT32 CShop::Update_DB( )
{
	// Get Shop Info
	// PB_BT_SOper_VersionControl 테이블의 Idx( 자동증가 ) 최대값을 얻어온다.
	UINT32 ui32NowVersion = (UINT32)g_pModuleDBShop->GetShopVersion_M();

	// 최초 상점 버전 등록을하지 않은 경우.
	if( ui32NowVersion == 0) return EVENT_ERROR_PBV15_SHOP_VERSION; 

	// 버전 변경 되지 않은 경우 정상리턴.
	if(m_ui32Version == ui32NowVersion )	return EVENT_ERROR_SUCCESS_1; 

	// DB에서 ShopData를 읽어 셋팅한다.
	INT32 i32RvShopData = g_pModuleDBShop->GetShopData_M() ;
	if( EV_FAILED( i32RvShopData ) ) 
	{
		return i32RvShopData;
	}

	// GoodsList셋팅.
	_SetGoodsList();
	
	ChangeShopVersion(ui32NowVersion);
	
	g_pLog->WriteLog( L"[CShop::Update_DB] Shop Update  Ver:%d, ItemCount:%d, RepairCount : %d, Goods:%d, Matching:%d", 
		m_ui32Version, m_pUseShopData->m_ui32ItemCount, m_pUseShopData->m_ui32RepairCount, m_pUseShopData->m_ui32GoodsCount, 
		m_pUseShopData->m_ui32MatchingCount );

	g_pTaskProcessor->SetCapsuleLoad(CAPSULE_LOAD_SHOP);

	return EVENT_ERROR_SUCCESS; 
}

void CShop::UpdateField_DB()
{
	// Get FieldShop Info
	if( false == g_pModuleDBShop->GetFieldShopInfo_M( &m_FieldShop ) )	return;
	
	g_pLog->WriteLog( L"Shop Field Update - Ver:%d", m_FieldShop.m_ui32Version );
}

BOOL CShop::_SetGoodsList(void)
{
	SHOP_DATA* pShopData = GetNotUseShopData();

	if( 0 == pShopData->m_ui32GoodsCount || 0 == pShopData->m_ui32ItemCount )	return FALSE;

	UINT8*	pui8GoodsCount;
	UINT32* pui32GoodsIdx;
	pui8GoodsCount = (UINT8*)i3MemAlloc( sizeof(UINT8) * pShopData->m_ui32GoodsCount );
	i3mem::FillZero( pui8GoodsCount, sizeof( UINT8 ) * pShopData->m_ui32GoodsCount );

	pui32GoodsIdx = (UINT32*)i3MemAlloc( sizeof(UINT32) * pShopData->m_ui32MatchingCount );
	i3mem::FillZero( pui32GoodsIdx, sizeof( UINT32 ) * pShopData->m_ui32MatchingCount );

	// 각 상품들이 아이템을 몇개씩 가지고 있는지를 계산하며 해당 Goods 의 Idx 를 구한다.
	for( UINT32 i = 0 ; i < pShopData->m_ui32MatchingCount ; i++ )
	{
		pui32GoodsIdx[i] = _GetFindGoodsIdx( pShopData, pShopData->m_pMatching[i]._ui32GoodsID );
		if( pShopData->m_ui32GoodsCount <= pui32GoodsIdx[i] )	continue;

		pui8GoodsCount[ pui32GoodsIdx[i] ]++;
	}

	for( UINT32 i = 0 ; i < pShopData->m_ui32GoodsCount ; i++ )
	{
		SHOP_GOODS_LIST* pGoods = &pShopData->m_pGoodsList[ i ];

		if( NULL != pGoods->_pGoodsData || NULL != pGoods->_ppItemData )	continue;

		pGoods->_pGoodsData		= &pShopData->m_pGoods[i];
		pGoods->_ui16ItemCount	= 0;
		if( 0 < pui8GoodsCount[i] )
		{
			pGoods->_ppItemData		= (SHOP_ITEM_DATA**)i3MemAlloc( sizeof(SHOP_ITEM_DATA*) * pui8GoodsCount[ i ] );
			pGoods->_pui32ItemArg	= (UINT32*)i3MemAlloc( sizeof(UINT32) * pui8GoodsCount[ i ] );

			i3mem::FillZero( pGoods->_ppItemData,	sizeof( SHOP_ITEM_DATA* ) * pui8GoodsCount[ i ] );
			i3mem::FillZero( pGoods->_pui32ItemArg,	sizeof( UINT32 ) * pui8GoodsCount[ i ] );
		}

		// 상품 가격,마크 셋팅
		for( UINT32 k = 0 ; k < pShopData->m_ui32PriceCount ; ++k )
		{
			SHOP_GOODS_PRICE_DB* pPrice = &pShopData->m_pPrice[ k ];

			if( pGoods->_pGoodsData->_ui32GoodsID != pPrice->m_ui32GoodsID ) continue;
			if( pPrice->m_ui8PriceType >= PRICE_TYPE_MAX ) continue; 

			pGoods->_pGoodsData->m_Price[pPrice->m_ui8PriceType].m_ui32Point	= pPrice->m_ui32Point;
			pGoods->_pGoodsData->m_Price[pPrice->m_ui8PriceType].m_ui32Cash		= pPrice->m_ui32Cash;
			pGoods->_pGoodsData->m_Price[pPrice->m_ui8PriceType].m_ui8Mark		= pPrice->m_ui8Mark;
		}

	}

	// ShopGoods 에 Item을 담는다.
	for( UINT32 i = 0 ; i < pShopData->m_ui32MatchingCount ; i++ )
	{
		if( pShopData->m_ui32GoodsCount <= pui32GoodsIdx[i] )							continue;

		SHOP_GOODS_LIST* pGoods = &pShopData->m_pGoodsList[ pui32GoodsIdx[i] ];

		UINT32 ui32ItemIdx;
		TItemCnt ui16GoodsIdx = -1;

		
			ui32ItemIdx = _GetFindItemIdx( pShopData, pShopData->m_pMatching[i]._ui32ItemID ); // 일반 아이템이라면
			if( pShopData->m_ui32ItemCount <= ui32ItemIdx )		continue;

			for( TItemCnt j = 0 ; j < pGoods->_ui16ItemCount ; j++ )
			{
				if( pGoods->_ppItemData[ j ] == &pShopData->m_pItem[ ui32ItemIdx ] )
				{
					ui16GoodsIdx = j;
				}
			}

			if( -1 == ui16GoodsIdx )
			{
				ui16GoodsIdx = pGoods->_ui16ItemCount;
				pGoods->_ui16ItemCount++;
			}

			pGoods->_ppItemData[ ui16GoodsIdx ]		= &pShopData->m_pItem[ ui32ItemIdx ];
			pGoods->_pui32ItemArg[ ui16GoodsIdx ]	= pShopData->m_pMatching[i]._ui32ItemArg + pShopData->m_pMatching[i]._ui32BonusArg;

	}
	I3MEM_SAFE_FREE( pui8GoodsCount );
	I3MEM_SAFE_FREE( pui32GoodsIdx );

	return TRUE;
}

UINT8	CShop::GetItemInfo_byGoodsID( UINT32 ui32GoodsID, UINT32 *pui32ItemID, UINT32 *pui32ItemArg )	// GoodsID로 ItemID와 ItemArg를 얻어온다.
{
	SHOP_MATCHING_DATA*	pMatching = GetMatchingAllData();

	for( UINT32 i = 0; i < m_pUseShopData->m_ui32MatchingCount; ++i )
	{
		if( pMatching->_ui32GoodsID != ui32GoodsID )
		{
			++pMatching;
			continue;
		}

		*pui32ItemID	= pMatching->_ui32ItemID;
		*pui32ItemArg	= pMatching->_ui32ItemArg + pMatching->_ui32BonusArg;
		break;
	}

	return 1;
}

UINT8	CShop::GetItemInfo_byGoodsID(  UINT32 ui32GoodsID,  ST_ITEM * pItem )	// GoodsID로 ItemID와 ItemArg를 얻어온다.
{
	UINT8 ui8ItemCount = 0;
	SHOP_MATCHING_DATA*	pMatching = GetMatchingAllData();
	UINT32 ui32MatchingCount = GetMatchingCount();

	for( UINT32 i = 0; i < ui32MatchingCount; ++i )
	{
		if( ui8ItemCount >= MAX_SHOP_GOODS_ITEM_COUNT ) break;

		if( pMatching->_ui32GoodsID == ui32GoodsID )
		{
			pItem[ui8ItemCount].m_TItemID	= pMatching->_ui32ItemID;
			pItem[ui8ItemCount].m_TItemArg	= pMatching->_ui32ItemArg + pMatching->_ui32BonusArg;
			++ui8ItemCount;
		}

		++pMatching;
	}

	return ui8ItemCount;	 
}

BOOL	CShop::IsSalesItem(UINT32 ui32ItemID)	// 판매중인 상품인지 ItemID로 체크.
{
	UINT32 ui32MatchingCount = GetMatchingCount();
	SHOP_MATCHING_DATA*	pMatching = GetMatchingAllData();

	for (UINT32 i = 0; i < ui32MatchingCount; ++i)
	{
		SHOP_MATCHING_DATA	*pMatching = GetMatching(i);
		if (!pMatching) continue;

		if (pMatching->_ui32ItemID != ui32ItemID)			continue;

		SHOP_GOODS_BASE* pGoods = GetGoodsData(pMatching->_ui32GoodsID);
		if (!pGoods) continue;

		if (GOODS_USE_ENABLE != pGoods->_ui8Enable) continue;
		if (GOODS_SALE_ALL != pGoods->_ui8SaleType && GOODS_SALE_BUY != pGoods->_ui8SaleType) continue;

		return TRUE;
	}
	return FALSE;
}