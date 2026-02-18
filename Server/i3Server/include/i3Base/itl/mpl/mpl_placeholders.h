#if !defined(pp_is_iterating)

#ifndef mpl_placeholders_h
#define mpl_placeholders_h

#include "mpl_arg.h"
#include "mpl_limit.h"

namespace i3
{
	namespace mpl
	{
		typedef arg<-1>		_;

		namespace placeholders
		{
			using ::i3::mpl::_;
		}
	}
}

#define pp_iteration_params_1  (3, (1, pp_inc(mpl_arity_limit), "mpl_placeholders.h"))
#include pp_iterate()

#endif

#else

#define i_	pp_frame_iteration(1)

namespace i3
{
	namespace mpl
	{
		typedef arg<i_> pp_cat(_,i_);
		
		namespace placeholders
		{
			using ::i3::mpl::pp_cat(_,i_);
		}
	}
}

#undef i_

#endif	// pp_is_iterating