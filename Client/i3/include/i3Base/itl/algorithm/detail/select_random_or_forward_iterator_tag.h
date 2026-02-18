#pragma once


#include "../../iterator.h"
#include "../../type_traits/is_same.h"
#include "../../mpl/mpl_if.h"
#include "../../mpl/mpl_and.h"

namespace i3
{
	namespace detail
	{
		template<class It1, class It2>
		struct select_random_or_forward_iterator_tag
		{
			typedef typename i3::iterator_traits<It1>::iterator_category		iter_cat1;
			typedef typename i3::iterator_traits<It2>::iterator_category		iter_cat2;
			typedef typename mpl::if_< 
				mpl::and_<	i3::is_same<iter_cat1, i3::random_access_iterator_tag>,
				i3::is_same<iter_cat1, iter_cat2> >, 
				i3::random_access_iterator_tag, i3::forward_iterator_tag>::type		type;
		};
	}
}
