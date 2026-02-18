
#if !defined(pp_is_iterating)

#ifndef mpl_apply_h
#define mpl_apply_h

#include "mpl_apply_fwd.h"
#include "mpl_apply_wrap.h"
#include "mpl_placeholders.h"
#include "mpl_lambda.h"

#include "mpl_limit.h"


#define		pp_iteration_limits ( 0, mpl_arity_limit )		// 1 ~10까지 특화 / 자체재귀..(양개구간임에 유의할것)
#define		pp_filename_1	"mpl_apply.h"
#include	pp_iterate()


#endif
#else
#define		i_			pp_iteration()

namespace i3
{
	namespace mpl
	{
		template<class F pp_enum_trailing_params_1(i_, class T)>
		struct pp_cat(apply, i_) : pp_cat(apply_wrap, i_)<typename lambda<F>::type pp_enum_trailing_params_1(i_, T) > { };
		
		#if i_ < mpl_arity_limit 
		
		template<class F pp_enum_trailing_params_1(i_, class T)>
		struct apply< F, pp_enum_params_1_spec(mpl_arity_limit, i_, T, na)> : pp_cat(apply, i_)<F pp_enum_trailing_params_1(i_, T) > {};

		#else
		
		template<class F, pp_enum_params_1(i_, class T)>
		struct apply : pp_cat(apply, i_)< F, pp_enum_params_1(i_, T) > {};

		#endif

	}
}

#undef		i_
#endif
