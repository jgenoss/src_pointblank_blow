#pragma once

namespace i3
{
	template<class InputIterator, class Fn1> inline
	Fn1 for_each(InputIterator First, InputIterator Last, Fn1 Func);
}

#include "detail/for_each.inl"
