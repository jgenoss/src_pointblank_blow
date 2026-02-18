#pragma once

namespace i3
{
	template<class InputIterator, class Pred> inline
	typename iterator_traits<InputIterator>::difference_type count_if( InputIterator f, InputIterator l, Pred pr);
	
}

#include "detail/count_if.inl"