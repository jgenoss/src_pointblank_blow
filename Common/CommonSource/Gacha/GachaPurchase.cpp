#include "pch.h"
#include "GachaPurchase.h"


//////////////////////////////////////////////////////////////////////////
// CPurchaseRecord
CPurchaseRecord::CPurchaseRecord()
: m_i32ItemKey(0)
, m_i32NumPurchase(0)
{
}

CPurchaseRecord::~CPurchaseRecord()
{
}

INT32 CPurchaseRecord::InsertPurchaseRecord(INT32 i32ItemKey, INT32 i32NumPurchase)
{
	m_i32ItemKey = i32ItemKey;
	m_i32NumPurchase = i32NumPurchase;

	return m_i32NumPurchase;
}

INT32 CPurchaseRecord::IncPurchaseRecord(INT32 i32NumPurchase)
{
	m_i32NumPurchase += i32NumPurchase;
	return m_i32NumPurchase;
}

void CPurchaseRecord::CopyPurchaseRecord(CPurchaseRecord* pPurchaseRecord)
{
	pPurchaseRecord->m_i32ItemKey = m_i32ItemKey;
	pPurchaseRecord->m_i32NumPurchase = m_i32NumPurchase;
}


//////////////////////////////////////////////////////////////////////////
// CPurchaseManager
CGachaPurchase::CGachaPurchase()
: m_i32RecordCount(0)
{
}

CGachaPurchase::~CGachaPurchase()
{
}

INT32 CGachaPurchase::IncPurchaseRecord(INT32 i32ItemKey, INT32 i32NumPurchase)
{
	for( INT32 i = 0; i < m_i32RecordCount; ++i )
	{
		if( m_arPurchaseRecord[i].GetItemKey() != i32ItemKey )
			continue;

		return m_arPurchaseRecord[i].IncPurchaseRecord(i32NumPurchase);
	}

	return m_arPurchaseRecord[m_i32RecordCount++].InsertPurchaseRecord(i32ItemKey, i32NumPurchase);
}

void CGachaPurchase::ResetPurchaseRecord(INT32 i32ItemKey)
{
	if( i32ItemKey == INVALID_ITEM_KEY )
		return;

	for( INT32 i = 0; i < m_i32RecordCount; ++i )
	{
		if( m_arPurchaseRecord[i].GetItemKey() == i32ItemKey )
			m_arPurchaseRecord[i].ResetPurchaseRecord();
	}
}

void CGachaPurchase::ResetPurchaseRecord()
{
	for( INT32 i = 0; i < m_i32RecordCount; ++i )
		m_arPurchaseRecord[i].ResetPurchaseRecord();
}

INT32 CGachaPurchase::GetPurchaseCount(INT32 i32ItemKey)
{
	if( i32ItemKey == INVALID_ITEM_KEY )
		return 0;

	for( INT32 i = 0; i < m_i32RecordCount; ++i )
	{
		if( m_arPurchaseRecord[i].GetItemKey() == i32ItemKey )
			return m_arPurchaseRecord[i].GetPurchaseCount();
	}

	return 0;
}

void CGachaPurchase::CopyPurchaseRecord(CGachaPurchase* pPurchaseMgr)
{
	pPurchaseMgr->m_i32RecordCount = m_i32RecordCount;

	for( INT32 i = 0; i < m_i32RecordCount; ++i )
		m_arPurchaseRecord[i].CopyPurchaseRecord(&pPurchaseMgr->m_arPurchaseRecord[i]);
}

void CGachaPurchase::MakeGachaPurchaseCount(GACHA_GROUP_INFO* pGachaGroup, i3NetworkPacket* pPacket)
{
	INT32 i32WinItemCount = pGachaGroup->GetWinItemCount();

	pPacket->WriteData( &i32WinItemCount, sizeof( INT32 ) );
	for( INT32 i = 0; i < i32WinItemCount ; ++i )
	{
		INT32 i32PurchaseCount = GetPurchaseCount(pGachaGroup->m_arWinItems[i].m_i32ItemKey);
		pPacket->WriteData( &i32PurchaseCount, sizeof( INT32 ) );
	}
}

CPurchaseRecord* CGachaPurchase::GetPurchaseRecord(INT32 i32Index)
{
	if( i32Index < 0 || m_i32RecordCount <= i32Index )
		return NULL;

	return &m_arPurchaseRecord[i32Index];
}
