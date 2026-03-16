#pragma once


namespace i3
{

	namespace detail
	{
		template<class from, class to>
		struct is_equivalent_impl
		{
			typedef	typename i3::remove_cv< typename i3::remove_reference<from>::type >::type	unqualified_referred_from;
			typedef	typename i3::remove_cv< typename i3::remove_reference<to>::type >::type		unqualified_referred_to;
			static const bool value = mpl::and_<is_same<unqualified_referred_from, unqualified_referred_to>, i3::is_convertible<from, to> >::value;
		};
	}

	template<class from, class to> struct is_equivalent : integral_constant<bool, detail::is_equivalent_impl<from, to>::value > {};

}

