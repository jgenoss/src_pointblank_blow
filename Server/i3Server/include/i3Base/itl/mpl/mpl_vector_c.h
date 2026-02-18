#if !defined(pp_is_iterating)

#ifndef mpl_vector_c_h
#define mpl_vector_c_h

#include "mpl_limit.h"
#include "mpl_na.h"
#include "mpl_vector_n_c.h"
#include "../preprocessor/ext/pp_enum_params_spec.h"

namespace i3
{
	namespace mpl
	{
		template< class T, pp_enum_params_with_a_default(mpl_vector_limit, long C, LONG_MAX) >
		struct vector_c;

		template< class T, pp_enum_params(mpl_vector_limit, long C) >
		struct vector_c : pp_cat(pp_cat(vector, mpl_vector_limit),_c)<T, pp_enum_params(mpl_vector_limit, C) >
		{ 
			typedef typename pp_cat(pp_cat(vector, mpl_vector_limit),_c)<T, pp_enum_params(mpl_vector_limit, C) >::type type;
		};
	}
}


#include	"../preprocessor/pp_iterate.h"

#define		pp_iteration_limits ( 0, pp_dec(mpl_vector_limit) )		// 0 ~39까지는 특화 / 자체재귀..(양개구간임에 유의할것)
#define		pp_filename_1	"mpl_vector_c.h"
#include	pp_iterate()


#endif // mpl_vector_c_h

#else

#define	i_		pp_iteration()

	
namespace i3
{
	namespace mpl
	{
		template< class T pp_enum_trailing_params(i_, long C) >
		struct vector_c<T, pp_enum_params_spec(mpl_vector_limit, i_, C, LONG_MAX) > : pp_cat(pp_cat(vector, i_),_c)<T pp_enum_trailing_params(i_, C)>
		{ 
			typedef typename pp_cat(pp_cat(vector, i_),_c)<T pp_enum_trailing_params(i_, C)>::type type;
		};
	}
}


#undef	i_

#endif // pp_is_iterating


