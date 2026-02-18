#if !defined(pp_is_iterating)

#ifndef mpl_apply_wrap_h
#define mpl_apply_wrap_h

#include "../type_traits/has_apply.h"
#include "mpl_na.h"
#include "mpl_limit.h"


namespace i3
{
	namespace mpl
	{
		template<class F, class Has_apply = typename has_apply<F>::type>
		struct apply_wrap0 : F::template apply<> {};						// false라면 템플릿 메타함수로 가정..

		template<class F> struct apply_wrap0<F, true_> : F::apply {};	// true라면 함수 아닌 타입으로 가정..		
	}

}

#include	"../preprocessor/pp_iterate.h"
#define		pp_iteration_limits ( 1, mpl_arity_limit )		// 1 ~ 10 까지... (양개)
#define		pp_filename_1	"mpl_apply_wrap.h"
#include	pp_iterate()

#endif	// mpl_apply_wrap_h

#else	// pp_is_itrating

#define i_			pp_iteration()

namespace i3
{
	namespace mpl
	{ 
		template<class F, pp_enum_params_1(i_, class T) >			// T1부터 시작..
		struct pp_cat(apply_wrap, i_) : F::template apply< pp_enum_params_1(i_, T) > { };
	}
}

#undef		i_

#endif
