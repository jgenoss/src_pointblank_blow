#include "i3CommonType.h"

#include "pool/static_large_object_cache.h"
#include "pool/align_up_down.h"

namespace i3
{

	large_object_cache_base::large_object_cache_base() : m_private_heap( i3::heap_create() ) 
	{
	}

	large_object_cache_base::~large_object_cache_base() 
	{  
		i3::heap_destroy(m_private_heap);  
	}
	
	void*	large_object_cache_base::allocate_from_malloc(size_t alloc_size, size_t size_index)
	{
		void* raw_data	= private_malloc( alloc_size );
		NetworkDump_Malloc( raw_data, alloc_size, __FILE__,__LINE__);
		void* result    = reinterpret_cast<void*>(i3::align_up( uintptr_t(raw_data) + sizeof(large_object_header), align_large_object ));
		large_object_header* header = reinterpret_cast<large_object_header*>( uintptr_t(result) - sizeof(large_object_header));
		header->raw_data = raw_data;
		header->size_index = size_index;
		return result;		
	}
	
	void	large_object_cache_base::cleanup_stack(mem_stack_lock_free& mem_stack)
	{
		void* data;
		large_object_header* header;
		while ( ( data = mem_stack.pop() ) != nullptr)
		{
			header = reinterpret_cast<large_object_header*>(uintptr_t(data) - sizeof(large_object_header) );
			NetworkDump_Free(header->raw_data, __FILE__, __LINE__);
			private_free(header->raw_data);
		}
	}
	
	static_large_object_cache::static_large_object_cache() : m_clean_counter(0), m_index_counter_for_cleanup(0)
	{

	}

	static_large_object_cache::~static_large_object_cache()
	{
		for (size_t i = 0 ; i < num_large_object_bucket ; ++i )
			large_object_cache_base::cleanup_stack(m_stack_array[i]);
	}
	

	void	static_large_object_cache::cleanup_if_old(long curr_counter)
	{
		long idx = ::InterlockedIncrement(&m_index_counter_for_cleanup) - 1;	// 이쪽으로 들어오는 인덱스를 서로 상이하게..만듬..
		idx %= num_large_object_bucket;

		mem_stack_lock_free& stack = m_stack_array[idx];

		if ( !stack.empty() )
		{
			counter_cleanup& cleanup = m_cleanup_array[idx];						// 아마도 다른 값일것임...(같은 값이라도 상관없다)

			if (curr_counter > cleanup.threshold )
			{
				// 삭제처리...
				large_object_cache_base::cleanup_stack(stack);
				cleanup.last_clean_counter = curr_counter;
			}
		}
	}

	void*	static_large_object_cache::allocate(size_t size)	// 1보다 항상 커야함.. //
	{
		size_t actual_size = i3::align_up(size , bucket_step_large_object);
		size_t size_index = (actual_size - 1 ) >> bucket_step_large_object_shift;	// 나머지가 없음..

		void*  result = nullptr;

		if ( size_index < num_large_object_bucket )
		{
			long curr_counter = ::InterlockedIncrement(&m_clean_counter);

			if ( (result = m_stack_array[size_index].pop() ) != nullptr)
				return result;

			cleanup_if_old(curr_counter);
			/// threshold갱신...
			counter_cleanup& cleanup = m_cleanup_array[size_index];
			cleanup.threshold = curr_counter * 2 - cleanup.last_clean_counter ;
		}

		size_t alloc_size  = actual_size + sizeof(large_object_header) + align_large_object;
		return allocate_from_malloc(alloc_size, size_index);
	}

	void		static_large_object_cache::deallocate(void* data )
	{
		large_object_header* header = reinterpret_cast<large_object_header*>( uintptr_t(data) - sizeof(large_object_header) );
		size_t size_index = header->size_index;

		if ( size_index < num_large_object_bucket )
			m_stack_array[size_index].push(data);
		else
			private_free( header->raw_data );
	}
	
	
	




}