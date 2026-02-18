#if !defined(pp_is_iterating)

#ifndef mpl_apply_fwd_h
#define mpl_apply_fwd_h

#include "mpl_na.h"
#include "mpl_limit.h"
#include "../preprocessor/ext/pp_enum_trailing_params_1.h"

namespace i3
{
	namespace mpl
	{
		template<class F, pp_enum_params_1_with_a_default(mpl_arity_limit, class T, na) >
		struct apply;
	}
}

#include	"../preprocessor/pp_iterate.h"

#define		pp_iteration_limits ( 0, mpl_arity_limit )		// 1 ~10까지 특화 / 자체재귀..(양개구간임에 유의할것)
#define		pp_filename_1	"mpl_apply_fwd.h"
#include	pp_iterate()

#endif
#else
#define		i_			pp_iteration()

namespace i3
{
	namespace mpl
	{
		template< class F pp_enum_trailing_params_1(i_, class T) >
		struct pp_cat(apply, i_);
	}
}

#undef		i_

#endif
