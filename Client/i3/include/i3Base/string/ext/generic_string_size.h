#pragma once

#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/iterator/iterator_traits.h"

namespace i3
{
	
	template<class Literal> inline typename enable_if_iterator<Literal,
	size_t>::type generic_string_size(Literal i)
	{
		// 
		typedef typename i3::iterator_value<Literal>::type	value_type;
		return	i3::char_traits<value_type>::length(i);
		//		
	}

	template<class Seq> inline typename disable_if_iterator<Seq,
	size_t>::type generic_string_size(const Seq& s)
	{
		return s.size();				// 대부분의 컨테이너가 size()를 갖고 있기 때문에 이것으로 종료..
	}
	
}
