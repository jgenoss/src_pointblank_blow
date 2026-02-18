#pragma once

#include "mpl_protect.h"

namespace i3
{
	namespace mpl
	{
		
		template<class T>
		struct unprotect { typedef T type; };
		
		template<class T>
		struct unprotect< mpl::protect<T> > { typedef T type; };

	}
}