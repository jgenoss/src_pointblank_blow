#include "i3CommonType.h"
#include "i3Memory.h"
#include "i3Stream.h"
#include "i3StreamFile.h"
#include "i3Debug.h"
#include "i3ErrorHandler.h"
#include "i3String.h"
#include "i3Macro.h"
#include "i3Performance.h"
#include "i3Mutex.h"

//#define			USE_WIN32_HEAP

#if defined( I3_WINDOWS	) || defined( I3_WINCE )
#if defined( I3_DEBUG)
	#include <crtdbg.h>
#endif
#include <stdio.h>

// Memory Allocation 함수들의 성능 확인 결과, 5000개를 할당한 후, random하게 alloc/free를 반복했을 때
// 아래와 같은 결과가 나옴.
//
// LocalAlloc	: 176 ms	(929308672 allocated)
// HeapAlloc	: 1446 ms	(929325056 allocated)
// malloc		: 209 ms	(904056832 allocated)
//
// 그런 이유로 HeapAlloc을 사용하던 기존의 Code를 malloc/free로 대체함.
static HANDLE	s_hHeap = NULL;
#endif

#define		I3_MEMORY_ALIGN			0x0000FFFF
#define		I3_MEMORY_STATE			0xFFFF0000

#define		DIVIDE_SIZE				1024
#define		INFO_SIZE				4
#define		GATHER_COUNT			1000

static INT32				s_Size				= -1;
static I3MEMBLOCK		*	s_pAllocBlockHead	= NULL;		
static I3MEMBLOCK		*	s_pFreeBlockHead	= NULL;
static LPI3MEM				s_pBuffer			= NULL;
static LPI3MEM				s_pPointer[2]		= {NULL, NULL};
static LPI3MEM				s_pMovePointer[2]	= {NULL, NULL};
static INT32				s_GatherCount		= 0; 
static BOOL					s_IsGame			= TRUE;
static	bool				s_bMemoryInit		= false;
static char					TempChar[256];

static i3Mutex2				s_MemoryCs; 

#if defined( I3_DEBUG)
static UINT32				s_AllocSize			= 0;
static UINT32				s_AllocCount		= 0; 
#endif

#if defined( I3_WIPI)
UINT32		s_hMem;
#endif

namespace i3mem
{
	I3_EXPORT_BASE void *	(*Alloc)(INT32 nBytes,	INT32 State, INT32 Align, const char * FileName ,INT32 Line) = NULL; 
	I3_EXPORT_BASE void *	(*Realloc)(void * pPointer,	INT32 nBytes,INT32 Align, const char * FileName, INT32 Line) = NULL; 
	I3_EXPORT_BASE void		(*Free)(void * p, const char * pszFile, INT32 line ) = NULL; 
	I3_EXPORT_BASE void		(*Compact)(void) = NULL; 
};

I3_EXPORT_BASE void i3MemoryInit( UINT32 nMB , char * pMemoryName)
{
	if( s_bMemoryInit == false)
	{
		if( nMB == 0 )
		{
			i3mem::Create(0, FALSE , pMemoryName);
		}else
		{
			i3mem::Create(1024 * 1024 * nMB,  TRUE, pMemoryName);
		}

		s_bMemoryInit = true;
	}
}

/*-----------------------------------------------------------------------------
//  내부적으로 사용되는 인라인 함수들입니다. 대부분 리스트를 생성할때 사용합니다. 
//  Alloc 리스트는 디버그용으로만 사용합니다. 
//----------------------------------------------------------------------------*/
inline void PushAllocList(I3MEMBLOCK * pBlock)
{
	//Set Alloc Block
	if(s_pAllocBlockHead == NULL)
	{			
		pBlock->m_Next	= NULL;		
		pBlock->m_Prev	= NULL; 
	}else
	{		
		s_pAllocBlockHead->m_Next	= pBlock;
		pBlock->m_Prev				= s_pAllocBlockHead; 
		pBlock->m_Next				= NULL;
	}
	s_pAllocBlockHead = pBlock;
}

inline void PopAllocList(I3MEMBLOCK * pBlock)
{
	I3MEMBLOCK	*	pTempBlock;
	if(s_pAllocBlockHead == pBlock)
	{
		s_pAllocBlockHead = pBlock->m_Prev;
		if( s_pAllocBlockHead != NULL)
			s_pAllocBlockHead->m_Next = NULL;
	}else
	{		
		if(pBlock->m_Prev != NULL)
		{
			pTempBlock			= pBlock->m_Prev; 
			pTempBlock->m_Next	= pBlock->m_Next; 
		}

		if(pBlock->m_Next != NULL)
		{
			pTempBlock			= pBlock->m_Next; 
			pTempBlock->m_Prev  = pBlock->m_Prev; 
		}
	}
	pBlock->m_Prev = NULL;
	pBlock->m_Next = NULL;
}


