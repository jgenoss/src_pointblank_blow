// i3ElementBase.cpp

#include "i3CommonType.h"
#include "i3ElementBase.h"
#include "i3MetaTemplate.h" 
#include "i3Debug.h"
#include "i3ErrorHandler.h"
#include "i3ClassMeta.h"
#include "i3Performance.h"
#include "i3Macro.h"
#include "i3Mutex.h"

#if defined( I3_DEBUG)
#include "i3List.h"
#endif

#include <crtdbg.h>


I3_ABSTRACT_CLASS_INSTANCE( i3ElementBase, NULL);

#if defined( I3_DEBUG)
	i3ElementBase * s_pElementHead = NULL;
	static i3Mutex2 s_LinkListcs;
	static INT32	s_MemTag = 0;
#endif

 
i3ElementBase::i3ElementBase()
: m_RefCount(0), m_pPool( NULL)

#if defined( I3_DEBUG)
, m_pSourceFilePath( NULL), m_SourceLineCount(0), m_pLeft(NULL), m_pRight(NULL)
#endif
{
#if defined( I3_DEBUG)
	s_LinkListcs.Enter(); 
	{
		m_pLeft = NULL;
		m_pRight = NULL;

		if( s_pElementHead == NULL)
		{
			s_pElementHead = this;
		}
		else
		{
			m_pRight = s_pElementHead;
			s_pElementHead->m_pLeft = this;

			s_pElementHead = this;
		}
	}
	s_LinkListcs.Leave(); 

	m_MemTag = s_MemTag;
#endif
}

i3ElementBase::~i3ElementBase()
{
#if defined( I3_DEBUG)
	s_LinkListcs.Enter(); 
	{
		i3ElementBase * pLeft, * pRight;

		pLeft = m_pLeft;
		pRight = m_pRight;

		if( pLeft != NULL)
		{
			if( pLeft->m_pRight != this)
			{
				I3FATAL( "Bug!!\n");
			}
			pLeft->m_pRight = pRight;
		}

		if( pRight != NULL)
		{
			if( pRight->m_pLeft != this)
			{
				I3FATAL( "Bug!!\n");
			}
			pRight->m_pLeft = pLeft;
		}

		m_pLeft = NULL;
		m_pRight = NULL;

		if( s_pElementHead == this)
			s_pElementHead = pRight;
	}
	s_LinkListcs.Leave(); 
#endif
}

void i3ElementBase::Release(void)
{
	//I3ASSERT( m_RefCount > 0 );
	//I3ASSERT( g_pi3DebugPointer != this );

	if( m_RefCount > 0 )
	{
		m_RefCount --;
		if( m_RefCount == 0)
		{
			delete this;
		}
	}
	else
	{
#ifdef I3_DEBUG
		char conv[512];

		I3TRACE( "OBJECT : %p\n", this);
		I3TRACE( "META : %p\n", this->GetMeta());
		I3TRACE( "META NAME : %s\n", this->GetMeta()->GetClassName());

		this->_getInfoString( conv, 512);
		I3TRACE( "INFO : %s\n", conv);
		I3ASSERT_0;
#endif
	}

	/*
	아래의 코드는 실수에 의해 Reference Count가 맞지 않은 Bug 등을
	감출 가능성이 있기 때문에 제거합니다.

	else if( m_RefCount == 0 )
	{
		m_RefCount = -1;
		delete this;
	}
	*/
}

void i3ElementBase::CopyTo( i3ElementBase * /* pDest */, I3_COPY_METHOD /* method */)
{
}

#ifdef I3_DEBUG
	void *	i3ElementBase::operator new( size_t sz, const char *  pszFile , int  line )
	{
		i3ElementBase * pNew;

		pNew = (i3ElementBase *) i3mem::Alloc( sz, 0, I3MEMORY_MINIMUM_ALLOCSIZE, pszFile, line);

		pNew->m_bBounded = FALSE;

		return pNew;
	}

	void *	i3ElementBase::operator new( size_t /* sz */ , void * pBuf, const char * pszFile, int line )
	{
		i3ElementBase * pNew;

		pNew = (i3ElementBase *) pBuf;
		pNew->m_bBounded = TRUE;

		return pNew;
	}

	#if defined( I3_COMPILER_VC)
		void i3ElementBase::operator delete( void * p, const char *, int)
		{
			i3MemoryPool * pPool = ((i3ElementBase *)p)->m_pPool;

			if( pPool == NULL)
			{
				// i3MemoryPool에서 할당된 것이 아님.
				i3ElementBase * pNew = (i3ElementBase *)p;

				i3MemFree( pNew );
			}
			else
			{
				pPool->Free( p);
			}
		}

		void i3ElementBase::operator delete( void * p, void *, const char *, int)
		{
			i3MemoryPool * pPool = ((i3ElementBase *)p)->m_pPool;

			if( pPool != NULL)
				pPool->Free( p);
		}
	#endif //#if defined( I3_COMPILER_VC)

	void i3ElementBase::operator delete( void * p)
	{
		i3MemoryPool * pPool = ((i3ElementBase *)p)->m_pPool;

		if( pPool == NULL)
		{
			i3ElementBase * pNew = (i3ElementBase *)p;
			//i3MemFreeByHandle( pNew->m_Memidx);
            i3MemFree( pNew );
		}
		else
		{
			pPool->Free( p);
		}
	}
