#pragma once

// i3String::substr의 일반화 버전..

#include "../../itl/range/detail/enable_if_iterator.h"
#include "generic_string_copy.h"
#include "generic_string_size.h"

namespace i3
{
	
	namespace detail
	{
		template<class LitOrSeq, class SrcRng>
		void substr_impl(LitOrSeq dest, const SrcRng& src, size_t pos, size_t count)
		{
			if (count == 0)
				count = i3::generic_string_size(src) - pos;
			typename i3::range_iterator<const SrcRng>::type beg = i3::literal_begin(src);
			i3::generic_string_copy(dest, beg + pos, beg + pos + count);
		}
	}
	
	template<class DestIt, class SrcRng> inline typename i3::enable_if_iterator<DestIt>::type 
	substr(DestIt dest, const SrcRng& src, size_t pos, size_t count /*= 0*/)
	{
		detail::substr_impl<DestIt, SrcRng>(dest, src, pos, count);
	}
	
	template<class DestSeq, class SrcRng> inline typename i3::disable_if_iterator<DestSeq>::type 
	substr(DestSeq& dest, const SrcRng& src, size_t pos, size_t count /*= 0*/)
	{	
		detail::substr_impl<DestSeq&, SrcRng>(dest, src, pos, count);		
	}

}
