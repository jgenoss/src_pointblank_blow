#pragma once

// 기존에 있던 object pool이 OS나 common_pool쪽으로의 반납처리가 없어서 메모리고갈 원인이 될수 있어 모듈을 바꿉니다..
#include "i3Base/tls/tls_ptr.h"

namespace i3
{
	class compact_pool;
	struct mem_block_header;
	struct tss_free_block_by_size;

	class I3_EXPORT_BASE compact_pool_by_thread : public i3::shared_tls< compact_pool_by_thread >
	{
	public:
	
		typedef i3::shared_tls< compact_pool_by_thread > shared_tls_type;

		compact_pool_by_thread();
		~compact_pool_by_thread();

		void*					allocate(compact_pool* owner);			// owner안에 size값존재..
		void					deallocate( void* data );
		void					on_create_tls();

	private:

		void*						allocate_from_new_block( DWORD threadID, mem_block_header*& active_block, size_t actual_size);
		void						deallocate_same_thread(mem_block_header*& active_block, mem_block_header* block, void* data);
		mem_block_header*			get_new_block(DWORD threadID, size_t actual_size);

		void						deallocate_other_thread(mem_block_header* block, void* data);

		mem_block_header*			m_active_block;		// 활성블록...
		mem_block_header*			m_full_block;		// 꽉차서 분리된 블록...

		void*						m_cp_reserve_handle;	// VirtualAlloc 분할을 위한 핸들임...
		tss_free_block_by_size*		m_free_block;		// tss프리블록..
	};
		
	
}

