#pragma once

#include "variant.h"
#include "detail/enable_recursive.h"

#include "../mpl/mpl_equal.h"

namespace i3
{
	namespace detail { namespace variant
	{	
		// recursive_variant_를 찾을때 또다른 i3::recursive_variant가 있으면 그것은 조사하지 않는다..

		template<pp_enum_params(variant_limit, class T), class RecursiveVariant>
		struct substitute< i3::variant< recursive_flag<T0>, pp_enum_shifted_params(variant_limit, T)>, RecursiveVariant,
										recursive_variant_>
		{
			typedef i3::variant< recursive_flag<T0>, pp_enum_shifted_params(variant_limit, T) > type;
		};
		
		// 그외의 경우는 recursive_variant_가 발견되는 것들은 RecursiveVariant로 치환한다...

		template<class Seq, class RecursiveVariant>
		struct substitute_variant_over
		{
			typedef typename mpl::transform< Seq, 
				mpl::protect< quoted_enable_recursive<RecursiveVariant, mpl::true_> > >::type types;
			
			typedef typename mpl::if_< mpl::equal<Seq, types, is_same<mpl::_1, mpl::_2> >,
				i3::variant< Seq >,
				i3::variant< types > >::type type;
		};
		
		template<class Variant, class RecursiveVariant>
		struct substitute_variant_plain;

		template<pp_enum_params(variant_limit, class T), class RecursiveVariant>
		struct substitute_variant_plain<  i3::variant< pp_enum_params(variant_limit, T) >, RecursiveVariant >
		{

#define repeat_substitute_params(z, n, d)		typedef typename enable_recursive<pp_cat(T, n), RecursiveVariant, mpl::true_>::type	pp_cat(t, n);
			pp_repeat(variant_limit, repeat_substitute_params, ~)
#undef  repeat_substitute_params	

			typedef i3::variant<pp_enum_params(variant_limit, t)> type;
		};

		template<pp_enum_params(variant_limit, class T), class RecursiveVariant>
		struct substitute< i3::variant< pp_enum_params(variant_limit, T) >, RecursiveVariant, recursive_variant_>
		{
			
			typedef typename mpl::eval_if< mpl::and_<mpl::is_sequence<T0>, mpl::is_na<T1> >, 
				substitute_variant_over<T0, RecursiveVariant>, 
				substitute_variant_plain<i3::variant< pp_enum_params(variant_limit, T) >, RecursiveVariant> >::type type;
			
		};
		
	} }


	template< pp_enum_params(variant_limit, class T) >
	struct make_recursive_variant 
	{
		typedef i3::variant< detail::variant::recursive_flag<T0>, pp_enum_shifted_params(variant_limit, T) > type;
	};

	template< class Seq>
	struct make_recursive_variant_over
	{
		typedef typename make_recursive_variant<Seq>::type	type;
	};

}

