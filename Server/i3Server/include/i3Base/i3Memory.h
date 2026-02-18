#ifndef __I3MEMORY_H
#define __I3MEMORY_H

//#include "i3CommonType.h"
#include "i3Debug.h"

/*------------------------------------------------------------------------------------------------*/
//외부에서 호출할때는 이부분에 있는것만 사용해주세요. 네임스페이스에 있는 함수들은 직접 사용하지 마세요. 
#if defined( I3_WINDOWS) || defined( I3_WINCE )
	#define i3MemAlloc(s)					i3mem::AllocTool( s, 0, I3MEMORY_MINIMUM_ALLOCSIZE, __FILE__, __LINE__)
	#define i3MemAlign(s, a)				i3mem::Alloc( s, 0, a, __FILE__, __LINE__)

	#define i3MemReAlloc(p, s)				i3mem::Realloc(p, s, I3MEMORY_MINIMUM_ALLOCSIZE, __FILE__, __LINE__) 	
	#define i3MemReAlign(p, s, a)			i3mem::Realloc(p, s, a, __FILE__, __LINE__)
#else
	#define i3MemAlloc(s)					i3mem::Alloc( s, 0, I3MEMORY_MINIMUM_ALLOCSIZE, __FILE__, __LINE__)
	#define i3MemAlign(s, a)				i3mem::Alloc( s, 0, a, __FILE__, __LINE__)

	#define i3MemReAlloc(p, s)				i3mem::Realloc(p, s, I3MEMORY_MINIMUM_ALLOCSIZE, __FILE__, __LINE__) 
	#define i3MemReAlign(p, s, a)			i3mem::Realloc(p, s, a, __FILE__, __LINE__)	
#endif

#define	i3MemFree( p )					i3mem::FreeTool(p, __FILE__, __LINE__)
#define i3MemDump( i )					i3mem::Dump(i)

/*------------------------------------------------------------------------------------------------*/
typedef char*	LPI3MEM;
#define I3MEMORY_MINIMUM_ALLOCSIZE		16
#define I3MEMORY_DEBUG_FILE				0X00000001
#define I3MEMORY_DEBUG_TEXT				0X00000002
#define I3MEMORY_DEBUG_SUM				0X00000004
#define I3MEMORY_DEBUG_EACH				0X00000008
#define I3_MEM_BUFFER_MAX_COUNT			1000000

#define ALIGNUP( nAddress, nBytes ) ( ( ( (UINT32) nAddress) + (nBytes)-1) & (~((nBytes)-1) ) ) 

struct I3MEMBLOCK
{
	LPI3MEM			m_pPointer;
	I3MEMBLOCK	*	m_Prev;
	I3MEMBLOCK	*	m_Next;
	INT32			m_Size;	
	INT32			m_StateAlign;			//Align (0x0000FFFF)& State (0xFFFF0000)

	#if defined( I3_DEBUG)
		const char *	m_pAllocPos;
		INT32		m_Line;
	#endif
};

typedef bool (*I3MEM_ENUM_CALLBACK)( I3MEMBLOCK * pBlock, void * pUserData);

struct I3MEMFILEHEADER
{
	char		m_FILE[4]; 
	UINT32		m_Ver; 
	LPI3MEM		m_pPointer[2]; 
	UINT32		m_AllocCount; 
	UINT32		m_FreeCount; 
	UINT32		m_Reserved[8]; 
}; 

struct I3MEMFILEBLOCKINFO
{
	LPI3MEM			m_MyPointer;
	I3MEMBLOCK		m_MemBlockInfo; 
}; 

#ifdef I3_DEBUG
#ifndef I3_WINCE
	#define I3_MEM_NET_DUMP	
#endif
#endif

#ifdef I3_MEM_NET_DUMP	
struct MEM_NET_ALLOC_INFO
{	
	UINT32	_Size;
	UINT32	_Address;  	
	char	_String[128];
};

struct FILE_MAPPING_HEADER
{
	UINT32 _size;
	UINT32 _idxWrite;
	UINT32 _idxRead;	
};

#endif

