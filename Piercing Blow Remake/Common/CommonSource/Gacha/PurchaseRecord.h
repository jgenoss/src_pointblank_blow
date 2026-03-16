#pragma once


//////////////////////////////////////////////////////////////////////////
//
// CPurchaseRecord
//
class CPurchaseRecord
{
public:
	INT32				InsertPurchaseRecord(INT32 i32ItemKey, INT32 i32NumPurchase);
	INT32				IncPurchaseRecord(INT32 i32NumPurchase);
	void				ResetPurchaseRecord()							{ m_i32NumPurchase = 0; }

	INT32				GetItemKey()									{ return m_i32ItemKey; }
	INT32				GetPurchaseCount()								{ return m_i32NumPurchase; }

	void				CopyPurchaseRecord(CPurchaseRecord* pPurchaseRecord);
	void				MakePurchaseRecordPacket(GACHA_ITEM_INFO* pGachaItem);


public:
	CPurchaseRecord();
	virtual ~CPurchaseRecord();


private:
	INT32				m_i32ItemKey;
	INT32				m_i32NumPurchase;
};



//////////////////////////////////////////////////////////////////////////
//
// CPurchaseManager
//
class CPurchaseManager
{
public:
	INT32				GetPurchaseCount(INT32 i32ItemKey);
	INT32				IncPurchaseRecord(INT32 i32ItemKey, INT32 i32NumPurchase = 1);
	void				ResetPurchaseRecord(INT32 i32ItemKey);

	INT32				GetRecordCount()					{ return m_i32RecordCnt; }
	CPurchaseRecord*	GetPurchaseRecord(INT32 i32Index);

	void				CopyPurchaseRecord(CPurchaseManager* pPurchaseMgr);
	void				MakePurchaseRecordPacket(GACHA_GROUP_INFO* pGachaGroup);


public:
	CPurchaseManager();
	virtual ~CPurchaseManager();


public:
	INT32				m_i32RecordCnt;
	CPurchaseRecord		m_arPurchaseRecord[GACHA_GROUP_COUNT*GACHA_ITEM_COUNT_BY_TAB];
};