#include "i3CommonType.h"
#include "i3ClassMeta.h"
#include "i3List.h"
#include "i3BinList.h"
#include "i3Debug.h"
#include "i3ErrorHandler.h"
#include "i3Performance.h"
#include "i3Macro.h"
#include "i3String.h"
#include "i3CRC32.h"
#include "i3Hash.h"



namespace
{
	i3MemoryPool**	g_ppPoolToDel = NULL;
	INT32			g_iNumPool = 0;

	void		CleanUpPool()
	{
		if (g_ppPoolToDel)
		{
			for (INT32 i = 0 ; i < g_iNumPool ; ++i)
			{
				delete g_ppPoolToDel[i];
			}
			delete [] g_ppPoolToDel;
			g_ppPoolToDel = NULL;
		}
	}

	struct init 
	{
		init() {	::atexit(CleanUpPool); }
	} _;
}

static i3ClassMeta *	s_pClassMetaHead = NULL;
static i3BinList *		s_pClassMetaList = NULL;

void* i3GetClassMetaList()
{
	return s_pClassMetaList;
}

UINT32 _hash_( char * pBuf, INT32 len)
{
	return CRC32( 0xFFFFFFFF, (UINT8 *) pBuf, len);
	//return i3Hash::AP( pBuf, len);
	//return i3Hash::DEK( pBuf, len);
}

static INT32		_ClassMetaCompareProc( i3ClassMeta * p1, i3ClassMeta * p2)
{
	if( p1->getHashCode() > p2->getHashCode())
		return 1;
	else if( p1->getHashCode() < p2->getHashCode())
		return -1;

	return 0;
}

#if defined( I3_DEBUG)
bool i3ClassMeta::isAlready( char * pszClassName)
{
	i3ClassMeta * pTemp = s_pClassMetaHead;

	while( pTemp != NULL)
	{
		if( strcmp( pszClassName, pTemp->GetClassName()) == 0)
			return true;

		pTemp = pTemp->m_pNext;
	}

	return false;
}
#endif

i3ClassMeta::i3ClassMeta( UINT32 classSz, char * pszClassName, char * pszBaseClassName, I3_CLASSMETA_TYPE type)
{
	m_pNext = NULL;

	m_ClassSize = classSz;
	m_pszClassName = pszClassName;
	m_pszBaseClassName = pszBaseClassName;
	m_HashCode = _hash_( m_pszClassName, i3String::Length( m_pszClassName));

	m_pBaseClass = NULL;
	i3mem::FillZero( &m_ProxyInfo, sizeof( m_ProxyInfo));
	m_ProxyCount = 0;
	m_Style = 0;

	switch( type)
	{
		case I3_CLASSMETA_TYPE_CONCRETE :
			AddStyle( I3_CLASSMETA_STYLE_CONCRETE);
			break;

		case I3_CLASSMETA_TYPE_GHOST :
			AddStyle( I3_CLASSMETA_STYLE_GHOST);
			break;
	}

	m_pPool = NULL;

	#if defined( I3_DEBUG)
	m_InstanceCount	= 0;
	if( isAlready( pszClassName))
	{
		I3TRACE1( "%s class is registered already.\n", pszClassName);
	}
	#endif
	
	if( s_pClassMetaHead == NULL)
		s_pClassMetaHead = this;
	else
	{
		m_pNext = s_pClassMetaHead;
		s_pClassMetaHead = this;
	}
}

i3ClassMeta::~i3ClassMeta(void)
{
	RemoveProxyInfo();
}

i3ClassMeta *	i3ClassMeta::GetConcreteClass( I3_CLASSMETA_TYPE type)
{
	INT32 i;
	UINT32 style = I3_CLASSMETA_STYLE_CONCRETE;

	switch( type)
	{
		case I3_CLASSMETA_TYPE_GHOST :		style = I3_CLASSMETA_STYLE_GHOST;		break;
	}

	for( i = 0; i < m_ProxyCount; i++)
	{
		i3ClassMeta * pMeta = m_ProxyInfo[i].m_pMeta;
		
		if( pMeta->GetStyle() == style)
			return pMeta;
	}

	return NULL;
}

void i3ClassMeta::RemoveProxyInfo(void)
{
	m_ProxyCount = 0;
}

