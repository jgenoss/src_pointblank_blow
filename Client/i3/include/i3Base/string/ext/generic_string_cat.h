#pragma once

//
// generic_append_range에 구현되어 있다.
//
#include "../../itl/range/generic_append_range.h"
#include "generic_string_size.h"

namespace i3
{
	namespace detail
	{
		template<class Seq, class Range> inline 
		std::size_t generic_string_cat_if_seq(Seq& s, const Range& rng)
		{
			typedef typename i3::range_value<Seq>::type		Ch1;
			typedef typename Range::value_type				Ch2;
			compile_assert_pred( ( i3::is_same<Ch1, Ch2> ) );

			i3::generic_append_range(s, rng);
			return i3::generic_string_size(s);	
		}

		template<class Literal, class Range> inline 
		std::size_t generic_string_cat_if_lit(Literal ctn, const Range& rng)
		{
			typedef typename i3::iterator_value<Literal>::type		Ch1;
			typedef typename Range::value_type						Ch2;
			compile_assert_pred( ( i3::is_same<Ch1, Ch2> ) );


			typedef Literal										output_iterator;
			typedef typename i3::iterator_value<Literal>::type	value_type;

			output_iterator end_it = ctn;
			std::size_t res_len = i3::char_traits<value_type>::length( &(*ctn) );
			i3::advance(end_it, res_len);
		
			end_it = i3::copy( rng.begin(), rng.end(), end_it);
			*end_it = value_type();

			return res_len + rng.size();
		}
	}
	
	template<class Seq, class Range> inline 
	typename i3::disable_if_iterator<Seq, 
	std::size_t>::type generic_string_cat(Seq& s, const Range& rng)
	{
		return detail::generic_string_cat_if_seq(s, i3::as_literal(rng));
	}

	template<class Literal, class Range> inline 
	typename i3::enable_if_iterator<Literal, 
	std::size_t>::type generic_string_cat(Literal ctn, const Range& rng)	
	{
		return detail::generic_string_cat_if_lit(ctn, i3::as_literal(rng));		
	}

}