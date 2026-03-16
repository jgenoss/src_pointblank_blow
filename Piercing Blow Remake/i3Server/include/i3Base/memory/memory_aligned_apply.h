#pragma once



namespace i3
{
///
	size_t	calculate_aligned_malloc_size(size_t size, size_t adjusted_align);		// 이것으로 malloc..
	void*	apply_aligned_malloc( void* raw_data, size_t adjusted_align);
	void*	get_raw_data_from_aligned_malloc( void* aligned_data);
///	
		
}

namespace i3
{

	__forceinline
	size_t	calculate_aligned_malloc_size(size_t size, size_t adjusted_align) { return sizeof(void*) + size + adjusted_align;  }
	
	__forceinline
	void*	apply_aligned_malloc(  void* raw_data, size_t adjusted_align)
	{
		uintptr_t result = ( uintptr_t(raw_data) + sizeof(void*) + adjusted_align - 1 ) & ~(adjusted_align - 1);
		((uintptr_t*)result)[-1] = (uintptr_t)raw_data;
		return (void*)result;
	}
	
	__forceinline
	void*	get_raw_data_from_aligned_malloc( void* aligned_data)
	{
		uintptr_t ptr = (uintptr_t) aligned_data;
		ptr = ( ptr & ~( sizeof(void*) -1 ) ) - sizeof(void*);
		return (void*) *((uintptr_t*)ptr);
	}

}
