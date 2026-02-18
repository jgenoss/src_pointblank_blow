#pragma once

#include "../preprocessor/arithmetic/pp_dec.h"
#include "../preprocessor/ext/pp_enum_params_1.h"
#include "../preprocessor/ext/pp_enum_params_1_from_to.h"
#include "../preprocessor/ext/pp_enum_datas.h"
#include "../preprocessor/ext/pp_enum_params_1_with_a_default.h"
#include "../preprocessor/ext/pp_enum_params_1_from_to_with_a_default.h"



#include "mpl_bool.h"
#include "mpl_limit.h"
#include "mpl_na.h"

#define dec_arity			pp_dec(mpl_arity_limit)

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< bool C, pp_enum_params_1( dec_arity, class T) >
			struct and_impl : false_ {};
			
			template< pp_enum_params_1( dec_arity, class T) >
			struct and_impl< true, pp_enum_params_1(dec_arity, T) > : and_impl< T1::type::value, pp_enum_params_1_from_to(1, dec_arity, T), true_> {};

			template<> struct and_impl< true, pp_enum_datas(dec_arity, true_) > : true_ {};
		}

		template< pp_enum_params_1_with_a_default(2, class T, na), pp_enum_params_1_from_to_with_a_default(2, mpl_arity_limit, class T, true_) >
		struct and_	: aux::and_impl< T1::type::value, pp_enum_params_1_from_to(1, mpl_arity_limit, T) > {	};
	}
}


#undef dec_arity

#define na_spec (2, and_)
#include "aux_/mpl_na_spec.h"
