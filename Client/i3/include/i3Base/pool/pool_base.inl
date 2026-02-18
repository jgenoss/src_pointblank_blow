

namespace i3
{




/*
	inline
	void	pool_base::expand_memory_size_by_pow2(size_t pow2)
	{
		m_next_raw_memory_size <<= pow2;
	}
*/	
	inline
	void	pool_base::deallocate_same_thread( mem_block_header*& active_block, mem_block_header* block, void* data )
	{	
		detail::deallocate_same_thread(m_free_block_list, active_block, block, data);
	}

	inline
	void*	pool_base::allocate_from_new_block( DWORD threadID, mem_block_header*& active_block, size_t size_index, size_t actual_size)
	{
		mem_block_header* block = get_new_block(threadID, size_index, actual_size);
		active_block = block;								// 항상 존재..	
		return detail::allocate_from_alloc_ptr(block);		// 항상 성공해야함..free_list가 없으므로 이 함수만...
	}
	
	template<class D> inline
	pool_tls_base<D>::pool_tls_base(size_t init_memory_size) : pool_base(init_memory_size)
	{
	}

	template<class D> inline
	pool_tls_base<D>::~pool_tls_base()
	{
		pool_base::clean_up( i3::shared_tls<D>::get_thread_id() );
	}
	
	
	
	
}