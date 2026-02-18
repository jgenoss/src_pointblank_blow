#pragma once

#include "utf8_to_utf16.h"
#include "utf16_to_mb.h"
#include "curr_cp.h"
#include "safe_string_copy.h"

namespace i3
{
	namespace detail
	{
		template<class DestSeq>
		struct utf8_to_mb_dispatch
		{
			template<class U8Range> inline
			static void call( const U8Range& src_rng, DestSeq& dest, unsigned int codepage)
			{
				i3::stack_wstring tmp_wstr;
				i3::utf8_to_utf16( src_rng, tmp_wstr);
				
				i3::stack_string tmp_str;
				i3::utf16_to_mb( tmp_wstr, tmp_str, codepage);

				DestSeq tmp(tmp_str.begin(), tmp_str.end());
				i3::copy_or_swap(dest, tmp);
			}
		};
		
		template<>
		struct utf8_to_mb_dispatch<i3::string>
		{
			template<class U8Range> inline
			static void call(const U8Range& src_rng, i3::string& dest, unsigned int codepage)
			{
				i3::stack_wstring tmp_wstr;
				i3::utf8_to_utf16( src_rng, tmp_wstr);
				wide_to_mb_impl(codepage, &*tmp_wstr.begin(), tmp_wstr.size(), dest);
			}
		};
		
		template<class mplSize, class Ch, class Traits, class Alloc>
		struct utf8_to_mb_dispatch<i3::stack_basic_string<mplSize, Ch, Traits, Alloc>  >
		{
			template<class U8Range> inline
			static void call(const U8Range& src_rng, i3::stack_basic_string<mplSize, Ch, Traits, Alloc>& dest, 
				unsigned int codepage)
			{
				i3::stack_wstring tmp_wstr;
				i3::utf8_to_utf16( src_rng, tmp_wstr);

				wide_to_mb_impl2(codepage, &*tmp_wstr.begin(), tmp_wstr.size(), dest);
			}
		};

		template<>
		struct utf8_to_mb_dispatch<i3::rc_string>
		{
			template<class U8Range> inline
			static void call(const U8Range& src_rng, i3::rc_string& dest, unsigned int codepage)
			{
				i3::stack_wstring tmp_wstr;
				i3::utf8_to_utf16( src_rng, tmp_wstr);

				i3::stack_string tmp_str;
				wide_to_mb_impl2(codepage, &*tmp_wstr.begin(), tmp_wstr.size(), tmp_str);

				dest = tmp_str;
			}
		};

	}
		
	// 안타깝게도 codepage는 windowsAPI값이 노출된다.
	template<class U8Range, class MBSeq> inline typename i3::disable_if_iterator<MBSeq>::type
	utf8_to_mb( const U8Range& src, MBSeq& dest, unsigned int codepage /*= curr_cp()*/ )
	{
		detail::utf8_to_mb_dispatch<MBSeq>::call(as_literal(src), dest, codepage);
	}
	
	template<class U8Range, class MBLit> inline typename i3::enable_if_iterator<MBLit>::type
	utf8_to_mb( const U8Range& src, MBLit dest, std::size_t dest_buff_size, unsigned int codepage /*= curr_cp()*/ )
	{
		typedef typename get_iterator_range_type<const U8Range>::type src_range_type;
		src_range_type src_rng = i3::as_literal(src);
		i3::stack_wstring tmp_wstr;
		i3::utf8_to_utf16(src_rng, tmp_wstr);
		
		i3::stack_string tmp_str;
		detail::wide_to_mb_impl2(codepage, &*tmp_wstr.begin(), tmp_wstr.size(), tmp_str);

		i3::safe_string_copy(dest, tmp_str, dest_buff_size);
	}

}
