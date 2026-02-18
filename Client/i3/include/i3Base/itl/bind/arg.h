#pragma once

#include "is_placeholder.h"

namespace i3
{
	template<int I> 
	struct arg 
	{
		arg(){}
		template<class T>
		arg(const T&) { typedef char T_must_be_placeholder[ I == is_placeholder<T>::value ? 1 : -1 ]; }
	};

	template<int I> __forceinline
	bool operator==(const arg<I>&, const arg<I>&) { return true; }
	
	template<int I> struct is_placeholder< arg<I> > : integral_constant<int, I> {};
	template<int I> struct is_placeholder< arg<I>(*)() > : integral_constant<int, I> {};
}
