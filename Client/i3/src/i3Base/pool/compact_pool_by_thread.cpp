#include "i3CommonType.h"

#include "i3Base/pool/compact_pool_by_thread.h"
#include "i3Base/pool/pool_base_impl.h"
#include "i3Base/pool/compact_pool.h"
#include "i3Base/pool/pool_base.h"
#include "i3Base/pool/align_up_down.h"

namespace i3
{
	namespace
	{
		static const int		commit_count = 4;
		
		struct cp_reserve_handle;

		struct cp_commit_handle
		{
			BYTE*				m_commit;
			cp_commit_handle*	m_commit_next;			// 프리리스트에만 쓴다..
			cp_reserve_handle*	m_reserve_handle;
		};

		struct cp_reserve_handle
		{
			BYTE*				m_start;
			cp_commit_handle*	m_freelist;
			intptr_t			m_ref_count;
			
			cp_reserve_handle*	m_reserve_next;
			cp_reserve_handle*	m_reserve_prev;

			cp_commit_handle	m_storage[ commit_count ];
		};
		
		mem_block_header*		cp_alloc(void*&		last_reserve_handle, void*& new_commit_handle, DWORD threadID)
		{
			cp_reserve_handle*  lrh =	(cp_reserve_handle*)last_reserve_handle;

			if (lrh == nullptr)
			{
				cp_reserve_handle* new_r = (cp_reserve_handle*) i3::tls_safe_malloc(threadID, sizeof(cp_reserve_handle) );
				new_r->m_start = (BYTE*)::VirtualAlloc(NULL, pool_block_size * commit_count, MEM_RESERVE, PAGE_NOACCESS);
				new_r->m_freelist = nullptr;
				new_r->m_ref_count = 1;
				new_r->m_reserve_next = nullptr; // lrh;		// 커버리티 이슈를 위한 주석처리 : lrh == nullptr이므로 코드 주석..
				new_r->m_reserve_prev = nullptr;
				
//				if (lrh != nullptr)							// 커버리티 이슈를 위한 주석처리 : lrh == nullptr이므로 코드 주석..
//					lrh->m_reserve_prev = new_r;

				for (size_t i = 0; i < commit_count - 1; ++i)
				{
					cp_commit_handle*  c = &new_r->m_storage[ commit_count - 1 - i];
					c->m_commit = new_r->m_start + pool_block_size * ( commit_count - 1 - i );
					
					c->m_commit_next = new_r->m_freelist;		new_r->m_freelist = c;
					c->m_reserve_handle = new_r;
				}
				
				cp_commit_handle* c = &new_r->m_storage[0];
				c->m_commit = new_r->m_start;
				::VirtualAlloc(c->m_commit, pool_block_size, MEM_COMMIT, PAGE_READWRITE);
				
				c->m_reserve_handle = new_r;
				last_reserve_handle = new_r;
				new_commit_handle   = c;

				return (mem_block_header*)c->m_commit;
			}
			
			cp_commit_handle*  c = lrh->m_freelist;
			lrh->m_freelist = lrh->m_freelist->m_commit_next;
			++lrh->m_ref_count;		
			::VirtualAlloc( c->m_commit, pool_block_size, MEM_COMMIT, PAGE_READWRITE);
			
			if (lrh->m_ref_count == commit_count )		// 프리리스트에 아무것도 없을 것임..완전이 떼내어, 연결을 제거한다.
			{
				last_reserve_handle = lrh->m_reserve_next;
				if (lrh->m_reserve_next)
					lrh->m_reserve_next->m_reserve_prev = nullptr;

				lrh->m_reserve_next = nullptr;		// 연결제거..반납이 될때 다시 연결이될것임..
			}

			c->m_commit_next = nullptr;			// 자유목록에서 떼었기때문에 링크를 제거..
			new_commit_handle = c;

			return (mem_block_header*)c->m_commit;			// mem_block헤더의 내용은 좀더 나중에 채운다..
		}
		
