#pragma once

#include "detail/enable_if_iterator.h"
#include "empty.h"

namespace i3
{
	template<class It> inline typename enable_if_iterator<It, 
	bool>::type generic_empty(It i)
	{
		typedef typename i3::iterator_value<It>::type	value_type;
		return *i == value_type();
	}
	
	template<class Seq> inline typename disable_if_iterator<Seq, 
	bool>::type generic_empty(const Seq& s)
	{
		return i3::empty(s);
	}

}
