#if !defined(pp_is_iterating)

#ifndef function_decomposition_h
#define function_decomposition_h

#include "../function_types/ft_function_arity.h"
#include "../function_types/ft_result_type.h"
#include "../function_types/ft_parameter_types.h"
#include "../mpl/mpl_transform_view.h"

#include "../preprocessor/ext/pp_repeat_binary_params_with_a_cat.h"

namespace i3
{
	//
	// (주의) 이 루틴은 callable검사를 하지 않음...(이것을 상속한 클래스에서 처리하도록 한다..)
	// R, P0~Pn 이외의 다른 정보는 이 시점에서 유용하지 않으므로 모두 생략한다...
	//

	namespace detail
	{
		template< class F, std::size_t Arity, class PT> 
		struct function_decomposition_impl;
		

		template<class ParameterTypes, class ParamTransform>
		struct modify_parameter_types : mpl::transform_view<ParameterTypes, ParamTransform> {};

		template<class ParameterTypes>
		struct modify_parameter_types<ParameterTypes, mpl::identity<mpl::_> > : ParameterTypes {};

	}
	
	template<class F, class ParamTransform = mpl::identity<mpl::_> >
	struct function_decomposition : detail::function_decomposition_impl<F, ft::function_arity<F>::value, ParamTransform > {};

}

#define		pp_iteration_params_1	(3, (0, ft_arity_limit, "function_decomposition.h"))		// 0 ~ 10까지 특화 / 자체재귀..(양개구간임에 유의할것)
#include	pp_iterate()
		
#endif

#else // pp_is_iterating

#define i_				pp_frame_iteration(1)
	
namespace i3
{
	namespace detail
	{
		template<class F, class PT>
		struct function_decomposition_impl<F, i_, PT>
		{	
			typedef F																			function_type;
			typedef typename ft::result_type<F>::type											result_type;
			typedef result_type																	R;
			typedef modify_parameter_types<ft::parameter_types<F>, PT>							parameter_types;
			static const std::size_t															arity = i_;
			
			#define function_decomposition_params(z, n, d)		typedef typename mpl::at_c<parameter_types, n >::type	pp_cat(param, n);
			pp_repeat(i_, function_decomposition_params, ~)
			#undef  function_decomposition_params
			
			pp_repeat_binary_params_with_a_cat(i_, typedef param, P, ; )

		};
	}
	
	//
	// 인수 갯수를 미리 알고있다면, 아래의 함수를 대신 써도 된다..(달리 안정성보장하지 않으므로, 반드시 인수가 일치해야한다...)
	//

	template< class F, class ParamTransform = mpl::identity<mpl::_> >
	struct pp_cat(function_decomposition, i_) : detail::function_decomposition_impl<F, i_, ParamTransform> {};
		
	
}

#undef i_

#endif // pp_is_iterating

