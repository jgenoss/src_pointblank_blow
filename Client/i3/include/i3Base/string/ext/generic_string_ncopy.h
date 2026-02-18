#pragma once

// NCopy일반화..

#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "generic_string_copy.h"

namespace i3
{
	
	template<class Seq> inline
	typename i3::disable_if_iterator<Seq, std::size_t>::type 
	generic_string_ncopy(Seq& out, const Seq& src_rng, size_t len)
	{
		if (std::size_t(src_rng.size()) < len )
		{
			len = std::size_t(src_rng.size());
			out = src_rng;
		}
		else
		{
			i3::generic_string_copy(out, i3::begin(src_rng), i3::begin(src_rng) + len);
		}
		return len;
	}

	namespace detail
	{

		template<class Seq, class FwdRange> inline
		std::size_t generic_string_ncopy_if_seq(Seq& out, const FwdRange& src_rng, size_t len)
		{
			if (std::size_t(src_rng.size()) < len )
				len = std::size_t(src_rng.size());

			i3::generic_string_copy(out, src_rng.begin(), src_rng.begin() + len);
			return len;
		}
	
		// 반복자 입력의 경우는 strncpy의 규칙을 일단 따르기로 한다..
		// 충분이 공간이 확보된 상태에서 작게 카피하면 널이 붙고..
		// 꽉차거나 소스보다 공간이 작으면 널이 붙지 않는다..
		//

		template<class It, class FwdRange> inline
		std::size_t generic_string_ncopy_if_lit(It out, const FwdRange& src_rng, size_t len)
		{
			if ( std::size_t(src_rng.size()) < len)
			{
				len = std::size_t(src_rng.size());
				It next = i3::copy(src_rng.begin(), src_rng.end(), out);
				*next = typename i3::iterator_value<It>::type();			// 공간이 넉넉하면 널이 붙고..그렇지 않으면 안붙는다..
			}
			else
			{
				i3::copy(src_rng.begin(), src_rng.begin() + len, out);
			}
			return len;
		}
	}

	template<class Seq, class FwdRange> inline
	typename i3::disable_if_iterator<Seq, 
	std::size_t>::type generic_string_ncopy(Seq& out, const FwdRange& src_rng, size_t len)
	{
		return detail::generic_string_ncopy_if_seq(out, i3::as_literal(src_rng), len);
	}
	
	template<class It, class FwdRange> inline
	typename i3::enable_if_iterator<It, 
	std::size_t>::type generic_string_ncopy(It out, const FwdRange& src_rng, size_t len)
	{
		return detail::generic_string_ncopy_if_lit(out, i3::as_literal(src_rng), len);
	}

	
}