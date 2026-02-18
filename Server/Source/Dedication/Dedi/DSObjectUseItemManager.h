#pragma once

#include "DSObject.h"

class CDSObjectUseItemManager
{
public:
	CDSObjectUseItemManager();
	~CDSObjectUseItemManager();

	void				SetObjectList(CDSObject* objectList, UINT8* objectidxList);
	void				SetObjectCount(INT32 count) { m_i32ObjectCount = count; }

	void				Reset();
	void				Update();

	bool				GetObjectInRPG7();

private:
	// 우선 최대 오브젝트 개수가 255개라 많아야 100개로 이거면 됩니다.
	CDSObject*			m_pObject[100];

	INT32				m_i32ObjectCount;
};