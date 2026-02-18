#if !defined(pp_is_iterating)

#ifndef i3DebugImp_h
#define i3DebugImp_h

#include "itl/preprocessor/ext/pp_enum_params_cat_params.h"
#include "itl/mpl/mpl_size_t.h"

#define i3DebugImpArityLimit		20


#define pp_iteration_params_1 (3, (1, i3DebugImpArityLimit, "i3DebugImp.h"))
#include pp_iterate()


#endif // i3DebugImp_h

#else // pp_is_iterating

#define i_		pp_frame_iteration(1)

template<class ModuleRng, class FmtRng, pp_enum_params(i_, class A)> __forceinline
void i3Log( const ModuleRng& strModule, const FmtRng& fmt, pp_enum_params_cat_params(i_, const A, &, a) )
{
	typedef typename i3::range_value<FmtRng>::type Ch;
	i3::stack_basic_string<i3::mpl::size_t<512>, Ch>		temp_str;	
	i3::sprintf(temp_str, fmt, pp_enum_params(i_, a));
	i3Log(strModule, temp_str);
}

template<class FmtRng, pp_enum_params(i_, class A)> __forceinline
void I3PRINTF( const FmtRng& fmt, pp_enum_params_cat_params(i_, const A, &, a) )
{
	typedef typename i3::range_value<FmtRng>::type Ch;
	i3::stack_basic_string<i3::mpl::size_t<512>, Ch>		temp_str;	
	i3::sprintf(temp_str, fmt, pp_enum_params(i_, a));
	I3PRINTF(temp_str);
}

#undef i_

#endif // pp_is_iterating