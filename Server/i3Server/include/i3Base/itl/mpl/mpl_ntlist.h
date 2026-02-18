#if !defined(pp_is_iterating)

#ifndef mpl_ntlist_h
#define mpl_ntlist_h

#include "mpl_ntlist_n.h"

namespace i3
{
	namespace mpl
	{
		
		#define ntlist_enum_params(z, n, d)	T pp_cat(C, pp_inc(n) ) = pp_cat(C, n)
		
		template< class T, T C0, pp_enum( pp_dec(mpl_list_limit), ntlist_enum_params,~) >
		struct ntlist;		

		#undef ntlist_enum_params
		
		// ntype<T, C0> , ntype<T, C1>, ntype<T, C2> ... 이런식으로...
		
		
		template<class T, pp_enum_params(mpl_list_limit, T C) >		// 꽉찬 경우..
		struct ntlist : pp_cat(ntlist, mpl_list_limit)<T, pp_enum_params(mpl_list_limit, C)>
		{
			typedef typename pp_cat(ntlist, mpl_list_limit)<T, pp_enum_params(mpl_list_limit, C)>::type type;
		};
		
	}
}

#define		pp_iteration_limits ( 1, pp_dec(mpl_list_limit) )		// 0 ~39까지는 특화 / 자체재귀..(양개구간임에 유의할것)
#define		pp_filename_1	"mpl_ntlist.h"
#include	pp_iterate()

#endif // mpl_ntlist_h

#else // pp_is_iterating

#define		i_		pp_iteration()
#define		dec_i_	pp_dec(i_)

namespace i3
{
	namespace mpl
	{
		template<class T, pp_enum_params(i_, T C)>		
		struct ntlist<T, pp_enum_params_spec(mpl_list_limit, i_, C, pp_cat(C,dec_i_) )> : pp_cat(ntlist, i_)< T, pp_enum_params(i_, C) >
		{
			typedef typename pp_cat(ntlist, i_)< T, pp_enum_params(i_, C) >::type type;
		};
	}
}

#undef		dec_i_
#undef		i_

#endif // pp_is_iterating