		void		cp_free(void*&		last_reserve_handle, mem_block_header* m)
		{
			cp_commit_handle*	c = (cp_commit_handle*)m->cp_commit_h;		// 널값이면 안된다..
			DWORD				threadID = m->threadID;						// 디커밋되기 전에 보존해야됨..
			
			::VirtualFree( c->m_commit, pool_block_size, MEM_DECOMMIT);
			
			cp_reserve_handle* r = c->m_reserve_handle;
			--r->m_ref_count;

			if ( r->m_ref_count > 0 )			// 이경우엔 프리리스트에 다시 재등록..
			{
				c->m_commit_next = r->m_freelist;
				r->m_freelist = c;				// 재활용율을 높이기 위해서는 앞으로 보내야한다..(슬라이싱처리)
				
				cp_reserve_handle* last_r = (cp_reserve_handle*)last_reserve_handle;

				if (last_r != r )
				{
					if (r->m_reserve_prev)
						r->m_reserve_prev->m_reserve_next = r->m_reserve_next;
					if (r->m_reserve_next)
						r->m_reserve_next->m_reserve_prev = r->m_reserve_prev;
					
					r->m_reserve_next = last_r;

					if (last_r)
						last_r->m_reserve_prev = r;

					r->m_reserve_prev = nullptr;
					last_reserve_handle = r;				// 리저브핸들을 앞으로 보냄..
				}				
			}
			else								// 이 경우엔 예약된것 까지 모두 날려버린다..
			{
				if (r->m_reserve_prev)
					r->m_reserve_prev->m_reserve_next = r->m_reserve_next;
				if (r->m_reserve_next)
					r->m_reserve_next->m_reserve_prev = r->m_reserve_prev;
				
				cp_reserve_handle* last_r = (cp_reserve_handle*)last_reserve_handle;

				if (last_r == r )
				{
					last_reserve_handle = last_r->m_reserve_next;
					if (last_r->m_reserve_next)
						last_r->m_reserve_next->m_reserve_prev = nullptr;
				}

				::VirtualFree( r->m_start, 0, MEM_RELEASE);
				i3::tls_safe_free( threadID, r);
			}
					
		
		}
		
		// 이건 어플리케이션 종료시에만 쓴다.
		void	cp_cleanup(mem_block_header* m)
		{
			cp_commit_handle*	c = (cp_commit_handle*)m->cp_commit_h;		// 널값이면 안된다..
			DWORD				threadID = m->threadID;						// 디커밋되기 전에 보존해야됨..
			
			::VirtualFree( c->m_commit, pool_block_size, MEM_DECOMMIT);

			cp_reserve_handle* r = c->m_reserve_handle;
			--r->m_ref_count;
			
			if (r->m_ref_count == 0)
			{
				::VirtualFree(r->m_start, 0, MEM_RELEASE);
				i3::tls_safe_free(threadID, r);
			}
		}

		i3::vector<mem_block_header*>	g_delayed_virtual_free_list;
		void				process_delayed_virtual_free();

		struct delayed_virtual_free_initializer 
		{
			delayed_virtual_free_initializer()
			{
				i3::delayed_atexit(process_delayed_virtual_free);
			}
		} _delayed_virtual_free_initializer_;

		void				add_delayed_virtual_free(mem_block_header* data)
		{
			g_delayed_virtual_free_list.push_back(data);
		}
		
		void				process_delayed_virtual_free()
		{
			const size_t num = g_delayed_virtual_free_list.size();
			for ( size_t i = 0 ; i < num ; ++i)
			{
			//	i3::virtual_free(g_delayed_virtual_free_list[i]);
				cp_cleanup(g_delayed_virtual_free_list[i]);
			}
			g_delayed_virtual_free_list.clear();
		}

	}

	namespace detail
	{

		void		process_restore_block_to_active_compact(mem_block_header*& full_block, mem_block_header*& active_block, mem_block_header* block )
		{
			if ( !detail::update_block_full_flag(block) )		// none.....(active는 존재할수 있음)
			{
				//				assert( block != active_block);
				//				assert( block->prev == nullptr && block->next == nullptr);
				// 블록을 full_block에서 떼야함..
				if ( block == full_block)
					full_block = block->next;
				if ( block->prev )
					block->prev->next = block->next;
				if (block->next)
					block->next->prev = block->prev;

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
					block->next = nullptr;
					block->prev = nullptr;
					active_block = block;
				}
			}
		}

		void		process_return_block_to_os(void*& reserve_handle, mem_block_header*& full_block, mem_block_header*& active_block, mem_block_header* block )
		{
			if ( block == active_block )
			{
				active_block =  block->prev;		// active_block제거시엔 prev로 이동, 혹은 prev널인 경우 없음...
			}
			else
			if ( block == full_block)
			{
				full_block = block->next;
			}

			if ( block->prev )
				block->prev->next = block->next;
			if (block->next)
				block->next->prev = block->prev;

		//	i3::virtual_free(block);
			cp_free(reserve_handle, block);
		}

