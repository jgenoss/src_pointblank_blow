#pragma once

namespace i3
{
	template<class OutIt, class Diff, class T> inline
	void fill_n(OutIt f, Diff count, const T& val);
	
	template<class OutIt, class T> inline
	void fill_1(OutIt f, const T& val);
}

#include "detail/fill_n.inl"
