#pragma once

#include "../iterator.h"

namespace i3
{
	template<class It>
	struct iterator_value { typedef typename i3::iterator_traits<It>::value_type	type;   };

	template<class It>
	struct iterator_reference { typedef typename i3::iterator_traits<It>::reference	type;   };

	template<class It>
	struct iterator_pointer { typedef typename i3::iterator_traits<It>::pointer	type;   };

	template<class It>
	struct iterator_difference { typedef typename i3::iterator_traits<It>::difference_type	type;   };

	template<class It>
	struct iterator_category { typedef typename i3::iterator_traits<It>::iterator_category	type;   };

}
