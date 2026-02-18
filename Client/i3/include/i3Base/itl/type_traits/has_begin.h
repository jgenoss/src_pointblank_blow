#pragma once

namespace i3
{
	template<class T> struct has_begin;
}

#define has_xxx		(has_begin, begin)
#include "has_xxx.h"
