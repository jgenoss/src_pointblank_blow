#if !defined(pp_is_iterating)


#ifndef mpl_numeric_op_h
#define mpl_numeric_op_h

#include "../preprocessor/ext/pp_enum_params_1_spec.h"

#include "../mpl_numeric_cast.h"
#include "../mpl_apply_wrap.h"
#include "../mpl_if.h"
#include "../mpl_tag.h"
#include "../mpl_na.h"
#include "../mpl_limit.h"

#endif

// 타헤더 파일 참조이외에는
// 인클루드 가드 처리 하지 않음..

#if !defined(aux_op_arity)
#define aux_op_arity		mpl_arity_limit
#endif

#if !defined(aux_op_impl_name)
#define aux_op_impl_name	pp_cat(aux_op_prefix, _impl)
#endif

#if !defined(aux_op_tag_name)
#define aux_op_tag_name		pp_cat(aux_op_prefix, _tag)
#endif

namespace i3
{
	namespace mpl
	{
		template<class Tag1, class Tag2>
		struct aux_op_impl_name : if_c<(Tag1::value > Tag2::value), 
											aux::cast2nd_impl<aux_op_impl_name<Tag1, Tag1>, Tag1, Tag2>,
											aux::cast1st_impl<aux_op_impl_name<Tag2, Tag2>, Tag1, Tag2> >::type {};
		
		template<> struct aux_op_impl_name<na,na>
		{
			template< class U1, class U2 > struct apply 
			{
				typedef apply type;	static const int value = 0;
			};
		};

		template< class Tag > struct aux_op_impl_name<na,Tag>
		{
			template< class U1, class U2 > struct apply 
			{
				typedef apply type;	static const int value = 0;
			};
		};

		template< class Tag > struct aux_op_impl_name<Tag,na>
		{
			template< class U1, class U2 > struct apply 
			{
				typedef apply type;	static const int value = 0;
			};
		};

		template< class T > struct aux_op_tag_name {	typedef typename T::tag type;  };
		
		
#if		aux_op_arity != 2

#define numeric_op_identity_func(unused1, unused2, x) x
#define numeric_op_right_operand(unused, i, n) , pp_cat(n, pp_add(i, 2))>
#define numeric_op_n_calls(i, n) pp_repeat(pp_dec(i), numeric_op_identity_func, aux_op_name< ) N1 pp_repeat(pp_sub(i, 1), numeric_op_right_operand, n )

		template<class N1 = na, class N2 = na, pp_enum_params_1_from_to_with_a_default(2, mpl_arity_limit, class N, na) >
		struct aux_op_name : numeric_op_n_calls( aux_op_arity, N ) {};

#define  pp_iteration_params_1 (3, (pp_dec(aux_op_arity), 2, "mpl_numeric_op.h") )			// 감소방향..
#include pp_iterate()
		
#undef   numeric_op_n_calls
#undef	 numeric_op_right_operand
#undef	 numeric_op_identity_func

#else	// aux_op_arity == 2
		template<class N1 = na, class N2 = na>
		struct aux_op_name
#endif
			: aux_op_impl_name< typename aux_op_tag_name<N1>::type, typename aux_op_tag_name<N2>::type >::template apply<N1, N2>::type
		{
		};
	}
}

#define na_spec (2, aux_op_name)
#include "mpl_na_spec.h"

#else // pp_is_iterating

#define i_			pp_frame_iteration(1)

		template< pp_enum_params_1(i_, class N)>
		struct aux_op_name< pp_enum_params_1_spec(mpl_arity_limit, i_, N, na) >
#if		i_ != 2
			: numeric_op_n_calls( i_, N )
		{
		};
#endif // i_ != 2

#undef i_

#endif // pp_is_iterating
