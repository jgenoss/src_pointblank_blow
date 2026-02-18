// i3List.cpp

#include "i3CommonType.h"
#include "../include/i3Base/i3Memory.h"
#include "../include/i3Base/i3String.h"
#include "i3List.h"
#include "../include/i3Base/i3Debug.h"
#include "../include/i3Base/i3ErrorHandler.h"
#include "../include/i3Base/i3Performance.h"
#include "../include/i3Base/i3Macro.h"
#include "../include/i3Base/i3ResourceFile.h"
#include "../include/i3Base/i3XMLFile.h"


#define		NEARESTMULTIPLE( val, unit)			(((val + unit - 1) / unit) * unit)

I3_CLASS_INSTANCE( i3List, i3ElementBase);
I3_CLASS_INSTANCE( i3StringList, i3ElementBase);

i3List::i3List( INT32 AllocUnit)
{
	I3ASSERT(AllocUnit > 0);

	m_MaxCount				= 0;
	m_Count					= 0;
	m_AllocUnit				= AllocUnit;
	m_bOptimizeWhenClear	= TRUE;
	m_bOnceMode				= FALSE;
	Items					= NULL;

#if defined(USING_DEFAULT_STATIC_BUFFER)
	i3mem::FillZero(_defaultBuf, sizeof(_defaultBuf));
	m_bIsUseMalloc = FALSE; 
#endif

#if defined(USING_NON_RECURSIVE_QSORT)
	m_pStackBuffer		= NULL;
	m_nStackDepth		= 0;
	m_nStackTop			= -1;
#endif
}

i3List::~i3List(void)
{
	__Free();
}

inline	void	i3List::__Free()
{
#if defined(USING_DEFAULT_STATIC_BUFFER)
	if( m_bIsUseMalloc )I3MEM_SAFE_FREE_POINTER(Items);
#else
	I3MEM_SAFE_FREE_POINTER( Items ); 
#endif

#if defined(USING_NON_RECURSIVE_QSORT)
	I3MEM_SAFE_FREE_POINTER( m_pStackBuffer);
	m_nStackDepth		= 0;
	m_nStackTop			= -1;
#endif
}

BOOL i3List::ResizeBuffer( INT32 nCount )
{
	INT32 NewCount = NEARESTMULTIPLE( nCount, m_AllocUnit );

#if defined(USING_DEFAULT_STATIC_BUFFER)
	// 새로운 할당사이즈가 멤버 배열로 커버할 수 있다면
	// 멤버 배열로 생성한다.
	if( m_MaxCount != NewCount)
	{
		if (m_bIsUseMalloc)
		{
			Items = (void **) i3MemReAlloc( Items,  NewCount * sizeof(void *) );
			if(NULL == Items)
			{	
				// I3Log 남길것				
			#ifdef UNLOCK_LOG
				I3BCLOG(LOCATION, "[ERROR]ResizeBuffer Fail(%d)[m_MaxCount(%d) != NewCount(%d)] ", nCount, m_MaxCount, NewCount);				
			#endif
				return FALSE;
			}
			m_MaxCount = NewCount;
		}
		else
		{
			Items = _defaultBuf;
			m_MaxCount = DEFAULT_STATIC_BUFFER_SIZE;

			if ( nCount >= DEFAULT_STATIC_BUFFER_SIZE)
			{
				Items = (void **) i3MemAlloc( (NewCount * sizeof(void *)));
				i3mem::Copy(Items, _defaultBuf, sizeof(_defaultBuf));
				m_bIsUseMalloc = TRUE;
				m_MaxCount = NewCount;
			}
		}
	}
#else

	if( m_MaxCount != NewCount)
	{
		if( Items == NULL)
		{
			Items = (void **) i3MemAlloc( (NewCount * sizeof(void *)));
		}
		else
		{
			Items = (void **) i3MemReAlloc( Items,  NewCount * sizeof(void *) );
		}
	}
	m_MaxCount = NewCount;
#endif

	return TRUE;
}

