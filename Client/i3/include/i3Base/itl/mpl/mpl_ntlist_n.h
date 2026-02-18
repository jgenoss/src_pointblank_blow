#if !defined(pp_is_iterating)

#ifndef mpl_ntlist_n_h
#define mpl_ntlist_n_h


// ntype<T, C0> , ntype<T, C1>, ntype<T, C2> ... 이런식으로...
#include "mpl_list_n.h"
#include "mpl_ntype.h"

#define		pp_iteration_limits ( 1, mpl_list_limit )		// 0 ~39까지는 특화 / 자체재귀..(양개구간)
#define		pp_filename_1	"mpl_ntlist_n.h"
#include	pp_iterate()


#endif // mpl_ntlist_n_h

#else // pp_is_iterating

#define		i_			pp_iteration()

namespace i3
{
	namespace mpl
	{
		
		#define ntlist_enum_ntypes(z, n, d)	ntype<T, pp_cat(C, n)>

		template<class T, pp_enum_params(i_, T C)>
		struct pp_cat(ntlist, i_) : pp_cat(list, i_)< pp_enum(i_, ntlist_enum_ntypes,~) >
		{
			typedef pp_cat(ntlist, i_)	type;
			typedef T					value_type;
		};

		#undef	ntlist_enum_ntypes	

	}
}


#undef		i_

#endif // pp_is_iterating