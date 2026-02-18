#pragma once

//
// generic_string_ncopy로 모두 처리하려다가, 문제가 되어 함수를 재작성해서 다시 적용..
// 버퍼사이즈보다 하나 작은 것이 글자수 제한이 된다..(세번째인수가 널값이 포함된 사이즈임)
// i3String::Copy를 똑같이 대체해주는 루틴이다...(i3::generic_string_ncopy와 버퍼사이즈 관리와 널
// 널값 처리부분외에는 큰 차이가 나지 않음)
//

#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "generic_string_copy.h"

namespace i3
{
	// 입력과 출력 인수 타입이 모두 같은 경우를 분기한다.

	template<class Seq> inline
	typename i3::disable_if_iterator<Seq, std::size_t>::type 
	safe_string_copy(Seq& out, const Seq& src_rng, size_t out_buff_size)
	{
		if (!out_buff_size)	return 0;
		size_t len = --out_buff_size;
		
		if ( src_rng.size() < len )
		{
			len = std::size_t(src_rng.size());
			out = src_rng;
		}
		else
		{
			return i3::generic_string_copy(out, i3::begin(src_rng), i3::begin(src_rng) + len);
		}

		return len;
	}

	template<class Seq, class FwdRange> inline
	typename i3::disable_if_iterator<Seq, std::size_t>::type 
	safe_string_copy(Seq& out, const FwdRange& src_rng, size_t out_buff_size)
	{
		typedef typename i3::range_value<Seq>::type		Ch1;
		typedef typename i3::get_iterator_range_type<const FwdRange>::type iter_range_type;
		typedef typename iter_range_type::value_type	Ch2;

		compile_assert_pred( ( i3::is_same<Ch1, Ch2> ) );

		if (!out_buff_size)	return 0;
		
		size_t len = --out_buff_size;

		iter_range_type it_rng = i3::as_literal(src_rng);
		if (std::size_t(it_rng.size()) < len )
			len = std::size_t(it_rng.size());

		return i3::generic_string_copy(out, it_rng.begin(), it_rng.begin() + len);
	}
	
	
	template<class It, class FwdRange> inline
	typename i3::enable_if_iterator<It, std::size_t>::type 
	safe_string_copy(It out, const FwdRange& src_rng, size_t out_buff_size)
	{
		typedef typename i3::iterator_value<It>::type		Ch1;
		typedef typename i3::get_iterator_range_type<const FwdRange>::type iter_range_type;
		typedef typename iter_range_type::value_type		Ch2;
		compile_assert_pred( ( i3::is_same<Ch1, Ch2> ) );

		if (!out_buff_size)	return 0;

		size_t len = --out_buff_size;
		iter_range_type it_rng = i3::as_literal(src_rng);
		if ( std::size_t(it_rng.size()) < len)
		{
			len = std::size_t(it_rng.size());
		}
		
		It next = i3::copy(it_rng.begin(), it_rng.begin() + len, out);
		*next = typename i3::iterator_value<It>::type();	
		
		return len;
	}
		

	template<class Seq, class FwdIt> inline
	typename i3::disable_if_iterator<Seq, std::size_t>::type 
	safe_string_copy(Seq& out, FwdIt src_beg, FwdIt src_end, size_t out_buff_size)
	{
		typedef typename i3::range_value<Seq>::type		Ch1;
		typedef typename i3::iterator_value<FwdIt>::type	Ch2;
		compile_assert_pred( ( i3::is_same<Ch1, Ch2> ) );

		if (!out_buff_size)	return 0;

		size_t len = --out_buff_size;

		const size_t input_len = i3::distance(src_beg, src_end);
		if (input_len < len )
			len = input_len;

		return i3::generic_string_copy(out, src_beg, src_beg + len);
	}


	template<class It, class FwdIt> inline
	typename i3::enable_if_iterator<It, std::size_t>::type 
	safe_string_copy(It out, FwdIt src_beg, FwdIt src_end, size_t out_buff_size)
	{
		typedef typename i3::iterator_value<It>::type		Ch1;
		typedef typename i3::iterator_value<FwdIt>::type	Ch2;
		compile_assert_pred( ( i3::is_same<Ch1, Ch2> ) );

		if (!out_buff_size)	return 0;

		size_t len = --out_buff_size;

		const size_t input_len = i3::distance(src_beg, src_end);
		if ( input_len < len)
			len = input_len;
		
		It next = i3::copy(src_beg, src_beg + len, out);
		*next = typename i3::iterator_value<It>::type();	

		return len;
	}

}