void i3ClassMeta::SetConcreteClass( i3ClassMeta * pMeta)
{
	INT32 idx = -1;
	INT32 i;

	I3ASSERT( m_ProxyCount < I3_CLASSMETA_PROXY_MAX);
	
	// 기존에 등록되어 있던 Class Meta인지 확인한다.
	for( i = 0; i < m_ProxyCount;i++)
	{
		if( m_ProxyInfo[i].m_pMeta == pMeta)
		{
			idx = i;
			break;
		}
	}

	if( idx != -1)
	{
		// 기존이 존재하던 Class Meta라면 일단 제거했다가,
		// 선두 위치에 다시 삽입한다.
		INT32 MoveLength = m_ProxyCount - idx - 1;

		if( MoveLength > 0)
			memcpy( &m_ProxyInfo[idx], &m_ProxyInfo[idx+1], MoveLength * sizeof(I3CLASSMETAPROXYINFO));

		m_ProxyCount--;
	}

	// 선두에 넣기 위해 공간 마련
	{
		INT32 MoveSize = (m_ProxyCount) * sizeof(I3CLASSMETAPROXYINFO);

		if( MoveSize > 0)
			memmove( &m_ProxyInfo[1], &m_ProxyInfo[0], MoveSize);
	}

	m_ProxyInfo[0].m_pMeta = pMeta;
	m_ProxyCount++;

	pMeta->SetStyle( I3_CLASSMETA_STYLE_CONCRETE);

	SetStyle( I3_CLASSMETA_STYLE_ABSTRACT);	
}

void i3ClassMeta::SetGhostClass( i3ClassMeta * pMeta)
{
	I3ASSERT( m_ProxyCount < I3_CLASSMETA_PROXY_MAX);

	pMeta->AddStyle( I3_CLASSMETA_STYLE_GHOST);

	m_ProxyInfo[ m_ProxyCount].m_pMeta = pMeta;

	m_ProxyCount++;
}

#if defined( I3_COMPILER_VC)
#pragma warning( push)
#pragma warning( disable : 4100)
#endif

#if defined( I3_COMPILER_EVC)
#pragma warning( disable : 4514 )
#endif


#if defined( I3_DEBUG)

i3ElementBase *	i3ClassMeta::CreateInstance( I3_CLASSMETA_TYPE type, const char * pszFile, int line)
{
	return NULL;
}

i3ElementBase * i3ClassMeta::BindInstance( void * pBuf, I3_CLASSMETA_TYPE type, const char * pszFile, int line)
{
	return NULL;
}

#else
i3ElementBase *	i3ClassMeta::CreateInstance( I3_CLASSMETA_TYPE type)
{
	return NULL;
}

i3ElementBase *	i3ClassMeta::BindInstance( void * pBuf, I3_CLASSMETA_TYPE type)
{
	return NULL;
}
#endif

#if defined( I3_COMPILER_VC)
#pragma warning( pop)
#endif

char * i3ClassMeta::GetPersistClassName(void)
{
	if( IsStyle( I3_CLASSMETA_STYLE_CONCRETE) && (m_pBaseClass != NULL))
	{
		return m_pBaseClass->GetPersistClassName();
	}

	return GetClassName();
}

BOOL i3ClassMeta::_ClassMetaTerminateProc( void * pData)
{
	INT32 i;

	if( s_pClassMetaList == NULL)
		return TRUE;
	
	g_iNumPool = 0;
	
	for( i = 0; i < s_pClassMetaList->GetCount(); i++)
	{
		i3ClassMeta * pMeta = (i3ClassMeta *) s_pClassMetaList->Items[i];
		if( pMeta->GetMemoryPool() != NULL)
			++g_iNumPool;
	}
	
	if (g_iNumPool > 0)
		g_ppPoolToDel = new i3MemoryPool*[g_iNumPool];
	
	g_iNumPool = 0;

	for( i = 0; i < s_pClassMetaList->GetCount(); i++)
	{
		i3ClassMeta * pMeta = (i3ClassMeta *) s_pClassMetaList->Items[i];

		if( pMeta->GetMemoryPool() != NULL)
		{
			i3MemoryPool * pMemPool = pMeta->GetMemoryPool();
			g_ppPoolToDel[g_iNumPool] = pMemPool; ++g_iNumPool;
//			delete pMemPool;
		}
	}

	s_pClassMetaList->Release();
	s_pClassMetaList = NULL;

	return TRUE;
}

