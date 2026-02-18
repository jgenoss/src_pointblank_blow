#if !defined(pp_is_iterating)

#ifndef bind_list_av
#define bind_list_av

#include "../../preprocessor/ext/pp_repeat_binary_params_1_with_a_cat.h"

#define		pp_iteration_params_1	(3, (1, bind_arity_limit, "bind_list_av.h"))	// 1단계 파일반복..
#include	pp_iterate()

#endif // bind_list_av

#else // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3
{
	namespace bi
	{
		template<pp_enum_params_1(i_, class A)>
		struct pp_cat(list_av_, i_)
		{
			pp_repeat_binary_params_1_with_a_cat(i_, typedef typename add_value<A,>::type B, ;)
			typedef pp_cat(list,i_)<pp_enum_params_1(i_, B)> type;
		};
	}
}

#undef i_

#endif // pp_is_iterating