/*-----------------------------------------------------------------------------
// Name: Network Dump 
// Desc: 
//----------------------------------------------------------------------------*/
#if defined(I3_MEM_NET_DUMP)
	static BOOL					s_bNetDump			= FALSE; 
	static FILE_MAPPING_HEADER	* s_pMappingHeader	= NULL; 
	static MEM_NET_ALLOC_INFO	* s_pNetAllocInfo	= NULL;
	static HANDLE				s_hFile;
	//static i3Mutex				s_DumpCs; 	

	void DumpWriteFileBuffer(MEM_NET_ALLOC_INFO * pAllocInfo )
	{
		if( s_pMappingHeader == NULL ) return; 
//		s_DumpCs.Enter(); 
		{	            
			if( (s_pMappingHeader->_idxWrite - s_pMappingHeader->_idxRead) < s_pMappingHeader->_size)
			{
				UINT32 Writepos = s_pMappingHeader->_idxWrite % s_pMappingHeader->_size; 
				i3mem::Copy( &s_pNetAllocInfo[Writepos], pAllocInfo, sizeof(MEM_NET_ALLOC_INFO)); 
				s_pMappingHeader->_idxWrite ++;
			}else
			{
				I3TRACE("i3mem::[DumpWriteFileBuffer] Need Memory Block \n"); 
			}
		}
//		s_DumpCs.Leave(); 
	}
#endif


void i3mem::SetDumpEnable(BOOL bEnable)
{
#if defined(I3_MEM_NET_DUMP)
	s_bNetDump	= bEnable; 	
#endif
}

/*-----------------------------------------------------------------------------
// Name: I3Mem::Create
// Desc: Create Memory Manager
//----------------------------------------------------------------------------*/
void i3mem::Create(INT32 Size, BOOL IsGame, char * pMemoryName)
{
//#if defined( I3_DEBUG)
//	_CrtSetDbgFlag( _CRTDBG_CHECK_ALWAYS_DF);
//#endif

	
	if(IsGame)
	{
		//게임용으로 사용할때 
		Alloc	= AllocGame; 
		Realloc = ReallocGame; 
		Free	= FreeGame; 
		Compact	= CompactGame; 

	#ifdef I3_PSP	
		s_pBuffer = (LPI3MEM) memalign( 16, Size ); 
	#elif defined( I3_WINDOWS) || defined( I3_WINCE )
		s_pBuffer = (LPI3MEM)::LocalAlloc( LPTR, Size );
	#elif defined( I3_WIPI)
		s_hMem = MC_knlAlloc( Size);
		s_pBuffer = (LPI3MEM) MC_GETDPTR( s_hMem);
	#else
		s_pBuffer = (LPI3MEM)malloc( (size_t) Size );
	#endif

	#ifdef I3_DEBUG
		if(s_pBuffer == NULL)
		{		
			I3TRACE("[i3mem::Create] s_pBuffer is Null Alloc fail \n"); 
			I3ASSERT_0; 
		}
	#endif
		FillZero( s_pBuffer, Size); 

		//Set Use Memory
		s_pPointer[0]	= s_pMovePointer[0] = &s_pBuffer[0]; 
		s_pPointer[1]	= s_pMovePointer[1] = &s_pBuffer[Size];
		s_Size			= Size;
#if defined( I3_DEBUG)
		s_AllocSize		= 0;
		s_AllocCount	= 0; 
#endif
		//Set Free Block 
		s_pAllocBlockHead	= NULL;
		s_pFreeBlockHead	= NULL;

		//Make Free Block 
		s_pFreeBlockHead	= (I3MEMBLOCK *)s_pBuffer; 
		s_pFreeBlockHead->m_pPointer	= NULL;
		s_pFreeBlockHead->m_Size		= s_Size - ( sizeof(I3MEMBLOCK) + INFO_SIZE ); 
		s_pFreeBlockHead->m_StateAlign	= 0;

	#ifdef I3_DEBUG
		I3TRACE2("[i3mem::Create] Create Memory Size = %d Pointer 0x%x \n", s_Size, s_pBuffer);
	#endif
	}
	else
	{
		//툴용으로 사용할때 
		Alloc				= AllocTool; 
		Realloc				= ReallocTool;
		Free				= FreeTool;
		Compact				= CompactTool; 

		s_pPointer[0]		= s_pMovePointer[0] = NULL; 
		s_pPointer[1]		= s_pMovePointer[1] = NULL;
		s_Size				= 0;
#if defined( I3_DEBUG)
		s_AllocSize			= 0;		
		s_AllocCount		= 0; 
#endif
		s_pAllocBlockHead	= NULL;
		s_pFreeBlockHead	= NULL;

		#if defined( I3_WINDOWS)
		{
			#if defined( USE_WIN32_HEAP)
				s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, 0, 0);
				I3ASSERT( s_hHeap != NULL);

			/*
			{
				ULONG data = 2;
				HeapSetInformation( s_hHeap, HeapCompatibilityInformation, (PVOID) &data, sizeof(data));
			}
			*/
			#endif
		}
		#endif

		#ifdef I3_DEBUG
		I3TRACE("[i3mem::Create] Tool Create Memory \n");
		#endif
	}

