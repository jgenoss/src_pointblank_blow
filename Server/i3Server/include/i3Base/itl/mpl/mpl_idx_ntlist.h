#if !defined(pp_is_iterating)

#ifndef mpl_idx_ntlist_h
#define mpl_idx_ntlist_h

#include "mpl_idx_ntlist_n.h"

namespace i3
{
	namespace mpl
	{
		
		#define idx_ntlist_enum_params(z, n, d)	int pp_cat(N, pp_inc(n)) = -1, T pp_cat(C, pp_inc(n) ) = pp_cat(C, n)
		
		template< class T, int N0, T C0, pp_enum( pp_dec(mpl_list_limit), idx_ntlist_enum_params,~) >
		struct idx_ntlist;		

		#undef idx_ntlist_enum_params
		
		// idx_ntype<N0, T, C0> , idx_ntype<N1, T, C1>, idx_ntype<N2, T, C2> ... 이런식으로...
		
		#define idx_ntlist_enum_params1(z, n, d) int pp_cat(N, n), T pp_cat(C, n) 
		#define idx_ntlist_enum_params2(z, n, d) pp_cat(N, n), pp_cat(C, n)
		
		template<class T, pp_enum(mpl_list_limit, idx_ntlist_enum_params1, ~) >		// 꽉찬 경우..
		struct idx_ntlist : pp_cat(idx_ntlist, mpl_list_limit)<T, pp_enum(mpl_list_limit, idx_ntlist_enum_params2, ~)>
		{
			typedef typename pp_cat(idx_ntlist, mpl_list_limit)<T, pp_enum(mpl_list_limit, idx_ntlist_enum_params2, ~)>::type type;
		};
		
		
	}
}

#define		pp_iteration_limits ( 1, pp_dec(mpl_list_limit) )		// 0 ~39까지는 특화 / 자체재귀..(양개구간임에 유의할것)
#define		pp_filename_1	"mpl_idx_ntlist.h"
#include	pp_iterate()

#undef idx_ntlist_enum_params2
#undef idx_ntlist_enum_params1

#endif // mpl_idx_ntlist_h

#else // pp_is_iterating

#define		i_		pp_iteration()
#define		dec_i_	pp_dec(i_)
#define		rem_i_	pp_sub(mpl_list_limit, i_)

#define		idx_ntlist_enum_params3(z,n,d)	-1, pp_cat(C, dec_i_) 

namespace i3
{
	namespace mpl
	{
		template<class T, pp_enum(i_, idx_ntlist_enum_params1,~)>		
		struct idx_ntlist<T, pp_enum(i_, idx_ntlist_enum_params2,~), pp_enum(rem_i_, idx_ntlist_enum_params3,~)> 
			: pp_cat(idx_ntlist, i_)< T, pp_enum(i_, idx_ntlist_enum_params2,~) >
		{
			typedef typename pp_cat(idx_ntlist, i_)< T, pp_enum(i_, idx_ntlist_enum_params2,~) >::type type;
		};
	}
}

#undef		idx_ntlist_enum_params3

#undef		rem_i_
#undef		dec_i_
#undef		i_

#endif // pp_is_iterating

