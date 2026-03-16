#pragma once


#include "large_object_cache_base.h"
#include "mem_stack_lock_free.h"
#include "../inst/ginst.h"

namespace i3
{

	// 해당 클래스들의 alignment를 확인해볼것..

#pragma warning(push)
#pragma warning(disable : 4324)

	class static_large_object_cache : public shared_ginst<static_large_object_cache, large_object_cache_base>
	{
	public:
		static_large_object_cache();
		~static_large_object_cache();
		void*					allocate( size_t size );
		void					deallocate(void* data );
	private:
		
		void					cleanup_if_old(long curr_counter);

		mem_stack_lock_free		m_stack_array[ num_large_object_bucket ];

		__declspec( align(64) )			// false sharing을 막기 위한 사이즈확장..
		struct counter_cleanup			// 스택당 하나씩 나란이 배치..
		{
			counter_cleanup() : last_clean_counter(0), threshold(0) {}
			volatile long	last_clean_counter;		// 갱신시점 이후에 curr_clean_counter값을 덮어씌움..
			volatile long	threshold;
			size_t			align_dummy[(64 -( sizeof(long)*2)) / sizeof(size_t)];
		};

		counter_cleanup		m_cleanup_array[ num_large_object_bucket ];
		volatile long		m_clean_counter;									// alloc과 dealloc모두에 적용시킴..
		volatile long		m_index_counter_for_cleanup;						// 클린업카운터..num_large_object_bucket만큼 회전시킴..(해당인덱스를 조사해 클린업처리)
		
	};

	template<> struct use_delayed_atexit_ginst< static_large_object_cache   > : true_type {};

#pragma warning(pop)
	
}
