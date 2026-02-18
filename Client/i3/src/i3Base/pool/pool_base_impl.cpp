#include "i3CommonType.h"

#include "i3Base/pool/pool_base_impl.h"
#include "i3Base/pool/mem_block_header.h"
#include "i3Base/memory/memory_alloc.h"
#include "i3Base/pool/pool_base.h"
#include "pool/cas.h"

namespace i3
{
	namespace detail
	{
		
		void*		allocate_from_freelist(mem_block_header* block)
		{
			if (!block->freeList)
				return nullptr;
			free_object* result = block->freeList;
			block->freeList = result->next;
			++block->alloc_count;
			return result;
		}

		void*		allocate_from_alloc_ptr(mem_block_header* block)
		{
			if (!block->curr_alloc_ptr)
				return nullptr;

			void* result = block->curr_alloc_ptr;
			block->curr_alloc_ptr = (void*) ( uintptr_t(result) - block->obj_actual_size );
			if ( uintptr_t(block->curr_alloc_ptr) < uintptr_t(block) + sizeof(mem_block_header) )
				block->curr_alloc_ptr = nullptr;
			++block->alloc_count;

			return result;
		}

		void*		allocate_from_block(mem_block_header* block)
		{
			void*	result;
			if ( (result = allocate_from_freelist(block)) != nullptr)
				return result;
			if ( (result = allocate_from_alloc_ptr( block)) != nullptr)
				return result;
			block->is_full = 1;			// 프리리스트와 얼록포인터를 동시에 고려해서 체크..
			return nullptr;
		}
		
		mem_block_header*		move_active_block_front(mem_block_header*& active_block)
		{
			mem_block_header* block = active_block;
			mem_block_header* result = block->prev;			// 현재 액티브블록의 링크드리스트를 삭제하는게 더 좋을것 같다..
			block->prev = nullptr;

			if (result)	result->next = nullptr;				// 액티브 오른쪽을 무효화시킴..
			return 	active_block = result;	
		}
		
		mem_block_header*		move_active_block_front_compact( mem_block_header*& full_block, mem_block_header*& active_block)
		{
			mem_block_header* block = active_block;
			mem_block_header* result = block->prev;			// 현재 액티브블록의 링크드리스트를 삭제하는게 더 좋을것 같다..
			block->prev = nullptr;

			block->next = full_block;
			if (full_block)
				full_block->prev = block;
			full_block  = block;							// 기존 액티브블록을 full_block쪽으로 옮긴다..
			
			if (result)	result->next = nullptr;				// 액티브 오른쪽을 무효화시킴..
			return 	active_block = result;	
		}

		void*		allocate_from_active_blocks(mem_block_header*& active_block)
		{
			mem_block_header* block = active_block;

			if (block)
			{
				void*	result;
				if ( (result = detail::allocate_from_block(block)) != nullptr)
					return result;
				if ( (block = detail::move_active_block_front(active_block)) != nullptr)		// 이 조건을 만족하면 무조건 할당되야함..
					return  detail::allocate_from_block(block);
			}
			return nullptr;

		}

		void*		allocate_from_active_blocks_compact(mem_block_header*& full_block, mem_block_header*& active_block)
		{
			mem_block_header* block = active_block;

			if (block)
			{
				void*	result;
				if ( (result = detail::allocate_from_block(block)) != nullptr)
					return result;
				if ( (block = detail::move_active_block_front_compact(full_block, active_block)) != nullptr)		// 이 조건을 만족하면 무조건 할당되야함..
					return  detail::allocate_from_block(block);
			}
			return nullptr;
		}

		void		process_restore_block_to_active( mem_block_header*& active_block, mem_block_header* block )
		{
			if ( !detail::update_block_full_flag(block) )		// none.....(active는 존재할수 있음)
			{
				//				assert( block != active_block);
				//				assert( block->prev == nullptr && block->next == nullptr);
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
		
		mem_block_header*	allocate_from_tss_blocks_impl(tss_free_block_by_size* tss_bucket)
		{
			mem_block_header* block = tss_bucket->m_tss_free_block;		// 첫블록..

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
				if ( i3::cas((void* volatile*)&block->freeList_tss, head, nullptr) )
					break;
				head = block->freeList_tss;
			}

			//  최소한 1개이상 있음을 보증해야함..
			free_object* tail = head;
			--block->alloc_count;

			for ( ; tail->next ; tail = tail->next , --block->alloc_count ); // 카운트 세는 일이 어려우므로, 루프로 돌림..

			tail->next = block->freeList;
			block->freeList = head;
			
			return block;
		}
		
		void*	allocate_from_tss_blocks(mem_block_header*& active_block, tss_free_block_by_size* tss_bucket)
		{
			if ( !tss_bucket->m_tss_free_block ) 
				return nullptr;

			mem_block_header*	block = allocate_from_tss_blocks_impl(tss_bucket);
			
			if ( !detail::update_block_full_flag(block) )
				active_block = block;			// 액티브로 만들것...bucket->m_active_block은 널값일 것이다..

			// 이번 블록에 대해서 우선 freelist처리..
			return detail::allocate_from_freelist(block);			// 무조건 유효해야함..
		}

		void*	allocate_from_tss_blocks_compact(mem_block_header*& full_block, mem_block_header*& active_block, tss_free_block_by_size* tss_bucket)
		{
			if ( !tss_bucket->m_tss_free_block ) 
				return nullptr;

			mem_block_header*	block = allocate_from_tss_blocks_impl(tss_bucket);

			if ( !detail::update_block_full_flag(block) )
			{
				if ( block == full_block)
					full_block = block->next;
				if ( block->prev )
					block->prev->next = block->next;
				if (block->next)
					block->next->prev = block->prev;

				block->next = nullptr;
				block->prev = nullptr;
				active_block = block;			// 액티브로 만들것...bucket->m_active_block은 널값일 것이다..
			}

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