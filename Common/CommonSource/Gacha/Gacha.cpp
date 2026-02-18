#include "pch.h"
#include "Gacha.h"

#define		GACHA_SHOP_FILE_NAME				L".\\GachaShop.dat"
#define		GACHA_SHOP_FILE_NAME_ASCII			".\\GachaShop.dat"

//////////////////////////////////////////////////////////////////////////
//
// CRandomItem
//
void CRandomItem::SetRandomItemInfo(GACHA_RANDOM_ITEM* pRandomItem)
{
	m_ui32GoodsID		= pRandomItem->m_ui32GoodsId;
	m_i32Probability	= pRandomItem->m_i32Probability;
}

void CRandomItem::MakeRandomItemPacket(GACHA_RANDOM_ITEM* pRandomItem)
{
	pRandomItem->m_ui32GoodsId		= m_ui32GoodsID;
	pRandomItem->m_i32Probability	= m_i32Probability;
}


//////////////////////////////////////////////////////////////////////////
//
// CGachaItem
//
CGachaItem::CGachaItem()
: m_i32ItemKey(INVALID_ITEM_KEY)
, m_ui32GoodsID(INVALID_GOODS_ID)
, m_i32RemainAmount(0)
, m_i32TotalAmount(0)
, m_i32WinningIdx(INVALID_WIN_INDEX)
, m_i32RandomItemCnt(0)
{
	for( INT32 i = 0; i < PROB_SECTION_COUNT; ++i )
		m_arWinningProbs[i] = 0;
}

CGachaItem::~CGachaItem()
{
}

bool CGachaItem::IsWinningWin()
{
	--m_i32RemainAmount;

	if( m_i32RemainAmount < 0 )
	{	
		ResetWinningIndex();
		return true;
	}

	if( m_i32RemainAmount != m_i32WinningIdx )
		return false;

	ResetWinningIndex();

	return true;
}

void CGachaItem::ResetWinningIndex()
{	
	m_i32RemainAmount = m_i32TotalAmount;

	INT32 i32ProbSection = RAND_DK( PROBABILITY_MAX );

	INT32 i32SectionIdx = 0;
	INT32 i32AccumProb = 0;
	for( INT32 i = 0; i < PROB_SECTION_COUNT; ++i )
	{
		if( i32ProbSection < i32AccumProb )
		{
			i32SectionIdx = i;
			break;
		}

		i32AccumProb += m_arWinningProbs[i];
	}

	if( m_arWinningProbs[i32SectionIdx] == 0 )
	{
		m_i32WinningIdx = INVALID_WIN_INDEX;
		return;
	}
		
	m_i32WinningIdx = i32AccumProb + RAND_DK( m_arWinningProbs[i32SectionIdx] );

	if( m_i32WinningIdx > m_i32RemainAmount )
	{	// 뒤쪽에 확률이 몰린 케이스 입니다. 마지막에 당첨되도록 예외처리 합니다.
		m_i32WinningIdx = 0;
	}
}

void CGachaItem::SetGachaItemInfo(SS_GACHA_ITEM_INFO* pGachaItem, GACHA_GROUP eGroup)
{
	m_i32ItemKey		= pGachaItem->m_i32ItemKey;
	m_ui32GoodsID		= pGachaItem->m_ui32GoodsID;
	m_i32RemainAmount	= pGachaItem->m_i32RemainAmount;
	m_i32TotalAmount	= pGachaItem->m_i32TotalAmount;
	m_i32GloryCount		= pGachaItem->m_i32GloryCount;

	for( INT32 i = 0; i < PROB_SECTION_COUNT; ++i )
		m_arWinningProbs[i]		= pGachaItem->m_arWinProbs[i];

	m_i32RandomItemCnt	= pGachaItem->m_i32RandomItemCnt;
	for( INT32 k = 0; k < m_i32RandomItemCnt; ++k )
		m_arRandomItems[k].SetRandomItemInfo(&pGachaItem->m_arRandomItems[k]);
}

