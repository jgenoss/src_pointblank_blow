#pragma once

#include "iterator.h"
#include "../iterator/iterator_traits.h"

namespace i3
{
	template<class T>
	struct range_value : iterator_value< typename range_iterator<T>::type > {};
}
