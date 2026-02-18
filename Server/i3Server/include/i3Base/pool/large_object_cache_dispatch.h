#pragma once

#include "large_object_cache_base.h"
#include "mem_stack_lock_free.h"
#include "align_up_down.h"
#include "static_adjust_size_for_pool.h"

namespace i3
{
	namespace detail
	{
					
		template<std::size_t AllocSize, std::size_t SizeIndex, bool larger_than_bucket_size >
		class local_large_object_cache_dispatch : public large_object_cache_base
		{
		public:
			void*	allocate();
			void	deallocate(void* data);
		};

		//
		//  로컬 풀에서 메모리 반납 여부 --> 우선 heap에 반납하지않도록 가정..
		//

		template<std::size_t AllocSize, std::size_t SizeIndex>
		class local_large_object_cache_dispatch<AllocSize, SizeIndex, false> : public large_object_cache_base
		{
		public:
			local_large_object_cache_dispatch();
			~local_large_object_cache_dispatch();
			void*					allocate();
			void					deallocate(void* data );
		private:
			mem_stack_lock_free		m_stack;
		};
		
		template<std::size_t ObjSize>
		struct select_large_object_cache_dispatch_with_object_size
		{
			static const size_t object_actual_size = i3::static_align_up<ObjSize, bucket_step_large_object>::value; 

			static const size_t object_alloc_size = object_actual_size + sizeof(large_object_header) + align_large_object;
			static const size_t object_size_index  = (object_actual_size - 1) >> bucket_step_large_object_shift;
			static const bool	larger_than_bucket_size   = (object_size_index >= num_large_object_bucket);
			
			typedef detail::local_large_object_cache_dispatch<object_alloc_size, object_size_index, larger_than_bucket_size> type;
		};

		template<class T>
		struct select_large_object_cache_dispatch : select_large_object_cache_dispatch_with_object_size<static_adjust_size_for_pool<T>::value> {};
	
	}

}
