#pragma once

#include "generic_string_size.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/generic_append_range.h"

namespace i3
{

	namespace detail
	{
		template<class Seq, class FwdRange> 
		std::size_t safe_string_cat_if_seq(Seq& out, const FwdRange& src_rng, size_t out_buff_size)
		{

			typedef typename i3::range_value<Seq>::type			Ch1;
			typedef typename FwdRange::value_type				Ch2;
			compile_assert_pred( ( i3::is_same<Ch1, Ch2> ) );

			std::size_t prev_out_size = i3::generic_string_size(out);

			if (!out_buff_size) return prev_out_size;
			
			size_t req_len = out_buff_size -1;		// 널값에 해당되는 1 제외..

			std::size_t src_len = (std::size_t)src_rng.size();
			
			if (prev_out_size + src_len <= req_len)
			{
				i3::generic_append_range(out, src_rng);
				return prev_out_size + src_len;
			}

			i3::generic_append_range(out, src_rng.begin(), src_rng.begin() + req_len);
			return prev_out_size + req_len;
		}

		template<class It, class FwdRange> 
		std::size_t safe_string_cat_if_lit(It out, const FwdRange& src_rng, size_t out_buff_size)
		{
			typedef typename i3::iterator_value<It>::type		Ch1;
			typedef typename FwdRange::value_type				Ch2;
			compile_assert_pred( ( i3::is_same<Ch1, Ch2> ) );

			std::size_t prev_out_size	= i3::char_traits< Ch1 >::length(&*out);
			
			if (!out_buff_size) return prev_out_size;
			
			size_t req_len = out_buff_size - 1;		// 널값에 해당되는 1 제외..

			out += prev_out_size;

			std::size_t src_len = std::size_t(src_rng.size());

			if (prev_out_size + src_len <= req_len )
			{
				It next = i3::copy(src_rng.begin(), src_rng.end(), out);
				*next = Ch1();	
				return prev_out_size + src_len;
			}

			It next = i3::copy(src_rng.begin(), src_rng.end() + req_len, out);
			*next = Ch1();	
			return prev_out_size + req_len;
		}

	}

	template<class Seq, class FwdRange> inline typename i3::disable_if_iterator<Seq, 
	std::size_t>::type safe_string_cat(Seq& out, const FwdRange& src_rng, size_t out_buff_size)
	{
		return detail::safe_string_cat_if_seq(out, i3::as_literal(src_rng), out_buff_size);
	}

	template<class It, class FwdRange> inline typename i3::enable_if_iterator<It, 
	std::size_t>::type safe_string_cat(It out, const FwdRange& src_rng, size_t out_buff_size)
	{
		return detail::safe_string_cat_if_lit(out, i3::as_literal(src_rng), out_buff_size);
	}
		
}