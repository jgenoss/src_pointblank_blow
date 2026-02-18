#if !defined(pp_is_iterating)

#ifndef mpl_ntvector_h
#define mpl_ntvector_h

#include "mpl_ntvector_n.h"

namespace i3
{
	namespace mpl
	{
		
		#define ntvector_enum_params(z, n, d)	T pp_cat(C, pp_inc(n) ) = pp_cat(C, n)
		
		template< class T, T C0, pp_enum( pp_dec(mpl_vector_limit), ntvector_enum_params,~) >
		struct ntvector;		

		#undef ntvector_enum_params
		
		// ntype<T, C0> , ntype<T, C1>, ntype<T, C2> ... 이런식으로...
		
		
		template<class T, pp_enum_params(mpl_vector_limit, T C) >		// 꽉찬 경우..
		struct ntvector : pp_cat(ntvector, mpl_vector_limit)<T, pp_enum_params(mpl_vector_limit, C)>
		{
			typedef typename pp_cat(ntvector, mpl_vector_limit)<T, pp_enum_params(mpl_vector_limit, C)>::type type;
		};
		
	}
}

#define		pp_iteration_limits ( 1, pp_dec(mpl_vector_limit) )		// 0 ~39까지는 특화 / 자체재귀..(양개구간임에 유의할것)
#define		pp_filename_1	"mpl_ntvector.h"
#include	pp_iterate()

#endif // mpl_ntvector_h

#else // pp_is_iterating

#define		i_		pp_iteration()
#define		dec_i_	pp_dec(i_)

namespace i3
{
	namespace mpl
	{
		template<class T, pp_enum_params(i_, T C)>		
		struct ntvector<T, pp_enum_params_spec(mpl_vector_limit, i_, C, pp_cat(C,dec_i_) )> : pp_cat(ntvector, i_)< T, pp_enum_params(i_, C) >
		{
			typedef typename pp_cat(ntvector, i_)< T, pp_enum_params(i_, C) >::type type;
		};
	}
}

#undef		dec_i_
#undef		i_

#endif // pp_is_iterating