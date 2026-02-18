#pragma once

#include "reverse_iterator.h"

namespace i3
{
	template<class T>
	struct range_const_reverse_iterator : range_reverse_iterator<const T> 
	{
	};
}