#if defined(I3_MEM_NET_DUMP)
	if(pMemoryName != NULL)
	{
		INT32 DumpCount = I3_MEM_BUFFER_MAX_COUNT; 
		INT32 DumpMemSize = sizeof(MEM_NET_ALLOC_INFO) * I3_MEM_BUFFER_MAX_COUNT; 

		s_hFile = ::OpenFileMapping(FILE_MAP_WRITE, FALSE, pMemoryName);	

		if( !s_hFile ) 
		{
			s_hFile = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, DumpMemSize,  pMemoryName);
			if(s_hFile)
			{
				s_pMappingHeader = (FILE_MAPPING_HEADER*) ::MapViewOfFile(s_hFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
				s_pMappingHeader->_size = DumpCount; 
				s_pMappingHeader->_idxWrite = 0; 
				s_pMappingHeader->_idxRead  = 0;
				s_pNetAllocInfo = (MEM_NET_ALLOC_INFO*) ((char*)s_pMappingHeader + sizeof(FILE_MAPPING_HEADER) ); 
			}
			else 
			{
				I3TRACE("[i3mem::Init] Error I3_MEM_NET_DUMP Failed \n");
			}
		}
		else
		{
			s_pMappingHeader = (FILE_MAPPING_HEADER*) ::MapViewOfFile(s_hFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);	
			s_pNetAllocInfo  = (MEM_NET_ALLOC_INFO*) ((char*)s_pMappingHeader + sizeof(FILE_MAPPING_HEADER) ); 
		}
	}
	else
	{
		s_pMappingHeader = NULL; 		
	}
#endif

	s_IsGame = IsGame; 
	return;  
}

/*-----------------------------------------------------------------------------
// Name: I3MemDestroy
// Desc: Destroy Memory Manager
//----------------------------------------------------------------------------*/
void i3mem::Destroy(void)
{
	//Delete Memory
	if (s_pBuffer == NULL)
	{
		I3TRACE("[i3mem::Destroy] s_pBuffer is NULL \n"); 
		return; 
	}

#ifdef I3_PSP
	free( s_pBuffer);
#elif defined( I3_WINDOWS ) || defined( I3_WINCE )
	if( s_pBuffer != NULL)
	{
		::LocalFree( s_pBuffer);
	}

	#if defined( USE_WIN32_HEAP)
		if( s_hHeap != NULL)
		{
			::HeapDestroy( s_hHeap);
			s_hHeap = NULL;
		}
	#endif

#elif defined( I3_WIPI)
	MC_knlFree( s_hMem ); 	
#else
	free( s_pBuffer );
#endif

	//Set Value 
	s_Size				= -1;
	s_pBuffer			= NULL;
	s_pPointer[0]		= s_pPointer[1]		= NULL;	
	s_pMovePointer[0]	= s_pMovePointer[1] = NULL;

#if defined(I3_MEM_NET_DUMP)
	CloseHandle( s_hFile ); 
#endif
	return;
}