void CGachaItem::MakeGachaItemInfoPacket(GACHA_ITEM_INFO* stGachaItem)
{
	stGachaItem->m_i32ItemKey			= m_i32ItemKey;
	stGachaItem->m_ui32GoodsID			= m_ui32GoodsID;;
	stGachaItem->m_i32RemainAmount		= m_i32RemainAmount;
	stGachaItem->m_i32TotalAmount		= m_i32TotalAmount;
	stGachaItem->m_i32GloryCount		= m_i32GloryCount;

	for( INT32 i = 0; i < m_i32RandomItemCnt; ++i )
		stGachaItem->m_arRandomGoodsIDs[i] = m_arRandomItems[i].GetGoodsID();
}

void CGachaItem::MakeGachaItemInfoPacket(SS_GACHA_ITEM_INFO* stGachaItem)
{
	stGachaItem->m_i32ItemKey		= m_i32ItemKey;
	stGachaItem->m_ui32GoodsID		= m_ui32GoodsID;;
	stGachaItem->m_i32RemainAmount	= m_i32RemainAmount;
	stGachaItem->m_i32TotalAmount	= m_i32TotalAmount;
	stGachaItem->m_i32GloryCount	= m_i32GloryCount;

	for( INT32 i = 0; i < PROB_SECTION_COUNT; ++i )
		stGachaItem->m_arWinProbs[i] = m_arWinningProbs[i];

	stGachaItem->m_i32RandomItemCnt = m_i32RandomItemCnt;
 	for( INT32 i = 0; i < m_i32RandomItemCnt; ++i )
		m_arRandomItems[i].MakeRandomItemPacket(&stGachaItem->m_arRandomItems[i]);
}

UINT32 CGachaItem::GetWinningRandomGoodsID()
{
	INT32 i32GoodsIdx = RAND_DK(PROBABILITY_MAX);

	INT32 i32ProbSum = 0;
	for( INT32 i = 0; i < m_i32RandomItemCnt; ++i )
	{
		i32ProbSum += m_arRandomItems[i].GetProbability();

		if( i32GoodsIdx < i32ProbSum )
			return m_arRandomItems[i].GetGoodsID();
	}

	return INVALID_GOODS_ID;
}

//////////////////////////////////////////////////////////////////////////
//
// CGachaItemGroup
//
CGachaGroup::CGachaGroup()
: m_eGachaItemGroup(GACHA_GROUP_NONE)
, m_i32LuckySalesVolume(0)
, m_i32LuckyProbability(0)
, m_ui32LuckyGoodsID(0)
//, m_i32GloryItemCount(INVALID_GLORY_CNT)
, m_i32WinItemCount(0)
, m_i32RandomItemCnt(0)
{
	for( INT32 i = 0; i < RANDOM_ITEM_COUNT_BY_TAB; ++i )
		m_arRandomGoodsIDs[i] = INVALID_GOODS_ID;
}

CGachaGroup::~CGachaGroup()
{
}

UINT32 CGachaGroup::IsWinningLucky()
{
	++m_i32LuckySalesVolume;

	INT32 i32CurSales = m_i32LuckyProbability - m_i32LuckySalesVolume;
	if( i32CurSales != m_i32LuckyWinningIdx )
		return INVALID_GOODS_ID;

	ResetLuckyWinningIdx();

	return m_ui32LuckyGoodsID;
}

bool CGachaGroup::IsValidRequest(INT32 i32ItemIdx, INT32 i32ItemKey)
{
	if( m_i32WinItemCount <= i32ItemIdx )
		return false;

	if( m_arWinItems[i32ItemIdx].GetItemKey() != i32ItemKey )
		return false;

	return true;
}

void CGachaGroup::ResetLuckyWinningIdx()
{
	m_i32LuckySalesVolume = 0;

	if( m_i32LuckyProbability == 0 )
		m_i32LuckyWinningIdx = INVALID_WIN_INDEX;

	m_i32LuckyWinningIdx = RAND_DK(m_i32LuckyProbability);
}

