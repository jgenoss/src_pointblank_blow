#pragma once

#include "decay.h"

// remove_reference 대신 decay로 변경..
// decay의 경우, 레퍼런스 제거에 덧붙여 배열은 포인터로 변경시키고, 함수원형은 함수포인터형으로 변경하며...
// 오버로딩 규칙 1, 2번째 조건에 부합된다...
//

namespace i3
{

	namespace detail
	{
		template<class from, class to>
		struct is_equivalent_impl2
		{
			typedef typename mpl::eval_if<	mpl::and_< is_pointer<from>, is_pointer<to> >, 
											remove_pointer<from>, decay<from> >::type  from2;
			typedef typename mpl::eval_if< mpl::and_< is_pointer<from>, is_pointer<to> >, 
											remove_pointer<to>, decay<to> >::type	to2;

			typedef	typename i3::remove_cv< from2 >::type	unqualified_referred_from;
			typedef	typename i3::remove_cv< to2 >::type	unqualified_referred_to;

			static const bool value = is_same<unqualified_referred_from, unqualified_referred_to>::value;
		};

		template<class from, class to>
		struct is_equivalent_impl : mpl::eval_if< is_convertible<from, to>, 
			is_equivalent_impl2<from, to>, false_type>
		{
		};
	}

	template<class from, class to> struct is_equivalent : integral_constant<bool, detail::is_equivalent_impl<from, to>::value > {};

}

