#include "pch.h"
#include "Shop.h"
#include "IGShopContext.h"
#include "Designation/DesigUtil.h"

CShop::CShop()
{
	m_IsLoadShop			= false;
	m_bFirstShopFileCheck	= true;

	m_ui32Version			= 0;

	// revision 54070 상점 패킷을 제대로 못 받을 때 처리 수정. 이제 상점에서 패킷을 못받으면, 재접속 할 필요 없이 다시 상점으로 들어가면 패킷 받아서 처리하도록 수정.
	m_ui32GoodsVersion		= 0;
	m_ui32ItemVersion		= 0;
	m_ui32MatchingVersion	= 0;
	m_ui32RepairVersion		= 0;
	m_ui32CouponVersion		= 0;

	m_ui32GoodsVerify = 0;
	m_ui32ItemVerify = 0;
	m_ui32MatchingVerify = 0;
	m_ui32RepairVerify = 0;
	m_ui32CouponVerify = 0;

	m_ui32GoodsCount		= 0;
	m_ui32ItemCount			= 0;
	m_ui32MatchingCount		= 0;
	m_ui32RepairCount		= 0;
	m_ui32CouponCount		= 0;

	i3mem::FillZero( m_pGoods,		sizeof(m_pGoods) );
	i3mem::FillZero( m_pItem,		sizeof(m_pItem) );
	i3mem::FillZero( m_pMatching,	sizeof(m_pMatching) );
	i3mem::FillZero( m_pRepair,		sizeof(m_pRepair) );
	i3mem::FillZero( m_pShopGoods,	sizeof(m_pShopGoods) );

#if defined(USE_SALE_COUPON)
	i3mem::FillZero( m_pCoupon,		sizeof(m_pCoupon));
#endif

	// Shop버전
	m_hFile = INVALID_HANDLE_VALUE;

	m_strShopMD5Key[NET_MD5_KEY_SIZE] = 0;

	// Shop 파일 있으면 MD5Key 생성
	if( true == OpenShopFile() )
	{
		MakeShopMD5Key();
	}
}

CShop::~CShop()
{
	Destory();
}

