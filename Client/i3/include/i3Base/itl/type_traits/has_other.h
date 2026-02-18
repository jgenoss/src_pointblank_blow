#pragma once

namespace i3
{
	template<class T> struct has_other;
}

#define has_xxx		(has_other, other)
#include "has_xxx.h"
