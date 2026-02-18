#pragma once


namespace i3
{
	template<class T> 
	struct make_const_reference_result {	typedef const T&		type;	};

	template<class T>
	struct make_const_reference_result<T&> { typedef T&		type;	};

}
