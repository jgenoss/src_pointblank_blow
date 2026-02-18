// i3ElementBase.cpp

#include "i3CommonType.h"
#include "i3ElementBase.h"
#include "i3Debug.h"
#include "i3ErrorHandler.h"
#include "i3Performance.h"
#include "i3Macro.h"
#include "i3Mutex.h"
#include "i3StreamFile.h"

#if defined( I3_DEBUG)
	#include "i3CRC32.h"
#endif


#include <crtdbg.h>

I3_ROOT_CLASS_INSTANCE( i3ElementBase);

#if defined( I3_DEBUG)
	i3ElementBase * s_pElementHead = nullptr;
	static i3Mutex2 s_LinkListcs;
	static INT32	s_MemTag = 0;

//#define USE_REFCOUNT_REPORT
#endif

 
i3ElementBase::i3ElementBase()
{
#if defined( I3_DEBUG) && 0
	s_LinkListcs.Enter(); 

	{
		if( s_pElementHead == nullptr)
		{
			s_pElementHead = this;
		}
		else
		{
			m_pRight = s_pElementHead;

			s_pElementHead = m_pRight->m_pLeft = this;
		}
	}
	s_LinkListcs.Leave(); 

	m_MemTag = s_MemTag;
#endif

}

i3ElementBase::~i3ElementBase()
{
#if defined( I3_DEBUG) && 0
	s_LinkListcs.Enter(); 
	{
		i3ElementBase * pLeft, * pRight;

		pLeft = m_pLeft;
		pRight = m_pRight;

		if( pLeft != nullptr)
		{
			if( pLeft->m_pRight != this)
			{
				I3TRACE( "Bug!!\n");
			}

			pLeft->m_pRight = pRight;
		}

		if( pRight != nullptr)
		{
			if( pRight->m_pLeft != this)
			{
				I3TRACE( "Bug!!\n");
			}

			pRight->m_pLeft = pLeft;
		}

		m_pLeft = nullptr;
		m_pRight = nullptr;

		if( s_pElementHead == this)
		{
			s_pElementHead = pRight;
		}

		if( s_pElementHead != nullptr)
		{
			I3ASSERT( s_pElementHead->m_RefCount >= 0);
		}
	}
	s_LinkListcs.Leave(); 

	//m_RefDebugList.clear();
#endif
}

void i3ElementBase::Release(void)
{
#if defined( I3_WINDOWS)
	INT32 rv = (INT32)::InterlockedDecrement( (LONG*) &m_RefCount);
#else
	I3ASSERT_0;
	m_RefCount--;	// 다른 플랫폼에서 ThreadSafe하게 변경할 필요가 있다.!!!!!!
#endif

	if( rv == 0)
	{
		//	delete this;
#if defined( I3_DEBUG)
		//s_LinkListcs.Enter(); 
#endif

		this->destroy_instance();

#if defined( I3_DEBUG)
		//s_LinkListcs.Leave(); 
#endif
	}
#ifdef I3_DEBUG
	else if( rv == -1)
	{
		char conv[512];

		I3TRACE( "OBJECT : %p\n", this);
		if( this->meta() != nullptr)
		{
			I3TRACE( "META : %p\n", this->meta());
			I3TRACE( "META NAME : %s\n", this->meta()->class_name());
		}

		this->_getInfoString( conv, 512);
		I3TRACE( "INFO : %s\n", conv);
		I3ASSERT_0;
	}
#endif
}

#if defined( I3_DEBUG)

static i3Mutex2 sRefMutex;

INT32 i3ElementBase::AddRef( const char * pszFile, INT32 line)
{
#if defined( USE_REFCOUNT_REPORT)
	{
		i3Monitor2 mon( &sRefMutex);

		i3::string str;
		i3::sprintf( str, "AddRef %s (%d)", pszFile, line);

		vmData * data = new vmData;
		data->_str = str;
		data->_refCnt = m_RefCount;
		m_RefDebugList.push_back( data);
		if( m_RefDebugList.size() > 32)
		{
			delete m_RefDebugList.at(0);
			m_RefDebugList.erase( m_RefDebugList.begin());
		}
	}
#endif

	return AddRef();
}

void i3ElementBase::Release( const char * pszFile, INT32 line)
{
#if defined( USE_REFCOUNT_REPORT)
	{
		i3Monitor2 mon( &sRefMutex);

		if( m_RefCount > 1)
		{
			i3::string str;
			i3::sprintf( str, "Release %s (%d)", pszFile, line);

			vmData * data = new vmData;
			data->_str = str;
			data->_refCnt = m_RefCount;
			m_RefDebugList.push_back( data);
			if( m_RefDebugList.size() > 32)
			{
				delete m_RefDebugList.at(0);
				m_RefDebugList.erase( m_RefDebugList.begin());
			}
		}
		else
		{
			for( UINT32 i = 0; i < m_RefDebugList.size(); i++)
			{
				delete m_RefDebugList.at(i);
			}
			m_RefDebugList.clear();
		}
	}
#endif

	Release();
}

#endif