GACHA_RESULT CGachaGroup::IsWinningGacha(INT32 i32ItemIdx, INT32 i32ItemKey, INT32 i32Purchase, OUT UINT32& ui32WinningGoodsID)
{
	if( !CheckItemIndexValidation(i32ItemIdx) )
		return GACHA_RESULT_NONE;

	if( m_arWinItems[i32ItemIdx].GetItemKey() != i32ItemKey )
		return GACHA_RESULT_NONE;

	CGachaItem* pGachaItem = &m_arWinItems[i32ItemIdx];
	if( pGachaItem->IsWinningWin() )
	{
		ui32WinningGoodsID = pGachaItem->GetGoodsID();
		return GACHA_RESULT_WIN;
	}

	if( pGachaItem->IsWinningGlory(i32Purchase) )
	{
		ui32WinningGoodsID = pGachaItem->GetGoodsID();
		return GACHA_RESULT_GLORY; 
	}

	UINT32 ui32RandomGoodsID = pGachaItem->GetWinningRandomGoodsID();
	if( ui32RandomGoodsID == INVALID_GOODS_ID )
	{
		if( m_i32RandomItemCnt == 0 )
			return GACHA_RESULT_NONE;

		INT32 i32RandomItemIdx = RAND_DK(m_i32RandomItemCnt);
		ui32RandomGoodsID = m_arRandomGoodsIDs[i32RandomItemIdx];
	}

	if( ui32RandomGoodsID != INVALID_GOODS_ID )
	{
		ui32WinningGoodsID = ui32RandomGoodsID;
		return GACHA_RESULT_RANDOM;
	}

	return GACHA_RESULT_NONE;
}

void CGachaGroup::MakeGachaInfoPacket(SS_GACHA_GROUP_INFO* pGachaGroup)
{
	pGachaGroup->m_eGachaGroup			= m_eGachaItemGroup;
	pGachaGroup->m_ui32LuckyGoodsID		= m_ui32LuckyGoodsID;
 	pGachaGroup->m_i32LuckyProbability	= m_i32LuckyProbability;
 	//pGachaGroup->m_i32GloryCount		= m_i32GloryItemCount;
	pGachaGroup->m_i32WinItemCount		= m_i32WinItemCount;
	pGachaGroup->m_i32RandomItemCnt		= m_i32RandomItemCnt;

	for( INT32 i = 0; i < m_i32WinItemCount; ++i )
		m_arWinItems[i].MakeGachaItemInfoPacket(&pGachaGroup->m_arWinItems[i]);

	for( INT32 i = 0; i < m_i32RandomItemCnt; ++i )
		pGachaGroup->m_arRandomGoodsIDs[i] = m_arRandomGoodsIDs[i];
}

void CGachaGroup::MakeGachaInfoPacket(GACHA_GROUP_INFO* pGachaGroup)
{
	pGachaGroup->m_eGachaGroup			= m_eGachaItemGroup;
	pGachaGroup->m_ui32LuckyGoodsID		= m_ui32LuckyGoodsID;
	//pGachaGroup->m_i32GloryCount		= m_i32GloryItemCount;

	pGachaGroup->m_i32WinItemCount		= m_i32WinItemCount;
	for( INT32 i = 0; i < m_i32WinItemCount; ++i )
		m_arWinItems[i].MakeGachaItemInfoPacket(&pGachaGroup->m_arWinItems[i]);

	pGachaGroup->m_i32RandomItemCnt		= m_i32RandomItemCnt;
	for( INT32 i = 0; i < m_i32RandomItemCnt; ++i )
		pGachaGroup->m_arRandomGoodsIDs[i] = m_arRandomGoodsIDs[i];
}

void CGachaGroup::SetGachaGroupInfo(SS_GACHA_GROUP_INFO* pGachaGroup)
{
	m_eGachaItemGroup		= pGachaGroup->m_eGachaGroup;
	m_ui32LuckyGoodsID		= pGachaGroup->m_ui32LuckyGoodsID;
 	m_i32LuckyProbability	= pGachaGroup->m_i32LuckyProbability;
 	//m_i32GloryItemCount		= pGachaGroup->m_i32GloryCount;
	m_i32WinItemCount		= pGachaGroup->m_i32WinItemCount;
	m_i32RandomItemCnt		= pGachaGroup->m_i32RandomItemCnt;

	for( INT32 i = 0; i < pGachaGroup->m_i32WinItemCount; ++i )
		m_arWinItems[i].SetGachaItemInfo(&pGachaGroup->m_arWinItems[i],m_eGachaItemGroup );

	for( INT32 i = 0; i < pGachaGroup->m_i32RandomItemCnt; ++i )
		m_arRandomGoodsIDs[i] = pGachaGroup->m_arRandomGoodsIDs[i];
}

void CGachaGroup::Initialize()
{
	ResetLuckyWinningIdx();

	for( INT32 i = 0; i < m_i32WinItemCount; ++i )
		m_arWinItems[i].ResetWinningIndex();
}

