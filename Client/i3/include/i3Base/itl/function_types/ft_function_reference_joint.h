#pragma once

#include "ft_synthesize.h"
#include "../mpl/mpl_joint_view.h"
#include "../mpl/mpl_single_view.h"

namespace i3
{
	namespace ft
	{
		template<class R, class Params, class Tag = null_tag>
		struct function_reference_joint 
			: synthesize_function_reference< mpl::joint_view< mpl::single_view<R>, Params>, Tag> {};
	}
}
