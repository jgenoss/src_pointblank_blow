#pragma once

#include "../itl/preprocessor/pp_cat.h"

namespace i3
{
	template<class T>
	struct ginst_initializer
	{
		ginst_initializer()
		{
			ginst<T>::create_explicit();
		}
	};
}

#define ginst_global_init(type)	static i3::ginst_initializer<type>		pp_cat(pp_cat(_ginst_initializer_, type), _; )
