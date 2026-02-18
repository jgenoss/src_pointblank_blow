#pragma once

namespace i3
{

	template<class InIt, class OutIt, class Pred1, class T> inline
	OutIt replace_copy_if(InIt f, InIt l, OutIt dest, Pred1 pr, const T& new_val);

}

#include "detail/replace_copy_if.inl"
