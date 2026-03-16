#if !defined(pp_is_iterating)
#ifndef mpl_quote_h
#define mpl_quote_h

#include "mpl_void.h"
#include "../type_traits/has_type.h"
#include "mpl_limit.h"

namespace i3
{
	namespace mpl
	{
		template<class T, bool Has_Type>
		struct quote_impl : T {};

		template<class T> 
		struct quote_impl< T,false > {	typedef T type;	};
	}
}

#define		pp_iteration_limits ( 1, mpl_arity_limit )		// 1 ~10까지 특화 / 자체재귀..(양개구간임에 유의할것)
#define		pp_filename_1	"mpl_quote.h"
#include	pp_iterate()


#endif
#else

#define		i_			pp_iteration()

namespace i3
{
	namespace mpl 
	{
		template< template <pp_enum_params_1(i_, class P)> class F, class Tag = void_>
		struct pp_cat(quote, i_)
		{
			template< pp_enum_params_1(i_, class U) >
			struct apply : quote_impl< F< pp_enum_params_1(i_, U) >, has_type< F<pp_enum_params_1(i_, U) > >::value >{};
		};
	}
}
#undef		i_
#endif
