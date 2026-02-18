#pragma once

#include "mpl_limit.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class Val> struct always
		{
			template< class T, pp_enum_params_1_from_to_with_a_default(1, mpl_arity_limit, class T, na) >	//class T, class T2 = na, ..., class T10 = na
			struct apply {	typedef Val		type;	};
		};
		
	}
}