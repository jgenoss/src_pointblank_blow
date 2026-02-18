#if !defined(pp_is_iterating)
#ifndef mpl_lambda_h
#define mpl_lambda_h

#include "mpl_lambda_fwd.h"
#include "mpl_bind.h"
#include "mpl_protect.h"
#include "mpl_quote.h"
#include "mpl_arg.h"
#include "mpl_bool.h"
#include "mpl_int_fwd.h"
#include "mpl_na.h"
#include "mpl_limit.h"

namespace i3
{
	namespace mpl
	{

		namespace aux
		{
			template< pp_enum_params_1_with_a_default(mpl_arity_limit , bool C, false) >
			struct lambda_or : true_ {};

			template<> struct lambda_or< pp_enum_datas(mpl_arity_limit, false) > : false_ {};
		}

		template<class T, class Tag>
		struct lambda 
		{
			typedef false_		is_le;
			typedef T			result_;			// boost 호환성..
			typedef T			type;
		};
		
		template<class T> struct is_lambda_expression : lambda<T>::is_le {};
		
		template< int N, class Tag>
		struct lambda< arg<N>, Tag>
		{
			typedef true_					is_le;
			typedef arg<N>					result_;
			typedef protect<result_>		type;
		};
		
		template< int N, class Tag>
		struct lambda< const arg<N>, Tag>
		{
			typedef true_					is_le;
			typedef const arg<N>			result_;
			typedef protect<result_>		type;
		};

	}

}

#define		pp_iteration_params_1	(3, (0, mpl_arity_limit, "mpl_lambda.h"))				// 1단계 파일반복..
#include	pp_iterate()

namespace i3
{
	namespace mpl
	{
		/// special case for 'protect'
		template< class T, class Tag >
		struct lambda< mpl::protect<T>, Tag >
		{
			typedef false_			is_le;
			typedef mpl::protect<T> result_;
			typedef result_			type;
		};
		
		/// specializations for the main 'bind' form
		template<class F, pp_enum_params_1(mpl_arity_limit, class T), typename Tag>
		struct lambda<bind< F, pp_enum_params_1(mpl_arity_limit, T)>, Tag>
		{
			typedef false_											is_le;
			typedef bind< F, pp_enum_params_1(mpl_arity_limit, T) > result_;
			typedef result_											type;
		};

		template<class F, class Tag1, class Tag2>
		struct lambda<lambda< F,Tag1 >, Tag2>
		{
			typedef lambda< F,Tag2 > l1;
			typedef lambda< Tag1,Tag2 > l2;
			typedef typename l1::is_le is_le;
			typedef aux::le_result2<is_le, Tag2, mpl::lambda, l1, l2> le_result_;
			typedef typename le_result_::result_ result_;
			typedef typename le_result_::type type;
		};
		
	}
}


#define na_spec (2, lambda)
#include "aux_/mpl_na_spec.h"



#endif
#else

#if			pp_iteration_depth() == 1

#define		i_			pp_frame_iteration(1)


#if	i_ > 0

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< class IsLE, class Tag, template<pp_enum_params_1(i_, class P)> class F, pp_enum_params_1(i_, class L)>
			struct pp_cat(le_result, i_)
			{
				typedef F< pp_enum_params_1_with_a_cat(i_, typename L, ::type)> result_;
				typedef result_		type;
			};

			template< class Tag, template<pp_enum_params_1(i_, class P)> class F, pp_enum_params_1(i_, class L)>
			struct pp_cat(le_result, i_)<true_, Tag, F, pp_enum_params_1(i_, L)>
			{
				typedef pp_cat(bind, i_)< pp_cat(quote, i_)<F, Tag>, pp_enum_params_1_with_a_cat(i_, typename L, ::result_)> result_;
				typedef protect<result_>	type;
			};
		}
		
		template< template< pp_enum_params_1(i_, class P) > class F, pp_enum_params_1(i_, class T), class Tag>
		struct lambda< F<pp_enum_params_1(i_, T)>, Tag>
		{
			#define pp_iteration_params_2	(3, (1, i_, "mpl_lambda.h"))		
			#include pp_iterate()
			
			typedef typename aux::lambda_or< pp_enum_params_1_with_a_cat(i_, is_le, ::value) >::type		is_le;
			
			typedef pp_cat(aux::le_result, i_)<is_le, Tag, F, pp_enum_params_1(i_, l)>					le_result_;
			
			typedef typename le_result_::result_		result_;
			typedef typename le_result_::type			type;
		};
	}
}

#endif		// i_ > 0

namespace i3
{
	namespace mpl 
	{
		template< class F pp_enum_trailing_params_1(i_, class T), class Tag>
		struct lambda< pp_cat(bind, i_)<F pp_enum_trailing_params_1(i_, T)>, Tag>
		{
			typedef false_	is_le;
			typedef pp_cat(bind, i_)< F pp_enum_trailing_params_1(i_, T)> result_;
			typedef result_		type;
		};
	}
}

#undef i_

#elif			pp_iteration_depth() == 2
	
#define		j_			pp_frame_iteration(2)

	typedef	lambda< pp_cat(T, j_), Tag >	pp_cat(l, j_);
	typedef typename pp_cat(l, j_)::is_le	pp_cat(is_le, j_);

#undef j_
#endif	//pp_iteration_depth
#endif	// pp_is_iterating