

namespace i3
{
	__forceinline
	void*			static_pool_malloc( size_t num, size_t align )
	{
		return static_pool::i()->allocate(num, align);
	}

	__forceinline
	void			static_pool_free( void* data )
	{
		static_pool::i()->deallocate(data);
	}
	
	template<class T> __forceinline
	void*			static_pool_type_malloc(identity<T>, std::size_t size)
	{
		return static_pool::i()->allocate(size, i3::alignment_of<T>::value );
	}

	template<class T> __forceinline
	void			static_pool_type_free(identity<T>, void* data)
	{
		static_pool::i()->deallocate(data);
	}

	__forceinline
	void*			static_pool_malloc_no_dbg( size_t num, size_t align)
	{
		return static_pool_malloc(num, align);
	}

	__forceinline
	void			static_pool_free_no_dbg( void* pData )
	{
		return static_pool_free(pData);
	}


#if defined(_DEBUG) && defined(I3_POOL_DBG)
	__forceinline
	void*			static_pool_malloc_dbg( size_t num, size_t align, const char* file, int line, const char* type_name)
	{
		align = i3::calculate_adjusted_align(align);				// 정렬 교체..
		size_t ext_num = i3::calculate_aligned_dbg_data_size( num, align);		// 사이즈교체..
		void* raw_data = static_pool_malloc(ext_num, align);
		return i3::apply_aligned_dbg_data(file, line, raw_data, align, num, type_name);
	}

	__forceinline
	void			static_pool_free_dbg( void* data)
	{
		static_pool_free( i3::cleanup_raw_data_from_aligned_dbg_data( data ) );
	}
	
	namespace detail
	{
		template<class T> __forceinline
		void*	static_pool_type_malloc_dbg_impl(identity<T>, std::size_t size, const char* file, int line, mpl::true_)
		{
			static const std::size_t align = i3::alignment_of<T>::value;
			size_t ext_num = i3::calculate_aligned_dbg_data_size( size, align);		// 사이즈교체..
			void* raw_data = static_pool_malloc(ext_num, align);
			return i3::apply_aligned_dbg_data(file, line, raw_data, align, size, typeid(T).name() );
		}
		
		template<class T> __forceinline
		void*	static_pool_type_malloc_dbg_impl(identity<T>, std::size_t size, const char* , int , mpl::false_)
		{
			return static_pool::i()->allocate(size, i3::alignment_of<T>::value );
		}
		
		template<class T> __forceinline
		void	static_pool_type_free_dbg_impl(identity<T>, void* data, mpl::true_)
		{
			static_pool_free( i3::cleanup_raw_data_from_aligned_dbg_data( data ) );
		}
		
		template<class T> __forceinline
		void	static_pool_type_free_dbg_impl(identity<T>, void* data, mpl::false_)
		{
			static_pool::i()->deallocate(data);
		}
	}

	template<class T> __forceinline
	void*			static_pool_type_malloc_dbg(identity<T> t, std::size_t size, const char* file, int line)
	{
		return detail::static_pool_type_malloc_dbg_impl(t, size, file, line, mpl::bool_< enable_pool_dbg<T>::value >() );
	}
	
	template<class T> __forceinline
	void			static_pool_type_free_dbg(identity<T> t, void* data)
	{
		detail::static_pool_type_free_dbg_impl(t, data, mpl::bool_< enable_pool_dbg<T>::value >() );
	}

#endif


	__forceinline	
	void*	static_pool::allocate( size_t num, size_t align ) 
	{ 
		return m_Pool->allocate(num,align); 
	}

	__forceinline	
	void	static_pool::deallocate( void* data )
	{  
		if (data) m_Pool->deallocate(data); 
	}

	
}