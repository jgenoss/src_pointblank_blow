#include "pch.h"
#include "UserHash.h"
#include "User.h"

I3_CLASS_INSTANCE(CUserHash, i3ElementBase);

static INT32 UserIdCompare(void* p1,void* p2)
{
	CUser* pNode1 = (CUser*)p1;
	CUser* pNode2 = (CUser*)p2;

	return (INT32)pNode1->GetUserId() - (INT32)pNode2->GetUserId();
}

CUserHash::CUserHash()
{
	m_hashTableList = NULL;	
}

CUserHash::~CUserHash()
{
	if (m_hashTableList)
	{
		for(UINT32 i = 0; i < MAX_CONNECT_USER_DIVISOR; i++)
		{
			I3_SAFE_RELEASE(m_hashTableList[i]);
		}

		I3MEM_SAFE_FREE(m_hashTableList);
	}
}

BOOL CUserHash::Create(void)
{
	m_hashTableList = (i3BinList**)i3MemAlloc(sizeof(i3BinList*) * MAX_CONNECT_USER_DIVISOR);
	
	if (NULL == m_hashTableList)
	{
		return FALSE;
	}

	for(INT32 i = 0; i < MAX_CONNECT_USER_DIVISOR; i++)
	{
		m_hashTableList[i] = i3BinList::NewObject();
		
		if (NULL == m_hashTableList[i])
		{
			return FALSE;
		}

		m_hashTableList[i]->SetCompareProc(UserIdCompare);
	}

	return TRUE;
}

void CUserHash::Insert(CUser* node)
{
	INT32 key = node->GetUserId() % MAX_CONNECT_USER_DIVISOR;
	m_hashTableList[key]->Add(node);
}

BOOL CUserHash::Delete(UINT32 userId)
{
	CUser findNode;
	findNode.SetUserId(userId);

	INT32 key = userId % MAX_CONNECT_USER_DIVISOR;
	INT32 idx = m_hashTableList[key]->FindItem(&findNode);

	if (0 <= idx)
	{
		m_hashTableList[key]->Delete(idx);
		return TRUE;
	}

	return FALSE;
}

BOOL CUserHash::Retrieve(UINT32 userId,CUser** node)
{
	CUser findNode;
	findNode.SetUserId(userId);

	INT32 key = userId % MAX_CONNECT_USER_DIVISOR;
	INT32 idx = m_hashTableList[key]->FindItem(&findNode);

	if (0 <= idx)
	{
		*node = (CUser*)m_hashTableList[key]->GetItem(idx);
		return TRUE;
	}

	return FALSE;
}
