#pragma once

//
// crt의 strncpy와 달리 무조건 끝에 널이 추가된다..
// 현재, i3String::NCopy가 이방식으로 쓰여지기 때문에 호환성을 위해 만듬..
//

#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"

namespace i3
{
	template<class Seq, class FwdRange> inline
	typename i3::disable_if_iterator<Seq, std::size_t>::type 
	string_ncopy_nullpad(Seq& out, const FwdRange& src_rng, size_t len)
	{
		typedef typename i3::range_value<Seq>::type		Ch1;
		typedef typename i3::get_iterator_range_type<const FwdRange>::type iter_range_type;
		typedef typename iter_range_type::value_type	Ch2;
		compile_assert_pred( ( i3::is_same<Ch1, Ch2> ) );

		iter_range_type it_rng = i3::as_literal(src_rng);
		if (std::size_t(it_rng.size()) < len )
			len = std::size_t(it_rng.size());
		Seq tmp( it_rng.begin(), it_rng.begin() + len);
		i3::copy_or_swap(out, tmp);
		return len;
	}
		
	template<class It, class FwdRange> inline
	typename i3::enable_if_iterator<It, std::size_t>::type 
	string_ncopy_nullpad(It out, const FwdRange& src_rng, size_t len)
	{
		typedef typename i3::iterator_value<It>::type		Ch1;
		typedef typename i3::get_iterator_range_type<const FwdRange>::type iter_range_type;
		typedef typename iter_range_type::value_type		Ch2;
		compile_assert_pred( ( i3::is_same<Ch1, Ch2> ) );

		iter_range_type it_rng = i3::as_literal(src_rng);
		if ( std::size_t(it_rng.size()) < len)
		{
			len = std::size_t(it_rng.size());
		}
				
		It next = i3::copy(it_rng.begin(), it_rng.begin() + len, out);
		*next = typename i3::iterator_value<It>::type();
		return len;
	}

}