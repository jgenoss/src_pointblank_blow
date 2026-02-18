#ifndef _HashTable_h
#define _HashTable_h

#include "UserFinder.h"

class CHashTable: public i3ElementBase
{
	I3_CLASS_DEFINE(CHashTable);

private:
	i3BinList	**	m_ppBinList;
	UINT32			m_nKeyCount;

public:
	CHashTable();
	virtual ~CHashTable();

	void Create(UINT32 ui32KeyCount, COMPAREPROC pCmpProc);
    BOOL Insert(UINT32 ui32Key,USER_NODE* pNode);
	void Delete(UINT32 ui32Key,USER_NODE* pNode);
	i3BinList* Retrieve(UINT32 ui32Key);

	static UINT32 MakeKeyForINT(INT64 i64Value);
	static UINT32 MakeKeyForString(const TTCHAR* pstrValue);
	static UINT32 MakeKeyForStream(const UINT8* pui8stream,INT32 i32streamSize);

#if defined I3_DEBUG
	void DumpCount(void);
#endif
};

#endif //_HashTable_h
