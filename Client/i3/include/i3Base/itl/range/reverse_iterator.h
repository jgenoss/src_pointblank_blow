#pragma once

#include "iterator.h"

namespace i3
{
	template<class T>
	struct range_reverse_iterator 
	{
		typedef i3::reverse_iterator< typename i3::range_iterator<T>::type> type;
	};

}
