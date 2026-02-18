#pragma once

#include "iterator.h"
#include "../iterator/iterator_traits.h"

namespace i3
{
	template<class T>
	struct range_difference : iterator_difference< typename range_iterator<T>::type > {};
}
