#pragma once

#include "mpl_identity.h"

#pragma warning(push, 3)
#pragma warning(disable: 4307)

namespace i3
{
	namespace mpl
	{
		template <class T>
		struct print : mpl::identity<T>
		{
			enum { n = sizeof(T) + -1 };
		};
	}
}

#pragma warning(pop)