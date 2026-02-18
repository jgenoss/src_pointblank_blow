#include "i3CommonType.h"
#include "pool/cache_pool_impl.h"

#include "pool/static_pool.h" //  "dbg_util_aligned_apply.h" 포함..
#include "pool/cas.h"
#include "pool/storage_pool_mt.h"


#pragma push_macro("static_pool_malloc")
#undef static_pool_malloc

#pragma push_macro("static_pool_free")
#undef static_pool_free



namespace i3
{
	namespace detail
	{
		
		cache_pool_st_impl::cache_pool_st_impl(std::size_t size, std::size_t capacity) : m_freelist(0), m_capacity(capacity) 
		{    
			init_freelist(size);
		}
		
		cache_pool_st_impl::cache_pool_st_impl() : m_freelist(0), m_capacity(0)
		{
			
		}

		cache_pool_st_impl::~cache_pool_st_impl() 
		{	
			free_object* del;

			for ( free_object* f = m_freelist ; f != 0 ; ) 
			{
				del = f;
				f = f->next;
				static_pool_free(del);	
			}
		}
		
		void	cache_pool_st_impl::init_freelist(std::size_t size)
		{
			free_object* new_obj  = 0;	free_object* last_obj = 0;

			const std::size_t num_alloc = m_capacity;
			for (std::size_t i = 0 ; i < num_alloc ; ++i )
			{
				new_obj = static_cast<free_object*>( static_pool_malloc(size) );
				new_obj->next = last_obj;
				last_obj = new_obj;
			}

			m_freelist = last_obj;
		}

		void*	cache_pool_st_impl::allocate_from_static_pool( std::size_t size)
		{
			m_capacity = m_capacity + m_capacity / 2 + 1;		// 50%증가식..
			init_freelist(size);
			return static_pool_malloc(size);				// 별도로 할당해 리턴..
		}


		void		cache_pool_mt_impl::tls_data::clean_up_freelist()
		{
			free_object* del;
			for (free_object* f = freelist_tss ; f != 0 ; ) 
			{
				del = f;
				f = f->next;
				static_pool_free(del);
			}
			
			freelist_tss = 0;

			for (free_object* f = freelist ; f != 0 ; )
			{
				del = f;
				f = f->next;
				static_pool_free(del);
			}

			freelist = 0;
		}

		cache_pool_mt_impl::tls_data::~tls_data()
		{
			clean_up_freelist();
		}
	
		// 이 함수는 해당 풀이 사용되지 않을때만 안전하다..
/*
		void		cache_pool_mt_impl::clean_up_freelist()
		{
			const DWORD num = i3::get_num_tls_thread_id();

			for (DWORD i = 0 ; i < num ; ++i)
			{
				detail_tls::thread_data* td = detail_tls::get_thread_data(i);

				if ( m_tls_pool.slot_idx() < td->slotlist.size()  )
				{
					tls_data* data = reinterpret_cast<tls_data*>(td->slotlist[m_tls_pool.slot_idx()]);				
					if (data)
						data->clean_up_freelist();
				}
			}
		}
*/
		void*		cache_pool_mt_impl::allocate_from_static_pool( std::size_t size)
		{
			tls_data&		td = *m_tls_pool;
			
			if (td.capacity > 0)
				td.capacity = td.capacity + td.capacity / 2 + 1;		// 50%증가식..
			else
				td.capacity = m_init_capacity;		
			
			const std::size_t num_alloc = td.capacity;
			
			free_object* new_obj  = 0;	free_object* last_obj = 0;

			for (std::size_t i = 0 ; i < num_alloc ; ++i )
			{
				new_obj = static_cast<free_object*>( static_pool_malloc(size) );
				new_obj->next = last_obj;
				last_obj = new_obj;
			}
			
			td.freelist = last_obj;

			return static_pool_malloc(size);
		}
	
		void*		cache_pool_mt_impl::allocate_from_freelist_tss(tls_data* td)
		{
			// 락프리구조를 가져야함..
			if (!td->freelist_tss) return 0;		// 없는거지...
			
			free_object* head = td->freelist_tss;
		
			for ( ; ; )
			{
				if ( i3::cas((void* volatile*)&td->freelist_tss, head, nullptr) )				// 통과하면, freeList_tss는 리셋되므로, deallocate로 인한 head변경의 연결걱정은 끝임..
					break;
				head = td->freelist_tss;
			}
			
			// tls를 쓰기 때문에 allocate의 레이스컨디션은 존재하지 않음(이 위치에서 head는 반드시 존재함...)
			free_object* tail = head;
			for ( ; tail->next ; tail = tail->next  );	// 카운트 세는 일이 어려우므로, 루프로 돌림..마지막값을 저장하는 방식도 생각할수 있지만..
														// 그렇게 되면, 마지막값에 대한 CAS검사가 들어가게 되거나, CAS2를 동원해야함.....
			tail->next = td->freelist;
			td->freelist = head->next;

			return head;
		}
	
		void		cache_pool_mt_impl::deallocate_to_freelist_tss(tls_data* td,  void* data)
		{
			// 블록에 해당되는 쓰레드 아이디객체에서 처리.. (락 프리가 요구된다..)
			// 타쓰레드 실행이므로, 할당 카운트 계산이 어렵다...(사실상 불가능...)
			free_object* free_obj = reinterpret_cast<free_object*>(data);

			for ( ; ; )
			{
				free_obj->next = td->freelist_tss;
				if ( i3::cas((void* volatile*)&td->freelist_tss, free_obj->next, free_obj) )
					break;
			}
		}

	
		
		
		
	}

	storage_pool_mt::storage_pool_mt(std::size_t num_reserve) : base_type( num_reserve ) 
	{

	}

	storage_pool_mt::storage_pool_mt() : base_type() 
	{

	}


}


#pragma pop_macro("static_pool_free")
#pragma pop_macro("static_pool_malloc")
