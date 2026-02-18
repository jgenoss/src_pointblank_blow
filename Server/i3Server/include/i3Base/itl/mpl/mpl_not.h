#pragma once

#include "mpl_bool.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<long C> struct not_impl : bool_<!C> {};
		}
		
		template< class T = na >
		struct not_ : aux::not_impl< T::type::value >{ };
	}
}

#define na_spec (1, not_)
#include "aux_/mpl_na_spec.h"

