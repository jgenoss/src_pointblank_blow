#pragma once

#include "is_call_possible_common.h"

namespace i3
{
	namespace detail
	{
		template <class src_type, class dest_type> struct clone_constness { typedef dest_type type; }; 
		template <class src_type, class dest_type> struct clone_constness<const src_type, dest_type> { typedef const dest_type type; }; 
	}
}
