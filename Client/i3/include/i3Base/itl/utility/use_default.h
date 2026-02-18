#pragma once

#include "../type_traits/is_convertible.h"

namespace i3
{
	struct use_default;			// 구현이 없음..타입만 존재..

	template<class To>
	struct is_convertible<use_default, To> : false_type {};
	
	template<class T, class DefaultFn>
	struct select_type_with_default : mpl::eval_if< is_same<T, use_default>, DefaultFn, mpl::identity<T> > {};
}
