#pragma once

#include "../mpl_apply.h"
#include "../mpl_deref.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< class F, class Iterator>
			struct iter_apply1 : apply1< F,typename deref<Iterator>::type > {};

			template< class F, class Iterator1, class Iterator2>
			struct iter_apply2 : apply2< F, typename deref<Iterator1>::type, typename deref<Iterator2>::type>{};
		}
	}
}
