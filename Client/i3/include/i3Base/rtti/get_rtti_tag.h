#pragma once

#include "has_static_rtti.h"
#include "../class_meta/has_static_meta.h"
#include "../itl/mpl/mpl_multi_if.h"

namespace i3
{
	template<class T>
	struct get_rtti_tag 
	{
		typedef typename mpl::multi_if< has_static_rtti<T>, rtti_available_tag, 
										has_static_meta<T>, class_meta_available_tag,
										rtti_na_tag>::type		type;
	};

}
