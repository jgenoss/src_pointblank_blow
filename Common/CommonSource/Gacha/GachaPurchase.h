#pragma once


//////////////////////////////////////////////////////////////////////////
// CPurchaseRecord
class CPurchaseRecord
{
public:
	INT32				InsertPurchaseRecord(INT32 i32ItemKey, INT32 i32NumPurchase);
	INT32				IncPurchaseRecord(INT32 i32NumPurchase);
	void				ResetPurchaseRecord()							{ m_i32NumPurchase = 0; }

	INT32				GetItemKey()									{ return m_i32ItemKey; }
	INT32				GetPurchaseCount()								{ return m_i32NumPurchase; }

	void				CopyPurchaseRecord(CPurchaseRecord* pPurchaseRecord);


public:
	CPurchaseRecord();
	virtual ~CPurchaseRecord();

	
private:
	INT32				m_i32ItemKey;
	INT32				m_i32NumPurchase;
};



//////////////////////////////////////////////////////////////////////////
// CPurchaseManager
class CGachaPurchase
{
public:
	INT32				GetPurchaseCount(INT32 i32ItemKey);
	INT32				IncPurchaseRecord(INT32 i32ItemKey, INT32 i32NumPurchase);
	void				ResetPurchaseRecord(INT32 i32ItemKey);
	void				ResetPurchaseRecord();

	void				SetRecordCount(INT32 i32RecordCount)	{ m_i32RecordCount = i32RecordCount; }
	INT32				GetRecordCount()						{ return m_i32RecordCount; }
	CPurchaseRecord*	GetPurchaseRecord(INT32 i32Index);

	void				CopyPurchaseRecord(CGachaPurchase* pPurchaseMgr);
	//void				MakePurchaseRecordPacket(GACHA_GROUP_INFO* pGachaGroup);
	void				MakeGachaPurchaseCount(GACHA_GROUP_INFO* pGachaGroup, i3NetworkPacket* pPacket);

public:
	CGachaPurchase();
	virtual ~CGachaPurchase();


public:
	INT32				m_i32RecordCount;
	CPurchaseRecord		m_arPurchaseRecord[GACHA_GROUP_COUNT*WIN_ITEM_COUNT_BY_TAB];
};