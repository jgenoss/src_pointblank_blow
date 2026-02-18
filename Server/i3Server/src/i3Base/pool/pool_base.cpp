#include "i3CommonType.h"
#include "pool/pool_base.h"

#include "pool/create_by_thread.h"
#include "pool/cas.h"

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
	
	pool_base::pool_base( size_t init_memory_size) : m_next_raw_memory_size(init_memory_size), 
		m_free_block_list(NULL), m_raw_memory_head(NULL)
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
		m_raw_memory_head = raw_memory_ctn::create(threadID, m_next_raw_memory_size, m_raw_memory_head);

		void*	big_block = m_raw_memory_head->get_virtual_memory();	// 최소한 첫주소만큼은 64K주소값을 보장..
		void*	big_block_end = (void*)( uintptr_t(big_block) + m_next_raw_memory_size );

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

		m_next_raw_memory_size <<= 1;
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

	namespace detail
	{
		
		void*		allocate_from_freelist(mem_block_header* block)
		{
			if (!block->freeList)
				return NULL;
			free_object* result = block->freeList;
			block->freeList = result->next;
			++block->alloc_count;
			return result;
		}

		void*		allocate_from_alloc_ptr(mem_block_header* block)
		{
			if (!block->curr_alloc_ptr)
				return NULL;

			void* result = block->curr_alloc_ptr;
			block->curr_alloc_ptr = (void*) ( uintptr_t(result) - block->obj_actual_size );
			if ( uintptr_t(block->curr_alloc_ptr) < uintptr_t(block) + sizeof(mem_block_header) )
				block->curr_alloc_ptr = NULL;
			++block->alloc_count;

			return result;
		}
		
		void*		allocate_from_block(mem_block_header* block)
		{
			void*	result;
			if ( (result = allocate_from_freelist(block)) != NULL ) 
				return result;
			if ( (result = allocate_from_alloc_ptr( block)) != NULL )
				return result;
			block->is_full = 1;			// 프리리스트와 얼록포인터를 동시에 고려해서 체크..
			return NULL;		
		}
		
		inline
		mem_block_header*		move_active_block_front(mem_block_header*& active_block)
		{
			mem_block_header* block = active_block;
			mem_block_header* result = block->prev;				// 현재 액티브블록의 링크드리스트를 삭제하는게 더 좋을것 같다..
			block->prev = NULL;	
			//block->next = NULL;		

			if (result)	result->next = NULL;				// 이것이 중요..(이번에 빠진듯 함!! 중대버그..2010/07/26)// 액티브 오른쪽을 무효화시킴..
			return 	active_block = result;	
		}

		void*					allocate_from_active_blocks(mem_block_header*& active_block)
		{
			mem_block_header* block = active_block;
			
			if (block)
			{
				void*	result;
				if ( (result = detail::allocate_from_block(block)) != NULL )
					return result;
				if ( (block = detail::move_active_block_front(active_block)) != NULL )		// 이 조건을 만족하면 무조건 할당되야함..
					return  detail::allocate_from_block(block);
			}
			return NULL;

		}

		const float		block_recycle_ratio = 0.25f;		// 25%이상 프리된 경우, 액티브쪽으로 옮기겠다는 의미임..
		const float		block_full_threshold = (pool_block_size - sizeof(mem_block_header)) * ( 1 - block_recycle_ratio );

		inline
		bool		update_block_full_flag(mem_block_header* block)
		{
			block->is_full = ( block->alloc_count * block->obj_actual_size > block_full_threshold  ) ? 1 : 0;
			return block->is_full != 0;
		}

		void		process_restore_block_to_active( mem_block_header*& active_block, mem_block_header* block )
		{
			if ( !detail::update_block_full_flag(block) )		// none.....(active는 존재할수 있음)
			{
//				assert( block != active_block);
//				assert( block->prev == NULL && block->next == NULL);

				if ( active_block )		// active가 있으면 그 앞에 놓고...
				{
					block->next = active_block;			// 이것이 빠져서 잠시 에러였었다...
					block->prev = active_block->prev;
					active_block->prev = block;
					if ( block->prev)
						block->prev->next = block;
				}
				else								// 없다면 이것이 active임...(tls에서조금씩 alloc될때 이럴수 있다..)
				{
					active_block = block;
				}
			}
		}

		void		process_return_block_to_free_block_list(mem_block_header*& free_block_list, mem_block_header*& active_block, mem_block_header* block )
		{
			if ( block == active_block )
			{
//				assert(block != block->prev);
				active_block =  block->prev;		// active_block제거시엔 prev로 이동, 혹은 prev널인 경우 없음...
			}
			
			if ( block->prev )
				block->prev->next = block->next;
			if (block->next)
				block->next->prev = block->prev;

			// 아래 2개를 제외한 나머지는 무효한 값임..
			block->curr_alloc_ptr = (void*)( uintptr_t(block) + pool_block_size );
			block->next = free_block_list;
			//
			free_block_list = block;		//	프리블록리스트에 등록..
		}
		
		void	deallocate_same_thread(	mem_block_header*& free_block_list, mem_block_header*& active_block, mem_block_header* block, void* data)
		{
			free_object* free_obj = reinterpret_cast<free_object*>(data);
			free_obj->next = block->freeList;
			block->freeList = free_obj;
			--block->alloc_count;

			if ( block->is_full )		// 완전 밀려난 블록인 경우..(다 소요된 것....alloc_count가 만땅..)이 블록을 액티브로 돌린다...
				process_restore_block_to_active(active_block, block);
			else
			if (block->alloc_count == 0)	// 할당카운트가 0이면 바로 블록 소거.. (이부분은 기존tbb와 확실이 다름) 재할당을 하지 않기때문에 빠른 처리가 필요하다..
				process_return_block_to_free_block_list(free_block_list, active_block, block);
		}
		
		void*	allocate_from_tss_blocks(mem_block_header*& active_block, tss_free_block_by_size* tss_bucket)
		{
			mem_block_header* block = tss_bucket->m_tss_free_block;		// 첫블록..
			
			if (!block)
				return NULL;
		
			for ( ; ; )
			{
				if ( i3::cas((void* volatile*)&tss_bucket->m_tss_free_block, block, block->tss_free_block_next))
					break;
				block = tss_bucket->m_tss_free_block;
			}

			// 이 블록이 분리되었음...(어딘가에 위치시켜야한다...)
			// 블록 내에 tss프리리스트 역시 락프리하게....프리리스트로 옮겨져야함...
			free_object* head = block->freeList_tss;

			for ( ; ; )
			{
				if ( i3::cas((void* volatile*)&block->freeList_tss, head, NULL) )
					break;
				head = block->freeList_tss;
			}

			//  최소한 1개이상 있음을 보증해야함..
			free_object* tail = head;
			--block->alloc_count;

			for ( ; tail->next ; tail = tail->next , --block->alloc_count ); // 카운트 세는 일이 어려우므로, 루프로 돌림..

			tail->next = block->freeList;
			block->freeList = head;

			if ( !detail::update_block_full_flag(block) )
				active_block = block;			// 액티브로 만들것...bucket->m_active_block은 널값일 것이다..

			// 이번 블록에 대해서 우선 freelist처리..
			return detail::allocate_from_freelist(block);			// 무조건 유효해야함..
		}

		void	deallocate_other_thread( tss_free_block_by_size* tss_free_block, mem_block_header* block, void* data )
		{
			// 블록에 해당되는 쓰레드 아이디객체에서 처리.. (락 프리가 요구된다..)
			// 타쓰레드 실행이므로, 할당 카운트 계산이 어렵다...(사실상 불가능...)
			free_object* free_obj = reinterpret_cast<free_object*>(data);
			for ( ; ; )
			{
				free_obj->next = block->freeList_tss;
				if ( i3::cas((void* volatile*)&block->freeList_tss, free_obj->next, free_obj) )
					break;
			}

			//
			// 최초로, tss리스트가 채워진 경우.. tss_free_block_next값과 bucket값에 변경이 가해져야함..
			if ( !free_obj->next )		// 애초에, freeList_tss가 널이었는데, 최초로 채워진 경우...
			{
				for ( ; ; )
				{
					block->tss_free_block_next = tss_free_block->m_tss_free_block;
					if ( i3::cas((void* volatile*)&tss_free_block->m_tss_free_block, block->tss_free_block_next, block) )
						break;	
				}
			}

		}

	}
	
}