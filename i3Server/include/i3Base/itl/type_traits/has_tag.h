#pragma once

namespace i3
{
	template<class T> struct has_tag;
}

#define has_xxx		(has_tag, tag)
#include "has_xxx.h"