void i3List::SetAllocationUnit( INT32 AllocUnit)
{
	INT32 NewCount;

	I3ASSERT( AllocUnit > 0);

	m_AllocUnit = AllocUnit;

	if( m_MaxCount > 0)
	{
		NewCount = NEARESTMULTIPLE( m_MaxCount, m_AllocUnit);

		ResizeBuffer( NewCount);
	}

	#ifdef UNLOCK_LOG
	I3BCLOG(LOCATION, "i3List::SetAllocationUnit() ResizeBuffer(m_AllocUnit(%d), NewCount(%d))", m_AllocUnit, NewCount);
	#endif
}

void i3List::OptimizeMemory(void)
{
	ResizeBuffer( m_Count);
	#ifdef UNLOCK_LOG
	I3BCLOG(LOCATION, "i3List::OptimizeMemory() ResizeBuffer(m_Count(%d))", m_Count);
	#endif
}

void i3List::SetCount( INT32 NewCount)
{
	if( NewCount+1 >= m_MaxCount) 
	{
		ResizeBuffer( NewCount + 1);
		#ifdef UNLOCK_LOG
		I3BCLOG(LOCATION, "i3List::SetCount() ResizeBuffer(NewCount(%d)+1 >= m_MaxCount(%d))", NewCount, m_MaxCount);
		#endif
	}

	m_Count = NewCount;
}

void i3List::SetItem( INT32 idx, void * p)
{
	if( idx >= m_MaxCount)
	{
		ResizeBuffer( idx + 1);
		#ifdef UNLOCK_LOG
		I3BCLOG(LOCATION, "i3List::SetItem() ResizeBuffer(idx(%d) >= m_MaxCount(%d))", idx, m_MaxCount);
		#endif
	}

	Items[idx] = p;
}

INT32 i3List::IndexOf( const void * p) const
{
	INT32 i;
	
	for( i = 0; i < m_Count; i++) 
	{
		if( Items[i] == p)
		{
			return i;
		}
	}

	return -1;
}

INT32 i3List::Add( void * p)
{
	BOOL Rv;

	if( m_bOnceMode == (BOOL)TRUE) 
	{
		INT32 idx;

		idx = IndexOf( p);
		if( idx != -1)
		{
			return idx;
		}
	}

	if( m_Count+1 >= m_MaxCount)
	{
		Rv = ResizeBuffer( m_Count + 1);
		#ifdef UNLOCK_LOG
		I3BCLOG(LOCATION, "i3List::Add() ResizeBuffer(m_Count(%d) + 1 >= m_MaxCount(%d))", m_Count, m_MaxCount);
		#endif
		if( Rv == FALSE) 
		{
			I3TRACE( "i3List::Add() ResizeBuffer() failed\n");
			#ifdef UNLOCK_LOG
			I3BCLOG(LOCATION, "[Fail]i3List::Add() ResizeBuffer(%d + 1) failed(m_MaxCount(%d))", m_Count, m_MaxCount);
			#endif
			return -1;
		}
	}

	Items[m_Count] = p;
	m_Count++;

	return m_Count-1;
}


INT32 i3List::Insert( INT32 idx, void * p)
{
	BOOL Rv;
	size_t MoveSize;

	if(m_Count+1 >= m_MaxCount)
	{
		Rv = ResizeBuffer( m_Count + 1);
		#ifdef UNLOCK_LOG
		I3BCLOG(LOCATION, "i3List::Insert(%d) ResizeBuffer(m_Count(%d) + 1 >= m_MaxCount(%d))", idx, m_Count, m_MaxCount);				
		#endif
		if( Rv == FALSE)
		{
			#ifdef UNLOCK_LOG
			I3BCLOG(LOCATION, "[Fail]i3List::Insert(%d) ResizeBuffer(m_Count(%d) + 1 >= m_MaxCount(%d))", idx, m_Count, m_MaxCount);				
			#endif
			return -1;
		}
	}

	MoveSize = ( m_Count - idx) * sizeof(void *);
	if( MoveSize > 0)
	{
		memmove( &(Items[idx+1]), &(Items[idx]), MoveSize);
	}

	Items[idx] = p;
	m_Count++;

	return idx;
}

