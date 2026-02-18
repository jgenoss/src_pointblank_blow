#pragma once

#include "begin.h"
#include "end.h"

namespace i3
{
	template<class T> inline
	bool empty(const T& r) { return begin(r) == end(r); }
}

