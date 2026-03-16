

namespace i3
{
	

	namespace detail_tls 
	{
		__forceinline
			thread_data*		get_curr_thread_data() {  return g_TlsManager.get_curr_thread_data();  }
		__forceinline
			thread_data*		get_thread_data(DWORD hthread) { return g_TlsManager.get_thread_data(hthread); }
	}

	I3_EXPORT_BASE __forceinline
	DWORD	get_curr_tls_thread_id() 
	{
		return detail_tls::get_curr_thread_data()->get_thread_id();
	}

	I3_EXPORT_BASE __forceinline
	DWORD	get_num_tls_thread_id()
	{
		return detail_tls::g_TlsManager.get_num_thread_data();
	}

	I3_EXPORT_BASE __forceinline
	size_t			get_num_tls_ptr()
	{
		return detail_tls::g_TlsManager.GetTlsListSize();
	}

	I3_EXPORT_BASE __forceinline
		void*				tls_malloc(size_t n)
	{
		return detail_tls::get_curr_thread_data()->tls_malloc(n);
	}

	I3_EXPORT_BASE __forceinline
		void				tls_free(void* ptr)
	{
		return detail_tls::get_curr_thread_data()->tls_free(ptr);
	}

	I3_EXPORT_BASE __forceinline
		void*				tls_malloc(DWORD hthread, size_t n)
	{
		return detail_tls::get_thread_data(hthread)->tls_malloc(n);
	}

	I3_EXPORT_BASE __forceinline
	void	tls_free(DWORD hthread, void* ptr)
	{
		detail_tls::get_thread_data(hthread)->tls_free(ptr);
	}


	I3_EXPORT_BASE __forceinline void*		tls_aligned_malloc(size_t n, size_t align)
	{
		return detail_tls::get_curr_thread_data()->tls_aligned_malloc(n, align);
	}

	I3_EXPORT_BASE __forceinline void		tls_aligned_free(void* ptr)
	{
		detail_tls::get_curr_thread_data()->tls_aligned_free(ptr);
	}

	I3_EXPORT_BASE __forceinline void*		tls_aligned_malloc(DWORD hThread, size_t n, size_t align)
	{
		return detail_tls::get_thread_data(hThread)->tls_aligned_malloc(n, align);
	}

	I3_EXPORT_BASE __forceinline void				tls_aligned_free(DWORD hThread, void* ptr)
	{
		return detail_tls::get_thread_data(hThread)->tls_aligned_free(ptr);
	}

	I3_EXPORT_BASE __forceinline void*				tls_safe_malloc(size_t n)
	{
		return detail_tls::get_curr_thread_data()->tls_safe_malloc(n);
	}

	I3_EXPORT_BASE __forceinline void				tls_safe_free(void* ptr)
	{
		detail_tls::get_curr_thread_data()->tls_safe_free(ptr);
	}

	I3_EXPORT_BASE __forceinline void*				tls_safe_malloc(DWORD hthread, size_t n)
	{
		return detail_tls::get_thread_data(hthread)->tls_safe_malloc(n);
	}

	I3_EXPORT_BASE __forceinline void				tls_safe_free(DWORD hthread, void* ptr)
	{
		detail_tls::get_thread_data(hthread)->tls_safe_free(ptr);
	}

	I3_EXPORT_BASE __forceinline void*				tls_safe_aligned_malloc(size_t n, size_t align)
	{
		return detail_tls::get_curr_thread_data()->tls_safe_aligned_malloc(n, align);
	}

	I3_EXPORT_BASE __forceinline void				tls_safe_aligned_free(void* ptr)
	{
		detail_tls::get_curr_thread_data()->tls_safe_aligned_free(ptr);
	}

	I3_EXPORT_BASE __forceinline void*				tls_safe_aligned_malloc(DWORD hThread, size_t n, size_t align)
	{
		return detail_tls::get_thread_data(hThread)->tls_safe_aligned_malloc(n, align);
	}

	I3_EXPORT_BASE __forceinline void				tls_safe_aligned_free(DWORD hThread, void* ptr)
	{
		detail_tls::get_thread_data(hThread)->tls_safe_aligned_free(ptr);
	}

	namespace detail_tls 
	{

		inline void*	get_value_default(const tls_base* , const thread_data* td, size_t slot ) { return	td->get_raw_value(slot); }

		template<class T>
		void*	get_value_init(const tls_base* t, const thread_data* td, size_t slot);

		inline
		thread_data::thread_data( heap_handle heapTls, heap_handle safe_heap, DWORD threadID) : 
		slotlist( heap_allocator<void*>( heapTls) ), fn_list( heap_allocator<fn_value_list>(heapTls) ),
		heapTls(heapTls), safe_heap(safe_heap), hthread(threadID) {}
		
		__forceinline
		void*		thread_data::tls_malloc(size_t n) const { return i3::heap_malloc(this->heapTls, n); }

		__forceinline
		void		thread_data::tls_free(void* ptr) const 	{	i3::heap_free(this->heapTls, ptr);	}

		__forceinline
		void*		thread_data::tls_aligned_malloc(size_t n, size_t align) const {	return i3::heap_aligned_malloc(this->heapTls, n, align); }

		__forceinline
		void		thread_data::tls_aligned_free(void* ptr) const { i3::heap_aligned_free(this->heapTls, ptr); }


		__forceinline
		void*		thread_data::tls_safe_malloc(size_t n) const {	return i3::heap_malloc(this->safe_heap, n);	}

		__forceinline
		void		thread_data::tls_safe_free(void* ptr) const {	i3::heap_free(this->safe_heap, ptr); }

		__forceinline
		void*		thread_data::tls_safe_aligned_malloc(size_t n, size_t align) const { return i3::heap_aligned_malloc(this->safe_heap, n, align); }
		
		__forceinline
		void		thread_data::tls_safe_aligned_free(void* ptr) const { 	i3::heap_aligned_free(this->safe_heap, ptr);	}		


		template<class T>
		void		thread_data::reserve_value()
		{
			this->fn_list.push_back( &get_value_init<T>);
			this->slotlist.push_back(NULL);
		}

		template<class T>
		void		thread_data::create_value(size_t slot)
		{
			T* pNewObj = (T*) i3::heap_aligned_malloc(this->heapTls, sizeof(T), __alignof(T) );
			//			if ( idx >= data_list.size() )
			//				data_list.resize( idx + 1);
			this->slotlist[slot] = pNewObj;				// idx가 함수호출 이전에 유효한 것을 가정하는게 좋다..
			new (pNewObj) T;

			this->fn_list[slot] = &get_value_default; 
		}


		__forceinline
		thread_data*	tls_mgr::get_curr_thread_data() const
		{
			return reinterpret_cast<thread_data*>(::TlsGetValue(m_dwTlsIdx));
		}

		__forceinline
		thread_data*	tls_mgr::get_thread_data(DWORD hthread) const
		{
			return m_usingSlotList[hthread];
		}


		__forceinline
		size_t			tls_mgr::GetTlsListSize() const 
		{ 
			return m_TlsBaseList.size(); 
		}
		
		__forceinline
		void			tls_mgr::PushBackTls(tls_base* tls)	// 이 처리에 락은 필요없음..
		{  
			m_TlsBaseList.push_back(tls); 
		}
	}
	
}