#pragma once

#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		// if_c 
		template<bool b, class T1, class T2> struct if_c { typedef T1	type; };
		template<class T1, class T2> struct if_c<false, T1, T2> { typedef T2 type; };

		// if_ 
		template<class B = na, class T1 = na, class T2 = na>
		struct if_ 
		{
			typedef typename if_c< bool( B::value ), T1, T2>::type type;
		};

		// eval_if
		template<class C = na, class F1 = na, class F2 = na>
		struct eval_if : if_<C, F1, F2>::type
		{
		};
		
		// eval_if_c
		template<bool c, class F1, class F2>
		struct eval_if_c : if_c<c, F1, F2>::type { };
		

	}

}

#define na_spec		(3, if_)
#include "aux_/mpl_na_spec.h"

#define na_spec		(3, eval_if)
#include "aux_/mpl_na_spec.h"
