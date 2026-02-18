#if !defined(pp_is_iterating)

// mpl 람다를 사용해 좀더 일반화..

#ifndef is_template_same_h
#define is_template_same_h

#include "template_arity.h"
#include "../mpl/mpl_unprotect.h"
#include "../mpl/mpl_apply_wrap.h"
#include "../mpl/mpl_lambda.h"
#include "../enable_if.h"
#include "../mpl/mpl_if.h"
#include "../type_traits/is_same.h"
#include "../type_traits/is_class.h"

namespace i3
{
	namespace detail
	{
		template<class LE, class D, int Arity>
		struct is_template_same_test 
		{
			static no_type	test(...);
		};

		template<class LE, class D, int Arity>
		struct is_check_template_same
		{
			static const D&			MakeFrom();
			static const bool value = sizeof(yes_type) == sizeof( is_template_same_test<LE, D, Arity>::test(MakeFrom()) );
			typedef mpl::bool_<value> type;
		};



		template<class L, class D>
		struct is_template_same_impl
		{
			typedef typename  mpl::lambda<L>::type			lambda_type;	// 컨테이너의 잘못된 인스턴스화를 예방하기 위해, 람다로 계산..
			static const int	max_arity = template_arity< typename mpl::unprotect<lambda_type>::type >::value - 1;	// D로 유추가 어렵다..
			typedef typename mpl::eval_if_c< (max_arity > 0) , is_check_template_same<lambda_type, D, max_arity>, is_same<L,D> >::type type;
		};
	}	

	template<class BaseTemplate, class Target >
	struct is_template_same : mpl::and_< is_class_or_union<Target>, detail::is_template_same_impl<BaseTemplate, Target> > {};
}

#define pp_iteration_params_1 (3, (1, mpl_arity_limit, "is_template_same.h" ) )
#include pp_iterate()


#endif // is_template_same_h

#else // pp_is_iterating

#define i_					pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
		template< class LE, class D>
		struct pp_cat(template_same_result,i_)				// i_는 자리표의 수..
		{
			template<pp_enum_params(i_, class A)>
			struct apply : is_same< typename pp_cat(mpl::apply_wrap, i_)< LE, pp_enum_params(i_, A) >::type, D > {};
		};

		template<class LE, class D>
		struct is_template_same_test<LE, D, i_>
		{
			template< template<pp_enum_datas(i_,class)> class F, pp_enum_params(i_, class U)>
			static yes_type test( const F< pp_enum_params(i_,U) >&, 
				typename enable_if< typename pp_cat(mpl::apply_wrap,i_)< pp_cat(template_same_result, i_)<LE, D>, pp_enum_params(i_,U)>::type, int>::type = 0);

			static no_type	test(...);
		};
	}
}



#undef i_

#endif // pp_is_iterating