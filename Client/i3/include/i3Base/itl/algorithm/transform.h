#pragma once

namespace i3
{
	
	template<class InputIterator, class OutputIterator, class Fn1> inline
	OutputIterator transform(InputIterator First, InputIterator Last, OutputIterator Dest, Fn1 Func);

	template<class InputIterator1, class InputIterator2, class OutputIterator, class Fn2> inline
	OutputIterator transform(InputIterator1 First1, InputIterator1 Last1, InputIterator2 First2, OutputIterator Dest, Fn2 Func);

}

#include "detail/transform.inl"