/*-----------------------------------------------------------------------------
// Name: 
// Desc: 
//----------------------------------------------------------------------------*/
void * i3mem::AllocGame(INT32 nBytes, INT32 State, INT32 Align, const char * FileName, INT32 Line)
{
	s_MemoryCs.Enter(); 

	I3MEMBLOCK	* pBlockPointer;
	I3MEMBLOCK	* pPreBlock; 
	LPI3MEM  pTempPointer; 
	LPI3MEM  pMemPointer; 
	I3ASSERT(nBytes != 0); 

	nBytes		= ALIGNUP( nBytes, Align);
	
	pBlockPointer = s_pFreeBlockHead;
	pPreBlock		= NULL; 
	
	//First Fit 
	while(pBlockPointer != NULL)
	{		
		if( (pBlockPointer->m_pPointer == NULL ) &&  (pBlockPointer->m_Size >= nBytes) )
		{
			pTempPointer = (LPI3MEM)pBlockPointer + sizeof(I3MEMBLOCK) + INFO_SIZE;
			pMemPointer = (LPI3MEM)ALIGNUP( pTempPointer , Align);
			if( pBlockPointer->m_Size >= ( (pMemPointer - pTempPointer) + nBytes) )
				break;
		}
		pPreBlock		= pBlockPointer;
		pBlockPointer	= pBlockPointer->m_Next;
	}
	
	if(pBlockPointer == NULL)
	{
	#ifdef I3_DEBUG
		Dump(); 
		I3TRACE1("[i3mem::Alloc] Not Enough Memory Size = %d, So Gather Free Memory Block \n", nBytes); 		
	#endif
		CompactGame();
		pBlockPointer = s_pFreeBlockHead;
		pPreBlock		= NULL; 
		
		//First Fit 
		while(pBlockPointer != NULL)
		{		
			if( (pBlockPointer->m_pPointer == NULL ) &&  (pBlockPointer->m_Size >= nBytes) )
			{
				pTempPointer = (LPI3MEM)pBlockPointer + sizeof(I3MEMBLOCK) + INFO_SIZE;
				pMemPointer = (LPI3MEM)ALIGNUP( pTempPointer , Align);			
				if( pBlockPointer->m_Size >= ( (pMemPointer - pTempPointer) + nBytes) )
					break;
			}
			pPreBlock		= pBlockPointer; 
			pBlockPointer	= pBlockPointer->m_Next;		
		}	
		
		if(pBlockPointer == NULL)
		{			
			I3TRACE1("[i3mem::Alloc] Not Enough Memory Size = %d \n", nBytes); 
			Dump(); 
			I3ASSERT_0; 
		}
	}

	//Free Block에서 삭제.
	if( s_pFreeBlockHead == pBlockPointer)	s_pFreeBlockHead = pBlockPointer->m_Next;
	else
	{
		I3ASSERT( pPreBlock != NULL);

		pPreBlock->m_Next = pBlockPointer->m_Next;
	}

	pTempPointer = (LPI3MEM)pBlockPointer + sizeof(I3MEMBLOCK) + INFO_SIZE;
	pMemPointer  = (LPI3MEM)ALIGNUP( pTempPointer , Align );

	//Set New Size 
	pBlockPointer->m_pPointer = pMemPointer; 
	pBlockPointer->m_StateAlign	= (Align & I3_MEMORY_ALIGN) | (State & I3_MEMORY_STATE);		

	//Set Memory Info 
	#if defined( I3_DEBUG)
	pBlockPointer->m_pAllocPos = FileName;
	pBlockPointer->m_Line = Line;
	#endif

	//Set Info Pointer
	LPI3MEM pInfoPointer = (LPI3MEM)(pMemPointer - INFO_SIZE);
	memcpy( pInfoPointer, &pBlockPointer, INFO_SIZE );

	if( pBlockPointer->m_Size > (INT32)( ( nBytes + sizeof(I3MEMBLOCK) + INFO_SIZE) + DIVIDE_SIZE ) )
	{
		I3MEMBLOCK	* pNewFreeBlock;
		LPI3MEM		pStartPointer;
		LPI3MEM		pCenterPointer; 
		LPI3MEM		pEndPointer;
		
		pStartPointer	= (LPI3MEM)pBlockPointer; 
		pCenterPointer	= pBlockPointer->m_pPointer + nBytes;
		pEndPointer		= (pStartPointer + INFO_SIZE + sizeof(I3MEMBLOCK) + pBlockPointer->m_Size ); 

		//Set New Block Memory size; 
		pBlockPointer->m_Size = (pCenterPointer - pStartPointer) - (INFO_SIZE + sizeof(I3MEMBLOCK)); 

		//Set New Free Block Memory;
		pNewFreeBlock = (I3MEMBLOCK	* )pCenterPointer;
		pNewFreeBlock->m_pPointer	= NULL; 
		pNewFreeBlock->m_Prev		= NULL; 
		pNewFreeBlock->m_Next		= NULL;
		pNewFreeBlock->m_Size		= (pEndPointer - pCenterPointer) - (INFO_SIZE + sizeof(I3MEMBLOCK)); 
		pNewFreeBlock->m_StateAlign = 0; 

		//Set FreeBlock 
		pNewFreeBlock->m_Next	= s_pFreeBlockHead;
		s_pFreeBlockHead		= pNewFreeBlock;
	}
#if defined( I3_DEBUG)
	s_AllocSize = s_AllocSize + pBlockPointer->m_Size; 
	s_AllocCount++;
#endif

	PushAllocList( pBlockPointer ); 
	FillZero( pMemPointer, pBlockPointer->m_Size); 
	
#if defined(I3_MEM_NET_DUMP)
	if(s_bNetDump)
	{
		MEM_NET_ALLOC_INFO SendBuffer; 
		SendBuffer._Size	= nBytes;
		SendBuffer._Address = (UINT32)pMemPointer; 		
		sprintf( SendBuffer._String, "%s - %d", FileName, Line); 
		
		DumpWriteFileBuffer( &SendBuffer );  
	}
#endif
	s_MemoryCs.Leave(); 

	return pMemPointer; 
}

