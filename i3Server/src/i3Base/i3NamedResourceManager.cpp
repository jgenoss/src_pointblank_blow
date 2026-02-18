#include "i3CommonType.h"
#include "i3NamedResourceManager.h"
#include "i3ResourceObject.h"
#include "i3BinList.h"
#include "i3Macro.h"
#include "i3ErrorHandler.h"
#include "i3String.h"
#include "i3CRC32.h"
#include "i3Performance.h"
#include <stdio.h>
#include "i3MD5.h"

// ================================================================================================
//
// i3NamedRes, i3NamedResourceManager
//
// ================================================================================================

I3_CLASS_INSTANCE( i3NamedResourceManager, i3ElementBase );

#define CRC_SEARCH

static bool		s_bCommonRes = false;

// Resource Manager들은 내부적으로 Single-Linked List로 연결되어지며, 
// 내부에서 자동적으로 Reset되어진다.
static i3NamedResourceManager *		s_pHead = NULL;
static i3Mutex2 *					s_pLinkMutex = NULL;

#if defined( CRC_SEARCH)
	static INT32 _CompareNamedResFunc( i3ResourceObject *p1, i3ResourceObject *p2 )
	{
		if( p1->getChecksum() > p2->getChecksum())
			return 1;
		else if( p1->getChecksum() < p2->getChecksum())
			return -1;

		return 0;
		//return p1->getChecksum() - p2->getChecksum();
	}

	static INT32 _FindProc( i3ResourceObject * p1, UINT128 * code)
	{
		if( p1->getChecksum() > *code)
			return 1;
		else if( p1->getChecksum() < *code)
			return -1;

		return 0;

		//return p1->getChecksum() - code;
	}
#else
	static INT32 _CompareNamedResFunc( i3ResourceObject * p1, i3ResourceObject * p2)
	{
		return _stricmp( p1->GetName(), p2->GetName());
	}

	static INT32 _FindProc( i3ResourceObject * p1, char * pszName)
	{
		return _stricmp( p1->GetName(), pszName);
	}
#endif

namespace
{
	void atexit_LinkMutex()
	{
		delete s_pLinkMutex;	s_pLinkMutex = NULL;
	}

}


i3NamedResourceManager::i3NamedResourceManager()
{
	m_pList = i3BinList::NewObject();
	m_pList->SetCompareProc( (COMPAREPROC) _CompareNamedResFunc );

	m_pMutex = new i3Mutex2;

	// Link to Linked-List
	if( s_pLinkMutex == NULL)
	{
		s_pLinkMutex = new i3Mutex2;
		::atexit(atexit_LinkMutex);
	}

	{
		i3Monitor2 mon( s_pLinkMutex);

		m_pNextResManager = s_pHead;
		s_pHead = this;
	}
}

i3NamedResourceManager::~i3NamedResourceManager()
{
	I3ASSERT( m_pMutex != NULL);

	{
		i3Monitor2	mon( m_pMutex);

		m_pList->SafeReleaseAll();
		I3_SAFE_RELEASE( m_pList );
	}

	delete m_pMutex;
	m_pMutex = NULL;

	// Unlink from Linked-List
	{
		i3Monitor2 mon( s_pLinkMutex);

		if( s_pHead == this)
		{
			s_pHead = m_pNextResManager;
		}
		else
		{
			i3NamedResourceManager * pPrev = s_pHead;

			while( pPrev->m_pNextResManager != this)
				pPrev = pPrev->m_pNextResManager;

			I3ASSERT( pPrev != NULL);

			pPrev->m_pNextResManager = m_pNextResManager;
		}

		m_pNextResManager = NULL;
	}
}

INT32 i3NamedResourceManager::DeleteUnusedObjects(void)
{
	INT32 i;
	i3ResourceObject * pObj;
	i3Monitor2 mon( m_pMutex);

	for( i = 0; i < m_pList->GetCount(); i++)
	{
		pObj = (i3ResourceObject *) m_pList->Items[i];

		if( (pObj->isCommonRes() == false) && (pObj->GetRefCount() == 1))
		{
			pObj->Release();

			m_pList->Delete( i);
			i--;
		}
	}

	return m_pList->GetCount();
} 

void i3NamedResourceManager::AddResource( i3ResourceObject * pObj, const char * pszName)
{
	I3ASSERT( pObj != NULL);

	if( pszName != NULL)
	{
		//pObj->SetName( conv);
		pObj->SetResourcePath( pszName);
	}

	pObj->setCommonRes( s_bCommonRes);

	{
		i3Monitor2	mon( m_pMutex);

		m_pList->Add( pObj);
		pObj->AddRef();
	}
}

i3ResourceObject *	i3NamedResourceManager::FindResource( const char * pszName)
{
	i3Monitor2 mon( m_pMutex);

	m_pList->SetCompareProc( (COMPAREPROC) _FindProc);

	char conv[MAX_PATH];
	i3String::Copy( conv, pszName, MAX_PATH );
	i3String::MakeUnixPath( conv);
	i3String::ToUpper( conv);

	#if defined( CRC_SEARCH)
		UINT128 code;

		i3MD5::Gen( conv, i3String::Length( conv), (char *) &code);

		INT32 nIndex = m_pList->FindItem( (void *) &code);
	#else
		INT32 nIndex = m_pList->FindItem( (void *) conv);
	#endif

	m_pList->SetCompareProc( (COMPAREPROC) _CompareNamedResFunc);

	if( nIndex < 0 )
	{
		return NULL;
	}

	i3ResourceObject *pRes = (i3ResourceObject *)m_pList->GetItem( nIndex );
	I3ASSERT( pRes != NULL );

	return pRes;
}

i3ResourceObject*	i3NamedResourceManager::GetResource( const char * pszResFile, const char * pszBasePath, const char *szName )
{
	return FindResource( szName);
}

UINT32				i3NamedResourceManager::SaveResource( const char * pszBasePath, i3ResourceObject * pObj)
{
	return STREAM_ERR;
}


void i3NamedResourceManager::Reset(void)
{
	i3NamedResourceManager * pManager;
	UINT32 Count = 0, prevCount;
	i3Monitor2 mon( s_pLinkMutex);

	do
	{
		pManager  = s_pHead;
		prevCount = Count;
		Count = 0;

		while( pManager != NULL)
		{
			Count += pManager->DeleteUnusedObjects();

			pManager = pManager->m_pNextResManager;
		}
	} while( (Count > 0) && (Count != prevCount));
}

i3NamedResourceManager * i3NamedResourceManager::FindByMeta( i3ClassMeta * pMeta)
{
	i3NamedResourceManager * pManager;
	i3Monitor2 mon( s_pLinkMutex);

	{
		pManager  = s_pHead;

		while( pManager != NULL)
		{
			if( pManager->IsExactTypeOf( pMeta))
			{
				return pManager;
			}

			pManager = pManager->m_pNextResManager;
		}
	}

	return NULL;
}

void i3NamedResourceManager::Dump(void)
{
	INT32 i;
	i3ResourceObject * pObj;

	I3TRACE( "----------------- Resource Manager Dump ----------------------\n");

	i3Monitor2 mon( m_pMutex);

	for( i = 0; i < m_pList->GetCount(); i++)
	{
		pObj = (i3ResourceObject *) m_pList->Items[i];

		I3TRACE( "[%04d] %p : %s\n", i, pObj, pObj->GetName());
	}
}

void i3NamedResourceManager::SetCommonResourceState( bool bState)
{
	s_bCommonRes = bState;
}