
namespace i3
{
	
	template<class T>	__forceinline
	void*				static_storage_pool_malloc(identity<T> )
	{
		return i3::static_storage_pool<T>::i()->allocate();
	}

	template<class T> __forceinline
	void				static_storage_pool_free(identity<T>, void* data)
	{
		i3::static_storage_pool<T>::i()->deallocate(data);
	}

	template<class T> __forceinline
	void*				static_storage_pool_malloc_no_dbg( identity<T> t)
	{
		return static_storage_pool_malloc(t);
	}

	template<class T> __forceinline
	void				static_storage_pool_free_no_dbg(identity<T> t, void* data)
	{
		static_storage_pool_free(t, data);
	}


#if defined(_DEBUG) && defined(I3_POOL_DBG)	

	namespace detail
	{
		template<class T> __forceinline
		void*		static_storage_pool_malloc_dbg_impl( identity<T> t, const char* file, int line, mpl::true_ )
		{
			void*	raw_data = i3::static_storage_pool_malloc(t);
			return i3::apply_aligned_dbg_data(file, line, raw_data, static_storage_pool<T>::object_alignment, sizeof(T) );
		}

		template<class T> __forceinline
		void*			static_storage_pool_malloc_dbg_impl( identity<T> t, const char* file, int line, mpl::false_ )
		{
			return i3::static_storage_pool_malloc(t);
		}

		template<class T> __forceinline
		void		static_storage_pool_free_dbg_impl(identity<T> t, void* data, mpl::true_ )
		{
			i3::static_storage_pool_free( t, i3::cleanup_raw_data_from_aligned_dbg_data( data ) );	
		}

		template<class T> __forceinline
		void		static_storage_pool_free_dbg_impl(identity<T> t, void* data, mpl::false_ )
		{
			i3::static_storage_pool_free(t, data);
		}
	}

	template<class T> __forceinline
	void*			static_storage_pool_malloc_dbg( identity<T> t, const char* file, int line )
	{
		return detail::static_storage_pool_malloc_dbg_impl(t, file, line, mpl::bool_<enable_pool_dbg<T>::value>() );
	}

	template<class T> __forceinline
	void			static_storage_pool_free_dbg(identity<T> t, void* data)
	{
		detail::static_storage_pool_free_dbg_impl(t, data, mpl::bool_<enable_pool_dbg<T>::value>() );
	}

#endif
		
}
