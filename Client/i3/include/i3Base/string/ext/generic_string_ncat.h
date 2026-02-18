#pragma once

#include "generic_string_size.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/generic_append_range.h"

namespace i3
{
	// ncopy와 비슷하게 생각해서 처리해야될것이다..
	// 리터럴문제때문에, 전체 사이즈를 리턴하도록 한다..
		
	namespace detail
	{
	
		template<class Seq, class FwdRange> 
		std::size_t generic_string_ncat_if_seq(Seq& out, const FwdRange& src_rng, size_t len)
		{
			if (std::size_t(src_rng.size()) < len )
				len = std::size_t(src_rng.size());

			i3::generic_append_range(out, src_rng.begin(), src_rng.begin() + len);

			return i3::generic_string_size(out);
		}
	
		template<class It, class FwdRange> 
		std::size_t generic_string_ncat_if_lit(It out, const FwdRange& src_rng, size_t len)
		{
		
			std::size_t prev_out_size	= i3::char_traits< typename i3::iterator_value<It>::type >::length(&*out);
			out += prev_out_size;

			std::size_t it_rng_size		= std::size_t(src_rng.size());

			if ( it_rng_size < len)
			{
				len = it_rng_size;
				It next = i3::copy(src_rng.begin(), src_rng.end(), out);
				*next = typename i3::iterator_value<It>::type();			
			}
			else
			{
				It next = i3::copy(src_rng.begin(), src_rng.begin() + len, out);
				*next = typename i3::iterator_value<It>::type();
			}
			return prev_out_size + len;
		}
	}

	template<class Seq, class FwdRange> inline typename i3::disable_if_iterator<Seq, 
	std::size_t>::type generic_string_ncat(Seq& out, const FwdRange& src_rng, size_t len)
	{
		return detail::generic_string_ncat_if_seq(out, i3::as_literal(src_rng), len);
	}

	template<class It, class FwdRange> inline typename i3::enable_if_iterator<It, 
	std::size_t>::type generic_string_ncat(It out, const FwdRange& src_rng, size_t len)
	{
		return detail::generic_string_ncat_if_lit(out, i3::as_literal(src_rng), len);
	}


}