INT32 CGachaGroup::GetItemKey(INT32 i32ItemIdx)
{
	if( !CheckItemIndexValidation(i32ItemIdx) )
		return INVALID_ITEM_KEY;

	return m_arWinItems[i32ItemIdx].GetItemKey();
}

INT32 CGachaGroup::GetRemainAmount(INT32 i32ItemIdx)
{
	if( !CheckItemIndexValidation(i32ItemIdx) )
		return 0;

	return m_arWinItems[i32ItemIdx].GetRemainAmount();
}

void CGachaGroup::SetRemainAmount(INT32 i32ItemIdx, INT32 i32ItemKey, INT32 i32RemainCount)
{
	if( !CheckItemIndexValidation(i32ItemIdx) )
		return;

	if( m_arWinItems[i32ItemIdx].GetItemKey() != i32ItemKey )
		return;

	m_arWinItems[i32ItemIdx].SetRemainAmount(i32RemainCount);
}

void CGachaGroup::MakeGachaStatePacket(GACHA_GROUP_STATE* pPacket)
{
	for( INT32 i = 0; i < m_i32WinItemCount; ++i )
		pPacket->m_arRemainCounts[i] = m_arWinItems[i].GetRemainAmount();
}

void CGachaGroup::MakeGachaStatePacketForClient(i3NetworkPacket* pPacket)
{
	pPacket->WriteData( &m_i32WinItemCount,		sizeof( INT32 ) );

	INT32 i32RemainCount = 0;
	for( INT32 i = 0; i < m_i32WinItemCount; ++i )
	{
		i32RemainCount = m_arWinItems[i].GetRemainAmount();
		pPacket->WriteData( &i32RemainCount,		sizeof( INT32 ) );
	}
}

void CGachaGroup::UpdateGachaState(GACHA_GROUP_STATE* pGachaState)
{
	for( INT32 i = 0; i < m_i32WinItemCount; ++i )
		m_arWinItems[i].SetRemainAmount(pGachaState->m_arRemainCounts[i]);
}

