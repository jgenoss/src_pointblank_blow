#pragma once

#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/utility/copy_or_swap.h"
#include "safe_string_copy.h"
#include "detail/utf16_to_utf8_impl2.h"

namespace i3
{
	namespace detail
	{
		I3_EXPORT_BASE void utf16_to_utf8_impl(const wchar_t* input, std::size_t input_size, i3::string& out);
		
		template<class DestSeq>
		struct utf16_to_utf8_dispatch
		{
			template<class WideRange> inline
			static void call( const WideRange& src_rng, DestSeq& dest)
			{
				i3::stack_string tmp_str;
				i3::utf16_to_utf8( src_rng, tmp_str);
				DestSeq tmp(tmp_str.begin(), tmp_str.end());
				i3::copy_or_swap(dest, tmp);
			}
		};
		
		template<>
		struct utf16_to_utf8_dispatch<i3::string>
		{
			template<class WideRange> inline
			static void call(const WideRange& src_rng, i3::string& dest)
			{
				utf16_to_utf8_impl(&*src_rng.begin(), src_rng.size(), dest);
			}
		};
		

		template<class mplSize, class Ch, class Traits, class Alloc>
		struct utf16_to_utf8_dispatch<i3::stack_basic_string<mplSize, Ch, Traits, Alloc> >
		{
			template<class U8Range> inline
			static void call(const U8Range& src_rng, i3::stack_basic_string<mplSize, Ch, Traits, Alloc>& dest)
			{
				utf16_to_utf8_impl2(&*src_rng.begin(), src_rng.size(), dest);
			}
		};

		template<>
		struct utf16_to_utf8_dispatch<i3::rc_string>
		{
			template<class WideRange> inline
			static void call(const WideRange& src_rng, i3::rc_string& dest)
			{
				i3::stack_string tmp_str;
				utf16_to_utf8_impl2(&*src_rng.begin(), src_rng.size(), tmp_str);
				dest = tmp_str;
			}
		};
	}
		
	// 안타깝게도 codepage는 windowsAPI값이 노출된다.
	template<class WideRange, class U8Seq> inline typename i3::disable_if_iterator<U8Seq>::type
	utf16_to_utf8( const WideRange& src, U8Seq& dest)
	{
		detail::utf16_to_utf8_dispatch<U8Seq>::call(as_literal(src), dest);
	}
	
	template<class WideRange, class U8Lit> inline typename i3::enable_if_iterator<U8Lit>::type
	utf16_to_utf8( const WideRange& src, U8Lit dest, std::size_t dest_buff_size)
	{
		typedef typename get_iterator_range_type<const WideRange>::type src_range_type;
		src_range_type src_rng = i3::as_literal(src);
		i3::stack_string tmp_str;
		detail::utf16_to_utf8_impl2(&*src_rng.begin(), src_rng.size(), tmp_str);
		i3::safe_string_copy(dest, tmp_str, dest_buff_size);
	}

}
