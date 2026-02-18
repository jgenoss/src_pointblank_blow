#pragma once

// i3String::ftoa의 일반화.. (sprintf와는 다르게 소수점 뒤 0이 제거된다..)
// 이 함수 구현의 경우, sprintf ("%f") 를 쓰는데, 
// 대체 함수도 마련되었고, double_to_format루틴이 완성되었으므로 이쪽이 더 낫다..
//

#include "../../itl/range/generic_append_range.h"
#include "../../itl/range/generic_push_back.h"
#include "../../itl/range/generic_clear.h"
#include "../../itl/type_traits/remove_reference.h"

namespace i3
{
	namespace detail_sprintf
	{
		I3_EXPORT_BASE size_t	double_to_fformat(double val, int precision, bool force_point, char* out, int* sign);
	}

	namespace detail
	{
		template<class SeqOrLit>
		void	ftoa_impl(double val, SeqOrLit lit, int fractLen)
		{
			typedef typename i3::remove_reference<SeqOrLit>::type	referred;
			typedef typename i3::range_value<referred>::type		value_type;

			char   buff[_CVTBUFSIZE];		// 고정..(wchar_t는 고려하지 않고 선계산 한다...)
			int    sign;

			if (fractLen == -1)
				fractLen = 6;		// sprintf의 실수형 디폴트 자릿수는 6으로 고정된다..

			size_t buff_writes = detail_sprintf::double_to_fformat(val, fractLen, false, buff, &sign);

			char* buff_end = buff + buff_writes;

			if (fractLen > 0)
			{

				for (; buff < buff_end; --buff_end)
				{
					if (*(buff_end - 1) != '0')					// sprintf와는 달리 끝0이나 소수점을 제거해준다...
						break;
				}

				if (buff < buff_end && *(buff_end - 1) == '.')		// sprintf와는 달리 끝0이나 소수점을 제거해준다...
					--buff_end;
			}

			i3::generic_clear(lit);

			if (sign)
				i3::generic_push_back(lit, value_type('-'));

			i3::generic_append_range(lit, buff, buff_end);		// 이 계산이 wchar_t에도 잘 적용되어야 한다..

		}

	}

	template<class Real, class OutLit> inline
		typename i3::enable_if_iterator<OutLit>::type
		ftoa(Real val, OutLit lit, int fractLen /*= -1*/)
	{
		detail::ftoa_impl<OutLit>(double(val), lit, fractLen);
	}

	template<class Real, class OutSeq> inline
		typename i3::disable_if_iterator<OutSeq>::type
		ftoa(Real val, OutSeq& seq, int fractLen /*= -1*/)
	{
		detail::ftoa_impl<OutSeq&>(double(val), seq, fractLen);
	}


}
