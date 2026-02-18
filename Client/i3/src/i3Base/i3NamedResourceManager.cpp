#include "i3CommonType.h"
#include "i3NamedResourceManager.h"
#include "i3ResourceObject.h"
#include "i3Macro.h"
#include "i3ErrorHandler.h"
#include "i3String.h"
#include "i3CRC32.h"
#include "i3Performance.h"
#include <stdio.h>
#include "i3MD5.h"
#include "itl/container_util.h"
#include "itl/algorithm_util.h"
#include "string/ext/make_unix_path.h"
#include "string/algorithm/to_upper.h"
#include "i3Base/string/ext/safe_string_copy.h"
#include "i3Base/string/ext/generic_string_size.h"

// ================================================================================================
//
// i3NamedRes, i3NamedResourceManager
//
// ================================================================================================

I3_CLASS_INSTANCE( i3NamedResourceManager);

#define CRC_SEARCH

static bool		s_bCommonRes = false;

// Resource Manager들은 내부적으로 Single-Linked List로 연결되어지며, 
// 내부에서 자동적으로 Reset되어진다.
static i3NamedResourceManager *		s_pHead = nullptr;
static i3Mutex2 *					s_pLinkMutex = nullptr;

#if defined( CRC_SEARCH)

	bool	operator<(const i3ResourceObject& a1, const i3ResourceObject& a2)
	{
		return a1.getChecksum() < a2.getChecksum();
	}
namespace
{
	struct FindProc
	{
		bool operator()( const i3ResourceObject * p1, const UINT128& code  ) const { return p1->getChecksum() < code;  }
		bool operator()( const UINT128& code, const i3ResourceObject * p1  ) const { return code < p1->getChecksum();  }
	};
}	
#else

	bool	operator<(const i3ResourceObject& a1, const i3ResourceObject& a2)
	{
		return  i3::generic_is_iless( a1.GetNameString(), a2.GetNameString());
	}

namespace
{
	struct FindProc
	{
		bool operator()( const i3ResourceObject * p1, const char* pszName  ) const { return i3::generic_is_iless( p1->GetNameString(), pszName) ;  }
		bool operator()( const char* pszName, const i3ResourceObject * p1  ) const { return i3::generic_is_iless( pszName, p1->GetNameString()) < 0;  }
	};
}
#endif

namespace
{
	void atexit_LinkMutex()
	{
		I3_SAFE_DELETE( s_pLinkMutex);
	}

}


i3NamedResourceManager::i3NamedResourceManager()
{
	
	m_pMutex = new i3Mutex2;
	NetworkDump_Malloc( m_pMutex, sizeof( i3Mutex2), __FILE__, __LINE__);

	// Link to Linked-List
	if( s_pLinkMutex == nullptr)
	{
		s_pLinkMutex = new i3Mutex2;
		NetworkDump_Malloc( s_pLinkMutex, sizeof( i3Mutex2), __FILE__, __LINE__);
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
	I3ASSERT( m_pMutex != nullptr);

	{
		i3Monitor2	mon( m_pMutex);
		
		i3::cu::for_each_container(m_List, i3::fu::mem_fun_safe(&i3ResourceObject::Release));
		m_List.clear();
	}

	I3_MUST_DELETE( m_pMutex);

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

			I3ASSERT( pPrev != nullptr);

			pPrev->m_pNextResManager = m_pNextResManager;
		}

		m_pNextResManager = nullptr;
	}
}

INT32 i3NamedResourceManager::DeleteUnusedObjects(void)
{	
	i3ResourceObject * pObj;
	i3Monitor2 mon( m_pMutex);

	for(size_t i = 0; i < m_List.size(); i++)
	{
		pObj = m_List.get_vector()[i];

		if( (pObj->isCommonRes() == false) )
		{
			if( pObj->GetRefCount() == 1)
			{
				I3_MUST_RELEASE( pObj);
				m_List.erase(m_List.begin() + i); 
				i--;
			}
			/*else
			{
				I3TRACE( "no delete object %s ref:%d\n", pObj->GetName(), pObj->GetRefCount());
			}*/
		}
	}

	return (INT32)m_List.size();
} 

