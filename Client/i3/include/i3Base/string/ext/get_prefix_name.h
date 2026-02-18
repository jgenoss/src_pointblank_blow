#pragma once

// 별로 쓸모는 없어보이지만, GetPrefixName()함수 호환성때문에 처리한다.
// 문자열의 뒤쪽 숫자를 제거해준 문자열을 리턴한다....
//
#include "../../itl/range/as_literal.h"
#include "../../itl/range/algorithm/rfind_not_if.h"
#include "generic_string_copy.h"
#include "../../itl/range/generic_clear.h"

namespace i3
{
	namespace detail
	{
		template<class LitOrSeq, class Rng>
		void get_prefix_name_impl(LitOrSeq dest, const Rng& src)
		{
			typedef i3::range_iterator<Rng>::type			it_type;

			it_type     it     = i3::range::rfind_not_if(src, i3::ptr_fun(isdigit) );
			
			if (it != src.end() )
				i3::generic_string_copy(dest, src.begin(), ++it);
			else
				i3::generic_clear(dest);
		}
	}

	template<class Lit, class Rng> inline typename i3::enable_if_iterator<Lit>::type
	get_prefix_name(Lit dest, const Rng& src)
	{
		typedef typename get_iterator_range_type<const Rng>::type it_rng_type;
		detail::get_prefix_name_impl<Lit, it_rng_type>(dest, i3::as_literal(src) );
	}
	
	template<class Seq, class Rng> inline typename i3::disable_if_iterator<Seq>::type
	get_prefix_name(Seq& dest, const Rng& src)
	{
		typedef typename get_iterator_range_type<const Rng>::type it_rng_type;
		detail::get_prefix_name_impl<Seq&, it_rng_type>(dest, i3::as_literal(src) );
	}
	
}