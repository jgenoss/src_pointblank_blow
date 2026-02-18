#pragma once

namespace i3
{
	template<class InIt, class T> inline
	T accumulate(InIt f, InIt l, T val);

	template<class InIt, class T, class Fn2> inline
	T accumulate(InIt f, InIt l, T val, Fn2 fn);


}

#include "detail/accumulate.inl"
