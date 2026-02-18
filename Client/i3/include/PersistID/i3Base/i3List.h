// i3List.h

#if !defined( __i3LIST_H)
#define __i3LIST_H

#include "i3ElementBase.h"
#include "i3MetaTemplate.h"
#include "i3Stream.h"
#include "i3XML.h"

class i3ResourceFile;
class i3XMLFile;

typedef INT32 (*COMPAREPROC)( void * p1, void *p2);

class I3_EXPORT_BASE i3List : public i3ElementBase
{
	I3_CLASS_DEFINE( i3List);

protected :

	INT32	m_MaxCount;
	INT32	m_Count;
	INT32	m_AllocUnit;
	BOOL	m_bOptimizeWhenClear;
	BOOL	m_bOnceMode;
//	INT32	m_ItemsIdx;

public:

	void	**	Items;
	
protected:

	BOOL ResizeBuffer( INT32 Count);

	INT32	_Partition( INT32 si, INT32 ei, COMPAREPROC pProc);
	void	_QuickSort( INT32 si, INT32 ei, COMPAREPROC pProc);

public:

	i3List( INT32 AllocUnit = 16);
	~i3List(void);

	virtual INT32 IndexOf( void * p);

	virtual INT32 Add( void * p);
	virtual INT32 Insert( INT32 Idx, void * p);
	virtual INT32 Delete( INT32 Idx);
	virtual INT32 Remove( void * p);

	virtual void Clear( BOOL FreeItem = FALSE);

	void	SetOptmizeEnableWhenClear( BOOL bFlag)				{ m_bOptimizeWhenClear = bFlag; }

	void	SetAllocationUnit( INT32 AllocUnit);

	void	SetOnceMode( BOOL bFlag = TRUE ){ m_bOnceMode = bFlag;	}

	INT32	GetCount(void)					{ return m_Count; }
	void	SetCount( INT32 Count);
	void *	GetItem( INT32 idx)				{ return Items[ idx]; }
	void	SetItem( INT32 idx, void * p);
	INT32	Find( void * p)					{ return IndexOf( p); }

	void *	GetFirst(void)					{ return Items[ 0]; }
	void *	GetLast(void)					{ return Items[ m_Count-1]; }

	void	Sort( COMPAREPROC pProc);

	UINT32	SaveTo( i3ResourceFile * pResFile);
	UINT32	LoadFrom( i3ResourceFile * pResFile, bool bAddRef = false);

	bool	SaveToXML( const char * pszName, i3XMLFile * pFile, i3XMLElement * pXML, bool bRef = false);
	bool	LoadFromXML( const char * pszName, i3XMLFile * pFile, i3XMLElement * pXML, bool bAddRef = false);

	void	Copy( i3List *List);
	void	AddFromList( i3List * pSrcList)
	{
		INT32 i;
		for( i = 0; i  < pSrcList->GetCount(); i++)
		{
			Add( pSrcList->Items[i]);
		}
	}

	void	OptimizeMemory(void);

	template<class T>
	void SafeDeleteAll()
	{
		INT32 i,n = GetCount();
		for( i=0; i<n; i++)
		{
			T *pItem = (T*)GetItem(i);
			if( pItem != NULL )
			{
				delete pItem;
			}
		}
		Clear();
	}

	// 기존에 template 사용하던 SafeReleaseAll< ClassName >() 방식이 SafeReleaseAll() 로 바뀌었습니다.
	// SafeReleaseAll() 로 사용하시길 바랍니다.

	template<class T>
	void TSafeReleaseAll()
	{
		INT32 i,n = GetCount();
		for( i=0; i<n; i++)
		{
			T *pItem = (T*)GetItem(i);
			if( pItem != NULL )
			{
				pItem->Release();
			}
		}
		Clear();
	}

	void SafeReleaseAll()
	{
		INT32 i,n = GetCount();
		for( i=0; i<n; i++)
		{
			i3ElementBase *pItem = (i3ElementBase*)GetItem(i);
			if( pItem != NULL )
			{
				pItem->Release();
			}
		}
		Clear();
	}
};

class I3_EXPORT_BASE i3StringList : public i3ElementBase
{
	I3_CLASS_DEFINE( i3StringList);

protected :

	INT32	m_MaxCount;
	INT32	m_Count;
	INT32	m_AllocUnit;

	BOOL	m_bOnceMode;
	BOOL	m_bCRLF;
//	INT32	m_ItemsIdx; 

public:

	char **	Items;

protected:

	BOOL	ResizeBuffer( INT32 nCount);

public:
	i3StringList( INT32 AllocUnit = 16);
	~i3StringList(void);

	virtual INT32 IndexOf( char * p);

	virtual INT32 Add( char * p, INT32 Length = -1);
	virtual INT32 Insert( INT32 Idx, char * p, INT32 Length = -1);
	virtual INT32 Delete( INT32 Idx);
	virtual INT32 Remove( char * p);

	virtual void Clear(void);

	void	SetAllocationUnit( INT32 AllocUnit);

	INT32	GetCount(void)					{ return m_Count; }
	void *	GetItem( INT32 idx)				{ return Items[ idx]; }
	void	SetItem( INT32 idx, char * p)		{ Items[idx] = p; }
	INT32	Find( char * p)					{ return IndexOf( p); }

	char *	GetFirst(void)					{ return Items[ 0]; }
	char *	GetLast(void)					{ return Items[ m_Count-1]; }

	BOOL	GetOnceMode(void)				{ return m_bOnceMode; }
	void	SetOnceMode( BOOL bFlag)		{ m_bOnceMode = bFlag; }
	
	void	OptimizeMemory(void);

	INT32    GetBufferSize(void);
	void	CopyFromBuffer( char *Buf);
	void	CopyToBuffer( char *Buf, INT32 Limit, INT32 * pOffsetTable = NULL);
	char *  MakeFileFilter(void);

	UINT32	SaveTo( i3ResourceFile * pResFile);
	UINT32	LoadFrom( i3ResourceFile * pResFile);
};

#endif
