#pragma once


namespace i3
{
	template<class T> 
	struct make_const_pointer_result {	typedef				const T*	type;	};

	template<class T>
	struct make_const_pointer_result<T&> { typedef				T*		type;	};

}
