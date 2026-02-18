#pragma once

#include "mpl_bool.h"


namespace i3
{
	namespace mpl
	{
		struct na {	typedef na	type;	enum { value = 0 };	};
		
		template<class T> struct is_na : false_ {};
		template<>	struct is_na<na> : true_ {};
		template<class T> struct is_not_na : true_ {};
		template<>	struct is_not_na<na> : false_ {};

		template< class T, class U > struct if_na {	typedef T type; };
		template< class U > struct if_na<na,U> {	typedef U type;	};
	}

}
