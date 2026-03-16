
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

		template<class T>
		void*	get_value_init(const tls_base* t, const thread_data* td, size_t slot)
		{ 
			const_cast<thread_data*>(td)->create_value<T>(slot);
			T* new_obj = reinterpret_cast<T*>(td->get_raw_value(slot));
			detail_tls::on_create_tls(new_obj, static_cast<tls_ptr<T>*>(const_cast<tls_base*>(t)), td->get_thread_id() );
			return new_obj;
		}
	}

	//
	// 2가지경우를 더 고려..
	// 1) 다른 dll으로부터, 메인app의 tls변수가 다시 생성되는 경우..--> 새로운 tls변수로 간주..
	// 2) 다른 dll의 tls선언으로, 메인app에 그 변수가 접근가능해야하는 경우.. --> 모든 쓰레드에 걸쳐 설정해야함..
	//

	template <class T>
	tls_ptr<T>::tls_ptr()
	{
		const DWORD numThread =	get_num_tls_thread_id();

		for (DWORD i = 0 ; i < numThread ; ++i)
		{
			detail_tls::thread_data* td = detail_tls::get_thread_data(i);
			td->reserve_value<T>();
		}
	}

	template <class T>
	void	tls_ptr<T>::reserve_value(DWORD hthread)			// DllMain 에서 처리..
	{ 
		detail_tls::thread_data* td = detail_tls::get_thread_data(hthread);
		td->reserve_value<T>();
	}

	
	template <class T>
	tls_ptr<T>::~tls_ptr() 
	{ 
		// 모든 쓰레드의 저장소를 여기서 제거함...
		const DWORD numThread = i3::get_num_tls_thread_id();
		
		for ( DWORD i = 0 ; i < numThread ; ++i )
		{
			T* data = reinterpret_cast<T*>(get_raw_by_thread_id(i));
			if (data)
			{
				data->~T();
				i3::tls_aligned_free(i, data);	
			}
		}
	}


}