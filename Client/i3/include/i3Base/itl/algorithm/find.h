#pragma once

namespace i3
{
	template<class InputIterator, class T> inline 
	InputIterator find(InputIterator First, InputIterator Last, const T& Val); 
}

#include "detail/find.inl"
