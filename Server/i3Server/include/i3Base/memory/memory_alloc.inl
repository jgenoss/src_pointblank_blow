
#pragma push_macro("HeapCreate")
#undef HeapCreate

#pragma push_macro("HeapDestroy")
#undef HeapDestroy

#pragma push_macro("HeapAlloc")
#undef HeapAlloc

#pragma push_macro("HeapFree")
#undef HeapFree 

#pragma push_macro("VirtualAlloc")
#undef VirtualAlloc

#pragma push_macro("VirtualFree")
#undef VirtualFree


namespace i3
{
	//
	// 참고사항 : intel tbb 역시 windows tls api를 막쓰고 있음..( windows TLS함수의 퍼포먼스를 더 이상 의심하지 말것..)
	//			  intel tbb의 성능 > lfh 성능의 2배이다...
	//
	//  프라이빗 힙..

	__forceinline
	heap_handle	heap_create()
	{
		return heap_handle(::HeapCreate(0, 0, 0));
	}

	__forceinline
	heap_handle heap_create_lfh()					// 프로세스힙,프라이빗힙에서는 켜두는게 좋지만..
	{												// tls용 힙으로는 적절치 않음..(LFH는 nolock을 지원하지 않음)
		HANDLE h = ::HeapCreate(0,0,0);
		ULONG HeapType = 2;
		::HeapSetInformation(h, HeapCompatibilityInformation, &HeapType, sizeof(HeapType));
		return heap_handle(h);
	}

	__forceinline
	heap_handle heap_create_nolock()
	{
		return heap_handle(::HeapCreate(HEAP_NO_SERIALIZE, 0, 0) );
	}

	__forceinline
	void		heap_destroy(heap_handle h)
	{
		::HeapDestroy( HANDLE(h));
	}

	//  기존 힙..

	__forceinline
	heap_handle get_process_heap()
	{
		return heap_handle(::GetProcessHeap());
	}

	__forceinline
	heap_handle get_crt_heap()						// 이것은 메인 어플 아닌 dll에서는 다를 수 있다는 것을 잊지 말것...
	{
		return heap_handle(::_get_heap_handle());
	}

	//	nolock힙에서 쓸만한 락루틴..
	__forceinline
	void		heap_lock(heap_handle h)
	{
		::HeapLock(HANDLE(h));
	}

	__forceinline
	void		heap_unlock(heap_handle h)
	{
		::HeapUnlock(HANDLE(h));
	}
	//	

	__forceinline
	void*		heap_malloc( heap_handle h, size_t	size)
	{
		return ::HeapAlloc(HANDLE(h), 0, size);
	}

	__forceinline
	void		heap_free(heap_handle h, void* data)
	{
		if (!data) return;
		::HeapFree(HANDLE(h), 0, data);		// 널검사정의가 없다..
	}

	__forceinline
	void*		heap_aligned_malloc( heap_handle h, size_t size, size_t align)
	{
		align = calculate_adjusted_align(align);
		void* data = ::HeapAlloc(HANDLE(h), 0, calculate_aligned_malloc_size(size, align) );
		return apply_aligned_malloc(data, align);
	}

	__forceinline
	void		heap_aligned_free(heap_handle h, void* data)
	{
		if (!data) return;
		::HeapFree(HANDLE(h), 0, get_raw_data_from_aligned_malloc(data) );
	}
	
	__forceinline
	void*		virtual_malloc( size_t size )			// 64k로 정렬된 4k배수 메모리 확보.
	{
		return ::VirtualAlloc(NULL, size, (MEM_RESERVE | MEM_COMMIT | MEM_TOP_DOWN), PAGE_READWRITE);
	}

	__forceinline
	void		virtual_free(void* data)
	{
		if (!data) return;
		::VirtualFree(data, 0, MEM_RELEASE);
	}
	
		
}

#pragma pop_macro("VirtualFree")
#pragma pop_macro("VirtualAlloc")
#pragma pop_macro("HeapFree")
#pragma pop_macro("HeapAlloc")
#pragma pop_macro("HeapDestroy")
#pragma pop_macro("HeapCreate")

