#pragma once

namespace i3
{

	template<class InputIterator, class Pred> inline
	InputIterator find_if(InputIterator First, InputIterator Last, Pred pr);
	
}

#include "detail/find_if.inl"