/*-----------------------------------------------------------------------------
// Name: Free(void * pPointer)
// Desc: 
//----------------------------------------------------------------------------*/
void i3mem::FreeGame(void * pPointer, const char * pszFile, INT32 line )
{	
	if(pPointer == NULL)return; 

	s_MemoryCs.Enter(); 
	I3MEMBLOCK	*	pMemBlock;	

	//Get Memory Info Block And Set Memory Info 
	memcpy( &pMemBlock, ((LPI3MEM)pPointer - INFO_SIZE), INFO_SIZE );

#if defined( I3_DEBUG)
	s_AllocSize = s_AllocSize - pMemBlock->m_Size; 
	s_AllocCount--;
#endif

	pMemBlock->m_pPointer	= NULL;
	pMemBlock->m_StateAlign = 0x00000000;

	PopAllocList( pMemBlock ); 

	//Set FreeBlock 
	pMemBlock->m_Next	= s_pFreeBlockHead;
	s_pFreeBlockHead	= pMemBlock;


	s_GatherCount++; 
	if( s_GatherCount > GATHER_COUNT)
	{
		CompactGame(); 
		s_GatherCount = 0; 
	}

#if defined(I3_MEM_NET_DUMP)
	if(s_bNetDump)
	{
		MEM_NET_ALLOC_INFO SendBuffer; 
		SendBuffer._Size	= 0;
		SendBuffer._Address = (UINT32)pPointer; 				
		sprintf( SendBuffer._String, "%s - %d", pszFile, line); 
		DumpWriteFileBuffer( &SendBuffer );  
		//s_pDumpUdp->Send((char*)&SendBuffer, 8, ::inet_addr("127.0.0.1"), MEM_DUMP_PORT);
	}
#endif

	s_MemoryCs.Leave(); 
	
	return;
} 

/*-----------------------------------------------------------------------------
// Name: CompactGame(void)
// Desc: 
//----------------------------------------------------------------------------*/
void i3mem::CompactGame(void)
{
	I3MEMBLOCK * pBlock;
	INT32		i; 

	//하나만 있기 때문에 할 필요없다. 
	if(s_pFreeBlockHead->m_Next == NULL)return;

	//Sort by Pointer 	
	{
		I3MEMBLOCK	* pStartBlock;
		I3MEMBLOCK	* pSelBlock; 
		I3MEMBLOCK	* pSelPreBlock; 
		I3MEMBLOCK	* pPreBlock;
		INT32		  Count; 

		pSelPreBlock	= NULL; 
		pPreBlock		= NULL; 
		pSelBlock		= s_pFreeBlockHead;
		pBlock			= s_pFreeBlockHead;
		Count			= 0; 

		//Find Start 
		while(pBlock != NULL)
		{
			if( pSelBlock > pBlock )
			{
				pSelPreBlock = pPreBlock; 
				pSelBlock	 = pBlock;				
			}
			pPreBlock	= pBlock; 
			pBlock		= pBlock->m_Next;
			Count++;
		}

		pStartBlock = pSelBlock;
		if( s_pFreeBlockHead == pStartBlock )
		{
			s_pFreeBlockHead = s_pFreeBlockHead->m_Next;
		}
		else 
		{
			I3ASSERT( pSelPreBlock != NULL);

			pSelPreBlock->m_Next = pStartBlock->m_Next;
		}
		pStartBlock->m_Next = NULL; 
	
		//Sort Free List 
		I3MEMBLOCK	* pNowBlock = pStartBlock;
		for( i = 0; i < (Count - 1); i++)
		{
			pSelPreBlock = NULL;
			pPreBlock	 = NULL;			
			pSelBlock	 = s_pFreeBlockHead;
			pBlock		 = s_pFreeBlockHead;

			while(pBlock !=NULL)
			{
				if( pSelBlock > pBlock )
				{
					pSelPreBlock = pPreBlock; 
					pSelBlock	 = pBlock;
				}
				pPreBlock = pBlock;
				pBlock	  = pBlock->m_Next; 
			}

			if( s_pFreeBlockHead == pSelBlock )
			{
				s_pFreeBlockHead = s_pFreeBlockHead->m_Next;
			}
			else 
			{
				I3ASSERT( pSelPreBlock != NULL);
				pSelPreBlock->m_Next = pSelBlock->m_Next;
			}

			pNowBlock->m_Next = pSelBlock; 
			pNowBlock = pSelBlock; 
			pNowBlock->m_Next = NULL;
		}
		s_pFreeBlockHead = pStartBlock; 
	}
	
	//Gather Free Block 
	{
		I3MEMBLOCK * pNextBlock; 
		pBlock = s_pFreeBlockHead;		
		while(pBlock != NULL)
		{
			if(pBlock->m_Next != NULL)
			{
				pNextBlock = pBlock->m_Next; 
				if( ((LPI3MEM)pBlock + pBlock->m_Size + sizeof(I3MEMBLOCK) + INFO_SIZE) == (LPI3MEM)pNextBlock )
				{
					pBlock->m_Size = pBlock->m_Size + pNextBlock->m_Size + sizeof(I3MEMBLOCK) + INFO_SIZE; 
					pBlock->m_Next = pNextBlock->m_Next; 
				}else
				{
					pBlock = pBlock->m_Next;
				}

			}else
			{
				pBlock = pBlock->m_Next;
			}
		}		
	}

	return; 
}

