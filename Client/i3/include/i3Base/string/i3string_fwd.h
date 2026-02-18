#pragma once

namespace i3
{
	template<class> class  pool_allocator;
	template<class> struct char_traits;
	
	template<class Ch, class traits = char_traits<Ch>, class Alloc = i3::pool_allocator<Ch> >
	class basic_string;
	
	typedef	i3::basic_string<char, char_traits<char>, i3::pool_allocator<char> >				string;
	typedef i3::basic_string<wchar_t, char_traits<wchar_t>, i3::pool_allocator<wchar_t> >		wstring;
	
}