bool CGachaGroup::CheckItemIndexValidation(INT32 i32ItemIndex)
{
	if( i32ItemIndex < 0 || m_i32WinItemCount <= i32ItemIndex )
	{
		I3TRACE("[CGachaGroup::CheckItemIndexValidation] Invalid item index.");
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
// CGachaShop
//
CGachaShop::CGachaShop()
: m_i32NoticeCnt(0)
, m_ui32LuckGoodsID(INVALID_GOODS_ID)
, m_ui32LuckyTime(0)
, m_hGachaShopFile(0)
{
}

UINT32 CGachaShop::IsWinningLucky(GACHA_GROUP eGachaGroup, TTCHAR* szCharName)
{
	if( !CheckGroupValidation(eGachaGroup) )
		return INVALID_GOODS_ID;

	UINT32 ui32LuckyGoodsID = m_arGachaGroups[eGachaGroup].IsWinningLucky();
	if( ui32LuckyGoodsID != INVALID_GOODS_ID )
		SetLuckyUser(szCharName, ui32LuckyGoodsID);

	return ui32LuckyGoodsID;
}

GACHA_RESULT CGachaShop::IsWinningGacha(GACHA_GROUP eGachaGroup, INT32 i32ItemIdx, INT32 i32ItemKey, INT32 i32Purchase, OUT UINT32& ui32WinningGoodsID)
{
	if( !CheckGroupValidation(eGachaGroup) )
		return GACHA_RESULT_NONE;

	return m_arGachaGroups[eGachaGroup].IsWinningGacha(i32ItemIdx, i32ItemKey, i32Purchase, ui32WinningGoodsID);
}
//
//UINT32 CGachaShop::GetPaymentGoodsID(GACHA_GROUP eGachaGroup, INT32 i32Amount )
//{
//	if( !CheckGroupValidation(eGachaGroup) )
//		return INVALID_GOODS_ID;
//
//	switch( i32Amount ) 
//	{
//	case PAYMENT_COUNT_1:
//	case PAYMENT_COUNT_5:
//	case PAYMENT_COUNT_10:
//		break;
//	default:
//		return INVALID_GOODS_ID;
//	}
//
//	return m_arGachaGroups[eGachaGroup].GetPaymentGoodsID(i32Amount);
//}

//INT32 CGachaShop::GetRequireTicket(GACHA_GROUP eGachaGroup)
//{
//	if( !CheckGroupValidation(eGachaGroup) )
//		return INVALID_TICKET_CNT;
//
//	return m_arGachaGroups[eGachaGroup].GetRequireTicket();
//}

INT32 CGachaShop::GetItemKey(GACHA_GROUP eGachaGroup, INT32 i32ItemIdx)
{
	if( !CheckGroupValidation(eGachaGroup) )
		return INVALID_ITEM_KEY;

	return m_arGachaGroups[eGachaGroup].GetItemKey(i32ItemIdx);
}

INT32 CGachaShop::GetRemainAmount(GACHA_GROUP eGachaGroup, INT32 i32ItemIdx)
{
	if( !CheckGroupValidation(eGachaGroup) )
		return 0;

	return m_arGachaGroups[eGachaGroup].GetRemainAmount(i32ItemIdx);
}
//
//PURCHASE_TYPE	CGachaShop::GetPurchaseType(GACHA_GROUP eGachaGroup, INT32 i32ItemIdx)
//{
//	if( !CheckGroupValidation(eGachaGroup) )
//		return PURCHASE_COUNT;
//
//	return m_arGachaGroups[eGachaGroup].GetPurchaseType(i32ItemIdx);
//}
//
//UINT32	CGachaShop::GetGachaGoodsPrice(GACHA_GROUP eGachaGroup, INT32 i32ItemIdx, PAYMENT_COUNT_TYPE eCountType, PURCHASE_TYPE ePurchaseType)
//{
//	if( !CheckGroupValidation(eGachaGroup) )
//		return NULL;
//
//	return m_arGachaGroups[eGachaGroup].GetGachaGoodsPrice(i32ItemIdx, eCountType, ePurchaseType);
//}
//
//UINT32	CGachaShop::GetGachaGoodsID(GACHA_GROUP eGachaGroup, INT32 i32ItemIdx, PAYMENT_COUNT_TYPE eCountType)
//{
//	if( !CheckGroupValidation(eGachaGroup) )
//		return INVALID_GOODS_ID;
//
//	return m_arGachaGroups[eGachaGroup].GetGachaGoodsID(i32ItemIdx, eCountType);
//}
//
//char*	CGachaShop::GetGachaGoodsName(GACHA_GROUP eGachaGroup, INT32 i32ItemIdx, PAYMENT_COUNT_TYPE eCountType)
//{
//	if( !CheckGroupValidation(eGachaGroup) )
//		return DEFAULT_GACHA_GOODS_NAME;
//
//	return m_arGachaGroups[eGachaGroup].GetGachaGoodsName(i32ItemIdx, eCountType);
//}
// 

bool CGachaShop::CheckGroupValidation(GACHA_GROUP eGachaGroup)
{
	if( eGachaGroup < GACHA_GROUP_POOR || GACHA_GROUP_COUNT <= eGachaGroup )
	{
		I3TRACE("[CGachaShop::CheckGroupValidation] Invalid group index.\n");
		return false;
	}

	return true;
}
//
//INT32 CGachaShop::GetBonusCount(PAYMENT_COUNT_TYPE eCountType)
//{
//	switch( eCountType )
//	{
//	case PAYMENT_COUNT_1:
//		return 0;
//	case PAYMENT_COUNT_5:
//		return BONUS_COUNT_1ST;
//	case PAYMENT_COUNT_10:
//		return BONUS_COUNT_2ND;
//	}
//
//	return 0;
//}

bool CGachaShop::IsValidRequest(T_GoodsID TGoodsID, INT32 i32ItemIdx)
{
	GACHA_GROUP eGroup =  GetGachaGroupByGoodsID( TGoodsID );
	if( !CheckGroupValidation(eGroup) )
		return false;

	INT32 i32ItemKey =  GET_GOODS_NUMBER( TGoodsID );
 
	return m_arGachaGroups[eGroup].IsValidRequest(i32ItemIdx, i32ItemKey);
}

void CGachaShop::Initialize(GACHA_GROUP eGachaGroup)
{
	if( !CheckGroupValidation(eGachaGroup) )
		return;
	
	m_arGachaGroups[eGachaGroup].Initialize();
}

void CGachaShop::MakeGachaStatePacket(PACKET_GACHA_SHOP_STATE* pPacket)
{
	for( INT32 i = 0; i < GACHA_GROUP_COUNT; ++i )
		m_arGachaGroups[i].MakeGachaStatePacket(&(pPacket->m_arGachaGroups[i]));
}

void CGachaShop::MakeGachaStatePacketForClient(GACHA_GROUP eGroup, i3NetworkPacket* pPacket)
{
	if( !CheckGroupValidation(eGroup) )
		return;

	m_arGachaGroups[eGroup].MakeGachaStatePacketForClient(pPacket);
}

void CGachaShop::SetGachaRemainCount(GACHA_GROUP eGachaGroup, INT32 i32ItemIdx, INT32 i32ItemKey, INT32 i32RemainCount)
{
	if( !CheckGroupValidation(eGachaGroup) )
		return;

	m_arGachaGroups[eGachaGroup].SetRemainAmount(i32ItemIdx, i32ItemKey, i32RemainCount);
}

void CGachaShop::SetGachaGroupInfo(SS_GACHA_GROUP_INFO* pGachaGroup)
{
	if( !CheckGroupValidation(pGachaGroup->m_eGachaGroup) )
		return;

	m_arGachaGroups[pGachaGroup->m_eGachaGroup].SetGachaGroupInfo(pGachaGroup);
}

void CGachaShop::InsertNotice(TTCHAR* szNotice)
{
	if( m_i32NoticeCnt >= GACHA_NOTICE_COUNT ) m_i32NoticeCnt = 0;
	i3String::Copy(m_szNotices[m_i32NoticeCnt++], szNotice, GACHA_NOTICE_LENGTH);
}

void CGachaShop::SetLuckyUser(TTCHAR* szUserName, UINT32 ui32LuckyGoodsID)
{
	TTCHAR szTemp[NET_CHARA_NICK_NAME_SIZE] = {0, };

	i3String::Copy(szTemp, szUserName, NET_CHARA_NICK_NAME_SIZE);
	
	INT32 i32Index = 0;
	TTCHAR* szChar = szTemp;
	while( *szChar != '\0' )
	{
		if( i32Index % 2  )
			*szChar = '*';

		i32Index++;
		szChar++;
	}
	
	i3String::Copy(szUserName, szTemp, NET_CHARA_NICK_NAME_SIZE);
	i3String::Copy(m_szLuckyUser, szTemp, NET_CHARA_NICK_NAME_SIZE);

	m_ui32LuckGoodsID = ui32LuckyGoodsID;
}

void CGachaShop::MakePacketGachaInfo(GACHA_GROUP eGachaGroup, SS_GACHA_GROUP_INFO* pGachaGroup)
{
	if( !CheckGroupValidation(eGachaGroup) )
		return;

	m_arGachaGroups[eGachaGroup].MakeGachaInfoPacket(pGachaGroup);
}

void CGachaShop::MakePacketGachaInfo(GACHA_GROUP eGachaGroup, GACHA_GROUP_INFO* pGachaGroup)
{
	if( !CheckGroupValidation(eGachaGroup) )
		return;

	m_arGachaGroups[eGachaGroup].MakeGachaInfoPacket(pGachaGroup);
}

void CGachaShop::MakePacketGachaNotice(INT32 i32Index, PACKET_GACHA_NOTICE* pGachaNotice)
{
	if( i32Index < 0 || m_i32NoticeCnt <= i32Index )
		return;

	memcpy(pGachaNotice->m_szNotice, m_szNotices[i32Index], sizeof(TTCHAR) * GACHA_NOTICE_LENGTH);
}

void CGachaShop::UpdateGachaState(PACKET_GACHA_SHOP_STATE* pShopState)
{
	for(INT32 i = 0; i < GACHA_GROUP_COUNT; ++i )
		m_arGachaGroups[i].UpdateGachaState(&pShopState->m_arGachaGroups[i]);
}



void CGachaShop::MakeGachaShopMD5Key(void)
{
	// ShopFile로 MD5Key를 생성합니다.
	unsigned char md5Key[NET_MD5_HASH_SIZE] = {};
	UINT128 ui128Temp;
	
	i3MD5::Getmd5_file(GACHA_SHOP_FILE_NAME_ASCII, md5Key);
	memcpy(&ui128Temp, md5Key, sizeof(char)*NET_MD5_HASH_SIZE);
	i3MD5::GetMD5String(m_strGachaShopMD5Key, ui128Temp);
	 
	return;
}


BOOL CGachaShop::SaveGachaShopFile()
{
	// 파일이 있으면 지운다.
	DeleteGachaShopFile();

	INT32			   i32ReadLength = 0;
 
	if( CreateGachaShopFile() == FALSE)	return FALSE;

	// Group Info
	for( INT32 i = 0 ; i < GACHA_GROUP_COUNT ; ++i )
	{
		GACHA_GROUP	eGroup = (GACHA_GROUP)i;
		GACHA_GROUP_INFO stGroup;
		MakePacketGachaInfo( eGroup, &stGroup );

		::WriteFile(m_hGachaShopFile, &stGroup.m_eGachaGroup,					sizeof(GACHA_GROUP) ,		(LPDWORD)&i32ReadLength, NULL);
		if( stGroup.m_eGachaGroup > GACHA_GROUP_NONE )
		{
			//::WriteFile(m_hGachaShopFile, &stGroup.m_i32GloryCount,				sizeof(INT32) ,			(LPDWORD)&i32ReadLength, NULL);
			::WriteFile(m_hGachaShopFile, &stGroup.m_ui32LuckyGoodsID,			sizeof(UINT32) ,		(LPDWORD)&i32ReadLength, NULL);

			stGroup.m_i32WinItemCount = MIN( stGroup.m_i32WinItemCount, WIN_ITEM_COUNT_BY_TAB );
			::WriteFile(m_hGachaShopFile, &stGroup.m_i32WinItemCount,			sizeof(INT32) ,		(LPDWORD)&i32ReadLength, NULL);
			::WriteFile(m_hGachaShopFile, stGroup.m_arWinItems,					sizeof(GACHA_ITEM_INFO)*stGroup.m_i32WinItemCount ,		(LPDWORD)&i32ReadLength, NULL);

			stGroup.m_i32RandomItemCnt = MIN( stGroup.m_i32RandomItemCnt, RANDOM_ITEM_COUNT_BY_TAB);
			::WriteFile(m_hGachaShopFile, &stGroup.m_i32RandomItemCnt,			sizeof(INT32) ,		(LPDWORD)&i32ReadLength, NULL);
			::WriteFile(m_hGachaShopFile, stGroup.m_arRandomGoodsIDs,			sizeof(UINT32)*stGroup.m_i32RandomItemCnt ,				(LPDWORD)&i32ReadLength, NULL);
		}
	}
	
	if( INVALID_HANDLE_VALUE == m_hGachaShopFile )
	{			
		::CloseHandle( m_hGachaShopFile );
		m_hGachaShopFile = INVALID_HANDLE_VALUE; 
	}

	// GachaShop.dat 파일에 대한 MD5Key를 생성한다.
	MakeGachaShopMD5Key();
 
	return TRUE;
}

BOOL CGachaShop::CreateGachaShopFile()
{
	char	wstrFileName[MAX_PATH];

	if( m_hGachaShopFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hGachaShopFile );
		m_hGachaShopFile = INVALID_HANDLE_VALUE;
	}
	
	i3String::Copy(wstrFileName, GACHA_SHOP_FILE_NAME_ASCII, MAX_PATH);
	
	m_hGachaShopFile = ::CreateFile(wstrFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( INVALID_HANDLE_VALUE == m_hGachaShopFile )
	{			
		::CloseHandle( m_hGachaShopFile );
		m_hGachaShopFile = INVALID_HANDLE_VALUE; 
		return FALSE; 
	}

	SetFilePointer(m_hGachaShopFile, 0, NULL, FILE_BEGIN);

	return TRUE;
}


BOOL CGachaShop::DeleteGachaShopFile()
{
	char	wstrFileName[MAX_PATH];

	if( m_hGachaShopFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hGachaShopFile );
		m_hGachaShopFile = INVALID_HANDLE_VALUE;
	}

	i3String::Copy(wstrFileName, GACHA_SHOP_FILE_NAME_ASCII, MAX_PATH);

	if( !DeleteFile(wstrFileName) ) return FALSE;

	return TRUE;
}
