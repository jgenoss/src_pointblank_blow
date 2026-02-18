#if !defined(pp_is_iterating)

#ifndef mpl_set_n_c_h
#define mpl_set_n_c_h

#include "mpl_set_core.h"
#include "mpl_set_n.h"

#include "mpl_limit.h"

#include	"../preprocessor/pp_iterate.h"

#define		pp_iteration_limits ( 1, mpl_set_limit )		// 0 ~39까지는 특화 / 자체재귀..(양개구간)
#define		pp_filename_1	"mpl_set_n_c.h"
#include	pp_iterate()


#endif //mpl_set_n_c_h

#else // pp_is_iterating

#define		i_			pp_iteration()
#define		dec_i_		pp_dec(i_)

namespace i3
{
	namespace mpl
	{
		template<class T, pp_enum_params(i_, T C) >
		struct pp_cat(pp_cat(set,i_),_c) : s_item<	integral_c<T, pp_cat(C,dec_i_)>, 
													pp_cat(pp_cat(set,dec_i_),_c)<T pp_enum_trailing_params(dec_i_,C)> >
		{
			typedef pp_cat(pp_cat(set,i_),_c) type;	
		};
		
	}
}

#undef		dec_i_
#undef		i_

#endif //pp_is_iterating