INT32 i3List::Delete( INT32 Idx)
{
	INT32 MoveLength;

	if( m_Count == 0)
	{
		return -1;
	}

	MoveLength = m_Count - Idx - 1;
	if( MoveLength > 0)
		memcpy( &Items[Idx], &Items[Idx+1], MoveLength * sizeof(void *));
	m_Count--;

	return m_Count;
}

INT32 i3List::Remove( void * p)
{
	INT32 Idx;

	Idx = IndexOf( p);
	if( Idx == -1)
	{
		return -1;
	}

	Delete( Idx);

	return Idx;
}

void i3List::Clear( BOOL bFreeItem)
{
	if( bFreeItem == FALSE)
	{
		if( m_bOptimizeWhenClear == TRUE)
		{
			ResizeBuffer( m_AllocUnit);
			#ifdef UNLOCK_LOG
			I3BCLOG(LOCATION, "i3List::Clear() ResizeBuffer(m_AllocUnit(%d))", m_AllocUnit);
			#endif
		}

		m_Count = 0;
	}
	else
	{
		m_MaxCount = m_Count = 0;
		__Free();
	}
}

void i3List::_InsertionSort( INT32 si, INT32 ei, COMPAREPROC pProc)
{
	INT32 i, j;
	for( i=si+1; i<=ei; i++)
	{
		const void* temp = Items[ i];
		j = i;

		while( --j >= 0 && pProc( Items[j], (void*) temp) > 0) 
		{
			Items[j+1] = Items[j];
		}

		Items[j+1] = (void*) temp;
	}
}

INT32 i3List::_Partition( INT32 si, INT32 ei, COMPAREPROC pProc)
{	//	위키피아에 있는 퀵정렬 알고리즘을 사용함.
	//	http://ko.wikipedia.org/wiki/%ED%80%B5_%EC%A0%95%EB%A0%AC#.EC.86.8C.EC.8A.A4.EC.BD.94.EB.93.9C
	INT32 pivot = si, left = si, right = ei;
	//INT32 median = si + (abs(ei - si) / 2);		//	중간 위치

	const void * pPivotItem = Items[ si];	//	주의! const로 지정해야 밑에서 얘기치 않게 포인터 주소가 바뀌는 상황을 방지함.

	while( left < right)
	{
		while( (pProc( Items[ right], (void*) pPivotItem) >= 0) && (left < right))
		{
			right--;
		}

		if( left != right)
		{
			Items[ left] = Items[ right];

			left++;
		}

		while( (pProc( Items[ left], (void*) pPivotItem) <= 0) && (left < right))
		{
			left++;
		}

		if( left != right)
		{
			Items[ right] = Items[ left];

			right--;
		}
	}

	Items[ left] = (void*) pPivotItem;

	pivot = left;

	return pivot;

	/* 기존 소스
	INT32 pi, i;

	pi = si;
	
	for( i = si + 1; i <= ei; i++)
	{
		if( pProc( Items[i], Items[pi]) < 0)
		{
			pi++;

			// Swap
			{
				void * pTemp = Items[i];
				Items[i] = Items[ pi];
				Items[ pi] = pTemp;
			}
		}
	}

	// Swap
	{
		void * pTemp = Items[si];
		Items[si] = Items[ pi];
		Items[pi] = pTemp;
	}
	
	return pi;*/	
}

/*--------------------------------------------------
Recursive Quick sort (by komet)

장점:
정렬할 데이터가 이미 준비되어 있고 모든 데이터를 정렬해야 할 경우 가장 빠른 수행속도를 보여준다.

단점:
과다로 재귀호출시 스택 오버플로 나므로 사용시 주의 요함.
축(Pivot)값이 같은 것끼리는 순서관계가 파괴된다. (중요한 데이터의 경우 퀵정렬을 안쓰는 것이 좋다?)
안정성이 없다.
--------------------------------------------------*/
void i3List::_RecursiveQuickSort( INT32 si, INT32 ei, COMPAREPROC pProc)
{
	INT32 pivot = _Partition( si, ei, pProc);

	if( si < pivot)
		_RecursiveQuickSort( si, pivot - 1, pProc);

	if( ei > pivot)
		_RecursiveQuickSort( pivot + 1, ei, pProc);
}

/*--------------------------------------------------
Non Recursive Quick sort (by komet)
--------------------------------------------------*/
#if defined(USING_NON_RECURSIVE_QSORT)