/*-----------------------------------------------------------------------------
// Name: Realloc(void * pPointer, UINT32 nBytes, INT32 Align, const char * FileName, INT32 Line)
// Desc: 
//----------------------------------------------------------------------------*/
void * i3mem::ReallocGame(void * pPointer, INT32 nBytes, INT32 Align, const char * FileName, INT32 Line)
{	
	I3MEMBLOCK	*	pOldMemBlock; 
	void		*	pNewPointer;  

	memcpy( &pOldMemBlock, ((LPI3MEM)pPointer - INFO_SIZE), INFO_SIZE );	

	if(Align == -1)Align = ( pOldMemBlock->m_StateAlign & I3_MEMORY_ALIGN );

	pNewPointer = AllocGame(nBytes, (pOldMemBlock->m_StateAlign & I3_MEMORY_STATE), Align, FileName, Line);
	i3mem::Copy(pNewPointer, pPointer, (INT32) MIN(nBytes, pOldMemBlock->m_Size) );
	FreeGame(pPointer, FileName, Line);
#if defined(I3_MEM_NET_DUMP)
	if(s_bNetDump)
	{
		MEM_NET_ALLOC_INFO SendBuffer; 
		SendBuffer._Size	= 0;
		SendBuffer._Address = (UINT32)pPointer; 		
		sprintf( SendBuffer._String, "%s - %d", FileName, Line); 		
		DumpWriteFileBuffer( &SendBuffer );  		
	}
	if(s_bNetDump)
	{
		MEM_NET_ALLOC_INFO SendBuffer; 
		SendBuffer._Size	= nBytes;
		SendBuffer._Address = (UINT32)pNewPointer; 				
		sprintf( SendBuffer._String, "%s - %d", FileName, Line); 
		DumpWriteFileBuffer( &SendBuffer ); 
	}
#endif
	return pNewPointer; 
}

/*-----------------------------------------------------------------------------
// Name: 
// Desc: 
//----------------------------------------------------------------------------*/
void i3mem::SetState(void * pPoint)
{
	pPoint = NULL;
	return; 
}

/*-----------------------------------------------------------------------------
// Name: 
// Desc: 
//----------------------------------------------------------------------------*/
INT32 i3mem::GetState(void * pPoint)
{
	pPoint = NULL;
	return -1; 
}

/*-----------------------------------------------------------------------------
// Name: UINT32 GetFreeMemorySize(void)
// Desc: Destroy Memory Manager
//----------------------------------------------------------------------------*/
void i3mem::Dump(INT32 Type)
{
	BOOL OutText	= FALSE; 
	BOOL OutFile	= FALSE; 
	BOOL OutSum		= FALSE; 
	BOOL OutEach	= FALSE; 	

	OutFile  = ( Type & I3MEMORY_DEBUG_FILE  ) ? TRUE : FALSE; 
	OutText  = ( Type & I3MEMORY_DEBUG_TEXT  ) ? TRUE : FALSE; 
	OutSum	 = ( Type & I3MEMORY_DEBUG_SUM   ) ? TRUE : FALSE; 
	OutEach  = ( Type & I3MEMORY_DEBUG_EACH  ) ? TRUE : FALSE;
	
	//-----------------------------------------------------------------------------------------
	//파일로 덤프 합니다. 
	if(OutFile)
	{
		i3FileStream			DumpFile;

		if( DumpFile.Create("c:\\MemDump.i3m", STREAM_WRITE) == FALSE)
		{
			I3TRACE( "Could not create a memory dump file (c:\\MemDump.i3m)\n");
			return;
		}

		if(OutEach)
		{
			I3TRACE("Memory File Dump Start\n");		
			I3MEMBLOCK	* pBlockPointer = s_pAllocBlockHead;
			while(pBlockPointer != NULL)
			{
#if defined( I3_DEBUG)
				sprintf( (PSTR) TempChar, (PSTR)"0x%p\t%s\t%d\t%d\n", pBlockPointer, pBlockPointer->m_pAllocPos, pBlockPointer->m_Line, pBlockPointer->m_Size); 
				
				DumpFile.WriteLine( TempChar );
#endif
				pBlockPointer = pBlockPointer->m_Prev;
			}
			I3TRACE("Memory File Dump End\n");		
		}

		DumpFile.Close(); 
	}
	
	//-----------------------------------------------------------------------------------------
	//Trace로 덤프합니다. 
	if(OutText)
	{		
		if(OutSum)
		{
			I3TRACE("====================== SUM MEMORY ==================================\n"); 
			i3String::ValueToStr((INT32)(s_pPointer[1] - s_pPointer[0]), TempChar, 128); 
			I3TRACE1("Total Memory Size : %s \n", TempChar);

			I3MEMBLOCK	* pBlock;
			INT32	AllocBlock	= 0; 
			INT32	FreeBlock	= 0; 
			INT32	AllocMemSize = 0; 
			INT32	FreeMemSize	 = 0; 
			pBlock = s_pAllocBlockHead;
			while(pBlock != NULL)
			{
				AllocMemSize = AllocMemSize + pBlock->m_Size; 
				AllocBlock++; 
				pBlock = pBlock->m_Prev; 
			}

			pBlock = s_pFreeBlockHead;
			while(pBlock != NULL)
			{			
				FreeMemSize = FreeMemSize + pBlock->m_Size; 
				FreeBlock++;
				pBlock = pBlock->m_Next; 
			}

			I3TRACE3("Use Block Count ALL = %d ALLOC = %d FREE = %d \n",(AllocBlock + FreeBlock), AllocBlock, FreeBlock);
			I3TRACE2("Memory Size Alloc = %d Free = %d \n",AllocMemSize ,FreeMemSize);
#if defined( I3_DEBUG)
			I3TRACE( "Memory Alloc Count = %d\n", s_AllocCount);
			I3TRACE( "Memory Alloc Size  = %d\n", s_AllocSize);
#endif


			I3TRACE("====================================================================\n"); 
		}
		
		if(OutEach)
		{
			I3TRACE("====================== SUM MEMORY ==================================\n");		
			I3MEMBLOCK	* pBlockPointer = s_pAllocBlockHead;
			while(pBlockPointer != NULL)
			{
#if defined( I3_DEBUG)
				sprintf( (PSTR) TempChar, (PSTR)"FileName %s %d\n",pBlockPointer->m_pAllocPos, pBlockPointer->m_Line); 
				I3TRACE( TempChar );
#endif
				
				pBlockPointer = pBlockPointer->m_Prev;
			}

			I3TRACE("====================================================================\n");
		}
	}
	return; 
}

