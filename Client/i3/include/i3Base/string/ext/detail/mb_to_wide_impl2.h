#pragma once

namespace i3
{
	namespace detail 
	{
		//
		// stack_basic_string을 염두에 두고 작성한다.
		// 

		template<class WideSeq>
		void mb_to_wide_impl2(unsigned int codepage, const char* input, std::size_t input_size, WideSeq& out)
		{
			if (input_size == 0)
			{
				out.clear();	return;		// 처리 없음.
			}

			// out에 뭔가를 넣지 말고, tmp에 넣고, 스웝처리..
			out.resize(input_size);		// 일단 동일크기로 넣어둔다..(낙관적 처리)

			int n = i3String::MultiByteToWideChar(codepage, 0, input, input_size, &out[0], out.size());
			if ( n > 0 )
			{
				out.erase(n);
				return;
			}

			n = i3String::MultiByteToWideChar(codepage, 0, input, input_size, nullptr, 0);		// 필요량..
			out.append(n - out.size(), 0); 
			i3String::MultiByteToWideChar(codepage, 0, input, input_size, &out[0], out.size());
		}

	}

}