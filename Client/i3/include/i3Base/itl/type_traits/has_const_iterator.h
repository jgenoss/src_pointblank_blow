#pragma once


namespace i3
{
	template<class T> struct has_const_iterator;
}

#define has_xxx		(has_const_iterator, const_iterator)
#include "has_xxx.h"
