#include "i3CommonType.h"
#include "string/ext/utf16_to_mb.h"

namespace i3
{
	namespace detail
	{
		//
		// 65001 65000 은 들어가지 않도록 유의한다.
		//
		void wide_to_mb_impl( unsigned int codepage, 
			const wchar_t* input, std::size_t input_size, i3::string& out)  
		{
			if (input_size == 0) 
			{
				out.clear();	return;
			}
			
			i3::string res( input_size * 2, 0);		// 모두 한글이라는 가정이라면 갯수는 2배로 넉넉이 잡는다.

			int n = i3String::WideCharToMultiByte(codepage, 0, input, input_size, &res[0], res.size(), nullptr, nullptr);
			if ( n > 0)
			{
				res.erase(n);
				out.swap(res);	
				return;				// 정상 종료..대부분 이렇게 처리될것이다..
			}
			
			n = i3String::WideCharToMultiByte(codepage, 0, input, input_size, nullptr, 0, nullptr, nullptr);	//필요량..
			res.append(n - res.size(), 0); 
			i3String::WideCharToMultiByte(codepage, 0, input, input_size, &res[0], res.size(), nullptr, nullptr);
			
			out.swap(res);
		}
	}
}
