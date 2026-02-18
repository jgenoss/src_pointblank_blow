#pragma once

#include "has_trivial_destructor.h"

namespace i3
{
	template<class T>
	struct has_nothrow_destructor : has_trivial_destructor<T> {};
	
}
