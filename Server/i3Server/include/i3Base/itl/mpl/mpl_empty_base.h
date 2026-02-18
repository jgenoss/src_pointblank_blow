#pragma once

#include "mpl_bool.h"

namespace i3
{
	namespace mpl
	{
		struct empty_base {};
		
		template<class T> struct is_empty_base : false_ {};
		template<> struct is_empty_base<empty_base> : true_ {};

		// TODO : type_traits 특화한번 해야함..  ( 이부분은 나중에)
		
	}
}