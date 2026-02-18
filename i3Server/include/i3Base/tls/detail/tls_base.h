#pragma once


#include "tls_mgr.h"


namespace i3
{


	I3_EXPORT_BASE DWORD			get_curr_tls_thread_id();
	I3_EXPORT_BASE DWORD			get_num_tls_thread_id();
	I3_EXPORT_BASE size_t			get_num_tls_ptr();

	I3_EXPORT_BASE void*			tls_malloc(size_t n);
	I3_EXPORT_BASE void				tls_free(void* ptr);
	I3_EXPORT_BASE void*			tls_malloc(DWORD hthread, size_t n);
	I3_EXPORT_BASE void				tls_free(DWORD hthread, void* ptr);

	I3_EXPORT_BASE void*			tls_aligned_malloc(size_t n, size_t align);
	I3_EXPORT_BASE void				tls_aligned_free(void* ptr);
	I3_EXPORT_BASE void*			tls_aligned_malloc(DWORD hThread, size_t n, size_t align);
	I3_EXPORT_BASE void				tls_aligned_free(DWORD hThread, void* ptr);

	I3_EXPORT_BASE void*			tls_safe_malloc(size_t n);
	I3_EXPORT_BASE void				tls_safe_free(void* ptr);
	I3_EXPORT_BASE void*			tls_safe_malloc(DWORD hthread, size_t n);
	I3_EXPORT_BASE void				tls_safe_free(DWORD hthread, void* ptr);

	I3_EXPORT_BASE void*			tls_safe_aligned_malloc(size_t n, size_t align);
	I3_EXPORT_BASE void				tls_safe_aligned_free(void* ptr);
	I3_EXPORT_BASE void*			tls_safe_aligned_malloc(DWORD hThread, size_t n, size_t align);
	I3_EXPORT_BASE void				tls_safe_aligned_free(DWORD hThread, void* ptr);



	class I3_EXPORT_BASE tls_base : private noncopyable
	{
	public:
		
		virtual ~tls_base();
		virtual void		reserve_value(DWORD hthread) = 0;
	
	protected:
		tls_base();
		void*				get() const;
		void*				get_by_thread_id(DWORD hthread) const;
		void*				get_raw() const;
		void*				get_raw_by_thread_id(DWORD hthread) const;

		size_t				slot_idx() const { return m_slotidx; }
//		void				SetData_ByThread(void* pNewData);
//		void*				GetData_ByThreadId(DWORD hthread) const;		// 이 함수의 주의점..반드시 해당 쓰레드가 유효해야함..
//		void				SetData_ByThreadId(DWORD hthread, void* pNewData);

	private:
		size_t				m_slotidx;
	};

	inline tls_base::tls_base() : m_slotidx( get_num_tls_ptr() )	// 메인쓰레드에서만 처리하도록...
	{
		detail_tls::g_TlsManager.PushBackTls(this);
	}

	__forceinline tls_base::~tls_base() {}

	__forceinline void*		tls_base::get() const 
	{  
		detail_tls::thread_data* td = detail_tls::get_curr_thread_data();
		return td->get_value(this, m_slotidx);
	}

	__forceinline void*		tls_base::get_by_thread_id(DWORD hthread) const 
	{ 
		detail_tls::thread_data* td = detail_tls::get_thread_data(hthread);
		return td->get_value(this, m_slotidx);
	}

	__forceinline void*		tls_base::get_raw() const
	{
		detail_tls::thread_data* td = detail_tls::get_curr_thread_data();
		return td->get_raw_value(m_slotidx);
	}
	__forceinline void*		tls_base::get_raw_by_thread_id(DWORD hthread) const
	{
		detail_tls::thread_data* td = detail_tls::get_thread_data(hthread);
		return td->get_raw_value(m_slotidx);
	}



}