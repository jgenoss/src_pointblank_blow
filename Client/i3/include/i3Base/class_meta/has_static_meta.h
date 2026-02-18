#pragma once


#define has_mem_xxx (has_static_meta, static_meta)
#include "../itl/type_traits/has_mem_xxx.h"

namespace i3
{
	template<class T> struct has_static_meta;
	struct class_meta_available_tag {};
}