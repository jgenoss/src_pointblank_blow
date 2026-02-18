#pragma once

namespace i3
{
	
	template<class InputIterator, class T> inline
	typename iterator_traits<InputIterator>::difference_type count( InputIterator f, InputIterator l, const T& val);
		
}

#include "detail/count.inl"
