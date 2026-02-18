#if !defined(pp_is_iterating)

#ifndef mpl_idx_ntlist_n_h
#define mpl_idx_ntlist_n_h

#include "mpl_list_n.h"
#include "mpl_idx_ntype.h"

// idx_ntype<T, C0> , idx_ntype<T, C1>, idx_ntype<T, C2> ... 이런식으로...

#define idx_ntlist_enum_params1(z, n, d) int pp_cat(N, n), T pp_cat(C, n) 
#define idx_ntlist_enum_params2(z, n, d) idx_ntype<pp_cat(N, n), T, pp_cat(C, n)>
	

#define		pp_iteration_limits ( 1, mpl_list_limit )		// 0 ~39까지는 특화 / 자체재귀..(양개구간)
#define		pp_filename_1	"mpl_idx_ntlist_n.h"
#include	pp_iterate()

#undef idx_ntlist_enum_params2
#undef idx_ntlist_enum_params1

#endif // mpl_idx_ntlist_n_h

#else // pp_is_iterating

#define		i_			pp_iteration()

namespace i3
{
	namespace mpl
	{
		
		template<class T, pp_enum(i_, idx_ntlist_enum_params1,~) >
		struct pp_cat(idx_ntlist, i_) : pp_cat(list, i_)< pp_enum(i_, idx_ntlist_enum_params2,~) >
		{
			typedef pp_cat(idx_ntlist, i_)	type;
			typedef T					value_type;
		};

	}
}


#undef		i_

#endif // pp_is_iterating