void i3mem::Enum( I3MEM_ENUM_CALLBACK pUserProc, void * pUserData)
{
	s_MemoryCs.Enter();

	I3MEMBLOCK	* pBlock = s_pAllocBlockHead;

	while( pBlock != NULL)
	{
		if( pUserProc( pBlock, pUserData) == false)
			break;

		pBlock = pBlock->m_Prev;
	}

	s_MemoryCs.Leave();
}

/*-----------------------------------------------------------------------------
// Name: AllocTool(INT32 nBytes, INT32 State, INT32 Align, const char * FileName, INT32 Line)
// Desc: 
//----------------------------------------------------------------------------*/
void * i3mem::AllocTool(INT32 nBytes, INT32 State, INT32 Align, const char * FileName, INT32 Line)
{
	s_MemoryCs.Enter(); 
	INT32 allocSz = nBytes;
	I3MEMBLOCK * pBlock;
	void * pRv; 

	I3ASSERT(nBytes != 0); 

	#if defined( I3_DEBUG)
		allocSz += sizeof(I3MEMBLOCK);
	#endif

	#if defined(I3_WINDOWS) || defined( I3_WINCE )

		#if defined( USE_WIN32_HEAP)
			pBlock = (I3MEMBLOCK *)::HeapAlloc( s_hHeap, 0, allocSz);
		#else
			#if defined( I3_DEBUG)
				pBlock = (I3MEMBLOCK *) _malloc_dbg( allocSz, _NORMAL_BLOCK, FileName, Line);
			#else
				pBlock = (I3MEMBLOCK *) malloc( allocSz);
			#endif
		#endif

		if( pBlock == NULL)
		{
			s_MemoryCs.Leave();
			return NULL;
		}

	#ifdef FILL_ZERO_HEAP_AND_PHYSIX_DATA
		i3mem::FillZero(pBlock, allocSz); 
	#else
		i3mem::Fill( pBlock, 0xCD, allocSz);
	#endif

	#else
		pBlock = (I3MEMBLOCK *) malloc( (size_t) allocSz);
	#endif

	#if defined( I3_DEBUG)
		s_AllocCount++;
		s_AllocSize += nBytes;
	#endif


	#if defined(I3_MEM_NET_DUMP)
		if(s_bNetDump)
		{
			MEM_NET_ALLOC_INFO SendBuffer; 
			SendBuffer._Size	= nBytes;
			SendBuffer._Address = (UINT32)pBlock; 			
			sprintf( SendBuffer._String, "%s - %d", FileName, Line); 

			DumpWriteFileBuffer( &SendBuffer ); 					
			//s_pDumpUdp->Send((char*)&SendBuffer, sizeof(MEM_NET_ALLOC_INFO), ::inet_addr("127.0.0.1"), MEM_DUMP_PORT);
		}
	#endif

	#if defined( I3_DEBUG)
		memset( pBlock, 0, sizeof(I3MEMBLOCK));

		pBlock->m_Prev = s_pAllocBlockHead;
		if( s_pAllocBlockHead != NULL)
			s_pAllocBlockHead->m_Next = pBlock;

		s_pAllocBlockHead = pBlock;

		pBlock->m_Size = nBytes;

		pBlock->m_pAllocPos = FileName;
		pBlock->m_Line = Line;

		pRv =  (void *) (((char *) pBlock) + sizeof(I3MEMBLOCK));
	#else
		pRv =  (void *) pBlock; 
	#endif

	s_MemoryCs.Leave(); 

	return pRv; 
}

