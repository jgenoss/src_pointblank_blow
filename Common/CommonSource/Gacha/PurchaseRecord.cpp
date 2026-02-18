#include "pch.h"
#include "PurchaseRecord.h"


//////////////////////////////////////////////////////////////////////////
//
// CPurchaseRecord
//
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

void CPurchaseRecord::MakePurchaseRecordPacket(GACHA_ITEM_INFO* pGachaItem)
{
	pGachaItem->m_i32PurchaseAmount = m_i32NumPurchase;
}



//////////////////////////////////////////////////////////////////////////
//
// CPurchaseManager
//
CPurchaseManager::CPurchaseManager()
: m_i32RecordCnt(0)
{
}

CPurchaseManager::~CPurchaseManager()
{
}

INT32 CPurchaseManager::IncPurchaseRecord(INT32 i32ItemKey, INT32 i32NumPurchase)
{
	for( INT32 i = 0; i < m_i32RecordCnt; ++i )
	{
		if( m_arPurchaseRecord[i].GetItemKey() != i32ItemKey )
			continue;

		return m_arPurchaseRecord[i].IncPurchaseRecord(i32NumPurchase);
	}

	m_arPurchaseRecord[m_i32RecordCnt].InsertPurchaseRecord(i32ItemKey, i32NumPurchase);
	++m_i32RecordCnt;

	return 0;
}

void CPurchaseManager::ResetPurchaseRecord(INT32 i32ItemKey)
{
	if( i32ItemKey == INVALID_ITEM_KEY )
		return;

	for( INT32 i = 0; i < m_i32RecordCnt; ++i )
	{
		if( m_arPurchaseRecord[i].GetItemKey() == i32ItemKey )
			m_arPurchaseRecord[i].ResetPurchaseRecord();
	}
}

INT32 CPurchaseManager::GetPurchaseCount(INT32 i32ItemKey)
{
	if( i32ItemKey == INVALID_ITEM_KEY )
		return 0;

	for( INT32 i = 0; i < m_i32RecordCnt; ++i )
	{
		if( m_arPurchaseRecord[i].GetItemKey() == i32ItemKey )
			return m_arPurchaseRecord[i].GetPurchaseCount();
	}

	return 0;
}

void CPurchaseManager::CopyPurchaseRecord(CPurchaseManager* pPurchaseMgr)
{
	pPurchaseMgr->m_i32RecordCnt = m_i32RecordCnt;

	for( INT32 i = 0; i < m_i32RecordCnt; ++i )
		m_arPurchaseRecord[i].CopyPurchaseRecord(&pPurchaseMgr->m_arPurchaseRecord[i]);
}

void CPurchaseManager::MakePurchaseRecordPacket(GACHA_GROUP_INFO* pGachaGroup)
{
	for( INT32 i = 0; i < GACHA_ITEM_COUNT_BY_TAB; ++i )
	{
		pGachaGroup->m_arGachaItems[i].m_i32PurchaseAmount = 0;

		for( INT32 j = 0; j < m_i32RecordCnt; ++j )
		{
			if( pGachaGroup->m_arGachaItems[i].m_i32ItemKey == m_arPurchaseRecord[j].GetItemKey() )
				m_arPurchaseRecord[j].MakePurchaseRecordPacket(&pGachaGroup->m_arGachaItems[i]);
		}
	}
}

CPurchaseRecord* CPurchaseManager::GetPurchaseRecord(INT32 i32Index)
{
	if( i32Index < 0 || m_i32RecordCnt <= i32Index )
		return NULL;

	return &m_arPurchaseRecord[i32Index];
}