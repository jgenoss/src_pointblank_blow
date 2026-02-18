
#ifndef mpl_arithmetic_op_h
#define mpl_arithmetic_op_h

#include "../mpl_integral_c.h"
#include "mpl_largest_int.h"
#include "../mpl_limit.h"
#endif

// no include guard


#if !defined(aux_op_prefix)
#define aux_op_prefix		aux_op_name
#endif


#include "mpl_numeric_op.h"


namespace i3
{
	namespace mpl
	{
		template<>
		struct aux_op_impl_name<integral_c_tag, integral_c_tag>
		{
			template< class N1, class N2 > 
			struct apply : integral_c< typename aux::largest_int<typename N1::value_type, typename N2::value_type>::type
									, ( N1::value aux_op_token N2::value) >
			{
			};
		};
	}
}

#undef aux_op_tag_name
#undef aux_op_impl_name
#undef aux_op_arity
#undef aux_op_prefix
#undef aux_op_name
#undef aux_op_token