//////////////////////////////////////////////////////////////////////////////////////
namespace i3mem
{
	//======================================================================================//
	//외부 호출용.
	I3_EXPORT_BASE void	Create(INT32 MemorySizeInBytes, BOOL IsGame = TRUE, char * pMemoryName = NULL);
	I3_EXPORT_BASE void	Destroy(void);

	extern I3_EXPORT_BASE	void *	(*Alloc)(INT32 nBytes,	INT32 State, INT32 Align, const char * FileName ,INT32 Line);
	I3_EXPORT_BASE			void *	AllocGame(INT32 nBytes,	INT32 State, INT32 Align, const char * FileName, INT32 Line);
	I3_EXPORT_BASE			void *	AllocTool(INT32 nBytes,	INT32 State, INT32 Align, const char * FileName, INT32 Line);

	extern I3_EXPORT_BASE	void *	(*Realloc)(void * pPointer,	 INT32 nBytes,INT32 Align, const char * FileName, INT32 Line);
	I3_EXPORT_BASE			void *	ReallocGame(void * pPointer, INT32 nBytes,INT32 Align = -1,	const char * FileName = NULL, INT32 Line = 0);
	I3_EXPORT_BASE			void *	ReallocTool(void * pPointer, INT32 nBytes,INT32 Align = -1,	const char * FileName = NULL, INT32 Line = 0);

	extern I3_EXPORT_BASE	void (*Free)(void * p, const char * pszFile, INT32 line );
	I3_EXPORT_BASE			void FreeGame(void * p, const char * pszFile, INT32 line );
	I3_EXPORT_BASE			void FreeTool(void * p, const char * pszFile, INT32 line );
	
	
	I3_EXPORT_BASE void 	SetState(void *);
	I3_EXPORT_BASE INT32 	GetState(void *);

	//디버깅 용으로 사용합니다.
	I3_EXPORT_BASE UINT32	GetAllocSize(void);
	I3_EXPORT_BASE INT32	GetAllocCount(void);
	I3_EXPORT_BASE void		Dump(INT32 type = (I3MEMORY_DEBUG_TEXT | I3MEMORY_DEBUG_SUM) );
	I3_EXPORT_BASE void		Enum( I3MEM_ENUM_CALLBACK pUserProc, void * pUserData);

	//======================================================================================//
	//내부용 : 외부에서 호출하면 않됩니다. 
	extern I3_EXPORT_BASE	void (*Compact)(void);
	I3_EXPORT_BASE void		CompactGame(void); 
	I3_EXPORT_BASE void		CompactTool(void);

	//======================================================================================//
	//Inline copy, memset, Fill
	inline
	void	Copy( void * pDest, const void * pSrc, UINT32 sz)
	{
	#if defined( I3_WINDOWS ) || defined ( I3_WINCE)
			::CopyMemory( pDest, pSrc, sz);
	#else
			memcpy( pDest, pSrc, (size_t) sz);
	#endif
	}

	inline
	void	FillZero( void * pDest, UINT32 sz)
	{
	#if defined( I3_WINDOWS ) || defined ( I3_WINCE)
			::SecureZeroMemory( pDest, sz);
	#else
			memset( pDest, 0, sz);
	#endif
	}
	
	inline 
	void	Fill(void * pDest, INT32 Val, UINT32 sz)
	{
	#if defined( I3_WINDOWS ) || defined ( I3_WINCE)
			::memset( pDest, Val, sz);
	#else
			memset( pDest, Val, sz);
	#endif
	}

	//======================================================================================//
	// Debug 
	I3_EXPORT_BASE void	SetDumpEnable(BOOL bEnable); 

}

// i3 메모리 초기화
I3_EXPORT_BASE void i3MemoryInit( UINT32 nMB = 0 ,char * pMemoryName = NULL);

// NC 버전에서는 크래쉬가 되지 않게 위해 추가 - 박명진 2008. 10. 2.
#define FILL_ZERO_HEAP_AND_PHYSIX_DATA
// 언락 카운터 체크 중
//#define UNLOCK_LOG

#endif


