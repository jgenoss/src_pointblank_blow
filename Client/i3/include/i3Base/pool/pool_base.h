#pragma once

#include "enable_pool_dbg.h"
#include "pool_dbg_user_config.h"	// 안전을 위해 한번더 참조..
#include "pool_size_helper.h"

#include "../itl//noncopyable.h"
#include "../tls/tls_ptr.h"

#include "mem_block_header.h"

#include "maximum_object_size.h"

namespace i3
{
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
}

#include "pool_base_impl.h"

namespace i3
{
	class raw_memory_ctn;

	class I3_EXPORT_BASE pool_base : private noncopyable
	{
	protected:

		pool_base( size_t init_memory_size);
		~pool_base();

		void						clean_up(DWORD threadID);
		void*						allocate_from_new_block( DWORD threadID, mem_block_header*& active_block, size_t size_index, size_t actual_size);
		void						deallocate_same_thread(mem_block_header*& active_block, mem_block_header* block, void* data);
//		void						expand_memory_size_by_pow2(size_t pow2);

		void						malloc_raw_memory(DWORD threadID);

	private:
		mem_block_header*			get_raw_block(DWORD threadID);
		mem_block_header*			get_new_block(DWORD threadID, size_t size_index, size_t actual_size);

		mem_block_header*			m_free_block_list;						// 프리블록리스트..(반납된 블록들)
		raw_memory_ctn*				m_raw_memory_head;
	//	size_t						m_next_raw_memory_size;					// 1M으로 시작해서 2배씩..
		const size_t				m_raw_memory_size;
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