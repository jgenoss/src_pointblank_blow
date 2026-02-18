#pragma once


namespace i3
{

	template<class InputIt, class OutputIt, class T> inline
	OutputIt remove_copy(InputIt f, InputIt l, OutputIt dest, const T& val);


}

#include "detail/remove_copy.inl"
