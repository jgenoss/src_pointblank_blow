#pragma once

#include "enable_pool_dbg.h"
#include "pool_dbg_user_config.h"	// 안전을 위해 한번더 참조..
#include "pool_size_helper.h"

#include "../itl//noncopyable.h"
#include "../tls/tls_ptr.h"

#include "mem_block_header.h"

namespace i3
{

	namespace detail
	{
		template<size_t object_per_block, size_t size_index, bool match>
		struct static_calculate_maximum_object_size_idx_helper
		{
			static const size_t value = size_index;			
		};
		
		template<size_t object_per_block, size_t size_index>
		struct static_calculate_maximum_object_size_idx_helper<object_per_block, size_index, false>
		{
		private:
			static const size_t object_total_size = 
				static_convert_size_idx_to_actual_size<size_index - 1>::value * object_per_block;

		public:
			static const size_t value = 
				static_calculate_maximum_object_size_idx_helper
				<
					object_per_block, size_index - 1, 
					( object_total_size < pool_block_size - sizeof(mem_block_header) ) 
				>::value;
		};
	}

	
	
	
	template<size_t object_per_block>
	struct static_calculate_maximum_object_size
	{
	private:
		static const size_t result_temp = ( (pool_block_size - sizeof(mem_block_header) ) / object_per_block ) & ~(128 - 1 );
		static const size_t index_temp = static_convert_size_to_size_idx<result_temp>::value;
		static const size_t actual_total_size_temp = static_convert_size_idx_to_actual_size<index_temp>::value * object_per_block;

		static const size_t size_index = 
			detail::static_calculate_maximum_object_size_idx_helper
			<
				object_per_block, index_temp, ( actual_total_size_temp < pool_block_size - sizeof(mem_block_header) )
			>::value;
	public:
		static const size_t value = static_convert_size_idx_to_actual_size<size_index>::value;		
	};

	const size_t	maximum_object_size = static_calculate_maximum_object_size<1>::value;	// 1개라도 적당하면 무조건 넣도록 한다..
				//static_calculate_maximum_object_size<2>::value;

//	const size_t	maximum_object_size = ( (pool_block_size - sizeof(mem_block_header) ) /2) & ~(128 - 1);	// 8064바이트..



	struct pool_by_size
	{
		mem_block_header*		m_active_block;			// 현재 할당 블록..
	};	

	// 64바이트 캐시라인에 맞출것..
	struct tss_free_block_by_size						// align은 사전에 맞출것..
	{
		mem_block_header*		m_tss_free_block;		// 타쓰레드에서 반납되면, 일단 링크...
		size_t					padding[( pool_block_header_alignment - sizeof(void*) ) / sizeof(size_t)];
	};	

	class raw_memory_ctn;

	class I3_EXPORT_BASE pool_base : private noncopyable
	{
	protected:

		pool_base( size_t init_memory_size);
		~pool_base();

		void						clean_up(DWORD threadID);
		void*						allocate_from_new_block( DWORD threadID, mem_block_header*& active_block, size_t size_index, size_t actual_size);
		void						deallocate_same_thread(mem_block_header*& active_block, mem_block_header* block, void* data);
		void						expand_memory_size_by_pow2(size_t pow2);

		void						malloc_raw_memory(DWORD threadID);

	private:
		mem_block_header*			get_raw_block(DWORD threadID);
		mem_block_header*			get_new_block(DWORD threadID, size_t size_index, size_t actual_size);

		mem_block_header*			m_free_block_list;						// 프리블록리스트..(반납된 블록들)
		raw_memory_ctn*				m_raw_memory_head;
		size_t						m_next_raw_memory_size;					// 1M으로 시작해서 2배씩..
	};

	template<class Derived>
	class pool_tls_base :	protected pool_base, public i3::shared_tls<Derived>
	{
	protected:	
		typedef i3::shared_tls<Derived>				shared_tls_type;

		pool_tls_base(size_t init_memory_size);
		~pool_tls_base();
	};
	
		
	
	
}

#include "pool_base.inl"