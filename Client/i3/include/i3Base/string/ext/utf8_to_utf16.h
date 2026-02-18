#pragma once

#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/utility/copy_or_swap.h"
#include "safe_string_copy.h"
#include "detail/utf8_to_utf16_impl2.h"

namespace i3
{
	namespace detail
	{
		// 입력인수는 문자열배열로 하고, 출력인수로 wstring을 넘긴다..
		// 이 함수는 일반함수이다..은폐되었어도, 템플릿 함수에 의해 쓰일수 있기 때문에, DLL키워드가 필요함..
		I3_EXPORT_BASE void utf8_to_utf16_impl(const char* input, std::size_t input_size, i3::wstring& out);
		
		template<class DestSeq>
		struct  utf8_to_utf16_dispatch
		{
			template<class U8Range> inline
			static void call( const U8Range& src_rng, DestSeq& dest)
			{
				i3::stack_wstring tmp_wstr;
				i3::utf8_to_utf16(src_rng, tmp_wstr);
				DestSeq tmp(tmp_wstr.begin(), tmp_wstr.end());
				i3::copy_or_swap(dest, tmp);
			}
		};
		
		template<>
		struct  utf8_to_utf16_dispatch<i3::wstring>
		{
			template<class U8Range> inline
			static void call(const U8Range& src_rng, i3::wstring& dest)
			{
				utf8_to_utf16_impl(&*src_rng.begin(), src_rng.size(), dest);
			}
		};
		
		template<class mplSize, class Ch, class Traits, class Alloc>
		struct utf8_to_utf16_dispatch<i3::stack_basic_string<mplSize, Ch, Traits, Alloc> >
		{
			template<class U8Range> inline
			static void call(const U8Range& src_rng, i3::stack_basic_string<mplSize, Ch, Traits, Alloc>& dest)
			{
				utf8_to_utf16_impl2(&*src_rng.begin(), src_rng.size(), dest);
			}
		};
		
		template<>
		struct  utf8_to_utf16_dispatch<i3::rc_wstring>
		{
			template<class U8Range> inline
			static void call(const U8Range& src_rng, i3::rc_wstring& dest)
			{
				i3::stack_wstring tmp_wstr;
				utf8_to_utf16_impl2(&*src_rng.begin(), src_rng.size(), tmp_wstr);
				dest = tmp_wstr;
			}
		};

	}

	// 안타깝게도 codepage는 windowsAPI값이 노출된다.
	template<class U8Range, class WideSeq> inline typename i3::disable_if_iterator<WideSeq>::type
	utf8_to_utf16( const U8Range& src, WideSeq& dest)
	{
		detail::utf8_to_utf16_dispatch<WideSeq>::call(as_literal(src), dest);
	}
	
	template<class U8Range, class WideLit> inline typename i3::enable_if_iterator<WideLit>::type
	utf8_to_utf16( const U8Range& src, WideLit dest, std::size_t dest_buff_size)
	{
		typedef typename get_iterator_range_type<const U8Range>::type src_range_type;
		src_range_type src_rng = i3::as_literal(src);
		i3::stack_wstring tmp_wstr;
		detail::utf8_to_utf16_impl2(&*src_rng.begin(), src_rng.size(), tmp_wstr);
		i3::safe_string_copy(dest, tmp_wstr, dest_buff_size);
	}

}
