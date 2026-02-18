#pragma once

#include "mpl_limit.h"
#include "mpl_bool.h"
#include "mpl_na.h"
#include "../preprocessor/arithmetic/pp_dec.h"
#include "../preprocessor/ext/pp_enum_datas.h"
#include "../preprocessor/ext/pp_enum_params_1.h"
#include "../preprocessor/ext/pp_enum_params_1_with_a_default.h"
#include "../preprocessor/ext/pp_enum_params_1_from_to.h"
#include "../preprocessor/ext/pp_enum_params_1_from_to_with_a_default.h"

#define dec_arity			pp_dec(mpl_arity_limit)

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< bool C, pp_enum_params_1( dec_arity, class T) >
			struct or_impl : true_ {};
			
			template< pp_enum_params_1( dec_arity, class T) >
			struct or_impl< false, pp_enum_params_1(dec_arity, T) > : or_impl< T1::type::value, pp_enum_params_1_from_to(1, dec_arity, T), false_> {};

			template<> struct or_impl< false, pp_enum_datas(dec_arity, false_) > : false_ {};
		}

		template< pp_enum_params_1_with_a_default(2, class T, na), pp_enum_params_1_from_to_with_a_default(2, mpl_arity_limit, class T, false_) >
		struct or_	: aux::or_impl< T1::type::value, pp_enum_params_1_from_to(1, mpl_arity_limit, T) > {	};

		// na spec 2
		/*
		template<>
		struct or_< pp_enum_datas(2, na) >
		{
			template< pp_enum_params_1_with_a_partial_default(mpl_arity_limit, 2, class T, na) >
			struct apply : or_< pp_enum_params_1(2, T) > {};
		};

		template<class Tag>
		struct lambda< or_< pp_enum_datas(2, na) >, Tag >
		{
			typedef false_		is_le;
			typedef or_< pp_enum_datas(2) > result_;
			typedef or_< pp_enum_datas(2) > type;
		};
		*/
	}


}

#undef dec_arity

#define na_spec (2, or_)
#include "aux_/mpl_na_spec.h"


