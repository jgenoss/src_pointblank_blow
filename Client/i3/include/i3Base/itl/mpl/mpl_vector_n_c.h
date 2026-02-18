#if !defined(pp_is_iterating)

#ifndef mpl_vector_n_c_h
#define mpl_vector_n_c_h

#include "mpl_vector_core.h"
#include "mpl_vector_n.h"



#include	"../preprocessor/pp_iterate.h"


#define		pp_iteration_limits ( 1, mpl_vector_limit )		// 0 ~39까지는 특화 / 자체재귀..(양개구간)
#define		pp_filename_1	"mpl_vector_n_c.h"
#include	pp_iterate()


#endif // mpl_vector_n_c_h

#else	// pp_is_iterating

#define		i_			pp_iteration()

namespace i3
{
	namespace mpl
	{
		#define aux_vector_c_param_func(unused, i_, param) pp_comma_if(i_) integral_c<T,pp_cat(param,i_)> 
    
		template<class T, pp_enum_params(i_, T C)>
		struct pp_cat(pp_cat(vector,i_),_c) : pp_cat(vector,i_)< pp_repeat(i_,aux_vector_c_param_func,C) >
		{
			typedef pp_cat(pp_cat(vector,i_),_c) type;
			typedef T value_type;
		};

		#undef aux_vector_c_param_func
	}
}

#undef		i_

#endif	// pp_is_iterating
