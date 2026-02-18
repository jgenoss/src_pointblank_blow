#pragma once

namespace i3
{
	template<class T> struct has_iterator_category;
}

#define has_xxx		(has_iterator_category, iterator_category)
#include "has_xxx.h"
