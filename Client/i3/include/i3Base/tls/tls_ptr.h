#pragma once


#include "detail/tls_mgr.h"


namespace i3
{
	I3_EXPORT_BASE DWORD			get_curr_tls_thread_id();						
	I3_EXPORT_BASE DWORD			get_num_tls_thread_id();
	I3_EXPORT_BASE unsigned long	get_num_tls_ptr();

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

	
	template <class T>
	class tls_ptr : private noncopyable
	{
	public:

		__forceinline T*		get() const;
		__forceinline T*		get_raw_by_thread_id_unsafe(DWORD hthread) const;
		__forceinline T*		operator->() const { return get(); }
		__forceinline T&		operator *() const { return *get(); }

		size_t					slot_idx() const { return m_slotidx; }
		tls_ptr();
		~tls_ptr();
	private:
		unsigned long			m_slotidx;
	};

//	
	namespace detail_tls {	template<bool> struct shared_tls_on_create; }
//

	template<class T>
	class shared_tls				// 더 이상 상속 진행 어려움..(사용클래스에서는 다중상속을 허용시킴..)
	{
	public:
		tls_ptr<T>&		get_tls_ptr() const { return *m_pShared; }
		T*				get_raw_by_thread_id_unsafe(DWORD threadID) const { return m_pShared->get_raw_by_thread_id_unsafe(threadID); }
		DWORD			get_thread_id() const { return m_threadID; }
		void			on_create_tls() {}				// 정적 다형성이 가능함...(가상함수로 두지 말것!)

	private:
		template<bool>
		friend struct detail_tls::shared_tls_on_create;

		void			on_create(tls_ptr<T>* tls, DWORD threadID) {	m_pShared = tls;	m_threadID = threadID;	static_cast<T*>(this)->T::on_create_tls();   }
		tls_ptr<T>*		m_pShared;
		DWORD			m_threadID;
	};


}

#include "tls_ptr.inl"
