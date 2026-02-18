#pragma once

namespace i3
{
	
	namespace detail
	{
		//
		// stack_basic_string을 염두에 두고 작성한다.
		// 
		template<class MBSeq>
		void wide_to_mb_impl2( unsigned int codepage, 
			const wchar_t* input, std::size_t input_size, MBSeq& out)  
		{
			if (input_size == 0) 
			{
				out.clear();	return;
			}

			out.resize(input_size * 2);		// 모두 한글이라는 가정이라면 갯수는 2배로 넉넉이 잡는다.

			int n = i3String::WideCharToMultiByte(codepage, 0, input, input_size, &out[0], out.size(), nullptr, nullptr);
			if ( n > 0)
			{
				out.erase(n);
				return;				// 정상 종료..대부분 이렇게 처리될것이다..
			}

			n = i3String::WideCharToMultiByte(codepage, 0, input, input_size, nullptr, 0, nullptr, nullptr);	//필요량..
			out.append(n - out.size(), 0); 
			i3String::WideCharToMultiByte(codepage, 0, input, input_size, &out[0], out.size(), nullptr, nullptr);
		}
	}
		
}