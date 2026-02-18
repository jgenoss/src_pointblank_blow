#pragma once

#include "mpl_arg_fwd.h"
#include "mpl_bool.h"

namespace i3
{
	namespace mpl
	{
		template< class T > struct is_placeholder : bool_<false> {};
		template< int N > struct is_placeholder< arg<N> > : bool_<true> {};
	}
}
