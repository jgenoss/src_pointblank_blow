#include "stdafx.h"
#include "UdpRecv.h"
#include "i3Base/string/ext/safe_string_copy.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( CUdpRecv ); 

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
CUdpRecv::CUdpRecv()
{
	m_pHeader = NULL;
	m_hFile = NULL;
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
CUdpRecv::~CUdpRecv()
{
	ClearList();

	OnDestroy();

	if( m_pHeader )
	{		
		::UnmapViewOfFile(m_pHeader);
		m_pHeader = NULL;
	}

	if( m_hFile )
	{
		::CloseHandle(m_hFile);
		m_hFile = NULL;
	}
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/

static INT32 _MemCompareProc( MEM_NET_ALLOC_INFO * p1, MEM_NET_ALLOC_INFO * p2)
{
	if( p1->_Address > p2->_Address)
		return 1;
	else if( p1->_Address < p2->_Address)
		return -1;

	return 0;
}

static INT32 _MemFindProc( MEM_NET_ALLOC_INFO * p1, UINT32 pointer)
{
	if( p1->_Address > pointer)
		return 1;
	else if( p1->_Address < pointer)
		return -1;

	return 0;
}

BOOL CUdpRecv::OnCreate( UINT16 Port , BOOL * pMainRecv, CListCtrl	* pMainList)
{	
	// FileMapping
	m_hFile = ::OpenFileMapping(FILE_MAP_WRITE, FALSE, "D:/PBMem.txt");	// lpName 바꿀것

	if( !m_hFile ) 
	{
		INT32 DumpCount = I3_MEM_BUFFER_MAX_COUNT; 
		INT32 DumpMemSize = sizeof(MEM_NET_ALLOC_INFO) * I3_MEM_BUFFER_MAX_COUNT; 
		m_hFile = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, DumpMemSize,  "D:/PBMem.txt");
		m_pHeader = (FILE_MAPPING_HEADER*) ::MapViewOfFile(m_hFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		m_pHeader->_size = DumpCount; 
		m_pHeader->_idxWrite = 0; 
		m_pHeader->_idxRead  = 0;
		m_pStartInfo = (MEM_NET_ALLOC_INFO*) ((char*)m_pHeader + sizeof(FILE_MAPPING_HEADER)); 		
	}else
	{
		m_pHeader	= (FILE_MAPPING_HEADER*) ::MapViewOfFile(m_hFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);	
		m_pStartInfo = (MEM_NET_ALLOC_INFO*) ((char*)m_pHeader + sizeof(FILE_MAPPING_HEADER)); 
	}

	

	m_bIsRun = TRUE;
	
	//Set Main 
	m_pMainRecv	 = pMainRecv; 	
	m_pMainList = pMainList; 
	
	//List Create 
	//m_AllocList.reserve( 65536 ); 
	//m_pAllocList->SetCompareProc( (COMPAREPROC) _MemCompareProc);

	return Create("UdpHolePunch", 0, 4096, 0, PRIORITY_HIGH);		//Thread Create
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CUdpRecv::OnDestroy(void) 
{	
	m_bIsRun = FALSE;
	i3Thread::Suspend();
	i3Thread::Terminate();
//	I3_SAFE_RELEASE( m_pSocket ); 
	return TRUE; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CUdpRecv::BeforeRunning( void * pUserData)
{
	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/

static void cpMemNetInfo( MEM_NET_ALLOC_INFO * pDest, MEM_NET_ALLOC_INFO * pSrc)
{
	pDest->_Size = pSrc->_Size;
	pDest->_Address = pSrc->_Address;  	
	i3::safe_string_copy( pDest->_String, pSrc->_String, sizeof( pDest->_String));
}

static i3::vector<void*> sList;

UINT32 CUdpRecv::OnRunning( void * pUserData)
{
//	INT32	nRecvSize; 
	INT32	nMemorySize; 
	INT32	nReadCount; 
	
	while( m_bIsRun )
	{
		// 읽을 것 없으면 쉰다
		if( m_pHeader == NULL)
			continue;

		nReadCount = m_pHeader->_idxWrite - m_pHeader->_idxRead; 
		if( nReadCount == 0 )
		{
			Sleep(1);
			continue;
		}		

		m_WriteList.Enter();

		for( INT32 i = 0; i < nReadCount;i++)
		{
			UINT32 readIdx = m_pHeader->_idxRead % m_pHeader->_size;
			nMemorySize = m_pStartInfo[ readIdx]._Size;
						
			if(nMemorySize == 0)	
			{
				//Free
				Remove( (MEM_NET_ALLOC_INFO*) &m_pStartInfo[ readIdx]);
			}
			else
			{
				if( m_pStartInfo[ readIdx]._Command == MEMDUMP_CMD_CLEAR)
				{
					ClearList();
				}
				else if( m_pStartInfo[ readIdx]._Command == MEMDUMP_CMD_PRINT)
				{
					DumpFile();
				}
				else
				{
					//Alloc
					Insert( (MEM_NET_ALLOC_INFO*) &m_pStartInfo[ readIdx] ); 
				}
			}

			m_pHeader->_idxRead++;
		}
		m_WriteList.Leave(); 
	}

	return -1;
}

void CUdpRecv::Insert(MEM_NET_ALLOC_INFO * pMemInfo)
{
	//I3TRACE( "ADD : %p %s \n", pMemInfo->_Address, pMemInfo->_String);
	i3::unordered_map<UINT32, MEM_NET_ALLOC_INFO*>::iterator it = m_AllocList.find(pMemInfo->_Address);

	if( it != m_AllocList.end())
	{
		I3TRACE( "ADD duplicate: %p %s \n", pMemInfo->_Address, pMemInfo->_String);
		return;
	}

	MEM_NET_ALLOC_INFO* new_data = (MEM_NET_ALLOC_INFO*) i3::static_pool_malloc_no_dbg( sizeof(MEM_NET_ALLOC_INFO) );
	cpMemNetInfo( new_data, pMemInfo);
	m_AllocList.insert( i3::unordered_map<UINT32, MEM_NET_ALLOC_INFO*>::value_type(pMemInfo->_Address, new_data)); 

}

void CUdpRecv::Remove( MEM_NET_ALLOC_INFO * pMemInfo)
{
//	INT32 idx; 
//	MEM_NET_ALLOC_INFO * pListMemInfo;//, temp; 

	
	//bool bFind = false;

	i3::unordered_map<UINT32, MEM_NET_ALLOC_INFO*>::iterator it = m_AllocList.find( pMemInfo->_Address);

	if( it == m_AllocList.end())
	{
		I3TRACE( "Remove Unknown block : %p %s \n", pMemInfo->_Address, pMemInfo->_String);
		return;
	}

	i3::static_pool_free_no_dbg(it->second);
	m_AllocList.erase( it); 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CUdpRecv::dump(void)
{
	m_WriteList.Enter(); 
	{
		INT32 Idx; 
		UINT32 TotalSize = 0;		
		char Temp[128]; 
		MEM_NET_ALLOC_INFO * pListMemInfo;

		i3::vector<MEM_NET_ALLOC_INFO*>	list_for_sort;
		list_for_sort.reserve(m_AllocList.size());

		i3::unordered_map<UINT32, MEM_NET_ALLOC_INFO*>::iterator it_end = m_AllocList.end();
		for ( i3::unordered_map<UINT32, MEM_NET_ALLOC_INFO*>::iterator it =	m_AllocList.begin();  it != it_end; ++it)
		{
			list_for_sort.push_back(it->second);
		}
				
		struct fn 
		{
			bool operator()( const MEM_NET_ALLOC_INFO* lhs, const MEM_NET_ALLOC_INFO* rhs) const
			{
				return lhs->_Size > rhs->_Size;					// 큰게 먼저 나오게 조절..
			}
		};

		i3::sort(list_for_sort.begin(), list_for_sort.end(), fn() );

		const size_t num = list_for_sort.size();

		for ( size_t i = 0 ; i < num ; ++i )
		{
			pListMemInfo = list_for_sort[i];

			sprintf(Temp, "%x", pListMemInfo->_Address); 
			Idx = m_pMainList->InsertItem( i, Temp); 	

			sprintf(Temp, "%d", pListMemInfo->_Size);  
			m_pMainList->SetItemText( Idx, 1, Temp);

			m_pMainList->SetItemText( Idx, 2, pListMemInfo->_String);
			TotalSize = TotalSize + pListMemInfo->_Size; 
		}

		sprintf(Temp, "*TOTAL*"); 
		Idx = m_pMainList->InsertItem( 0, Temp); 
		sprintf(Temp, "%d", TotalSize);  
		m_pMainList->SetItemText( Idx, 1, Temp);
		sprintf(Temp, "Block Count = %d", m_AllocList.size());
		m_pMainList->SetItemText( Idx, 2, Temp);
	}
	m_WriteList.Leave();

	return; 
}

void CUdpRecv::DumpFile(void)
{
	HANDLE hFile = ::CreateFile("D:\\MemoryDump.txt", GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if(hFile == INVALID_HANDLE_VALUE)
	{			
		::CloseHandle( hFile );
		return;
	}

	m_WriteList.Enter(); 
	{
		UINT32 TotalSize = 0;		
		char Temp[128]; 
		INT32 Len;
		MEM_NET_ALLOC_INFO * pListMemInfo;

		std::vector<MEM_NET_ALLOC_INFO*> sortedList;

		SortByUpperSize( sortedList);

		for( UINT32 i = 0; i < sortedList.size(); i++)
		{
			i3::unordered_map<UINT32,MEM_NET_ALLOC_INFO*>::iterator it = m_AllocList.find( sortedList.at(i)->_Address);

			if( it != m_AllocList.end())
			{		
				pListMemInfo = (MEM_NET_ALLOC_INFO*) it->second;
				sprintf(Temp, "[%x|%d]%s\r\n", pListMemInfo->_Address, pListMemInfo->_Size, pListMemInfo->_String); 
				Len = i3::generic_string_size(Temp); 
				::WriteFile( hFile, Temp, Len, (LPDWORD)&Len, NULL ); 

				TotalSize = TotalSize + pListMemInfo->_Size; 
			}
		}

		sprintf(Temp, "[TOTAL|%d]Block Count = %d\r\n", TotalSize, m_AllocList.size()); 
		Len = i3::generic_string_size(Temp); 
		::WriteFile( hFile, Temp, Len, (LPDWORD)&Len, NULL ); 

	}
	m_WriteList.Leave();

	::CloseHandle( hFile );
	return; 
}

void CUdpRecv::ClearList(void)
{
	m_WriteList.Enter(); 
	{			
		i3::unordered_map<UINT32, MEM_NET_ALLOC_INFO*>::iterator it_end = m_AllocList.end();
		for ( i3::unordered_map<UINT32, MEM_NET_ALLOC_INFO*>::iterator it = m_AllocList.begin(); it != it_end ; ++it)
		{
			MEM_NET_ALLOC_INFO * pListMemInfo = it->second;
			i3::static_pool_free_no_dbg(pListMemInfo);
		}

		m_AllocList.clear();
	}
	m_WriteList.Leave(); 
	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CUdpRecv::AfterRunning( void * pUserData)
{
	return; 
}

static int _comp( const void * p1, const void * p2)
{
	MEM_NET_ALLOC_INFO * pInfo1, * pInfo2;

	pInfo1 = *((MEM_NET_ALLOC_INFO **) p1);
	pInfo2 = *((MEM_NET_ALLOC_INFO **) p2);

	if( pInfo1 != NULL && pInfo2 != NULL)
	{
		if( pInfo1->_Size < pInfo2->_Size)
			return 1;
		else if( pInfo1->_Size > pInfo2->_Size)
			return -1;
	}

	return 0;
}

void CUdpRecv::SortByUpperSize( std::vector<MEM_NET_ALLOC_INFO*> & sortedList )
{
	UINT32 TotalSize = 0;		
//	char Temp[128]; 
//	INT32 Len;
	MEM_NET_ALLOC_INFO * pListMemInfo;
	
	i3::unordered_map<UINT32,MEM_NET_ALLOC_INFO*>::iterator it = m_AllocList.begin();
	MEM_NET_ALLOC_INFO ** ppTemp = (MEM_NET_ALLOC_INFO**) i3MemAlloc( sizeof(MEM_NET_ALLOC_INFO*) * m_AllocList.size());

	UINT32 i = 0;
	while( it != m_AllocList.end())
	{		
		pListMemInfo = (MEM_NET_ALLOC_INFO*) it->second;

		ppTemp[i] = pListMemInfo;
		
		it++;
		i++;
	}

	::qsort( ppTemp, m_AllocList.size(), sizeof(MEM_NET_ALLOC_INFO*), _comp);

	for( UINT32 i = 0; i < m_AllocList.size(); i++)
	{
		sortedList.push_back( ppTemp[i]);
	}

	I3MEM_SAFE_FREE( ppTemp);
}
