#pragma once

namespace i3
{
	template<class T> struct has_static_rtti;

	struct rtti_available_tag {};
	struct rtti_na_tag {};
}

#define has_mem_xxx (has_static_rtti, static_rtti)
#include "../itl/type_traits/has_mem_xxx.h"

