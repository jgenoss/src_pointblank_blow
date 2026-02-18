#include "i3SceneDef.h"
#include "i3PhysixMemManager.h"

static HANDLE	s_hHeap = nullptr;

//#define		__HEAP__
#define		__MALLOC__
//#define		__ALLOC__

void		_PHS_Init(void)
{
	#if defined( __HEAP__)
		s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, 0, 0); //32 * 1024 * 1024, 0);
		if( s_hHeap == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "메모리 관리자를 생성할 수 없습니다.");
			return;
		}

		{
			ULONG data = 2;
			HeapSetInformation( s_hHeap, HeapCompatibilityInformation, (PVOID) &data, sizeof(data));
		}
	#elif defined( __MALLOC__)
	#elif defined( __ALLOC__)
	#endif
}

void		_PHS_Close(void)
{
	#if defined( __HEAP__)
		if( s_hHeap != nullptr)
		{
			::HeapDestroy( s_hHeap);
			s_hHeap = nullptr;
		}
	#elif defined( __MALLOC__)
	#elif defined( __ALLOC__)
	#endif
}

void *		_PHS_Alloc( UINT32 sz)
{
	#if defined( __HEAP__)
		return HeapAlloc( s_hHeap, 0, sz);

	#elif defined( __MALLOC__)
		return malloc( sz);

	#elif defined( __ALLOC__)
		return LocalAlloc( LPTR, sz);
	#endif
}

void *		_PHS_Realloc( void * p, UINT32 sz)
{
	#if defined( __HEAP__)
		return HeapReAlloc( s_hHeap, 0, p, sz);

	#elif defined( __MALLOC__)
		return realloc( p, sz);

	#elif defined( __ALLOC__)
		return LocalReAlloc( p, sz, 0);
	#endif
}

void		_PHS_Free( void * p)
{
	#if defined( __HEAP__)
		HeapFree( s_hHeap, 0, p);

	#elif defined( __MALLOC__)
		free( p);

	#elif defined( __ALLOC__)
		LocalFree( p);
	#endif
   }
