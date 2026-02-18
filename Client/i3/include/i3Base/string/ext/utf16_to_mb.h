#pragma once

#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/utility/copy_or_swap.h"
#include "../../i3String.h"
#include "curr_cp.h"

#include "detail/wide_to_mb_impl2.h"
#include "safe_string_copy.h"

namespace i3
{
	namespace detail
	{
		I3_EXPORT_BASE void wide_to_mb_impl(unsigned int codepage, const wchar_t* input, std::size_t input_size, 
			i3::string& out);
		
		template<class DestSeq>
		struct wide_to_mb_dispatch
		{
			template<class WideRange> inline
			static void call( const WideRange& src_rng, DestSeq& dest, unsigned int codepage)
			{
				i3::stack_string tmp_str;
				i3::utf16_to_mb(src_rng, tmp_str, codepage);
				DestSeq tmp(tmp_str.begin(), tmp_str.end());
				i3::copy_or_swap(dest, tmp);
			}
		};
		
		template<>
		struct wide_to_mb_dispatch<i3::string>
		{
			template<class WideRange> inline
			static void call(const WideRange& src_rng, i3::string& dest, unsigned int codepage)
			{
				wide_to_mb_impl(codepage, &*src_rng.begin(), src_rng.size(), dest);
			}
		};
		
		template<class mplSize, class Ch, class Traits, class Alloc>
		struct wide_to_mb_dispatch<i3::stack_basic_string<mplSize, Ch, Traits, Alloc> >
		{
			template<class WideRange> inline
			static void call(const WideRange& src_rng, i3::stack_basic_string<mplSize, Ch, Traits, Alloc>& dest, 
				unsigned int codepage)
			{
				wide_to_mb_impl2(codepage, &*src_rng.begin(), src_rng.size(), dest);
			}
		};
		
		template<>
		struct wide_to_mb_dispatch<i3::rc_string>
		{
			template<class WideRange> inline
			static void call(const WideRange& src_rng, i3::rc_string& dest, unsigned int codepage)
			{
				i3::stack_string tmp_str;
				wide_to_mb_impl2(codepage, &*src_rng.begin(), src_rng.size(), tmp_str);
				dest = tmp_str;
			}
		};
	}
		
	// 안타깝게도 codepage는 windowsAPI값이 노출된다.
	template<class WideRange, class MBSeq> inline typename i3::disable_if_iterator<MBSeq>::type
	utf16_to_mb( const WideRange& src, MBSeq& dest, unsigned int codepage /*= curr_cp()*/ )
	{
		detail::wide_to_mb_dispatch<MBSeq>::call(as_literal(src), dest, codepage);
	}
	
	template<class WideRange, class MBLit> inline typename i3::enable_if_iterator<MBLit>::type
	utf16_to_mb( const WideRange& src, MBLit dest, std::size_t dest_buff_size, unsigned int codepage /*= curr_cp()*/ )
	{
		typedef typename get_iterator_range_type<const WideRange>::type src_range_type;
		src_range_type src_rng = i3::as_literal(src);
		i3::stack_string tmp_str;
		detail::wide_to_mb_impl2(codepage, &*src_rng.begin(), src_rng.size(), tmp_str);
		i3::safe_string_copy(dest, tmp_str, dest_buff_size);
	}

}
