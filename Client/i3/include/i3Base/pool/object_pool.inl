
namespace i3
{
	
//////////////////////

	template<class T>
	class object_pool_large 
	{
	public:
		typedef i3::local_large_object_cache<T>		large_object_cache;
		
		large_object_cache*			get() { return &m_cache;  }
		const large_object_cache*	get() const {  return &m_cache;  }
	private:
		large_object_cache			m_cache;
	};

	template<class T>
	class object_pool_small : public pool_tls_base< object_pool_small<T>  >
	{
	public:

		enum {	num_obj_per_block	= ( pool_block_size - sizeof(mem_block_header) ) / object_pool<T>::object_size  };
		enum {  request_num_block = i3::static_calculate_block_length_with_elem_length<num_obj_per_block,  reserve_object_pool<T>::value >::value };
		enum {  request_malloc_size = request_num_block * pool_block_size  };
		
		static const size_t size_index  = i3::static_convert_size_to_size_idx<object_pool<T>::object_size>::value;
		static const size_t actual_size = i3::static_convert_size_to_actual_size< object_pool<T>::object_size>::value;

		typedef i3::shared_tls< object_pool_small<T> > shared_tls_type;

		object_pool_small();
		~object_pool_small();

		void*					allocate();			
		void					deallocate( void* data);
		void					on_create_tls();

	private:

		void					deallocate_other_thread(mem_block_header* block, void* data);

		mem_block_header*			m_active_block;		// 활성블록...
		tss_free_block_by_size*		m_free_block;		// tss프리블록..
	};


	template<class T>
	object_pool_small<T>::object_pool_small() : 
		pool_tls_base< object_pool_small<T> >( request_malloc_size ), m_active_block(nullptr), m_free_block(nullptr)
	{

	}

	template<class T>
	object_pool_small<T>::~object_pool_small()
	{
		i3::tls_aligned_free(shared_tls_type::get_thread_id(), m_free_block);
	}

	template<class T> 
	void			object_pool_small<T>::on_create_tls()
	{
		m_free_block = reinterpret_cast<tss_free_block_by_size*>(i3::tls_aligned_malloc(shared_tls_type::get_thread_id(), sizeof(tss_free_block_by_size), pool_block_header_alignment ));
		m_free_block->m_tss_free_block = nullptr;
	}

	template<class T> inline
	void*			object_pool_small<T>::allocate()
	{
		void* result;
		if ( (result = detail::allocate_from_active_blocks(m_active_block)) != nullptr)
			return result;

		if ( (result = detail::allocate_from_tss_blocks(m_active_block, m_free_block)) != nullptr)
			return result;

		return pool_base::allocate_from_new_block(shared_tls_type::get_thread_id(), m_active_block, size_index, actual_size);
	}


	template<class T> inline
	void			object_pool_small<T>::deallocate( void* data)
	{
//		if (!data) return;		널값 비교를 좀더 상위에 배치..
		mem_block_header* block = reinterpret_cast<mem_block_header*>(align_down( data, pool_block_size) );
		DWORD threadID = shared_tls_type::get_thread_id();

		if ( block->threadID == threadID )		// 동일 쓰레드..
			deallocate_same_thread( m_active_block, block, data);
		else
			get_raw_by_thread_id_unsafe( block->threadID )->deallocate_other_thread(block, data);
	}

	template<class T> inline
	void		object_pool_small<T>::deallocate_other_thread(mem_block_header* block, void* data)
	{
		detail::deallocate_other_thread( m_free_block, block, data );
	}
////////////

	template<class T> __forceinline
	void*				object_pool_malloc( object_pool<T>* pool_obj )
	{
		return pool_obj->allocate();
	}

	template<class T> __forceinline
	void				object_pool_free(object_pool<T>* pool_obj, void* data)
	{
		pool_obj->deallocate(data);
	}

	template<class T> __forceinline
	void*						object_pool_malloc_no_dbg(object_pool<T>* pool_obj)
	{
		return object_pool_malloc(pool_obj);
	}
	template<class T> __forceinline
	void						object_pool_free_no_dbg(object_pool<T>* pool_obj, void* data)
	{
		object_pool_free(pool_obj, data);
	}


#if defined(_DEBUG) && defined(I3_POOL_DBG)	

	namespace detail
	{
		template<class T> __forceinline
		void*				object_pool_malloc_dbg_impl( object_pool<T>* pool_obj, const char* file, int line, mpl::true_ )
		{
			void*	raw_data = object_pool_malloc(pool_obj);
			return i3::apply_aligned_dbg_data(file, line, raw_data, object_pool<T>::object_alignment, sizeof(T) );
		}

		template<class T> __forceinline
		void*				object_pool_malloc_dbg_impl( object_pool<T>* pool_obj, const char* file, int line, mpl::false_ )
		{
			return object_pool_malloc(pool_obj);
		}

		template<class T> __forceinline
		void				object_pool_free_dbg_impl(object_pool<T>* pool_obj, void* data, mpl::true_ )
		{
			object_pool_free( pool_obj, i3::cleanup_raw_data_from_aligned_dbg_data( data ) );	
		}

		template<class T> __forceinline
		void				object_pool_free_dbg_impl(object_pool<T>* pool_obj, void* data, mpl::false_ )
		{
			object_pool_free(pool_obj, data);
		}
	}

	template<class T> __forceinline
	void*				object_pool_malloc_dbg( object_pool<T>* pool_obj, const char* file, int line  )
	{
		return detail::object_pool_malloc_dbg_impl(pool_obj, file, line, mpl::bool_<enable_pool_dbg<T>::value>() );		
	}

	template<class T> __forceinline
	void				object_pool_free_dbg(object_pool<T>* pool_obj, void* data)
	{
		return detail::object_pool_free_dbg_impl(pool_obj, data,  mpl::bool_<enable_pool_dbg<T>::value>() );
	}

#endif	

////////////
	template <class T> __forceinline
	void*	object_pool<T>::allocate() { return m_pool.get()->allocate(); }

	template <class T> __forceinline
	void	object_pool<T>::deallocate( void* data ) { if (data) m_pool.get()->deallocate(data); }

}
