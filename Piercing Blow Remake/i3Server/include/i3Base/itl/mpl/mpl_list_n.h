#if !defined(pp_is_iterating)

#ifndef mpl_list_n_h
#define mpl_list_n_h


#include "mpl_list_core.h"

#include "../preprocessor/repetition/pp_enum_shifted_params.h"

namespace i3 
{	
	namespace mpl
	{

		template<class T = na> struct list0;
		template<> struct list0<na> : l_end { typedef l_end type; };

		template<class T0>
		struct list1 : l_item< long_<1>, T0, l_end> { typedef list1	type; };
				
	}
}


#include	"../preprocessor/pp_iterate.h"


#define		pp_iteration_limits ( 2, mpl_list_limit )		// 0 ~39까지는 특화 / 자체재귀..(양개구간)
#define		pp_filename_1	"mpl_list_n.h"
#include	pp_iterate()


#endif

#else

#define		i_			pp_iteration()

namespace i3
{
	namespace mpl
	{
		template< pp_enum_params(i_, class T) >
		struct pp_cat(list, i_) : l_item< long_<i_>, T0, pp_cat(list, pp_dec(i_))< pp_enum_shifted_params(i_, T) > >
		{
			typedef pp_cat(list, i_) type;
		};
	}
}

#undef		i_

#endif
