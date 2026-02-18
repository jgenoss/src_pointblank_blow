
namespace i3
{
	template<class T> __forceinline
	void*	storage_pool_malloc(storage_pool<T>* pool_obj)
	{
		return pool_obj->allocate();
	}

	template<class T> __forceinline
	void	storage_pool_free(storage_pool<T>* pool_obj, void* data)
	{
		if (data) pool_obj->deallocate(data);
	}

	template<class T> __forceinline
	void*	storage_pool_malloc_no_dbg(storage_pool<T>* pool_obj)
	{
		return storage_pool_malloc(pool_obj);
	}

	template<class T> __forceinline
	void	storage_pool_free_no_dbg(storage_pool<T>* pool_obj, void* data)
	{
		storage_pool_free(pool_obj, data);
	}


#if defined(_DEBUG) && defined(I3_POOL_DBG)	

	namespace detail
	{
		template<class T> __forceinline
		void*				storage_pool_malloc_dbg_impl( storage_pool<T>* pool_obj, const char* file, int line, mpl::true_ )
		{
			void*	raw_data = storage_pool_malloc(pool_obj);
			return i3::apply_aligned_dbg_data(file, line, raw_data, storage_pool<T>::object_alignment, sizeof(T) );
		}

		template<class T> __forceinline
		void*				storage_pool_malloc_dbg_impl( storage_pool<T>* pool_obj, const char* file, int line, mpl::false_ )
		{
			return storage_pool_malloc(pool_obj);
		}

		template<class T> __forceinline
		void				storage_pool_free_dbg_impl(storage_pool<T>* pool_obj, void* data, mpl::true_ )
		{
			storage_pool_free( pool_obj, i3::cleanup_raw_data_from_aligned_dbg_data( data ) );	
		}

		template<class T> __forceinline
		void				storage_pool_free_dbg_impl(storage_pool<T>* pool_obj, void* data, mpl::false_ )
		{
			storage_pool_free(pool_obj, data);
		}
	}

	template<class T> __forceinline
	void*				storage_pool_malloc_dbg( storage_pool<T>* pool_obj, const char* file, int line  )
	{
		return detail::storage_pool_malloc_dbg_impl(pool_obj, file, line, mpl::bool_<enable_pool_dbg<T>::value>() );		
	}

	template<class T> __forceinline
	void				storage_pool_free_dbg(storage_pool<T>* pool_obj, void* data)
	{
		return detail::storage_pool_free_dbg_impl(pool_obj, data,  mpl::bool_<enable_pool_dbg<T>::value>() );
	}

#endif	

}