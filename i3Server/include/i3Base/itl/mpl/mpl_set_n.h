#if !defined(pp_is_iterating)

#ifndef mpl_set_n_h
#define mpl_set_n_h


#include "mpl_set_core.h"
#include "mpl_limit.h"

#include	"../preprocessor/pp_iterate.h"


#define		pp_iteration_limits ( 1, mpl_set_limit )		// 0 ~39까지는 특화 / 자체재귀..(양개구간)
#define		pp_filename_1	"mpl_set_n.h"
#include	pp_iterate()

#endif //mpl_set_n_h

#else // pp_is_iterating

#define		i_			pp_iteration()
#define		dec_i_		pp_dec(i_)

namespace i3
{
	namespace mpl
	{
		template<pp_enum_params(i_, class T)>
		struct pp_cat(set, i_) : s_item< pp_cat(T, dec_i_), typename pp_cat(set, dec_i_)< pp_enum_params(dec_i_,T) >::item_ >
		{
			typedef pp_cat(set, i_)	type;
		};
	}
}

#undef		dec_i_
#undef		i_

#endif // pp_is_iterating
