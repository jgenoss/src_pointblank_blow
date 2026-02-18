

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
		return detail_tls::get_curr_thread_data()->hthread;
	}

	I3_EXPORT_BASE __forceinline
	DWORD	get_num_tls_thread_id()
	{
		return detail_tls::g_TlsManager.get_num_thread_data();
	}

	I3_EXPORT_BASE __forceinline
	unsigned long			get_num_tls_ptr()
	{
		return detail_tls::g_TlsManager.get_num_tls_ptr();
	}

	I3_EXPORT_BASE __forceinline
	void*				tls_malloc(size_t n)
	{
		return i3::heap_malloc(detail_tls::get_curr_thread_data()->heapTls, n);
	}

	I3_EXPORT_BASE __forceinline
	void				tls_free(void* ptr)
	{
		i3::heap_free(detail_tls::get_curr_thread_data()->heapTls, ptr);
	}

	I3_EXPORT_BASE __forceinline
	void*				tls_malloc(DWORD hthread, size_t n)
	{
		return i3::heap_malloc(detail_tls::get_thread_data(hthread)->heapTls, n);
	}

	I3_EXPORT_BASE __forceinline
	void	tls_free(DWORD hthread, void* ptr)
	{
		i3::heap_free(detail_tls::get_thread_data(hthread)->heapTls, ptr);
	}


	I3_EXPORT_BASE __forceinline void*		tls_aligned_malloc(size_t n, size_t align)
	{
		return i3::heap_aligned_malloc(detail_tls::get_curr_thread_data()->heapTls, n, align);
	}

	I3_EXPORT_BASE __forceinline void		tls_aligned_free(void* ptr)
	{
		i3::heap_aligned_free(detail_tls::get_curr_thread_data()->heapTls, ptr);
	}

	I3_EXPORT_BASE __forceinline void*		tls_aligned_malloc(DWORD hThread, size_t n, size_t align)
	{
		return i3::heap_aligned_malloc(detail_tls::get_thread_data(hThread)->heapTls, n, align);
	}

	I3_EXPORT_BASE __forceinline void		tls_aligned_free(DWORD hThread, void* ptr)
	{
		i3::heap_aligned_free(detail_tls::get_thread_data(hThread)->heapTls, ptr);
	}

	I3_EXPORT_BASE __forceinline void*				tls_safe_malloc(size_t n)
	{
		return i3::heap_malloc(detail_tls::get_curr_thread_data()->safe_heap, n);
	}

	I3_EXPORT_BASE __forceinline void				tls_safe_free(void* ptr)
	{
		i3::heap_free(detail_tls::get_curr_thread_data()->safe_heap, ptr);
	}

	I3_EXPORT_BASE __forceinline void*				tls_safe_malloc(DWORD hthread, size_t n)
	{
		return i3::heap_malloc(detail_tls::get_thread_data(hthread)->safe_heap, n);
	}

	I3_EXPORT_BASE __forceinline void				tls_safe_free(DWORD hthread, void* ptr)
	{
		i3::heap_free(detail_tls::get_thread_data(hthread)->safe_heap, ptr);
	}

	I3_EXPORT_BASE __forceinline void*				tls_safe_aligned_malloc(size_t n, size_t align)
	{
		return i3::heap_aligned_malloc(detail_tls::get_curr_thread_data()->safe_heap, n, align); 
	}

	I3_EXPORT_BASE __forceinline void				tls_safe_aligned_free(void* ptr)
	{
		i3::heap_aligned_free(detail_tls::get_curr_thread_data()->safe_heap, ptr);
	}

	I3_EXPORT_BASE __forceinline void*				tls_safe_aligned_malloc(DWORD hThread, size_t n, size_t align)
	{
		return i3::heap_aligned_malloc(detail_tls::get_thread_data(hThread)->safe_heap, n, align); 
	}

	I3_EXPORT_BASE __forceinline void				tls_safe_aligned_free(DWORD hThread, void* ptr)
	{
		i3::heap_aligned_free(detail_tls::get_thread_data(hThread)->safe_heap, ptr);
	}

	namespace detail_tls 
	{

		__forceinline
		thread_data*	tls_mgr::get_curr_thread_data() const
		{
			return reinterpret_cast<thread_data*>(::TlsGetValue(m_dwTlsIdx));
		}

		__forceinline
		thread_data*	tls_mgr::get_thread_data(DWORD hthread) const
		{
			return m_thread_data_list[hthread];
		}

	
		__forceinline
		unsigned long	tls_mgr::get_num_tls_ptr() const 
		{ 
		//	return m_TlsBaseList.size(); 
			return m_num_tls;
		}

	}
	
}