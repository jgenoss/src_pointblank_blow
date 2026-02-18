#pragma once

//
// 문자열 코드 변환 모듈의 경우, 유니코드간 변환은 모두 자체코드로 변경가능한 상태로 그렇게 처리하고..
// 멀티바이트 코드셋과 UTF16간에는 각 코드셋별 변환 코드들이 너무 많고 복잡하기 때문에, 윈도우즈것을 그대로 쓴다..
// (현재 이 방법이 boost locale이 윈도우즈에 대응해서 쓰이고 있는 방법이다...)

//
// 유닉스 계열 iconv를 직접 windows로 포팅해서 정적라이브러리를 구성하고 euc_kr -> UTF8 변환 퍼포먼스를 비교해보았는데...
// MS쪽이 MBTOWC / WCTOMB를 연달아 호출한것이라고 해도 windows쪽이 대략 1.8배가 빠르다...
//
#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/utility/copy_or_swap.h"
#include "../../i3String.h"
#include "curr_cp.h"

#include "detail/mb_to_wide_impl2.h"
#include "safe_string_copy.h"

namespace i3
{
	namespace detail
	{
		// 입력인수는 문자열배열로 하고, 출력인수로 wstring을 넘긴다..
		// mb는 글자당 1/2바이트를 소모하고, utf16은 글자당 2/4바이트(대부분 2이므로, 4는 일단 예외로만 처리)를 소모하는데, 
		// 한글기준으로는 mb길이가 2이면, 그냥 1개글자로 갯수는 오히려 줄어든다.. (따라서 낙관적으로 갯수는 동일하게 잡아도 된다.)
		//
		// 이 함수는 일반함수이다..은폐되었어도, 템플릿 함수에 의해 쓰일수 있기 때문에, DLL키워드가 필요함..
		I3_EXPORT_BASE void mb_to_wide_impl(unsigned int codepage, const char* input, std::size_t input_size, i3::wstring& out);
		
		template<class DestSeq>
		struct mb_to_wide_dispatch
		{
			template<class MBRange> inline
			static void call( const MBRange& src_rng, DestSeq& dest, unsigned int codepage)
			{
				i3::stack_wstring tmp_wstr;
				i3::mb_to_utf16(src_rng, tmp_wstr, codepage);
				DestSeq tmp(tmp_wstr.begin(), tmp_wstr.end());
				i3::copy_or_swap(dest, tmp);
			}
		};
		
		template<>
		struct mb_to_wide_dispatch<i3::wstring>
		{
			template<class MBRange> inline
			static void call(const MBRange& src_rng, i3::wstring& dest, unsigned int codepage)
			{
				mb_to_wide_impl(codepage, &*src_rng.begin(), src_rng.size(), dest);
			}
		};
		
		template<class mplSize, class Ch, class Traits, class Alloc>
		struct mb_to_wide_dispatch< i3::stack_basic_string<mplSize, Ch, Traits, Alloc> >
		{
			template<class MBRange> inline
			static void call(const MBRange& src_rng, i3::stack_basic_string<mplSize, Ch, Traits, Alloc>& dest, 
				unsigned int codepage)
			{
				mb_to_wide_impl2(codepage, &*src_rng.begin(), src_rng.size(), dest);
			}
		};
		
		template<>
		struct mb_to_wide_dispatch<i3::rc_wstring>
		{
			template<class MBRange> inline
				static void call(const MBRange& src_rng, i3::rc_wstring& dest, unsigned int codepage)
			{
				i3::stack_wstring tmp_wstr;
				mb_to_wide_impl2(codepage, &*src_rng.begin(), src_rng.size(), tmp_wstr);
				dest = tmp_wstr;
			}
		};
	}

	// 안타깝게도 codepage는 windowsAPI값이 노출된다.
	template<class MBRange, class WideSeq> inline typename i3::disable_if_iterator<WideSeq>::type
	mb_to_utf16( const MBRange& src, WideSeq& dest, unsigned int codepage /*= curr_cp()*/ )
	{
		detail::mb_to_wide_dispatch<WideSeq>::call(as_literal(src), dest, codepage);
	}
	
	template<class MBRange, class WideLit> inline typename i3::enable_if_iterator<WideLit>::type
	mb_to_utf16( const MBRange& src, WideLit dest, std::size_t dest_buff_size, unsigned int codepage /*= curr_cp()*/ )
	{
		typedef typename get_iterator_range_type<const MBRange>::type src_range_type;
		src_range_type src_rng = i3::as_literal(src);
		i3::stack_wstring tmp_wstr;
		detail::mb_to_wide_impl2(codepage, &*src_rng.begin(), src_rng.size(), tmp_wstr);
		i3::safe_string_copy(dest, tmp_wstr, dest_buff_size);
	}

}
