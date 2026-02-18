#pragma once


namespace i3
{
	template<class T> 
	struct make_reference_result {	typedef T&	type;	};

	template<class T>
	struct make_reference_result<T&> {	typedef T&		type;	};

}