		void	deallocate_same_thread_compact(void*& reserve_handle, mem_block_header*& full_block, mem_block_header*& active_block, mem_block_header* block, void* data)
		{
			free_object* free_obj = reinterpret_cast<free_object*>(data);
			free_obj->next = block->freeList;
			block->freeList = free_obj;
			--block->alloc_count;
			
			//
			// 되도록 메모리에서 빨리 소거되도록 조건 순서를 뒤집는다..(2013.05.28.수빈)
			//
			if (block->alloc_count == 0)	// 할당카운트가 0이면 바로 블록 소거.. 
				process_return_block_to_os(reserve_handle, full_block, active_block, block);
			else
			if ( block->is_full )		// 완전 밀려난 블록인 경우..(다 소요된 것....alloc_count가 만땅..)이 블록을 액티브로 돌린다...
				process_restore_block_to_active_compact(full_block, active_block, block);

//			else
//			if (block->alloc_count == 0)	// 할당카운트가 0이면 바로 블록 소거.. 
//				process_return_block_to_os(reserve_handle, full_block, active_block, block);
		}

	}


	compact_pool_by_thread::compact_pool_by_thread() : m_active_block(nullptr), m_full_block(nullptr),
		m_cp_reserve_handle(nullptr), m_free_block(nullptr)
	{

	}
	
	compact_pool_by_thread::~compact_pool_by_thread()
	{
		mem_block_header* prev_tmp = nullptr;
		for (mem_block_header* m = m_active_block; m != nullptr; m = prev_tmp )
		{
			prev_tmp = m->prev;
			add_delayed_virtual_free(m);
			//i3::virtual_free(m);
		}

		mem_block_header* next_tmp = nullptr;
		for (mem_block_header* m = m_full_block ; m != nullptr; m = next_tmp )
		{
			next_tmp = m->next;
			add_delayed_virtual_free(m);
			//i3::virtual_free(m);
		}

		i3::tls_aligned_free(shared_tls_type::get_thread_id(), m_free_block);
	}


	//inline
	void*	compact_pool_by_thread::allocate_from_new_block( DWORD threadID, mem_block_header*& active_block, size_t actual_size)
	{
		mem_block_header* block = get_new_block(threadID, actual_size);
		active_block = block;								// 항상 존재..	
		return detail::allocate_from_alloc_ptr(block);		// 항상 성공해야함..free_list가 없으므로 이 함수만...
	}


	void	compact_pool_by_thread::deallocate_same_thread(mem_block_header*& active_block, mem_block_header* block, void* data)
	{
		detail::deallocate_same_thread_compact(	m_cp_reserve_handle, m_full_block, active_block, block, data);
	}

	mem_block_header*	compact_pool_by_thread::get_new_block(DWORD threadID, size_t actual_size)
	{
	//	mem_block_header* result = static_cast<mem_block_header*>(i3::virtual_malloc(pool_block_size));		// 간단하게 16K생성..

		void* cp_commit_h = nullptr;
		mem_block_header* result = static_cast<mem_block_header*>(cp_alloc(m_cp_reserve_handle, cp_commit_h, threadID));

		memset(result, 0, sizeof(mem_block_header));			// 조금 작게 유지해도 될듯 한데..
		result->curr_alloc_ptr	= (void*) (uintptr_t(result) + pool_block_size - actual_size);		// 사이즈 하나를 내렸음!
		result->threadID		= threadID;
		result->obj_actual_size = actual_size;												
//		result->size_index		= static_cast<WORD>(size_index);			// size_index제거하고 가급적 실제 사이즈로 변경..
		result->cp_commit_h		= cp_commit_h;
		return result;		
	}

	void			compact_pool_by_thread::on_create_tls()
	{
		m_free_block = reinterpret_cast<tss_free_block_by_size*>(
			i3::tls_aligned_malloc(shared_tls_type::get_thread_id(), sizeof(tss_free_block_by_size), pool_block_header_alignment ));
		m_free_block->m_tss_free_block = nullptr;
	}

	void*			compact_pool_by_thread::allocate(compact_pool* owner)
	{
		void* result;
		if ( (result = detail::allocate_from_active_blocks_compact(m_full_block, m_active_block)) != nullptr)
			return result;

		if ( (result = detail::allocate_from_tss_blocks_compact(m_full_block, m_active_block, m_free_block)) != nullptr)
			return result;

		return compact_pool_by_thread::allocate_from_new_block(shared_tls_type::get_thread_id(), m_active_block, owner->actual_size() );
	}

	void			compact_pool_by_thread::deallocate( void* data)
	{
		mem_block_header* block = reinterpret_cast<mem_block_header*>(align_down( data, pool_block_size) );
		DWORD threadID = shared_tls_type::get_thread_id();

		if ( block->threadID == threadID )		// 동일 쓰레드..
			deallocate_same_thread( m_active_block, block, data);
		else
			get_raw_by_thread_id_unsafe( block->threadID )->deallocate_other_thread(block, data);
	}

	void		compact_pool_by_thread::deallocate_other_thread(mem_block_header* block, void* data)
	{
		detail::deallocate_other_thread( m_free_block, block, data );
	}



}

