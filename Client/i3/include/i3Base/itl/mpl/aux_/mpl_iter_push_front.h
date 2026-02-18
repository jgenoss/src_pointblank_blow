#pragma once

#include "../mpl_push_front.h"
#include "../mpl_deref.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<class Seq, class Iterator>
			struct iter_push_front
			{
				typedef typename push_front<Seq, typename deref<Iterator>::type>::type type;
			};
		}
	}
}
