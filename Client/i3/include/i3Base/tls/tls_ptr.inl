
#include <new>

namespace i3
{

	namespace detail_tls
	{

		template<bool is_Shared_Tls>
		struct shared_tls_on_create {		template<class T> static void call( T* data, tls_ptr<T>* t, DWORD threadID) {  data->on_create(t, threadID); }	};
		template<>
		struct shared_tls_on_create<false> {	template<class T> static void call(T*, tls_ptr<T>*, DWORD) {}	};

		template <class T> inline
		void on_create_tls( T* data, tls_ptr<T>* t, DWORD threadID )
		{
			shared_tls_on_create< i3::is_base_of<i3::shared_tls<T>, T>::value >::call(data, t, threadID);
		}

#pragma push_macro("new")
#undef new
		template<class T>
		T*		create_value(thread_data* td, const tls_ptr<T>* t, size_t slot) 
		{
			T* new_obj = (T*) i3::heap_aligned_malloc(td->heapTls, sizeof(T), __alignof(T) );
			td->slotlist[slot] = new_obj;				// idx가 함수호출 이전에 유효한 것을 가정하는게 좋다..
			::new (new_obj) T;
			detail_tls::on_create_tls(new_obj, const_cast<tls_ptr<T>*>(t), td->hthread);
			return new_obj;
		}
#pragma pop_macro("new")

		I3_EXPORT_BASE	void	resize_slot(thread_data* td, size_t slot);
		
	}


	template <class T>
	tls_ptr<T>::tls_ptr() : m_slotidx( detail_tls::g_TlsManager.obtain_tls_idx() )
	{

	}
	
	template <class T>
	tls_ptr<T>::~tls_ptr() 
	{ 
		// 모든 쓰레드의 저장소를 여기서 제거함...
		const DWORD numThread = i3::get_num_tls_thread_id();
		
		for ( DWORD i = 0 ; i < numThread ; ++i )
		{
			detail_tls::thread_data* td = detail_tls::get_thread_data(i);
			
			if ( m_slotidx < td->slotlist.size()  )
			{
				T* data = reinterpret_cast<T*>(td->slotlist[m_slotidx]);				
				if (data)
				{
					data->~T();
					i3::tls_aligned_free(i, data);	
					td->slotlist[m_slotidx] = nullptr;
				}
			}
		}	

		detail_tls::g_TlsManager.return_tls_idx(m_slotidx);
	}
	
	template <class T>  
	T*		tls_ptr<T>::get() const 
	{  
		detail_tls::thread_data* td = detail_tls::get_curr_thread_data();

		if (m_slotidx >= td->slotlist.size() )
			detail_tls::resize_slot(td, m_slotidx);

		if ( !td->slotlist[m_slotidx] )
			return detail_tls::create_value(td, this, m_slotidx);

		return reinterpret_cast<T*>(td->slotlist[m_slotidx]);
	}

	template <class T> __forceinline 
	T*		tls_ptr<T>::get_raw_by_thread_id_unsafe(DWORD hthread) const
	{
		return reinterpret_cast<T*>(detail_tls::get_thread_data(hthread)->slotlist[m_slotidx] );
	}



}