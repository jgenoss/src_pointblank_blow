
namespace i3
{
	
	template<class ID, class T>	__forceinline
	void*				static_size_pool_malloc(identity<ID>, identity<T>)
	{
		return i3::static_size_pool<ID>::allocate<T>();
	}

	template<class ID, class T> __forceinline
	void				static_size_pool_free(identity<ID>, identity<T>, void* data)
	{
		i3::static_size_pool<ID>::deallocate<T>(data);
	}

	template<class ID, class T> __forceinline
	void*				static_size_pool_malloc_no_dbg(identity<ID> id, identity<T> t)
	{
		return static_size_pool_malloc(id, t);
	}

	template<class ID, class T> __forceinline
	void				static_size_pool_free_no_dbg(identity<ID> id, identity<T> t, void* data)
	{
		static_size_pool_free(id, t, data);
	}


#if defined(_DEBUG) && defined(I3_POOL_DBG)	

	namespace detail
	{
		template<class ID, class T> __forceinline
		void*		static_size_pool_malloc_dbg_impl(identity<ID> id, identity<T> t, const char* file, int line, mpl::true_ )
		{
			void*	raw_data = i3::static_size_pool_malloc(id, t);
			return i3::apply_aligned_dbg_data(file, line, raw_data, static_adjust_align_for_pool<T>::value, sizeof(T) );
		}

		template<class ID, class T> __forceinline
		void*		static_size_pool_malloc_dbg_impl(identity<ID> id, identity<T> t, const char* file, int line, mpl::false_ )
		{
			return i3::static_size_pool_malloc(id, t);
		}

		template<class ID, class T> __forceinline
		void		static_size_pool_free_dbg_impl(identity<ID> id, identity<T> t, void* data, mpl::true_ )
		{
			i3::static_size_pool_free(id, t, i3::cleanup_raw_data_from_aligned_dbg_data( data ) );	
		}

		template<class ID, class T> __forceinline
		void		static_size_pool_free_dbg_impl(identity<ID> id, identity<T> t, void* data, mpl::false_ )
		{
			i3::static_size_pool_free(id, t, data);
		}
	}

	template<class ID, class T> __forceinline
	void*			static_size_pool_malloc_dbg(identity<ID> id, identity<T> t, const char* file, int line )
	{
		return detail::static_size_pool_malloc_dbg_impl(id, t, file, line, mpl::bool_<enable_pool_dbg<T>::value>() );
	}

	template<class ID, class T> __forceinline
	void			static_size_pool_free_dbg(identity<ID> id, identity<T> t, void* data)
	{
		detail::static_size_pool_free_dbg_impl(id, t, data, mpl::bool_<enable_pool_dbg<T>::value>() );
	}

#endif
		

	
	
	
}