const INT32 g_defStackDepth = 256;  // 스택 크기 필요시 더 늘리면 됩니다.

void i3List::_Qpush( INT32 left, INT32 right)
{
	m_nStackTop++;
	I3ASSERT( m_nStackTop >= 0 && m_nStackTop < m_nStackDepth);

	m_pStackBuffer[ m_nStackTop].left = left;
	m_pStackBuffer[ m_nStackTop].right = right;
}

void i3List::_Qpop( INT32* left, INT32* right)
{
	I3ASSERT( m_nStackTop >= 0 && m_nStackTop < m_nStackDepth);

	*left = m_pStackBuffer[ m_nStackTop].left;
	*right = m_pStackBuffer[ m_nStackTop].right;
	m_nStackTop--;
}

void i3List::_NonRecursiveQuickSort( INT32 si, INT32 ei, COMPAREPROC pProc)
{
	if( m_nStackDepth < m_Count)	I3MEM_SAFE_FREE_POINTER( m_pStackBuffer);

	//	퀵소트용 스택 버퍼 할당. 
	//	i3List에서 한번이라도 Sort를 하면 스택 버퍼는 한번만 생성되며 Sort가 끝난후에도 스택 버퍼는 남아있습니다.
	//	하지만 나중에 i3List 해제시 스택 버퍼는 해제됩니다.
	if( m_pStackBuffer == NULL)
	{	
		INT32 depth = g_defStackDepth;

		m_pStackBuffer = (QSTACK*) i3MemAlloc( sizeof( QSTACK) * depth);
		m_nStackDepth = depth;
	}

	i3mem::FillZero( m_pStackBuffer, sizeof( QSTACK) * m_nStackDepth);

	INT32 i,j,s = si,e = ei;

	m_nStackTop = -1;

	_Qpush( s, e);

	do 
	{
		_Qpop( &s, &e);

		do 
		{
			i = s;
			j = e;
			const void * pPivotItem = Items[ (s+e)/2];

			do
			{
				while( pProc( Items[i], (void*) pPivotItem) < 0)	i++;
				while( pProc( Items[j], (void*) pPivotItem) > 0)	j--;

				if( i <= j)
				{
					const void * temp = Items[i];
					Items[i] = Items[j];
					Items[j] = (void*) temp;
					i++;
					j--;
				}

			} while( i <= j);

			if( i<e)
			{
				_Qpush( i, e);
			}

			e = j;

		} while ( s < e);

	} while ( m_nStackTop != -1);
}
#endif

/*--------------------------------------------------
아이템 정렬 퍼포먼스 테스트

조건:
Intel Core2 Quad 2.4GHz
I3List 내에 INT32 정수 30000개 정렬을 3번 체크
GetTickCount() 함수로 시간 체크함.

결과:
Linear sort : (1) 22047 ms (2) 21562 ms (3) 21563 ms
Insertion sort : (1) 9187 ms (2) 9422 ms (3) 9250 ms
Recursive Quick sort  : (1) 16 ms  (2) 16 ms  (3) 15 ms
Non Recursive Quick sort : (1) 31 ms  (2) 31 ms  (3) 16 ms
--------------------------------------------------*/

#define CUTOFF (10)		// 퀵정렬 최소 갯수

void i3List::Sort( COMPAREPROC pProc)
{
	I3ASSERT( pProc != NULL);

	if( m_Count <= 1)	return;

	if( CUTOFF < m_Count)
	{
#if defined(USING_NON_RECURSIVE_QSORT)
		_NonRecursiveQuickSort( 0, m_Count - 1, pProc);
#else
		_RecursiveQuickSort( 0, m_Count - 1, pProc);	
#endif
	}
	else	// 작은 배열에서는 삽입정렬을 사용함으로서 효율을 증대시킨다.
	{
		_InsertionSort( 0, m_Count - 1, pProc);
	}

	// 선형 정렬
	/*INT32 i, j;

	for( i = 0; i < GetCount() - 1; i++)
	{
		for( j = i + 1; j < GetCount(); j++)
		{
			INT32 rv = pProc( Items[i], Items[j]);

			if( rv > 0)
			{
				void * pTemp = Items[i];
				Items[i] = Items[j];
				Items[j] = pTemp;
			}
		}
	}*/
}

