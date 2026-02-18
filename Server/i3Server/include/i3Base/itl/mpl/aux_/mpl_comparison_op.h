
#ifndef mpl_comparison_op
#define mpl_comparison_op

#include "../mpl_bool.h"

#endif

// no include guard

#if !defined(aux_op_prefix)
#define aux_op_prefix	aux_op_name
#endif

#define aux_op_arity	2

#include "mpl_numeric_op.h"

namespace i3
{
	namespace mpl
	{
		#if ( 0 aux_op_token 1 ) && !( 1 aux_op_token 0 ) && !(0 aux_op_token 0 )
		#define aux_op(N1, N2) ( N2::value > N1::value )
		#else
		#define aux_op(N1, N2) ( N1::value aux_op_token N2::value )
		#endif

		template<>
		struct aux_op_impl_name<integral_c_tag, integral_c_tag>
		{
			template< class N1, class N2 > 
			struct apply : bool_< aux_op(N1, N2) > {};
		};

		#undef aux_op
	}
}

#undef aux_op_tag_name
#undef aux_op_impl_name
#undef aux_op_arity
#undef aux_op_prefix
#undef aux_op_name
#undef aux_op_token