void	CShop::Destory()
{
	for( UINT32 i = 0 ; i < m_ui32GoodsCount ; i++ )
	{
#if defined(USE_SALE_COUPON)
		// 할당된 할인쿠폰 메모리 해제를 위해 추가
		SHOP_GOODS_LIST* pGoods = &m_pShopGoods[ i ];
		for( UINT32 idx = 0 ; idx < pGoods->_ui8ItemCount; idx++)
		{
			UINT32 nCouponIdx = pGoods->_ppItemData[idx]->_ui32ItemID;

			if( GET_ITEM_TYPE(nCouponIdx) != ITEM_TYPE_SALECOUPON )
				continue;

			I3MEM_SAFE_FREE(pGoods->_ppItemData[idx]);
		}
#endif
		m_pShopGoods[ i ]._pGoodsData	= nullptr;
		m_pShopGoods[ i ]._ui8ItemCount	= 0;
		m_pShopGoods[ i ]._ui32ItemDataCount	= 0;
		I3MEM_SAFE_FREE( m_pShopGoods[ i ]._pui32ItemArg );
		I3MEM_SAFE_FREE( m_pShopGoods[ i ]._ppItemData );			
	}

	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

UINT32	CShop::_GetFindGoodsIdx( UINT32 ui32GoodsID )
{
	for( UINT32 i = 0 ; i < m_ui32GoodsCount ; i++ )
	{
		if( m_pGoods[ i ]._ui32GoodsID == ui32GoodsID )
		{
			return i;
		}
	}
	return 0xFFFFFFFF;
}

UINT32	CShop::_GetFindItemIdx( UINT32 ui32ItemID )
{
	for( UINT32 i = 0 ; i < m_ui32ItemCount ; i++ )
	{
		if( m_pItem[ i ]._ui32ItemID == ui32ItemID )
		{
			return i;
		}
	}
	return 0xFFFFFFFF;
}

UINT32	CShop::_GetFindMatchingIdx( UINT32 ui32GoodsID )
{
	for( UINT32 i = 0 ; i < m_ui32MatchingCount ; i++ )
	{
		if( m_pMatching[ i ]._ui32GoodsID == ui32GoodsID )
			return i;
	}
	return 0xFFFFFFFF;
}

#if defined(USE_SALE_COUPON)
UINT32	CShop::_GetFindCouponIdx( UINT32 ui32CouponID )
{
	for( UINT32 i = 0 ; i < m_ui32CouponCount ; i++ )
	{
		if( m_pCoupon[ i ].m_ui32CouponID == ui32CouponID )
			return i;
	}
	return 0xFFFFFFFF;
}
#endif


void	CShop::SetVersion( UINT32 ui32Version )
{
	m_ui32Version	= ui32Version;
}

void	CShop::SetGoodsCount( UINT32 ui32Count )
{
	if( 0 >= ui32Count )	return;

	if( ui32Count > MAX_SHOP_GOODS_COUNT)
	{	// 배열 갯수보다 넘어서 셋팅하는 것을 막는다.
		I3PRINTLOG(I3LOG_NOTICE,"<<< Over Goods buffer count !!! (MAX_SHOP_GOODS_COUNT)");

		m_ui32GoodsCount	= MAX_SHOP_GOODS_COUNT;
	}
	else
	{
		m_ui32GoodsCount	= ui32Count;
	}	
}

void	CShop::SetItemCount( UINT32 ui32Count )
{
	if( 0 >= ui32Count )	return;

	if( ui32Count > MAX_SHOP_ITEM_COUNT)
	{	// 배열 갯수보다 넘어서 셋팅하는 것을 막는다.
		I3PRINTLOG(I3LOG_NOTICE,"<<< Over Item buffer count !!! (MAX_SHOP_ITEM_COUNT)");

		m_ui32ItemCount	= MAX_SHOP_ITEM_COUNT;
	}
	else
	{
		m_ui32ItemCount		= ui32Count;
	}	
}

UINT32	CShop::ConvertGoodsID( UINT32 ui32Count )
{
	
	
	UINT32 GoodsIdx;
	GoodsIdx = _GetFindMatchingIdx(ui32Count);
	

	return m_pMatching[GoodsIdx]._ui32ItemID;

}

void	CShop::SetRepairCount( UINT32 ui32Count )
{
	if( 0 >= ui32Count )	return;

	if( ui32Count > MAX_SHOP_ITEM_COUNT)
	{	// 배열 갯수보다 넘어서 셋팅하는 것을 막는다.
		I3PRINTLOG(I3LOG_NOTICE,"<<< Over Repair buffer count !!! (MAX_SHOP_ITEM_COUNT)");

		m_ui32RepairCount	= MAX_SHOP_ITEM_COUNT;
	}
	else
	{	
		m_ui32RepairCount	= ui32Count;
	}
}

void	CShop::SetMatchingCount( UINT32 ui32Count )
{
	if( 0 >= ui32Count )	return;
	
	if( ui32Count > MAX_SHOP_MATCHING_COUNT)
	{	// 배열 갯수보다 넘어서 셋팅하는 것을 막는다.
		I3PRINTLOG(I3LOG_NOTICE,"<<< Over Matching buffer count !!! (MAX_SHOP_MATCHING_COUNT)");

		m_ui32MatchingCount	= MAX_SHOP_MATCHING_COUNT;
	}
	else
	{
		m_ui32MatchingCount	= ui32Count;
	}	
}

void	CShop::SetCouponCount( UINT32 ui32Count )
{
	if( 0 >= ui32Count) return;

	if( ui32Count > MAX_SHOP_ITEM_COUNT)
	{
		I3PRINTLOG(I3LOG_NOTICE,"<<< Over Coupon Buffer Count !!! (MAX_SHOP_ITEM_COUNT)");

		m_ui32CouponCount = MAX_SHOP_ITEM_COUNT;
	}
	else
	{
		m_ui32CouponCount = ui32Count;
	}
}

void	CShop::SetGoodsVerify( UINT32 ui32Count)
{
	if( 0 == ui32Count) return;
	m_ui32GoodsVerify = ui32Count;
}

void	CShop::SetItemVerify( UINT32 ui32Count)
{
	m_ui32ItemVerify = ui32Count;
}

void	CShop::SetRepairVerify( UINT32 ui32Count)
{
	m_ui32RepairVerify = ui32Count;
}

void	CShop::SetMatchingVerify( UINT32 ui32Count)
{
	m_ui32MatchingVerify = ui32Count;
}
void	CShop::SetCouponVerify( UINT32 ui32Count)
{
	m_ui32CouponVerify = ui32Count;
}

void	CShop::AddGoodsVerify( UINT32 ui32Count)
{
	m_ui32GoodsVerify += ui32Count;
}

void	CShop::AddItemVerify( UINT32 ui32Count)
{
	m_ui32ItemVerify += ui32Count;
}

void	CShop::AddRepairVerify( UINT32 ui32Count)
{
	m_ui32RepairVerify += ui32Count;
}

void	CShop::AddMatchingVerify( UINT32 ui32Count)
{
	m_ui32MatchingVerify += ui32Count;
}
void	CShop::AddCouponVerify( UINT32 ui32Count)
{
	m_ui32CouponVerify += ui32Count;
}

bool	CShop::ShopListCompleted()
{
#if defined(USE_SALE_COUPON)	// reivision 54319 클라에서 상점 패킷 처리 시 예외처리 추가
	return (m_ui32CouponVersion != 0 && m_ui32GoodsVersion != 0 && m_ui32ItemVersion != 0 && m_ui32MatchingVersion != 0 && m_ui32RepairVersion != 0);
#else
	return (m_ui32GoodsVersion != 0 && m_ui32ItemVersion != 0 && m_ui32MatchingVersion != 0 && m_ui32RepairVersion != 0);
#endif
}

void	CShop::ShopVersionReset()
{
	m_ui32GoodsVersion = 0;
	m_ui32ItemVersion = 0;
	m_ui32RepairVersion = 0;
	m_ui32CouponVersion = 0;
	m_ui32MatchingVersion = 0;
}

// revision 54146 상점 패킷 받을때 확인 변수 초기화 해주도록 수정.
void	CShop::ShopVerifyReset()
{
	m_ui32GoodsVerify = 0;
	m_ui32ItemVerify = 0;
	m_ui32MatchingVerify = 0;
	m_ui32RepairVerify = 0;
	m_ui32CouponVerify = 0;
}

bool	CShop::SetGoodsList()
{
	Destory();

	if ( 0 == m_ui32GoodsCount )	return false;
	if ( 0 == m_ui32ItemCount )		return false;
	if ( 0 == m_ui32MatchingCount )	return false;

	UINT8*	pui8GoodsCount	= nullptr;
	UINT32*	pui32GoodsIdx	= nullptr;
	pui8GoodsCount = (UINT8*)i3MemAlloc( sizeof(UINT8) * m_ui32GoodsCount );
	i3mem::FillZero( pui8GoodsCount, sizeof( UINT8 ) * m_ui32GoodsCount );

	pui32GoodsIdx = (UINT32*)i3MemAlloc( sizeof(UINT32) * m_ui32MatchingCount );
	i3mem::FillZero( pui32GoodsIdx, sizeof( UINT32 ) * m_ui32MatchingCount );

	for( UINT32 i = 0 ; i < m_ui32MatchingCount ; i++ )
	{
		pui32GoodsIdx[i] = _GetFindGoodsIdx( m_pMatching[i]._ui32GoodsID );
		if( m_ui32GoodsCount <= pui32GoodsIdx[i] )			continue;

		pui8GoodsCount[ pui32GoodsIdx[i] ]++;
	}
	// ShopGoods 에 Item 을 담을 준비를한다.
	for( UINT32 i = 0 ; i < m_ui32GoodsCount ; i++ )
	{
		m_pShopGoods[i]._pGoodsData		= &m_pGoods[ i ];
		m_pShopGoods[i]._ui8ItemCount	= 0;
		if( 0 < pui8GoodsCount[ i ] )
		{

			m_pShopGoods[i]._ui32ItemDataCount = pui8GoodsCount[ i ];
			m_pShopGoods[i]._ppItemData		= (SHOP_ITEM_DATA**)i3MemAlloc( sizeof(SHOP_ITEM_DATA*) * pui8GoodsCount[ i ] );		
            m_pShopGoods[i]._pui32ItemArg	= (UINT32*)i3MemAlloc( sizeof(UINT32) * pui8GoodsCount[ i ] );
		}
	}
	// ShopGoods 에 Item을 담는다.
	for( UINT32 i = 0 ; i < m_ui32MatchingCount ; i++ )
	{
		if( m_ui32GoodsCount <= pui32GoodsIdx[i] )	continue;

		SHOP_GOODS_LIST* pGoods = &m_pShopGoods[ pui32GoodsIdx[i] ];

		if( GET_ITEM_TYPE( m_pMatching[i]._ui32ItemID ) != ITEM_TYPE_SALECOUPON )
		{
			UINT32 ui32ItemIdx = _GetFindItemIdx( m_pMatching[i]._ui32ItemID );
			if( m_ui32ItemCount <= ui32ItemIdx )		continue;

			// 배열 생성 갯수보다 많다 (메모리 침범 할수 있다)
			I3ASSERT( pGoods->_ui8ItemCount < pGoods->_ui32ItemDataCount);

			pGoods->_ppItemData[ pGoods->_ui8ItemCount ]	= &m_pItem[ ui32ItemIdx ];
			pGoods->_pui32ItemArg[ pGoods->_ui8ItemCount ]	= m_pMatching[i]._ui32ItemArg;
			pGoods->_ui8ItemCount++;
		}
#if defined(USE_SALE_COUPON)
		else
		{
			UINT32 ui32ItemIdx = _GetFindCouponIdx( m_pMatching[i]._ui32ItemID );
			if( m_ui32CouponCount <= ui32ItemIdx )		continue;

			// 배열 생성 갯수보다 많다 (메모리 침범 할수 있다)
			I3ASSERT( pGoods->_ui8ItemCount < pGoods->_ui32ItemDataCount);
			
			SHOP_ITEM_DATA* pCouponData = (SHOP_ITEM_DATA *) i3MemAlloc(sizeof(SHOP_ITEM_DATA));
			pCouponData->_ui32ItemID = m_pCoupon[ ui32ItemIdx ].m_ui32CouponID;

			pGoods->_ppItemData[ pGoods->_ui8ItemCount] = pCouponData;
			pGoods->_pui32ItemArg[ pGoods->_ui8ItemCount ]	= m_pMatching[i]._ui32ItemArg;
			pGoods->_ui8ItemCount++;
		}
#endif
	}

	I3MEM_SAFE_FREE( pui8GoodsCount );
	I3MEM_SAFE_FREE( pui32GoodsIdx );

	SetRepairList();

	m_IsLoadShop = true;

	return true;
}

bool	CShop::SetRepairList()
{
	// Item 에 수리를 담는다.
	for( UINT32 i = 0 ; i < m_ui32RepairCount ; i++ )
	{
		UINT32 ui32ItemIdx = _GetFindItemIdx( m_pRepair[i]._ui32ItemID );

		if( ui32ItemIdx < MAX_SHOP_ITEM_COUNT )
		{
			m_pItem[ ui32ItemIdx ]._ui32RepairPoint	= m_pRepair[i]._ui32PricePoint;
			m_pItem[ ui32ItemIdx ]._ui32RepairCash	= m_pRepair[i]._ui32PriceCash;
			m_pItem[ ui32ItemIdx ]._ui32Durability	= m_pRepair[i]._ui32Durability;
		}
	}
	return true;
}


SHOP_GOODS_LIST*	CShop::GetGoodsListData( UINT32 ui32GoodsID )
{
	UINT32 ui32Idx = _GetFindGoodsIdx( ui32GoodsID );
	if ( 0xFFFFFFFF == ui32Idx )
		return nullptr;
	return &m_pShopGoods[ ui32Idx ];
}

SHOP_GOODS_EXPANSION*		CShop::GetGoodsData( UINT32 ui32GoodsID  )
{
	UINT32 ui32Idx = _GetFindGoodsIdx( ui32GoodsID );
	if ( 0xFFFFFFFF == ui32Idx )
		return nullptr;
	return &m_pGoods[ ui32Idx ];
}

SHOP_ITEM_DATA*			CShop::GetItemData( UINT32 ui32ItemID  )
{
	UINT32 ui32Idx = _GetFindItemIdx( ui32ItemID );
	if ( 0xFFFFFFFF == ui32Idx )
		return nullptr;
	return &m_pItem[ ui32Idx ];
}

#if defined(USE_SALE_COUPON)
SALE_COUPON *		CShop::GetCouponData( UINT32 ui32ItemID )
{
	UINT32 ui32Idx = _GetFindCouponIdx(ui32ItemID);
	if ( 0xFFFFFFFF == ui32Idx )
		return nullptr;
	return &m_pCoupon[ ui32Idx ];
}
#endif



void CShop::GetGoodsPrice(UINT32 ui32GoodsID, GOODS_PRICE_INFO & info, INT32 price_idx)
{
	SHOP_GOODS_LIST * pGoodsList = GetGoodsListData(ui32GoodsID);
	if ( pGoodsList != nullptr)
	{
		UINT32 ui32Cash = pGoodsList->_pGoodsData->GetCash( static_cast<PRICE_TYPE>(price_idx) );
		UINT32 ui32Point = pGoodsList->_pGoodsData->GetPoint( static_cast<PRICE_TYPE>(price_idx) );
		UINT8 ui8Mark = pGoodsList->_pGoodsData->GetMark( static_cast<PRICE_TYPE>(price_idx) );

		info.BuildPriceInfo(ui32Cash, ui32Point, ui8Mark);
	}
}

void CShop::GetGoodsPrice(UINT32 ui32GoodsID, GOODS_PRICE_INFO * pOutPrice)
{
	SHOP_GOODS_LIST * pGoodsList = GetGoodsListData(ui32GoodsID);

	if(pGoodsList != nullptr)
	{
		for(INT32 idx = 0 ; idx < PRICE_TYPE_MAX ; ++idx)
		{
			PRICE_TYPE type = static_cast<PRICE_TYPE>(idx);

			UINT32 ui32Cash = pGoodsList->_pGoodsData->GetCash( type );
			UINT32 ui32Point = pGoodsList->_pGoodsData->GetPoint( type );
			UINT8 ui8Mark = pGoodsList->_pGoodsData->GetMark( type );

			pOutPrice[idx].BuildPriceInfo(ui32Cash, ui32Point, ui8Mark);
		}
	}
}

bool	CShop::IsPackageGoods(UINT32 ui32GoodsID)
{
	SHOP_GOODS_LIST * pGoodsList = GetGoodsListData(ui32GoodsID);

	if ( pGoodsList && pGoodsList->_ui8ItemCount > 1 )
		return true;

	return false;
}


UINT8		CShop::GetAuthType(UINT32 ui32ItemID)
{
	SHOP_ITEM_DATA * pItemData = GetItemData(ui32ItemID);
	if ( pItemData == nullptr)
	{
		// 상품과 매치되는 아이템을 찾지 못함. 서버에서 정상적으로 아이템 정보를 넘겨받지 못한거 같다. 문제가 있는것이므로 서버팀에 문의 
		return 0;
	}

	I3ASSERT( pItemData->_ui8AuthType != 0);

	return pItemData->_ui8AuthType;
}


// Shop버전


bool CShop::OpenShopFile()
{
	char	wstrFileName[MAX_PATH];

	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}
	
	i3::safe_string_copy( wstrFileName, SHOP_FILE_NAME_ASCII, MAX_PATH);
	
	if(FindShopFile() == true)
	{
		m_hFile = ::CreateFile(wstrFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

		if( INVALID_HANDLE_VALUE == m_hFile )
		{			
			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE; 
			return false; 
		}
	}
	else	return false;

	return true;
}

bool CShop::FindShopFile()
{
	WIN32_FIND_DATA		ffResult;
	HANDLE              hFind;
	bool				bRv			= false;
 
	char strFileName[MAX_PATH];
	i3::safe_string_copy(strFileName, SHOP_FILE_NAME_ASCII, MAX_PATH);

	hFind = FindFirstFile( (LPCSTR)strFileName, &ffResult);
    
	if ( hFind !=INVALID_HANDLE_VALUE )
	{
		BOOL          bContinue	= true;

		while( bContinue)
		{    
			if( (ffResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				bRv = true;
			}
	              
			bContinue = ::FindNextFile( hFind, &ffResult);
		}

		::FindClose(hFind);
	}
	return bRv;
}

bool CShop::DeleteShopFile()
{
	char	wstrFileName[MAX_PATH];

	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}

	i3::safe_string_copy(wstrFileName, SHOP_FILE_NAME_ASCII, MAX_PATH);

	if( !DeleteFile(wstrFileName) ) return false;

	return true;
}

bool CShop::SaveShopFile()
{
	// 파일이 있으면 지운다.
	DeleteShopFile();

	INT32			   i32ReadLength = 0;
 
	if( CreateShopFile() == false)	return false;

	::WriteFile(m_hFile, &m_ui32Version,		sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);

	::WriteFile(m_hFile, &m_ui32GoodsCount,		sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);
	I3ASSERT( m_ui32GoodsCount < MAX_SHOP_GOODS_COUNT);

	::WriteFile(m_hFile, &m_ui32ItemCount,		sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);
	I3ASSERT( m_ui32ItemCount < MAX_SHOP_ITEM_COUNT);
	::WriteFile(m_hFile, &m_ui32RepairCount,	sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);
	I3ASSERT( m_ui32RepairCount < MAX_SHOP_ITEM_COUNT);
	::WriteFile(m_hFile, &m_ui32MatchingCount,	sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);
	I3ASSERT( m_ui32MatchingCount < MAX_SHOP_MATCHING_COUNT);

	::WriteFile(m_hFile, &m_pGoods,				sizeof(SHOP_GOODS_EXPANSION) * m_ui32GoodsCount,		(LPDWORD)&i32ReadLength, NULL);

	// Item은 서버와 맞추기 위해 SHOP_ITEM_BASE 기준으로만 Write 합니다.
	for( UINT32 i = 0 ; i < m_ui32ItemCount ; i++ )
	{
        ::WriteFile(m_hFile, &m_pItem[i],		sizeof(SHOP_ITEM_BASE),		(LPDWORD)&i32ReadLength, NULL );
	}

	::WriteFile(m_hFile, &m_pRepair,			sizeof(SHOP_ITEM_REPAIR_DATA) * m_ui32RepairCount, (LPDWORD)&i32ReadLength, NULL);
	::WriteFile(m_hFile, &m_pMatching,			sizeof(SHOP_MATCHING_DATA) * m_ui32MatchingCount,	(LPDWORD)&i32ReadLength, NULL);

	// MD5Key 생성
	MakeShopMD5Key();
	
	if( INVALID_HANDLE_VALUE == m_hFile )
	{			
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}

	m_bFirstShopFileCheck = false;

	// 새로운 MD5Key에 대한 체크를 다시합니다.
	GameEventSender::i()->SetEvent( EVENT_REQUEST_GOODS_LIST);
 
	return true;
}

bool CShop::CreateShopFile()
{
	char	wstrFileName[MAX_PATH];

	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}
	
	i3::safe_string_copy(wstrFileName, SHOP_FILE_NAME_ASCII, MAX_PATH);
	
	m_hFile = ::CreateFile(wstrFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( INVALID_HANDLE_VALUE == m_hFile )
	{			
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
		return false; 
	}

	::SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);

	return true;
}