void i3ElementBase::CopyTo( i3ElementBase * /* pDest */, I3_COPY_METHOD /* method */)
{
}


#if defined( I3_DEBUG)
void	i3ElementBase::_getInfoString( char * pszStr, const INT32 nStrMax)
{
	pszStr[0] = 0;
}

void i3ElementBase::DumpObjects( INT32 nFlag, i3ClassMeta * pMeta, INT32 tag)
{
	i3ElementBase * pTemp = s_pElementHead;

	UINT32 cnt = 0, total = 0;

	I3TRACE( "-------------------------  Object Dump --------------------------\n");
	while( pTemp)
	{
		//I3TRACE( "%-32s : %08X (RefCount:%02d) [%05d] %s\n", pTemp->GetMeta()->GetClassName(), pTemp, pTemp->GetRefCount(), 
		//	__LINE__, __FILE__);

		if( pTemp->getMemTag() >= tag)
		{

			bool bDump = false;

			switch( nFlag )
			{
				// Dump All
			case I3ELEMENT_DUMP_ALL: bDump = true; break;
				// Dump i3 Only
			case I3ELEMENT_DUMP_I3: 
				{
					if( pTemp->meta()->class_name() == "i3"  )
					{
						bDump = true;
					}
				}
				break;
				// Dump except i3
			case I3ELEMENT_DUMP_NOT_I3:
				{
					if( pTemp->meta()->class_name() != "i3" )
					{
						bDump = true;
					}
				}
				break;
			}

			

			if( bDump )
			{
				if( (pMeta == nullptr) || pTemp->kind_of( pMeta))
				{
					if( pTemp->m_SourceLineCount > 0)
					{
						cnt++;

						char szInfo[512];

						pTemp->_getInfoString( szInfo, 512);

						I3TRACE( "%s(%d), %s, %s (%d)\n",
							pTemp->meta()->class_name().c_str(),
							pTemp->GetRefCount(),
							szInfo,
							pTemp->m_pSourceFilePath,
							pTemp->m_SourceLineCount
							);
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
	i3::vector<i3ClassMeta*> meta_list;
	i3::class_meta_system::i()->get_all_metas(meta_list);
	i3::vector<INT32>		 inst_count_list(meta_list.size());
	
	{
		i3::unordered_map<i3ClassMeta*, INT32>	meta_count_map;
		const size_t num_meta_list = meta_list.size();
		for (size_t i = 0 ; i < num_meta_list ; ++i)
			meta_count_map.insert(i3::make_pair(meta_list[i], i) );
		
		i3ElementBase * pTemp = s_pElementHead;
		while( pTemp)
		{
			i3ClassMeta* meta = pTemp->meta();
			i3::unordered_map<i3ClassMeta*, INT32>::iterator it = meta_count_map.find(meta);
			if (it != meta_count_map.end() )
				++inst_count_list[it->second];
			pTemp = pTemp->m_pRight;
		}
	}

	i3ClassMeta * pMeta;
	INT32	cnt = 0;
	static char TempChar[MAX_PATH];
	i3FileStream			DumpFile;

	if( DumpFile.Create("c:\\classdump.i3c", STREAM_WRITE) == false)
	{
		I3TRACE( "Could not create a class dump file (c:\\classdump.i3c)\n");
		return;
	}

	I3TRACE( "--------------------------------------i3Engine Classes--------------------------------------\n");

	for(size_t i = 0; i < meta_list.size(); i++)
	{
		pMeta = meta_list[i];

		//I3TRACE( "%5d [%08X] %s [%d bytes](%d) instances\n", i, pMeta->m_HashCode, pMeta->m_pszClassName, pMeta->m_ClassSize, pMeta->getInstanceCount());
		sprintf( (PSTR)TempChar, (PSTR)"%s\t%d\t%d\n", pMeta->class_name().c_str(), 
			(INT32)pMeta->class_size(), inst_count_list[i] );

		DumpFile.WriteLine( TempChar );	
		cnt += inst_count_list[i];
	}

	DumpFile.Close();

	I3TRACE( " Total %d classes: %d Instances \n", (INT32)meta_list.size(), cnt);
	I3TRACE( "--------------------------------------  End  -------------------------------------------------\n");

}

void i3ElementBase::SnapMemoryTag( INT32 tag)
{
	s_LinkListcs.Enter();

	i3ElementBase * pTemp = s_pElementHead;
	
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

	while( pTemp != nullptr)
	{
		if( pUserProc( pTemp, pUserData) == false)
			break;

		pTemp = pTemp->getRight();
	}

	s_LinkListcs.Leave();
}

void i3ElementBase::GetAllInstance( i3ClassMeta * pMeta, i3::vector<i3ElementBase*>& List)
{
	i3ElementBase * pTemp;

	s_LinkListcs.Enter();

	pTemp = getElementHead();

	while( pTemp != nullptr)
	{
		if( pTemp->kind_of( pMeta))
		{
			List.push_back(pTemp);
		}

		pTemp = pTemp->getRight();
	}

	s_LinkListcs.Leave();
}

#endif