#else // I3_DEBUG
	// -----------------------------------------------
	// Release Routine
	// -----------------------------------------------
	void *	i3ElementBase::operator new( size_t sz )
	{
		i3ElementBase * pNew;

		//INT32 hMem;
		//hMem = i3mem::Alloc( sz, 0, I3MEMORY_MINIMUM_ALLOCSIZE );
		pNew = (i3ElementBase *) i3MemAlloc( sz);

		//pNew->m_Memidx = hMem; 
		pNew->m_bBounded = FALSE;

		return pNew;
	}

	void *	i3ElementBase::operator new( size_t, void * pBuf )
	{
		i3ElementBase * pNew;

		pNew = (i3ElementBase *) pBuf;
		pNew->m_bBounded = TRUE;

		return pNew;
	}

	#if defined( I3_COMPILER_VC)
		void i3ElementBase::operator delete( void * p, const char *, int)
		{
			i3MemoryPool * pPool = ((i3ElementBase *)p)->m_pPool;

			if( pPool == NULL)
			{
				i3MemFree( p);
			}
			else
			{
				pPool->Free( p);
			}
		}

		void i3ElementBase::operator delete( void *p, void *)
		{
			i3MemoryPool * pPool = ((i3ElementBase *)p)->m_pPool;

			if( pPool != NULL)
			{
				pPool->Free( p);
			}
		}
	#endif

	void i3ElementBase::operator delete( void * p)
	{
		i3MemoryPool * pPool = ((i3ElementBase *)p)->m_pPool;

		if( pPool == NULL)
		{
			i3MemFree( p);
		}
		else
		{
			pPool->Free( p);
		}
	}
#endif //I3_DEBUG

#if defined( I3_DEBUG)
void	i3ElementBase::_getInfoString( char * pszStr, const INT32 nStrMax)
{
	pszStr[0] = 0;
}

void* i3GetClassMetaList();
void i3ElementBase::DumpObjects( INT32 nFlag, i3ClassMeta * pMeta, INT32 tag)
{
	i3ElementBase * pTemp = s_pElementHead;
	void *pClassMetaList = i3GetClassMetaList();
	UINT32 cnt = 0, total = 0;

	I3TRACE( "-------------------------  Object Dump --------------------------\n");
	while( pTemp)
	{
		//I3TRACE( "%-32s : %08X (RefCount:%02d) [%05d] %s\n", pTemp->GetMeta()->GetClassName(), pTemp, pTemp->GetRefCount(), 
		//	__LINE__, __FILE__);

		if( pTemp->getMemTag() >= tag)
		{
			if( pTemp != pClassMetaList )
			{
				BOOL bDump = FALSE;

				switch( nFlag )
				{
					// Dump All
				case I3ELEMENT_DUMP_ALL: bDump = TRUE; break;
					// Dump i3 Only
				case I3ELEMENT_DUMP_I3: 
					{
						if( strncmp( pTemp->GetMeta()->GetClassName(), "i3", 2 ) == 0 )
						{
							bDump = TRUE;
						}
					}
					break;
					// Dump except i3
				case I3ELEMENT_DUMP_NOT_I3:
					{
						if( strncmp( pTemp->GetMeta()->GetClassName(), "i3", 2 ) != 0 )
						{
							bDump = TRUE;
						}
					}
					break;
				}

				

				if( bDump )
				{
					if( (pMeta == NULL) || pTemp->IsTypeOf( pMeta))
					{
						if( pTemp->m_SourceLineCount > 0)
						{
							cnt++;

							char szInfo[512];

							pTemp->_getInfoString( szInfo, 512);

							I3TRACE( "%s(%d), %s, %s (%d)\n",
								pTemp->GetMeta()->GetClassName(),
								pTemp->GetRefCount(),
								szInfo,
								pTemp->m_pSourceFilePath,
								pTemp->m_SourceLineCount
								);
						}
					}
				}
			}
		}

		total++;
		pTemp = pTemp->m_pRight;
	}
	I3TRACE( "TOTAL : %d of %d objects\n", cnt, total);
	I3TRACE( "-----------------------------------------------------------------\n");
}

void i3ElementBase::ReportClassInstanceCounts(void)
{
	i3ElementBase * pTemp = s_pElementHead;
	while( pTemp)
	{
		pTemp->GetMeta()->AddInstanceCount(1);

		pTemp = pTemp->m_pRight;
	}

	i3ClassMeta::ReportMetaInstanceToFile();
}

void i3ElementBase::SnapMemoryTag( INT32 tag)
{
	i3ElementBase * pTemp = s_pElementHead;

	s_LinkListcs.Enter(); 
	while( pTemp)
	{
		pTemp->setMemTag( tag);

		pTemp = pTemp->m_pRight;
	}
	s_LinkListcs.Leave(); 
}

void i3ElementBase::setMemTagValue( INT32 tag)
{
	s_MemTag = tag;
}

i3ElementBase * i3ElementBase::getElementHead(void)
{
	return s_pElementHead;
}

void i3ElementBase::EnumObjects( DUMPCALLBACK pUserProc, void * pUserData)
{
	i3ElementBase * pTemp;

	s_LinkListcs.Enter();

	pTemp = getElementHead();

	while( pTemp != NULL)
	{
		if( pUserProc( pTemp, pUserData) == false)
			break;

		pTemp = pTemp->getRight();
	}

	s_LinkListcs.Leave();
}

void i3ElementBase::GetAllInstance( i3ClassMeta * pMeta, i3List * pList)
{
	i3ElementBase * pTemp;

	s_LinkListcs.Enter();

	pTemp = getElementHead();

	while( pTemp != NULL)
	{
		if( pTemp->IsTypeOf( pMeta))
		{
			pList->Add( pTemp);
		}

		pTemp = pTemp->getRight();
	}

	s_LinkListcs.Leave();
}

#endif
