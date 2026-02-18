#pragma once

#include "ft_synthesize.h"
#include "../mpl/mpl_joint_view.h"
#include "../mpl/mpl_single_view.h"

namespace i3
{	
	namespace ft
	{
		// 아래 함수가 잘 돌면, 나머지 함수 유형들도 모두 추가..

		template<class R, class Params, class Tag = null_tag>
		struct function_type_joint : synthesize_function< mpl::joint_view< mpl::single_view<R>, Params >, Tag> {};
	}
}

