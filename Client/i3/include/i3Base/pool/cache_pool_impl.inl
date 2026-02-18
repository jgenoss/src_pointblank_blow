
namespace i3
{
	namespace detail
	{
		inline void*	cache_pool_st_impl::allocate_impl()
		{
			if (!m_freelist) return 0;
			free_object* result = m_freelist;
			m_freelist = result->next;
			return result;
		}
		
		inline void		cache_pool_st_impl::deallocate_impl(void* data)
		{
			free_object* free_obj = reinterpret_cast<free_object*>(data);
			free_obj->next = m_freelist;
			m_freelist = free_obj;
		}

		inline void*	cache_pool_mt_impl::allocate_impl()
		{
			tls_data& td = *m_tls_pool;
			void* result;
			if ( (result = allocate_from_freelist(&td) ) != 0 )
				return result;
			return allocate_from_freelist_tss(&td);
		}

		inline void	cache_pool_mt_impl::deallocate_impl(void* data)
		{
			mem_block_header* block = reinterpret_cast<mem_block_header*>(align_down( data, pool_block_size) );		// static_pool의 그것임..(재활용)
			
			tls_data& td = *m_tls_pool.get_raw_by_thread_id_unsafe(block->threadID);

			if (block->threadID ==  i3::get_curr_tls_thread_id() )
				deallocate_to_freelist( &td, data );
			else
				deallocate_to_freelist_tss( &td, data); 
		}
		
		inline
		void*		cache_pool_mt_impl::allocate_from_freelist(tls_data* td)		// d는 절대 널값일수 없음..
		{
			if (!td->freelist) return 0;
			free_object* result = td->freelist;
			td->freelist = result->next;
			return result;
		}

		inline
		void		cache_pool_mt_impl::deallocate_to_freelist(tls_data* td,  void* data)
		{
			free_object* free_obj = reinterpret_cast<free_object*>(data);
			free_obj->next = td->freelist;
			td->freelist = free_obj;
		}
		
		
		
		
	}
}