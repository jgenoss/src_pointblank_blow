#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

class CHashTable
{
private:
	i3BinList**			m_ppBinList;
	UINT32				m_ui32KeyCount;
	
public:
	CHashTable();
	virtual ~CHashTable();

	void				Destroy();

	BOOL				Create( UINT32 ui32TableSize, COMPAREPROC pCmpProc );
	BOOL				Insert( T_UID TUID, void* p );
	void				Delete( T_UID TUID, void* p );
	i3BinList*			Retrieve( T_UID TUID );

#if defined BUILDMODE_DLG
	void				DumpCount(void);
#endif
};

#endif
