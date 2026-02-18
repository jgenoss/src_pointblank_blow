

namespace i3
{
	__forceinline
	void*			static_pool_malloc( size_t num )
	{
		return static_pool::i()->allocate(num);
	}

	__forceinline
	void			static_pool_free( void* data )
	{
		static_pool::i()->deallocate(data);
	}
	
	template<class T> __forceinline
	void*			static_pool_type_malloc(identity<T>, std::size_t size)
	{
		return static_pool_malloc(size);
	}

	template<class T> __forceinline
	void			static_pool_type_free(identity<T>, void* data)
	{
		static_pool_free(data);
	}

	__forceinline
	void*			static_pool_malloc_no_dbg( size_t num)
	{
		return static_pool_malloc(num);
	}

	__forceinline
	void			static_pool_free_no_dbg( void* pData )
	{
		return static_pool_free(pData);
	}


#if defined(_DEBUG) && defined(I3_POOL_DBG)
	__forceinline
	void*			static_pool_malloc_dbg( size_t num, const char* file, int line)
	{
		size_t ext_num = i3::calculate_dbg_data_size( num );		// 사이즈교체..
		void* raw_data = static_pool_malloc(ext_num);
		return i3::apply_dbg_data(file, line, raw_data, num);
	}

	__forceinline
	void			static_pool_free_dbg( void* data)
	{
		static_pool_free( i3::cleanup_raw_data_from_dbg_data( data ) );
	}
	
	namespace detail
	{
		__forceinline
		void*	static_pool_type_malloc_dbg_impl(std::size_t size, const char* file, int line, mpl::true_)
		{
			size_t ext_num = i3::calculate_dbg_data_size( size);		// 사이즈교체..
			void* raw_data = static_pool_malloc(ext_num);
			return i3::apply_dbg_data(file, line, raw_data, size );
		}
		
		__forceinline
		void*	static_pool_type_malloc_dbg_impl(std::size_t size, const char* , int , mpl::false_)
		{
			return static_pool_malloc(size);
		}
		
		__forceinline
		void	static_pool_type_free_dbg_impl(void* data, mpl::true_)
		{
			static_pool_free( i3::cleanup_raw_data_from_dbg_data( data ) );
		}
		
		__forceinline
		void	static_pool_type_free_dbg_impl(void* data, mpl::false_)
		{
			static_pool_free(data);
		}
	}

	template<class T> __forceinline
	void*			static_pool_type_malloc_dbg(identity<T> t, std::size_t size, const char* file, int line)
	{
		return detail::static_pool_type_malloc_dbg_impl( size, file, line, mpl::bool_< enable_pool_dbg<T>::value >() );
	}
	
	template<class T> __forceinline
	void			static_pool_type_free_dbg(identity<T> t, void* data)
	{
		detail::static_pool_type_free_dbg_impl( data, mpl::bool_< enable_pool_dbg<T>::value >() );
	}

#endif


	__forceinline	
	void*	static_pool::allocate( size_t num ) 
	{ 
		return m_Pool->allocate(num); 
	}

	__forceinline	
	void	static_pool::deallocate( void* data )
	{  
		if (data) m_Pool->deallocate(data); 
	}

	
}