#pragma once

#include "utf_to_utf_impl.h"

namespace i3
{
	namespace detail
	{

		// 이건 i3::stack_basic_string을 위해서 따로 빼놓는다.
		template<class WideSeq>
		void utf8_to_utf16_impl2(const char* input, std::size_t input_size, WideSeq& out)
		{
			out.clear();

			if (input_size == 0) 
				return;

			// 한글기준 3바이트라면 --> 갯수는 오히려 줄어드는 방향으로 가게되므로, 같은 사이즈를 예약 해두어도 괜찮을 것이다.
			out.reserve(input_size);

			const char* beg = input;
			const char* end = input + input_size;

			wchar_t     result_c[2];

			while ( beg != end )
			{
				uint32_t cp = detail_utf8::decode(beg, end);
				if (  ( cp & detail_utf::utf_error_mask) != detail_utf::utf_error_mask_result )		// 일단은 에러인경우 쌩깐다..
				{
					const int num_res = detail_utf16::encode(cp, result_c);
					out.append(result_c, num_res);
				}
			}

		}

	}
}
