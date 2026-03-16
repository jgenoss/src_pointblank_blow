#pragma once

#include "pool_base.h"

namespace i3
{
	
	class I3_EXPORT_BASE pool_by_thread : public pool_tls_base<pool_by_thread>
	{
	public:
		pool_by_thread();
		~pool_by_thread();

		void*					allocate( size_t size, size_t align);			
		void					deallocate( void* data);
		void					on_create_tls();

	private:
		void					deallocate_other_thread(mem_block_header* block, void* data);

		pool_by_size*				m_ap_pool_by_size;					// 이 2개를 완전 분리..
		tss_free_block_by_size*		m_ap_free_block_by_size;			//
	};
		
		
}