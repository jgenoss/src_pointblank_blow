#if !defined(pp_is_iterating)

#ifndef i3ErrorLogImp_h
#define i3ErrorLogImp_h

#include "itl/preprocessor/ext/pp_enum_params_cat_params.h"
#include "itl/mpl/mpl_size_t.h"

#define i3ErrorLogArityLimit		20


#define pp_iteration_params_1	(3, (1, i3ErrorLogArityLimit, "i3ErrorLogImp.h"))
#include pp_iterate()

#endif	// i3ErrorLogImp_h

#else // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3Error
{
	template<class FmtRange, pp_enum_params(i_, class A)> __forceinline
	void Log( const FmtRange& fmt, pp_enum_params_cat_params(i_, const A, &, a) )
	{
		typedef typename i3::range_value<FmtRange>::type Ch;
		i3::stack_basic_string<i3::mpl::size_t<512>, Ch>		temp_str;
		i3::sprintf(temp_str, fmt, pp_enum_params(i_, a));
		Log(temp_str);
	}
}

#undef i_

#endif // pp_is_iterating
