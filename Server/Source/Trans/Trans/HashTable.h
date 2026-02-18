#ifndef _HashTable_h
#define _HashTable_h

class CHashTable: public i3ElementBase
{
	I3_CLASS_DEFINE(CHashTable);

private:
	i3BinList	**			m_ppBinList;
	UINT32					m_n32KeyCount;	
	
public:
	CHashTable();
	virtual ~CHashTable();

	void					Destroy();

	bool		Create( UINT32 nTableSize,COMPAREPROC pCmpProc);
	BOOL		Insert( T_UID TUID, USER_NODE* pNode );
	void		Delete( T_UID TUID, USER_NODE* pNode );
	i3BinList*	Retrieve( T_UID TUID );

	static UINT32 MakeKeyForString(const TTCHAR* pstrValue);

#if defined I3_DEBUG
	void DumpCount(void);
#endif
};

#endif //_HashTable_h