void		CShop::MakeShopMD5Key(void)
{
	// ShopFile로 MD5Key를 생성합니다.
	unsigned char md5Key[NET_MD5_HASH_SIZE] = {};
	UINT128 ui128Temp;
	
	i3MD5::Getmd5_file(SHOP_FILE_NAME_ASCII, md5Key);
	memcpy(&ui128Temp, md5Key, sizeof(char)*NET_MD5_HASH_SIZE);
	i3MD5::GetMD5String(m_strShopMD5Key, ui128Temp, sizeof(m_strShopMD5Key));

	return;
}

void		CShop::LoadShopFile(void)
{
	if( OpenShopFile() == false)	
	{
		// 나올수 없는 상황입니다. 로그 남깁니다.
		I3PRINTLOG(I3LOG_DEFALUT, "Fail ShopFileLoad!!");
		return;		
	}

	m_bFirstShopFileCheck = false;
	INT32	i32ReadLength = 0;
	::ReadFile( m_hFile, &m_ui32Version,		sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);

	::ReadFile( m_hFile, &m_ui32GoodsCount,		sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);

	I3ASSERT( m_ui32GoodsCount < MAX_SHOP_GOODS_COUNT);
	::ReadFile(m_hFile, &m_ui32ItemCount,		sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);
	I3ASSERT( m_ui32ItemCount < MAX_SHOP_ITEM_COUNT);
	::ReadFile(m_hFile, &m_ui32RepairCount,		sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);
	I3ASSERT( m_ui32RepairCount < MAX_SHOP_ITEM_COUNT);
	::ReadFile(m_hFile, &m_ui32MatchingCount,	sizeof(UINT32), (LPDWORD)&i32ReadLength, NULL);
	I3ASSERT( m_ui32MatchingCount < MAX_SHOP_MATCHING_COUNT);
	::ReadFile(m_hFile, &m_pGoods,				sizeof(SHOP_GOODS_EXPANSION) * m_ui32GoodsCount,		(LPDWORD)&i32ReadLength, NULL);

	// Item은 서버와 맞추기 위해 SHOP_ITEM_BASE 기준으로만 Read 합니다.
	for( UINT32 i = 0 ; i < m_ui32ItemCount ; i++ )
	{
		::ReadFile(m_hFile, &m_pItem[i],		sizeof(SHOP_ITEM_BASE) ,		(LPDWORD)&i32ReadLength, NULL);
	}

	::ReadFile(m_hFile, &m_pRepair,				sizeof(SHOP_ITEM_REPAIR_DATA) * m_ui32RepairCount, (LPDWORD)&i32ReadLength, NULL);
	::ReadFile(m_hFile, &m_pMatching,			sizeof(SHOP_MATCHING_DATA) * m_ui32MatchingCount,	(LPDWORD)&i32ReadLength, NULL);

	SetGoodsList();

	IGShopContext* igshop = g_pFramework->GetIGShopContext();
	igshop->SetMainShopChange();

	// MD5Key 생성
	MakeShopMD5Key();

#ifdef I3_DEBUG
	// 확인용 로그
		//char strTemp[MAX_PATH];
		//i3::snprintf( strTemp, MAX_PATH, "***Shop file Load *** / Version:%d / ItemCount : %d, GoodsCount : %d, MatchingCount: %d, MD5Key : %s", 
		//	m_ui32Version, m_ui32ItemCount, m_ui32GoodsCount, m_ui32MatchingCount,  m_strShopMD5Key  );
		//I3PRINTLOG(I3LOG_DEFALUT,  strTemp );
#endif

	m_IsLoadShop	= true;

	if( INVALID_HANDLE_VALUE == m_hFile )
	{			
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}
	
	DesigUtil::ClearDesigItemInfosFromServer();
	DesigUtil::UpdateDesigItemInfosFromServer( 0, this->GetItemCount());

	return;
}