void i3List::Copy( const i3List *List)
{
	if( List->GetCount() <= 0)
	{
		return;
	}	
	
	m_Count = List->GetCount();

	ResizeBuffer( m_Count);
	#ifdef UNLOCK_LOG
	I3BCLOG(LOCATION, "i3List::Copy() ResizeBuffer(m_Count(%d))", m_Count);
	#endif

	memcpy( Items, List->Items, sizeof(void *) * m_Count);
}

UINT32	i3List::SaveTo( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	INT32 i;
	OBJREF ref;

	// Count
	{
		Rc = pStream->Write( &m_Count, sizeof(m_Count));
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "Could not write count data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	for( i = 0; i < GetCount(); i++)
	{
		ref = (OBJREF) pResFile->GetObjectPersistID( (i3PersistantElement *)Items[i]);

		Rc = pStream->Write( &ref, sizeof(ref));
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "Could not write pointer data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32	i3List::LoadFrom( i3ResourceFile * pResFile, bool bAddRef)
{
	UINT32 Rc, Result = 0;
	UINT32 cnt;
	INT32 i;
	OBJREF ref;
	i3ElementBase * pObj;
	i3Stream * pStream = pResFile->GetStream();

	// Count
	Rc = pStream->Read( &cnt, sizeof(cnt));
	if( Rc == STREAM_ERR)
	{
		I3FATAL( "Could not read count data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( cnt > 0)
	{
		SetCount( cnt);

		for( i = 0; i < (INT32) cnt; i++)
		{
			Rc = pStream->Read( &ref, sizeof(ref));
			if( Rc == STREAM_ERR)
			{
				I3FATAL( "Could not read pointer data.");
				return STREAM_ERR;
			}
			Result += Rc;

			if( bAddRef)
			{
				pObj = (i3ElementBase *) pResFile->FindObjectByID( ref);

				if( pObj != NULL)
					pObj->AddRef();

				Items[i] = (void *) pObj;
			}
			else
			{
				Items[i] = (void *) ref;
			}
		}
	}

	return Result;
}

bool	i3List::SaveToXML( const char * pszName, i3XMLFile * pFile, i3XMLElement * pXML, bool bRef)
{
	char szName[256];
	i3XMLElement * pChildXML;
	INT32 i;

	pChildXML = new i3XMLElement( pszName);

	pChildXML = (i3XMLElement*) pXML->LinkEndChild( pChildXML);
	I3ASSERT( pChildXML != NULL);

	pChildXML->SetAttribute( "Count", GetCount());
	
	for( i = 0; i < GetCount(); i++)
	{
		sprintf( szName, "Item%d", i);

		if( bRef)
		{
			i3PersistantElement * pObj = (i3PersistantElement *) Items[i];

			I3ASSERT( pObj->IsTypeOf( i3PersistantElement::GetClassMeta()));

			OBJREF ref = pFile->GetObjectPersistID( pObj);
			I3ASSERT( ref > 0);

			pChildXML->SetAttribute( szName, (INT32) ref);
		}
		else
		{
			pChildXML->SetAttribute( szName, (INT32) Items[i]);
		}
	}

	return true;
}

bool	i3List::LoadFromXML( const char * pszName, i3XMLFile * pFile, i3XMLElement * pXML, bool bAddRef)
{
	char szName[256];
	i3XMLElement * pChildXML;
	INT32 i, cnt;
	UINT32 val;

	pChildXML = (i3XMLElement *) pXML->FirstChildElement( pszName);
	I3ASSERT( pChildXML != NULL);

	pChildXML->Attribute( "Count", &cnt);

	if( cnt > 0)
	{
		SetCount( cnt);

		for( i = 0; i < cnt; i++)
		{
			sprintf( szName, "Item%d", i);

			if( pChildXML->Attribute( szName, (int *) &val) != NULL)
			{
				if( bAddRef)
				{
					i3PersistantElement * pObj = pFile->FindObjectByID( (OBJREF) val);

					I3ASSERT( pObj != NULL);

					pObj->AddRef();

					Items[i] = pObj;
				}
				else
				{
					Items[i] = (void *) val;
				}
			}
			else
			{
				I3WARN( "Found not found %d XML element from %s\n", szName, pszName);

				Items[i] = NULL;
			}
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// String List
i3StringList::i3StringList( INT32 AllocUnit)
{
	m_MaxCount = 0;
	m_AllocUnit = AllocUnit;
	m_Count = 0;

	Items = NULL;

	m_bCRLF = TRUE;

	m_bOnceMode = FALSE;
}

i3StringList::~i3StringList(void)
{
	INT32 i;

	if( Items != NULL)
	{
		for( i = 0; i < m_Count; i++)
		{
			i3MemFree( Items[i]);
		}

		i3MemFree( Items);
	}
	
	m_Count = 0;
}

BOOL i3StringList::ResizeBuffer( INT32 nCount)
{
	m_MaxCount = NEARESTMULTIPLE( nCount, m_AllocUnit);

	if( Items == NULL)
	{		
		Items = (char **) i3MemAlloc( m_MaxCount * sizeof(char *));
	}
	else
	{
		Items = (char **) i3MemReAlloc( Items, m_MaxCount * sizeof(char *));
	}

	if( Items == NULL) 
	{
		DebugBox( NULL, "i3StringList::ResizeBuffer() faield.");
		return FALSE;
	}

	return TRUE;
}

void i3StringList::SetAllocationUnit( INT32 AllocUnit)
{
	INT32 NewCount;

	m_AllocUnit = AllocUnit;

	NewCount = NEARESTMULTIPLE( m_MaxCount, m_AllocUnit);

	ResizeBuffer( NewCount);
	#ifdef UNLOCK_LOG
	I3BCLOG(LOCATION, "i3List::SetAllocationUnit(AllocUnit(%d)) ResizeBuffer(NewCount(%d))", AllocUnit, NewCount);
	#endif
}

void i3StringList::OptimizeMemory(void)
{
	ResizeBuffer( m_Count);
	#ifdef UNLOCK_LOG
	I3BCLOG(LOCATION, "i3List::OptimizeMemory() ResizeBuffer(m_Count(%d))", m_Count);
	#endif
}

INT32 i3StringList::IndexOf( char * p) const
{
	INT32 i;
	
	if( m_Count <= 0)
		return -1;

	for( i = 0; i < m_Count; i++) 
	{
		if( strcmp( (char *)Items[i], (char *)p) == 0)
			return i;
	}

	return -1;
}

INT32 i3StringList::Add( char * p, INT32 Length)
{
	BOOL Rv;
	char *pNewStr;

	I3ASSERT( p != NULL);

	if( m_bOnceMode == TRUE)
	{
		INT32 idx = IndexOf( p);

		if( idx != -1)
			return idx;
	}

	if( m_Count+1 >= m_MaxCount) 
	{
		Rv = ResizeBuffer( m_Count + 1);
		#ifdef UNLOCK_LOG
		I3BCLOG(LOCATION, "i3StringList::Add() ResizeBuffer(m_Count(%d)+1 >= m_MaxCount(%d))", m_Count, m_MaxCount);
		#endif
		if( Rv == FALSE)
		{
			#ifdef UNLOCK_LOG
			I3BCLOG(LOCATION, "[Fail]i3StringList::Add() ResizeBuffer(m_Count(%d)+1 >= m_MaxCount(%d))", m_Count, m_MaxCount);
			#endif
			return -1;
		}
	}

	if( Length < 0) 
	{
		pNewStr = (char *) i3MemAlloc( (UINT32)(i3String::Length(p) + 1));
		I3ASSERT(pNewStr); 

		strcpy( pNewStr, p);
	}
	else 
	{
		pNewStr = (char *) i3MemAlloc( (UINT32)(Length + 1));
		I3ASSERT(pNewStr); 
	
		if( Length > 0)
		{
			i3String::NCopy( pNewStr, p, Length);
		}

		pNewStr[ Length] = 0;
	}// end if()

	Items[m_Count] = pNewStr;
	m_Count++;

	return m_Count-1;
}

INT32 i3StringList::Insert( INT32 idx, char * p, INT32 Length)
{
	BOOL Rv;
	size_t MoveSize;
	char *NewStr;

	if( m_Count+1 >= m_MaxCount) 
	{
		Rv = ResizeBuffer( m_Count + 1);
		#ifdef UNLOCK_LOG
		I3BCLOG(LOCATION, "i3StringList::Insert(idx(%d)) ResizeBuffer(m_Count(%d)+1 >= m_MaxCount(%d))", idx, m_Count, m_MaxCount);
		#endif
		if( Rv == FALSE)
		{
			#ifdef UNLOCK_LOG
			I3BCLOG(LOCATION, "[Fail]i3StringList::Insert(idx(%d)) ResizeBuffer(m_Count(%d)+1 >= m_MaxCount(%d))", idx, m_Count, m_MaxCount);
			#endif
			return -1;
		}
	}

	MoveSize = ( m_Count - idx) * sizeof(void *);
	memmove( &(Items[idx+1]), &(Items[idx]), MoveSize);

	if( Length == -1) 
	{
		//INT32 idx; 
		//idx = i3MemAlloc( (UINT32)(i3String::Length(p) + 1));
		NewStr = (char *) i3MemAlloc( (UINT32)(i3String::Length(p) + 1));
		I3ASSERT( NewStr );  
		strcpy( NewStr, p);
	}
	else 
	{
		//INT32 idx; 
		//idx = i3MemAlloc( Length + 1);
		NewStr = (char *) i3MemAlloc( Length + 1);

		i3String::NCopy( NewStr, p, Length);
		NewStr[Length] = 0;
	}

	Items[idx] = NewStr;
	m_Count++;

	return idx;
}

INT32 i3StringList::Delete( INT32 Idx)
{
	INT32 MoveLength;

	if( m_Count == 0)
	{
		return -1;
	}

	i3MemFree( Items[Idx]);	

	MoveLength = m_Count - Idx - 1;
	memcpy( &Items[Idx], &Items[Idx+1], MoveLength * sizeof(void *));
	m_Count--;

	return m_Count;
}

INT32 i3StringList::Remove( char * p)
{
	INT32 Idx;

	Idx = IndexOf( p);
	if( Idx == -1)
	{
		return -1;
	}

	Delete( Idx);

	return Idx;
}

void i3StringList::Clear(void)
{
	INT32 i;

	for( i = 0; i < m_Count; i++) 
	{
		i3MemFree( (Items[i]) );
	}

	ResizeBuffer( m_AllocUnit);
	#ifdef UNLOCK_LOG
	I3BCLOG(LOCATION, "i3StringList::Clear() ResizeBuffer(m_AllocUnit(%d))", m_AllocUnit);
	#endif

	m_Count = 0;
}

INT32 i3StringList::GetBufferSize(void)
{
	INT32 i, Rv = 0;

	for( i = 0; i < m_Count; i++) 
	{
		Rv += (INT32)i3String::Length( Items[i]) + 2;
	}

	return Rv;
}

void i3StringList::CopyFromBuffer( char *Buf, bool bClear)
{
	char *pStart = Buf;

	if( bClear)
		Clear();

	while( *Buf) 
	{
		switch( *Buf) 
		{
			case '\r':
				Add( pStart, (INT32)(Buf - pStart));
				pStart = Buf + 1;

				while(*pStart == '\n')
					pStart ++;

				Buf = pStart;
				break;

			default :
				Buf++;
				break;
				
		}// end switch()
		
	}// end while()

	if( pStart < Buf) 
	{
		Add( pStart, (INT32)(Buf - pStart));
	}
}

void i3StringList::CopyToBuffer( char *Buf, INT32 Limit, INT32 * pOffsetTable)
{
	INT32 i, Length;
	INT32 Accm = 0;
	char * pStart = Buf;

	for( i = 0; i < m_Count; i++) 
	{
		Length = i3String::Length( Items[i]);

		if((Accm + Length) >= Limit) 
		{
			Length -= (Limit - Accm + 1);

			if( Length > 0)
			{
				i3String::NCopy( Buf, Items[i], Length);

				if( pOffsetTable != NULL)
					pOffsetTable[i] = Buf - pStart;
			}

			Accm = Limit - 1;
			Buf += Length;
			break;
		}
		else 
		{
			strcpy( Buf, Items[i]);

			if( pOffsetTable != NULL)
				pOffsetTable[i] = Buf - pStart;

			if( i < m_Count - 1) 
			{
				Buf[Length] = '\r';
				if( m_bCRLF == (BOOL)TRUE) 
				{
					Length ++;
					Buf[Length] = '\n';
				}
			}
		}// end if()

		Accm += Length + 1;
		Buf += Length + 1;

	}// end for()
	*Buf = 0;
}

char * i3StringList::MakeFileFilter(void)
{
	INT32 NeedSz = 0;
	INT32 i;
	char *Buf, *str, *pStart;

	for( i = 0; i < m_Count; i++) 
	{
		NeedSz = i3String::Length( (char *)Items[i]) + 1;
	}
	NeedSz++;
	//INT32 idx; 
	//idx = i3MemAlloc( NeedSz);
	Buf = (char *)i3MemAlloc( NeedSz);
	I3ASSERT(Buf); 

	pStart = Buf;
	memset( Buf, 0, NeedSz);

	for( i = 0; i < m_Count; i++) 
	{
		str = (char *)Items[i];

		while( *str) 
		{
			if( *str == '|')
				*Buf = 0;
			else
				*Buf = *str;

			str++;
			Buf++;
			
		}// end while()
		*Buf = 0;
		Buf++;
		
	}// end for()

	return pStart;
}


UINT32	i3StringList::SaveTo( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	INT32	size = GetBufferSize();

	Result = pStream->Write( &size, sizeof( INT32));
	I3_CHKIO( Result);

	if( size > 0)
	{
		char * pszBuffer = (char *) i3MemAlloc( size);
		CopyToBuffer( pszBuffer, size);

		Rc = pStream->Write( pszBuffer, size);
		I3_CHKIO( Rc);
		Result += Rc;

		I3MEM_SAFE_FREE( pszBuffer);
	}

	return Result;
}

UINT32 i3StringList::LoadFrom( i3ResourceFile * pResFile)
{
	INT32		size;
	UINT32		Rc, Result	= 0;
	i3Stream *	pStream		= pResFile->GetStream();

	Clear();
	
	Result = pStream->Read( &size, sizeof( INT32));
	I3_CHKIO( Result);

	if( size > 0)
	{
		char * pszBuffer = (char *) i3MemAlloc( size);
		Rc = pStream->Read( pszBuffer, size);
		I3_CHKIO( Rc);

		Result += Rc;

		CopyFromBuffer( pszBuffer);

		I3MEM_SAFE_FREE( pszBuffer);
	}

	return Result;
}

bool	i3StringList::SaveToXML( const char * pszName, i3XMLFile * pFile, i3XMLElement * pXML)
{
	char szName[256];
	i3XMLElement * pChildXML;
	INT32 i;

	pChildXML = new i3XMLElement( pszName);

	pChildXML = (i3XMLElement*) pXML->LinkEndChild( pChildXML);
	I3ASSERT( pChildXML != NULL);

	pChildXML->SetAttribute( "Count", GetCount());
	
	for( i = 0; i < GetCount(); i++)
	{
		sprintf( szName, "Item%d", i);

		pChildXML->SetAttribute( szName, (const char*)Items[i]);
	}

	return true;
}

bool	i3StringList::LoadFromXML( const char * pszName, i3XMLFile * pFile, i3XMLElement * pXML)
{
	char szName[256];
	i3XMLElement * pChildXML;
	INT32 i, cnt;
	//UINT32 val;	// unreferenced local variable

	pChildXML = (i3XMLElement *) pXML->FirstChildElement( pszName);
	I3ASSERT( pChildXML != NULL);

	pChildXML->Attribute( "Count", &cnt);

	if( cnt > 0)
	{
		SetAllocationUnit( cnt);

		for( i = 0; i < cnt; i++)
		{
			sprintf( szName, "Item%d", i);

			const char * pszString = pChildXML->Attribute( szName);
			I3ASSERT( pszString != NULL);

			Add( (char*)pszString);
		}
	}

	return true;
}