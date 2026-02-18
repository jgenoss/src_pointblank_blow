#pragma once

namespace i3
{
	template<class InIt1, class InIt2, class T> inline
	T inner_product(InIt1 f1, InIt1 l1, InIt2 f2, T val);

	template<class InIt1, class InIt2, class T, class Fn2_1, class Fn2_2> inline
	T inner_product(InIt1 f1, InIt1 l1, InIt2 f2, T val, Fn2_1 fn1, Fn2_2 fn2);
	
}

#include "detail/inner_product.inl"