void i3ClassMeta::Register(void)
{	
	if( s_pClassMetaList == NULL)
	{
		// ClassMeta가 아직 생성되기 전이기 때문에
		// NewObject() 등의 함수로 생성할 수 없다.
		// 그런 이유로 직접 new operator를 이용해 생성한다.
		s_pClassMetaList = NEW i3BinList;
		I3ASSERT( s_pClassMetaList != NULL);

		s_pClassMetaList->AddRef();

		// Class Meta의 Termination Procedure는 가장 낮은 우선 순위로, 가장 마지막에 호출되도록 할 필요가 있다.
		// 그것은 Memory Pool이 사라질 때, 내부에 포함된 모든 Class들이 RefCount와 무관하게 바로 삭제되며,
		// 또 Destructor 역시 호출되지 않은체 사라지기 때문이다.
		// 사실 상, 반드시 수정되어야 할 부분.
		i3System::AddTerminateProc( -1 * 0x7FFFFFFF, _ClassMetaTerminateProc, NULL);
	}

	if( s_pClassMetaList->GetCount() <= 0)
	{
		s_pClassMetaList->SetCompareProc( (COMPAREPROC) _ClassMetaCompareProc);
	}

	s_pClassMetaList->Add( this);

	if( (m_pszBaseClassName != NULL) && (m_pszBaseClassName[0] != '0'))
	{
		m_pBaseClass = i3ClassMeta::FindClassMetaByName( (const char *) m_pszBaseClassName);
		if( m_pBaseClass == NULL)
		{
			I3TRACE( "[META] Could not found %s base class meta of %s class.", m_pszBaseClassName, GetClassName());
		}

		if( m_pBaseClass != NULL)
		{
			if( IsStyle( I3_CLASSMETA_STYLE_CONCRETE))
			{
				m_pBaseClass->SetConcreteClass( this);
			}
			else if( IsStyle( I3_CLASSMETA_STYLE_GHOST))
			{
				m_pBaseClass->SetGhostClass( this);
			}
		}
	}
}

void i3ClassMeta::UnRegister(void)
{
	if( s_pClassMetaList != NULL)
	{
		INT32 cnt = s_pClassMetaList->GetCount();
		
		g_iNumPool = 0;

		for( INT32 i = 0;i < cnt; ++i)
		{
			i3ClassMeta * pMeta = (i3ClassMeta*)s_pClassMetaList->Items[i];

			if( pMeta->GetMemoryPool() != NULL)
				++g_iNumPool;
		}
		
		if (g_iNumPool > 0)
			g_ppPoolToDel = new i3MemoryPool*[g_iNumPool];

		g_iNumPool = 0;

		for( INT32 i = 0;i < cnt; ++i)
		{
			i3ClassMeta * pMeta = (i3ClassMeta*) s_pClassMetaList->Items[i];

			if( pMeta->IsStyle( I3_CLASSMETA_STYLE_ABSTRACT))
			{
				pMeta->RemoveProxyInfo();
			}

			if( pMeta->GetMemoryPool() != NULL)
			{
				i3MemoryPool * pMemPool = pMeta->GetMemoryPool();

				g_ppPoolToDel[g_iNumPool] = pMemPool;		++g_iNumPool;
			//	delete pMemPool;
			}
		}

		delete s_pClassMetaList;
		s_pClassMetaList = NULL;

	}
}

i3ClassMeta * i3ClassMeta::FindClassMetaByName( const char * pszName)
{
	UINT32 hash;

	hash = _hash_( (char *) pszName, i3String::Length( pszName));

	return FindClassMetaByHash( hash);
}

void i3ClassMeta::PrintClassMeta()
{
	i3ClassMeta * pMeta;

	for(INT32 i=0; i<s_pClassMetaList->GetCount(); i++)
	{
		pMeta = (i3ClassMeta *) s_pClassMetaList->Items[i];
		I3TRACE("Hash %d Class Name %s", pMeta->getHashCode(), pMeta->GetClassName());
	}
}

i3ClassMeta * i3ClassMeta::FindClassMetaByHash( UINT32 hash)
{
	INT32 start, end;
	INT32 idx, rv;
	i3ClassMeta * pMeta;

	if( s_pClassMetaList == NULL)
		return NULL;

	if( s_pClassMetaList->GetCount() <= 0)
		return NULL;
		
	start = 0;
	end = s_pClassMetaList->GetCount() - 1;

	while( start < end)
	{
		idx = (start + end) >> 1;
		
		pMeta = (i3ClassMeta *) s_pClassMetaList->Items[idx];

		if( pMeta->getHashCode() > hash)
			rv = 1;
		else if( pMeta->getHashCode() < hash)
			rv = -1;
		else
		{
			return pMeta;
		}
				 
		if( rv > 0)
			end = idx - 1;
		else
			start = idx + 1;
	}

	if( start <= end)
	{	
		idx = (start + end) >> 1;
		
		pMeta = (i3ClassMeta *) s_pClassMetaList->Items[ idx];

		if( pMeta->getHashCode() == hash)
		{
			return pMeta;
		}
	}
	
	return NULL;
}

bool i3ClassMeta::IsTypeOf( i3ClassMeta * pMeta)
{
	register i3ClassMeta * pTemp = this;

	while( pTemp != NULL)
	{
		if( pTemp == pMeta)
			return true;

		pTemp = pTemp->m_pBaseClass;
	}

	return false;
}

