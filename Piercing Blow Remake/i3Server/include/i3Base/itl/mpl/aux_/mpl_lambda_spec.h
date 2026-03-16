
// no include guard and pragma once
#ifndef mpl_lambda_spec_h
#define mpl_lambda_spec_h
#include "../mpl_void.h"
#include "../mpl_lambda_fwd.h"
#include "../mpl_int_fwd.h"
#endif // mpl_lambda_spec_h			// 헤더만 가드를 침..

#define		ls_i			pp_tuple_elem(2, 0, lambda_spec)
#define		name			pp_tuple_elem(2, 1, lambda_spec)

// lambda spec ls_i //

namespace i3
{
	namespace mpl
	{
		template< pp_enum_params_1(ls_i, class T), class Tag >
		struct lambda< name<pp_enum_params_1(ls_i, T)>, Tag> 
		{ 
			typedef false_ is_le;
			typedef name< pp_enum_params_1(ls_i, T) > result_; 
			typedef result_ type;
		};
	}
}


#undef		name
#undef		ls_i
#undef		lambda_spec
