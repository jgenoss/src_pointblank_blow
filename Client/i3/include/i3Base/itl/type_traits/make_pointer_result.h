#pragma once



namespace i3
{
	template<class T> 
	struct make_pointer_result {	typedef T*	type;	};

	template<class T>
	struct make_pointer_result<T&> {	typedef T*	type;	};
}
