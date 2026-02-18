#pragma once

namespace i3
{
	template<class InIt, class OutIt, class T> inline
	OutIt replace_copy(InIt f, InIt l, OutIt dest, const T& old_val, const T& new_val);
}

#include "detail/replace_copy.inl"
