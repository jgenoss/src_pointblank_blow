#pragma once

#include "vector_fwd.h"

namespace i3
{
	template<class> struct less;

	template<class T, class Ctn = i3::vector<T>, class Pred = i3::less<T> > 
	class priority_queue;
}
