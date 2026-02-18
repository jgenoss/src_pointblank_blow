#pragma once

#include "utf_to_utf_impl.h"

namespace i3
{
	namespace detail
	{
		template<class MBSeq>
		void utf16_to_utf8_impl2(const wchar_t* input, std::size_t input_size, MBSeq& out)
		{
			out.clear();
			if (input_size == 0)
				return;

			// 한글기준이라면 갯수는 일반적으로 3배까지 확장될수도 있다..
			out.reserve(input_size * 3);

			const wchar_t* beg = input;
			const wchar_t* end = input + input_size;

			char	result_c[4];		

			while( beg != end)
			{
				uint32_t cp = detail_utf16::decode(beg, end);
				if ( ( cp & detail_utf::utf_error_mask) != detail_utf::utf_error_mask_result )		// 일단은 에러인경우 쌩깐다..
				{
					const int num_res = detail_utf8::encode(cp, result_c);
					out.append(result_c, num_res);
				}
			}
		}
				
	}

}