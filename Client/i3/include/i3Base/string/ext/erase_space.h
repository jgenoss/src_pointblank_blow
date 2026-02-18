#pragma once

#include "../../itl/range/algorithm/remove_copy.h"
#include "../../itl/range/generic_size.h"
#include "../../itl/range/iterator_range.h"
#include "../../itl/range/value_type.h"

namespace i3
{
	
	template<class Rng, class OutIt> inline
	typename enable_if_iterator<OutIt, size_t>::type 
	erase_space(const Rng& in, OutIt out)
	{
		typedef typename i3::get_iterator_range_type<const Rng>::type rng_type;
		typedef typename i3::range_value<rng_type>::type in_value_type;
			
		OutIt it_end = i3::range::remove_copy(in, out, in_value_type(' ') );
		return i3::distance(out, it_end);
	}
	
	template<class Rng, class OutSeq> inline 
	typename disable_if_iterator<OutSeq, size_t>::type 
	erase_space(const Rng& in, OutSeq& out)
	{
		typedef typename i3::get_iterator_range_type<const Rng>::type rng_type;
		typedef typename i3::range_value<rng_type>::type in_value_type;
		
		typedef typename i3::range_iterator<OutSeq>::type	out_iterator;
		i3::range::remove_copy(in, out, in_value_type(' ') );
		return i3::generic_size(out);
	}
	
}