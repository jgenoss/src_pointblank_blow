#if !defined(pp_is_iterating)

#ifndef mpl_vector_h
#define mpl_vector_h

#include "mpl_limit.h"
#include "mpl_na.h"
#include "mpl_vector_n.h"
#include "../preprocessor/repetition/pp_enum_params_with_a_default.h"
#include "../preprocessor/ext/pp_enum_params_spec.h"
#include "../preprocessor/ext/pp_typename_if.h"


namespace i3
{
	namespace mpl
	{
		template< pp_enum_params_with_a_default(mpl_vector_limit, class T, na) >
		struct vector;

		template<pp_enum_params(mpl_vector_limit, class T) >
		struct vector : pp_cat(vector, mpl_vector_limit)<pp_enum_params(mpl_vector_limit, T) >
		{ 
			typedef typename pp_cat(vector, mpl_vector_limit)<pp_enum_params(mpl_vector_limit, T)>::type type;
		};
	}
}

#include	"../preprocessor/pp_iterate.h"

#define		pp_iteration_limits ( 0, pp_dec(mpl_vector_limit) )		// 0 ~39까지는 특화 / 자체재귀..(양개구간임에 유의할것)
#define		pp_filename_1	"mpl_vector.h"
#include	pp_iterate()


#endif	// mpl_vector_h

#else

#define		i_		pp_iteration()

namespace i3
{
	namespace mpl
	{
		template< pp_enum_params(i_, class T) >
		struct vector<pp_enum_params_spec(mpl_vector_limit, i_, T, na) > : pp_cat(vector, i_)<pp_enum_params(i_, T)>
		{ 
			typedef pp_typename_if(i_) pp_cat(vector, i_)<pp_enum_params(i_, T)>::type type;
		};
	}
}


#undef i_

#endif // pp_is_iterating
