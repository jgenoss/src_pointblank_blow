#if !defined(pp_is_iterating)
#ifndef mpl_arg_h
#define mpl_arg_h

#include "mpl_arg_fwd.h"
#include "mpl_na.h"
#include "mpl_limit.h"
#include "../template_utility/compile_assert.h"



// -1은 따로 구현하고 나머지는 모두 파일반복

namespace i3
{
	namespace mpl
	{
		template<> struct arg< -1 >
		{
			static const int value  = -1;

			template< pp_enum_params_1_with_a_default(mpl_arity_limit, class U, na) >
			struct apply
			{
				typedef U1 type;
				compile_assert_msg( !(is_na<type>::value), arg_must_not_be_na_type_ );
			};
		};
	}
}

#define		pp_iteration_limits ( 1, mpl_arity_limit )		// 1 ~10까지 특화 / 자체재귀..(양개구간임에 유의할것)
#define		pp_filename_1	"mpl_arg.h"
#include	pp_iterate()


#endif
#else
#define		i_			pp_iteration()


namespace i3
{
	namespace mpl
	{
		template<> struct arg<i_>
		{
			static const int			value = i_;
			typedef arg<pp_inc(i_)>		next;
			
			template< pp_enum_params_1_with_a_default(mpl_arity_limit, class U, na) >
			struct apply 
			{
				typedef pp_cat(U, i_)	type;
				compile_assert_msg( !is_na<type>::value, arg_must_not_be_na_type_);
			};
		};
		
	}
}

#undef		i_
#endif