void i3ClassMeta::GetAllDerivedClasses( i3List * pList, bool bRecursive)
{
	i3ClassMeta * pMeta;
	INT32 i;

	if( s_pClassMetaList == NULL)
		return;

	for( i = 0; i < s_pClassMetaList->GetCount(); i++)
	{
		pMeta = (i3ClassMeta *) s_pClassMetaList->Items[i];

		if( pMeta->m_pBaseClass == this)
		{
			pList->Add( pMeta);

			if( bRecursive)
			{
				pMeta->GetAllDerivedClasses( pList, bRecursive);
			}
		}
	}
}
#if defined( I3_DEBUG)
void i3ClassMeta::CheckCollision(void)
{
	i3ClassMeta * pMeta1, * pMeta2;
	INT32 i, j, cnt = 0;

	I3ASSERT( s_pClassMetaList != NULL);

	I3TRACE( "------------------ Check ClassMeta Hash code ---------------------\n");
	for( i = 0; i < s_pClassMetaList->GetCount() - 1; i++)
	{
		pMeta1 = (i3ClassMeta *) s_pClassMetaList->Items[i];

		for( j = i + 1; j < s_pClassMetaList->GetCount(); j++)
		{
			pMeta2 = (i3ClassMeta *) s_pClassMetaList->Items[j];

			if( pMeta1->getHashCode() == pMeta2->getHashCode())
			{
				I3TRACE( "COLLISION : %s(%d)[%08X] - %s(%d)[%08X]\n", pMeta1->GetClassName(), i, pMeta1->getHashCode(), pMeta2->GetClassName(), j, pMeta2->getHashCode());
				cnt++;
			}
		}
	}
	I3TRACE( " Total %d classes : %d collisions\n", s_pClassMetaList->GetCount(), cnt);
	I3TRACE( "------------------             end            ---------------------\n");
}
#endif

#if defined( I3_NO_STATIC_CLASSMETA)

#if defined( I3_WIPI_GIGA)
void *	i3ClassMeta::operator new( UINT32 sz )
#else
void *	i3ClassMeta::operator new( size_t sz )
#endif
{
	i3ClassMeta * pNew;
	INT32 hMem;

	hMem = i3MemAlloc( (UINT32) sz);

	pNew = (i3ClassMeta *) i3MemLock( hMem);

	return pNew;
}

void i3ClassMeta::operator delete( void * p)
{
	i3MemFree( p);
}
#endif

#if defined( I3_DEBUG)
void i3ClassMeta::DumpClassMeta(void)
{
	INT32 i;
	i3ClassMeta * pMeta;


	I3TRACE( "--------------------------------------i3Engine Classes--------------------------------------\n");

	if( s_pClassMetaList != NULL)
	{
		for( i = 0; i < s_pClassMetaList->GetCount(); i++)
		{
			pMeta = (i3ClassMeta *) s_pClassMetaList->Items[i];

			I3TRACE( "%5d [%08X] %s [%d bytes]\n", i, pMeta->m_HashCode, pMeta->m_pszClassName, pMeta->m_ClassSize);
		}
	}

	I3TRACE( "--------------------------------------------------------------------------------------------\n");
}

void i3ClassMeta::ReportMetaInstanceToFile(void)
{
	INT32 i;
	i3ClassMeta * pMeta;
	INT32	cnt = 0;

	static char TempChar[MAX_PATH];

	i3FileStream			DumpFile;

	if( DumpFile.Create("c:\\classdump.i3c", STREAM_WRITE) == FALSE)
	{
		I3TRACE( "Could not create a class dump file (c:\\classdump.i3c)\n");
		return;
	}

	I3TRACE( "--------------------------------------i3Engine Classes--------------------------------------\n");
	if( s_pClassMetaList != NULL)
	{
		for( i = 0; i < s_pClassMetaList->GetCount(); i++)
		{
			pMeta = (i3ClassMeta *) s_pClassMetaList->Items[i];

			//I3TRACE( "%5d [%08X] %s [%d bytes](%d) instances\n", i, pMeta->m_HashCode, pMeta->m_pszClassName, pMeta->m_ClassSize, pMeta->getInstanceCount());
			sprintf( (PSTR)TempChar, (PSTR)"%s\t%d\t%d\n", pMeta->m_pszClassName, pMeta->m_ClassSize, pMeta->getInstanceCount());

			DumpFile.WriteLine( TempChar );	
			cnt += pMeta->getInstanceCount();
		}
		I3TRACE(" Total %d classes: %d Instances \n", s_pClassMetaList->GetCount(), cnt);
	}
	DumpFile.Close();

	I3TRACE( "--------------------------------------  End  -------------------------------------------------\n");
}

#endif
