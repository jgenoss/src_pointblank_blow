#pragma once

#include "mpl_not.h"


namespace i3
{
	namespace mpl
	{
		template< class T1 >
		struct same_as { template< class T2 > struct apply : is_same<T1,T2> {}; };

		template< class T1 >
		struct not_same_as { template< class T2 > struct apply : not_< is_same<T1,T2> >{}; };
	}
}
