#if !defined(pp_is_iterating)

#ifndef mpl_bind_fwd_h
#define mpl_bind_fwd_h

#include "mpl_limit.h"
#include "mpl_na.h"
#include "../preprocessor/ext/pp_enum_trailing_params_1.h"
#include "../preprocessor/ext/pp_enum_params_1_with_a_default.h"

namespace i3 
{ 
	namespace mpl 
	{
		template<class F, pp_enum_params_1_with_a_default( mpl_arity_limit, class T, na ) >
		struct bind;
	} 
}

#define		pp_iteration_limits ( 0, mpl_arity_limit )		// 0 ~ 5 까지... (양개)
#define		pp_filename_1	"mpl_bind_fwd.h"
#include	pp_iterate()

#endif	// mpl_bind_fwd_h

#else	// pp_is_itrating

#define i_			pp_iteration()

namespace i3
{
	namespace mpl
	{
		template<class F pp_enum_trailing_params_1(i_, class T) >
		struct pp_cat(bind, i_);
	}
}

#undef		i_

#endif


