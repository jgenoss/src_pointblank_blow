#if !defined(pp_is_iterating)

#ifndef bind_apply_h
#define bind_apply_h

namespace i3
{
	template<class R>
	struct apply
	{
		typedef R		result_type;
		
		template<class F> result_type operator()(F& f) const { return f(); }

		#define		pp_iteration_params_1	(3, (1, bind_arity_limit, "apply.h"))				// 1단계 파일반복..
		#include	pp_iterate()

	};
}

#endif // bind_apply_h

#else // pp_is_iterating

#define i_		pp_frame_iteration(1)

		template<class F, pp_enum_params_1(i_, class A)>
		result_type operator()(F& f, pp_enum_binary_params_1(i_, A,& a)) const 
		{
			return f(pp_enum_params_1(i_, a));
		}

#undef i_

#endif // pp_is_iterating