#include "i3CommonType.h"
#include "i3Base/string/ext/mb_to_utf16.h"

namespace i3
{
	namespace detail
	{
		
		void mb_to_wide_impl(unsigned int codepage, const char* input, std::size_t input_size, i3::wstring& out)
		{
			if (input_size == 0)
			{
				out.clear();	return;		// 처리 없음.
			}

			// out에 뭔가를 넣지 말고, tmp에 넣고, 스웝처리..
			i3::wstring res( input_size, 0);		// 일단 동일크기로 넣어둔다..(낙관적 처리)
			int n = i3String::MultiByteToWideChar(codepage, 0, input, input_size, &res[0], res.size());
			if ( n > 0 )
			{
				res.erase(n);
				out.swap(res);	
				return;
			}
			
			n = i3String::MultiByteToWideChar(codepage, 0, input, input_size, nullptr, 0);		// 필요량..
			res.append(n - res.size(), 0); 
			i3String::MultiByteToWideChar(codepage, 0, input, input_size, &res[0], res.size());
			
			out.swap(res);
		}

	}
}
