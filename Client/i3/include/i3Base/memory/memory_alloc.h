#pragma once

#include "calculate_adjusted_align.h"
#include "memory_aligned_apply.h"

namespace i3
{
//
//  아직, 디버그유틸까지 고려해서 작성하지 말것...(힙할당은 거의 대부분 라이브러리개발자의 몫임)
//	
	typedef		intptr_t			heap_handle;		// intptr_t는 CRT의 표준핸들타입이다..

//
//  대충 이정도로 간단하게만...(비정렬과 정렬함수를 통합후 자동관리하는 것도 생각했지만, free쪽 분리가 난해해서 포기했음)
//

//  프라이빗 힙..
	heap_handle	heap_create();
	heap_handle heap_create_lfh();
	heap_handle heap_create_nolock();
	void		heap_destroy(heap_handle h);

//  기존 힙..
	heap_handle get_process_heap();
	heap_handle get_crt_heap();						// 이것은 메인 어플 아닌 dll에서는 다를 수 있다는 것을 잊지 말것...

//	nolock힙에서 쓸만한 락루틴..
	void		heap_lock(heap_handle h);
	void		heap_unlock(heap_handle h);
//	
	void*		heap_malloc( heap_handle h, size_t	size);
	void		heap_free(heap_handle h, void* data);
	void*		heap_aligned_malloc( heap_handle h, size_t size, size_t align);
	void		heap_aligned_free(heap_handle h, void* data);

//  후에 virtual alloc류가 필요하다면, 여기에 추가할것...
	void*		virtual_malloc( size_t size );			// 64k로 정렬된 4k배수 메모리 확보.
	void		virtual_free(void* data);
}



#include "memory_alloc.inl"


