

namespace i3
{
	namespace detail_ginst
	{
		template<class I, bool is_delayed>	struct atexit_ginst	{ static void call() {  i3::delayed_atexit(&ginst<I>::_destroy); }  };
		template<class I>					struct atexit_ginst<I, false> { static void call() {  ::atexit( &ginst<I>::_destroy);  } };	
	}


	template<class I>
	ginst<I>&	ginst<I>::operator=(I* rhs) 
	{ 
		if ( rhs != ms_i ) _destroy(); 
		ms_i = rhs; 
		return *this; 
	}	// optional function..

	template<class I>
	I*	ginst<I>::_create() 
	{ 
		if (ms_i != nullptr) return ms_i;

		detail_ginst::atexit_ginst<I, use_delayed_atexit_ginst::value>::call();
		return ms_i = i3::create_inst_memory<I>();
	}

	template<class I>
	void	ginst<I>::_destroy() 
	{ 
		i3::delete_inst_memory<I>();
		ms_i = nullptr;
	}
		
}