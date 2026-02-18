#pragma once

#include "mpl_na.h"

namespace i3 
{ 
	namespace mpl 
	{
		template<class T = na, int NotLE = 0>
		struct protect : T 
		{
			typedef protect		type;
		};
		
		// na spec 1 : 유일하게 매크로처리 하지 않음..
		template<>
		struct protect<na>
		{
			template< pp_enum_params_1_with_a_partial_default(mpl_arity_limit, 1, class T, na) >		// T1을 제외한 arity전체에 na를 디폴트로 지정..
			struct apply : protect<T1> {};
		};
	} 
}

