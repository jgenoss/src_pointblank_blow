
#ifndef mpl_shift_op_h
#define mpl_shift_op_h

#include "../mpl_integral_c.h"

#endif

#if !defined(aux_op_prefix)
#define aux_op_prefix aux_op_name
#endif

#define aux_op_arity	2

#include "mpl_numeric_op.h"


namespace i3
{
	namespace mpl
	{
		template<>
		struct aux_op_impl_name<integral_c_tag,integral_c_tag>
		{
			template< class N, class S > 
			struct apply : integral_c< typename N::value_type, ( N::value aux_op_token S::value ) > {};
		};
	}
}


#undef aux_op_tag_name
#undef aux_op_impl_name
#undef aux_op_arity
#undef aux_op_prefix
#undef aux_op_name
#undef aux_op_token
