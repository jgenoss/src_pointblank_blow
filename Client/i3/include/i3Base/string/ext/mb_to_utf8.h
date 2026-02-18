#pragma once

#include "mb_to_utf16.h"
#include "utf16_to_utf8.h"
#include "curr_cp.h"
#include "safe_string_copy.h"
// 거의 대부분의 라이브러리들이 그냥 2단계를 거친다..

namespace i3
{
	namespace detail
	{
		// 입력인수는 문자열배열로 하고, 출력인수로 wstring을 넘긴다..
		// mb는 글자당 1/2바이트를 소모하고, utf16은 글자당 2/4바이트(대부분 2이므로, 4는 일단 예외로만 처리)를 소모하는데, 
		// 한글기준으로는 mb길이가 2이면, 그냥 1개글자로 갯수는 오히려 줄어든다.. (따라서 낙관적으로 갯수는 동일하게 잡아도 된다.)
		//
		// 이 함수는 일반함수이다..은폐되었어도, 템플릿 함수에 의해 쓰일수 있기 때문에, DLL키워드가 필요함..
		
		template<class DestSeq>
		struct mb_to_utf8_dispatch
		{
			template<class MBRange> inline
			static void call( const MBRange& src_rng, DestSeq& dest, unsigned int codepage)
			{
				i3::stack_wstring tmp_wstr;
				i3::mb_to_utf16( src_rng, tmp_wstr, codepage);

				i3::stack_string tmp_str;
				i3::utf16_to_utf8(tmp_wstr, tmp_str);
				
				DestSeq tmp(tmp_str.begin(), tmp_str.end());
				i3::copy_or_swap(dest, tmp);
			}
		};
		
		template<>
		struct mb_to_utf8_dispatch<i3::string>
		{
			template<class MBRange> inline
			static void call(const MBRange& src_rng, i3::string& dest, unsigned int codepage)
			{
				i3::stack_wstring tmp_wstr;
				i3::mb_to_utf16(src_rng, tmp_wstr, codepage); 
				utf16_to_utf8_impl( &*tmp_wstr.begin(), tmp_wstr.size(), dest);
			}
		};
		
		template<class mplSize, class Ch, class Traits, class Alloc>
		struct mb_to_utf8_dispatch<i3::stack_basic_string<mplSize, Ch, Traits, Alloc>>
		{
			template<class MBRange> inline
			static void call(const MBRange& src_rng, i3::stack_basic_string<mplSize, Ch, Traits, Alloc>& dest, 
				unsigned int codepage)
			{
				i3::stack_wstring tmp_wstr;
				i3::mb_to_utf16(src_rng, tmp_wstr, codepage); 

				utf16_to_utf8_impl2( &*tmp_wstr.begin(), tmp_wstr.size(), dest);
			}
		};
		
		template<>
		struct mb_to_utf8_dispatch<i3::rc_string>
		{
			template<class MBRange> inline
				static void call(const MBRange& src_rng, i3::rc_string& dest, unsigned int codepage)
			{
				i3::stack_wstring tmp_wstr;
				i3::mb_to_utf16(src_rng, tmp_wstr, codepage); 

				i3::stack_string tmp_str;
				utf16_to_utf8_impl2( &*tmp_wstr.begin(), tmp_wstr.size(), tmp_str);

				dest = tmp_str;
			}
		};

	}

	// 안타깝게도 codepage는 windowsAPI값이 노출된다.
	template<class MBRange, class U8Seq> inline typename i3::disable_if_iterator<U8Seq>::type
	mb_to_utf8( const MBRange& src, U8Seq& dest, unsigned int codepage /*= curr_cp()*/ )
	{
		detail::mb_to_utf8_dispatch<U8Seq>::call(as_literal(src), dest, codepage);
	}
	
	template<class MBRange, class U8Lit> inline typename i3::enable_if_iterator<U8Lit>::type
	mb_to_utf8( const MBRange& src, U8Lit dest, std::size_t dest_buff_size, unsigned int codepage /*= curr_cp()*/ )
	{
		typedef typename get_iterator_range_type<const MBRange>::type src_range_type;
		src_range_type src_rng = i3::as_literal(src);
		i3::stack_wstring tmp_wstr;
		i3::mb_to_utf16(src_rng, tmp_wstr, codepage);

		i3::stack_string  tmp_str;
		detail::utf16_to_utf8_impl2(&*tmp_wstr.begin(), tmp_wstr.size(), tmp_str);

		i3::safe_string_copy(dest, tmp_str, dest_buff_size);
	}

}