void i3NamedResourceManager::AddResource( i3ResourceObject * pObj, const char * pszName)
{
	I3ASSERT( pObj != nullptr);

	if( pszName != nullptr)
	{
		//pObj->SetName( conv);
		pObj->SetResourcePath( pszName);
	}

	pObj->setCommonRes( s_bCommonRes);

	{
		i3Monitor2	mon( m_pMutex);

		I3_MUST_ADDREF( pObj);
		m_List.insert( pObj);
	}
}

void i3NamedResourceManager::DeleteResource( UINT32 idx)
{
	I3ASSERT( idx < m_List.size());

	i3Monitor2 mon( m_pMutex);
	
	i3ResourceObject * pObj = m_List.get_vector()[idx];
	I3_MUST_RELEASE( pObj);
	
	m_List.erase( m_List.begin() + idx);
}

i3ResourceObject *	i3NamedResourceManager::FindResource( const char * pszName)
{
	i3Monitor2 mon( m_pMutex);

	char conv[MAX_PATH];
	i3::safe_string_copy( conv, pszName, MAX_PATH );
	i3::make_unix_path( conv);
	i3::to_upper( conv);


	#if defined( CRC_SEARCH)
		UINT128 code;

		i3MD5::Gen( conv, i3::generic_string_size( conv), (char *) &code);

		i3::pair<i3::vector_multiset<i3ResourceObject*>::iterator, bool>  pr = i3::au::binary_find(m_List.begin(), m_List.end(), code, FindProc());
	#else

		i3::pair<i3::vector_multiset<i3ResourceObject*>::iterator, bool>  pr = i3::au::binary_find(m_List.begin(), m_List.end(), conv, FindProc());
	#endif

	if( pr.second == false )
	{
		return nullptr;
	}

	i3ResourceObject *pRes = *pr.first;
	I3ASSERT( pRes != nullptr );

	return pRes;
}

UINT32 i3NamedResourceManager::GetResourceCount( void)
{
	return m_List.size();
}
i3ResourceObject * i3NamedResourceManager::GetResource( UINT32 idx)
{
	I3ASSERT( idx < m_List.size());
	return m_List.get_vector()[idx];
}

i3ResourceObject*	i3NamedResourceManager::GetResource( const char * pszResFile, const char * pszBasePath, const char *szName,
														bool bBackgroundLoad /*= false*/ )
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
	UINT32 Count = 0;
//	UINT32 prevCount;
	i3Monitor2 mon( s_pLinkMutex);

	/*do
	{
		pManager  = s_pHead;
		prevCount = Count;
		Count = 0;*/

		pManager  = s_pHead;

		while( pManager != nullptr)
		{
			Count = pManager->DeleteUnusedObjects();
			I3TRACE( "%s UsingObject Cnt:%d\n", pManager->meta()->class_name(), Count );

			pManager = pManager->m_pNextResManager;
		}
	//} while( (Count > 0) && (Count != prevCount));
}

i3NamedResourceManager * i3NamedResourceManager::FindByMeta( i3ClassMeta * pMeta)
{
	i3NamedResourceManager * pManager;
	i3Monitor2 mon( s_pLinkMutex);

	{
		pManager  = s_pHead;

		while( pManager != nullptr)
		{
			if( pManager->same_of( pMeta))
			{
				return pManager;
			}

			pManager = pManager->m_pNextResManager;
		}
	}

	return nullptr;
}

void i3NamedResourceManager::Dump(void)
{
	
	i3ResourceObject * pObj;

	I3TRACE( "-------[%s] Resource Manager Dump ----------------------\n", static_meta()->class_name());

	i3Monitor2 mon( m_pMutex);

	for(size_t i = 0; i < m_List.size(); i++)
	{
		pObj = m_List.get_vector()[i];

		I3TRACE( "[%04d] %p(ref:%d) : %s\n", i, pObj, pObj->GetRefCount(), pObj->GetName());
	}
}

void i3NamedResourceManager::SetCommonResourceState( bool bState)
{
	s_bCommonRes = bState;
}

void i3NamedResourceManager::DumpAll( void)
{
	i3NamedResourceManager * pManager;
//	UINT32 Count = 0 , prevCount;
	i3Monitor2 mon( s_pLinkMutex);

//	do
//	{
		pManager  = s_pHead;
//		prevCount = Count;
//		Count = 0;

		while( pManager != nullptr)
		{
			pManager->Dump();

			pManager = pManager->m_pNextResManager;
		}
//	} while( (Count > 0) && (Count != prevCount));
}