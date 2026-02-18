#if !defined(pp_is_iterating)

#ifndef mpl_list_n_c_h
#define mpl_list_n_c_h

#include "mpl_list_core.h"
#include "mpl_integral_c.h"

namespace i3
{
	namespace mpl
	{
		template<class T> struct list0_c : l_end 
		{
			typedef l_end type;	typedef T value_type;
		};
		
		template<class T, T C0>
		struct list1_c : l_item<long_<1>, integral_c<T, C0>, l_end> 
		{
			typedef list1_c type; typedef T value_type;
		};
	}
}


#include	"../preprocessor/pp_iterate.h"


#define		pp_iteration_limits ( 2, mpl_list_limit )		// 0 ~39까지는 특화 / 자체재귀..(양개구간)
#define		pp_filename_1	"mpl_list_n_c.h"
#include	pp_iterate()


#endif //mpl_list_n_c_h

#else

#define		i_			pp_iteration()

namespace i3
{
	namespace mpl
	{
		template<class T, pp_enum_params(i_, T C)>
		struct pp_cat(pp_cat(list, i_), _c) : l_item<	long_<i_>, 
														integral_c<T, C0>, 
														pp_cat(pp_cat(list, pp_dec(i_)),_c)<T, pp_enum_shifted_params(i_, C)> >
		{
			typedef pp_cat(pp_cat(list, i_), _c) type;
			typedef T value_type;
		};
	}
}

#undef		i_

#endif