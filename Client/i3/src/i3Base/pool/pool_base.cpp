#include "i3CommonType.h"
#include "pool/pool_base.h"

#include "pool/create_by_thread.h"
#include "pool/align_up_down.h"



namespace i3
{


	class raw_memory_ctn
	{
	public:
		raw_memory_ctn(size_t size_alloc, raw_memory_ctn* link) : 
		  virtual_memory(i3::virtual_malloc(size_alloc)), next(link){}
		~raw_memory_ctn() {	i3::virtual_free(virtual_memory);	}
		void*						get_virtual_memory() const { return virtual_memory; }	

		static raw_memory_ctn*		create(DWORD threadID, size_t size_alloc, raw_memory_ctn* link)
		{
			return i3::create_by_thread(threadID, identity<raw_memory_ctn>(), size_alloc, link);
		}
		static void					destroy_all(DWORD threadID, raw_memory_ctn* head)
		{
			if (!head) return;

			raw_memory_ctn* next = head->next;
			i3::delete_by_thread(threadID, head);
			raw_memory_ctn::destroy_all(threadID, next);
		}
	private:
		void*				virtual_memory;
		raw_memory_ctn*		next;
	};
	
	pool_base::pool_base( size_t init_memory_size) : m_raw_memory_size(init_memory_size), 
		m_free_block_list(nullptr), m_raw_memory_head(nullptr)
	{
	}

	pool_base::~pool_base()
	{
	}

	void	pool_base::clean_up(DWORD threadID)
	{
		raw_memory_ctn::destroy_all(threadID, m_raw_memory_head);
	}
	
	const UINT		blocks_per_list_node = 16;		// 256K가 됨..처음 4회..

	void				pool_base::malloc_raw_memory(DWORD threadID)
	{
		m_raw_memory_head = raw_memory_ctn::create(threadID, m_raw_memory_size, m_raw_memory_head);

		void*	big_block = m_raw_memory_head->get_virtual_memory();	// 최소한 첫주소만큼은 64K주소값을 보장..
		void*	big_block_end = (void*)( uintptr_t(big_block) + m_raw_memory_size );

		size_t  split_size = blocks_per_list_node * pool_block_size;		// 256K

		mem_block_header*	split_block = reinterpret_cast<mem_block_header*>(big_block);
		void*				split_end;

		while ( uintptr_t(split_block) + pool_block_size <= uintptr_t(big_block_end) )
		{
			split_end = (void*)( uintptr_t( split_block) + split_size );

			if ( split_end > big_block_end )
				split_end = align_down( big_block_end, pool_block_size);// 페이지는 4K이고, 풀은 16K라서 정렬이 필요할수 있음..
			
			split_block->curr_alloc_ptr = split_end;
			split_block->next = m_free_block_list;
			
			m_free_block_list = split_block;

			split_block = reinterpret_cast<mem_block_header*>(split_end);
		}

	//	m_next_raw_memory_size <<= 1;
	}

	mem_block_header*	pool_base::get_raw_block(DWORD threadID)
	{
		mem_block_header* big_block = m_free_block_list;

		if ( !big_block )
		{
			pool_base::malloc_raw_memory(threadID);
			big_block = m_free_block_list;
		}
		
		big_block->curr_alloc_ptr = (void*)( uintptr_t(big_block->curr_alloc_ptr) - pool_block_size );
		mem_block_header* result = reinterpret_cast<mem_block_header*>(big_block->curr_alloc_ptr);

		if (result == big_block )	// 이번으로 다 쓰는 것이므로 다음번엔 그다음 빅블록으로..
			m_free_block_list = big_block->next;

		return result;
	}

	mem_block_header*	pool_base::get_new_block(DWORD threadID, size_t size_index, size_t actual_size)
	{
		mem_block_header* result = get_raw_block(threadID);		// 항상 성공..
		
		memset(result, 0, sizeof(mem_block_header));			// 조금 작게 유지해도 될듯 한데..
//		result->uniqueID		= pool_id;
		result->curr_alloc_ptr	= (void*) (uintptr_t(result) + pool_block_size - actual_size);		// 사이즈 하나를 내렸음!
		result->threadID		= threadID;
		result->obj_actual_size = actual_size;												
		result->size_index		= static_cast<WORD>(size_index);

		return result;		
	}

	
}