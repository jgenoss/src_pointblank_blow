#pragma once

#include "mpl_na.h"

#include "../preprocessor/arithmetic/pp_mul.h"
#include "../preprocessor/ext/pp_enum_params_1_spec.h"
#include "../preprocessor/repetition/pp_enum_params_with_a_default.h"


#define multi_if_arity				10		

#define pp_multi_if_arity			pp_inc(pp_mul(multi_if_arity, 2))

namespace i3
{
	namespace mpl
	{
	// multi_if
		namespace aux
		{
			// trueÀÎ °æ¿ì..
			#define dec_arity	pp_dec(pp_multi_if_arity)

			template< bool b, pp_enum_params_1( dec_arity, class T )>
			struct multi_if_impl { 	typedef T1		type;	};
			
			template< class T1, class T2>
			struct multi_if_impl<false, pp_enum_params_1_spec(dec_arity, 2, T, na)>
			{
				typedef T2		type;
			};

			template< pp_enum_params_1(dec_arity, class T)>
			struct multi_if_impl<false, pp_enum_params_1(dec_arity, T) > 
				: multi_if_impl<bool(T2::value), pp_enum_params_1_from_to(2, dec_arity, T), na, na>
			{
			};

			#undef dec_arity
		}

		template<pp_enum_params_with_a_default(pp_multi_if_arity, class T, na) >
		struct multi_if : aux::multi_if_impl<bool(T0::value), pp_enum_shifted_params( pp_multi_if_arity, T)>
		{
			
		};

		// multi_eval_if
		template<pp_enum_params_with_a_default(pp_multi_if_arity, class T, na)>
		struct multi_eval_if : multi_if<pp_enum_params(pp_multi_if_arity, T)>::type
		{
		};


	}
		
}

#undef pp_multi_if_arity

#define na_spec		(3, multi_if)
#include "aux_/mpl_na_spec.h"

#define na_spec		(3, multi_eval_if)
#include "aux_/mpl_na_spec.h"
