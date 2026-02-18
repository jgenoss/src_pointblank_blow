#pragma once

namespace i3
{
	struct mem_block_header;
	struct tss_free_block_by_size;

	namespace detail
	{
		void*	allocate_from_alloc_ptr(mem_block_header* block);
		void*	allocate_from_block(mem_block_header* block);	


		I3_EXPORT_BASE void*	allocate_from_active_blocks(mem_block_header*& active_block);
		void*	allocate_from_active_blocks_compact(mem_block_header*& full_block, mem_block_header*& active_block);


		void	deallocate_same_thread(	mem_block_header*& free_block_list, mem_block_header*& active_block, mem_block_header* block, void* data);
	//	void	deallocate_same_thread_compact( mem_block_header*& full_block, mem_block_header*& active_block, mem_block_header* block, void* data);

		I3_EXPORT_BASE void*	allocate_from_tss_blocks(mem_block_header*& active_block, tss_free_block_by_size* tss_bucket);
		void*	allocate_from_tss_blocks_compact(mem_block_header*& full_block, mem_block_header*& active_block, tss_free_block_by_size* tss_bucket);

		I3_EXPORT_BASE void	deallocate_other_thread( tss_free_block_by_size* tss_free_block, mem_block_header* block, void* data );

		const float		block_recycle_ratio = 0.25f;		// 25%이상 프리된 경우, 액티브쪽으로 옮기겠다는 의미임..
		const UINT		block_full_threshold = UINT( float(pool_block_size - sizeof(mem_block_header)) * ( 1.f - block_recycle_ratio ) );

		inline
		bool		update_block_full_flag(mem_block_header* block)
		{
			block->is_full = ( block->alloc_count * block->obj_actual_size > block_full_threshold  ) ? 1 : 0;
			return block->is_full != 0;
		}
	}

}