char*		CShop::GetShopMD5Key(void)			
{	
	// 실행중에 ShopFile이 변경될 수 도 있으니 다시한번 MD5Key를 생성합니다.
	MakeShopMD5Key();

	return m_strShopMD5Key; 
}


T_GoodsID		CShop::GetGachaGoodsID( UINT8 ui8GachaGroup, INT32 i32ItemKey, INT32 i32Arg )
{
	for( UINT32 i = 0 ; i < m_ui32MatchingCount ; i++ )
	{
		SHOP_MATCHING_DATA* pMatching = &m_pMatching[ i ];

		if( ITEM_TYPE_RSCOUPON != GET_GOODS_TYPE(pMatching->_ui32GoodsID) ) continue;

		if( ui8GachaGroup != GET_GOODS_SUBTYPE( pMatching->_ui32GoodsID ) ) continue;

		if( i32ItemKey != (INT32)GET_GOODS_NUMBER( pMatching->_ui32GoodsID ) ) continue;

		if( i32Arg != (INT32)pMatching->_ui32ItemArg ) continue;

		return pMatching->_ui32GoodsID;
	}	

	return 0;
}

SHOP_GOODS_EXPANSION*		CShop::GetGachaGoods( UINT8 ui8GachaGroup, INT32 i32ItemKey, INT32 i32Arg )
{
	return GetGoodsData( GetGachaGoodsID( ui8GachaGroup, i32ItemKey, i32Arg ) ); 
}

void		CShop::GetGachaMatcing( UINT8 ui8GachaGroup, INT32 i32ItemKey, MINI_MATCHING* pMiniMatching )
{
	if( !pMiniMatching ) return;

	for( UINT32 i = 0 ; i < m_ui32MatchingCount ; i++ )
	{
		SHOP_MATCHING_DATA* pMatching = &m_pMatching[ i ];

		if( ITEM_TYPE_RSCOUPON != GET_GOODS_TYPE(pMatching->_ui32GoodsID) ) continue;

		if( ui8GachaGroup != GET_GOODS_SUBTYPE( pMatching->_ui32GoodsID ) ) continue;

		if( i32ItemKey != (INT32)GET_GOODS_NUMBER( pMatching->_ui32GoodsID ) ) continue;

		i3mem::Copy( &pMiniMatching->m_aMatching[pMiniMatching->m_ui8Count], pMatching, sizeof( SHOP_MATCHING_DATA ) );

		++pMiniMatching->m_ui8Count;
	}	

	return ;
}