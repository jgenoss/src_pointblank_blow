#pragma once


namespace i3
{
	template<class T> struct has_iterator;
}

#define has_xxx		(has_iterator, iterator)
#include "has_xxx.h"