/*-----------------------------------------------------------------------------
// Name: Free(void * pPointer)
// Desc: 
//----------------------------------------------------------------------------*/
void i3mem::FreeTool(void * pPointer, const char * pszFile, INT32 line ) 
{
	s_MemoryCs.Enter(); 
	I3ASSERT( pPointer != NULL);
	I3MEMBLOCK * pBlock;

#if defined( I3_DEBUG)
	pBlock = (I3MEMBLOCK *) (((char *) pPointer) - sizeof(I3MEMBLOCK));

	I3MEMBLOCK * pPrev, * pNext;

	pPrev = pBlock->m_Prev;
	pNext = pBlock->m_Next;

	if( pPrev != NULL)
		pPrev->m_Next = pNext;

	if( pNext != NULL)
		pNext->m_Prev = pPrev;

	if( s_pAllocBlockHead == pBlock)
		s_pAllocBlockHead = pPrev;

	s_AllocCount--;
	s_AllocSize -= pBlock->m_Size;
#else
	pBlock = (I3MEMBLOCK *) pPointer;
#endif

	#if defined( USE_WIN32_HEAP)
		HeapFree( s_hHeap, 0, pBlock);
	#else
		#if defined( I3_DEBUG)
			_free_dbg( pBlock, _NORMAL_BLOCK);
		#else
			free( pBlock);
		#endif
	#endif

#if defined(I3_MEM_NET_DUMP)
	if(s_bNetDump)
	{
		MEM_NET_ALLOC_INFO SendBuffer; 
		SendBuffer._Size	= 0;
		SendBuffer._Address = (UINT32)pPointer; 				
		sprintf( SendBuffer._String, "%s - %d", pszFile, line); 
		DumpWriteFileBuffer( &SendBuffer );
	}
#endif	

	s_MemoryCs.Leave(); 

	return;
}

/*-----------------------------------------------------------------------------
// Name: Realloc(void * pPointer, UINT32 nBytes, INT32 Align, const char * FileName, INT32 Line)
// Desc: 
//----------------------------------------------------------------------------*/
void * i3mem::ReallocTool(void * pPointer, INT32 nBytes, INT32 Align, const char * FileName, INT32 Line)
{	
	if(pPointer == NULL) return Alloc(nBytes, 0, Align, FileName, Line); 

	s_MemoryCs.Enter(); 
	void * pRv; 
	I3MEMBLOCK * pNewBlock = NULL; 	
	INT32 allocSz = nBytes;
	I3MEMBLOCK * pBlock;
#if defined( I3_DEBUG)
	I3MEMBLOCK * pNext, * pPrev;
#endif
	

#if defined( I3_DEBUG)
	allocSz += sizeof(I3MEMBLOCK);

	pBlock = (I3MEMBLOCK *) (((char *) pPointer) - sizeof(I3MEMBLOCK));
	pNext = pBlock->m_Next;
	pPrev = pBlock->m_Prev;

	s_AllocSize -= pBlock->m_Size;
	s_AllocSize += nBytes;
#else
	pBlock = (I3MEMBLOCK *) pPointer;
#endif

	#if defined( USE_WIN32_HEAP)
		pNewBlock = (I3MEMBLOCK *) HeapReAlloc( s_hHeap, 0, pBlock, allocSz);
	#else
		#if defined( I3_DEBUG)
			pNewBlock = (I3MEMBLOCK *) _realloc_dbg( pBlock, (size_t) allocSz, _NORMAL_BLOCK, FileName, Line);
		#else
			pNewBlock = (I3MEMBLOCK *) realloc( pBlock, (size_t) allocSz);
		#endif
	#endif

	I3ASSERT( pNewBlock != NULL);

#if defined( I3_DEBUG)
	if( pPrev != NULL)
		pPrev->m_Next = pNewBlock;

	if( pNext != NULL)
		pNext->m_Prev = pNewBlock;

	if( s_pAllocBlockHead == pBlock)
		s_pAllocBlockHead = pNewBlock;

	pRv = (void *)(((char *) pNewBlock) + sizeof(I3MEMBLOCK));
#else
	pRv = (void *) pNewBlock;
#endif

	s_MemoryCs.Leave(); 

	return pRv; 
}

void i3mem::CompactTool(void)
{
	s_MemoryCs.Enter(); 

	#if defined( I3_WINDOWS) && defined( USE_WIN32_HEAP)
		::HeapCompact( s_hHeap, 0);
	#endif

	s_MemoryCs.Leave(); 
}

INT32	i3mem::GetAllocCount(void)
{
#if defined( I3_DEBUG)
	return s_AllocCount;
#else
	return 0;
#endif
}

/*-----------------------------------------------------------------------------
// Name: UINT32 GetFreeMemorySize(void)
// Desc: Destroy Memory Manager
//----------------------------------------------------------------------------*/
UINT32 i3mem::GetAllocSize(void)
{
#if defined( I3_DEBUG)
	return s_AllocSize; 
#else
	return 0;
#endif
}
