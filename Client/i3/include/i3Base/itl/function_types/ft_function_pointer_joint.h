#pragma once

#include "ft_synthesize.h"
#include "../mpl/mpl_joint_view.h"
#include "../mpl/mpl_single_view.h"

namespace i3
{
	namespace ft
	{
		template<class R, class Params, class Tag = null_tag>
		struct function_pointer_joint 
			: synthesize_function_pointer< mpl::joint_view< mpl::single_view<R>, Params>, Tag> {};
	}
}
