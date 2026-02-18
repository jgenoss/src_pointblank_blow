#pragma once


namespace i3
{
	template<class> class  pool_allocator;
	template<class> struct char_traits;
	
	template<class Ch, class traits = char_traits<Ch>, class Alloc = i3::pool_allocator<Ch> >
	class rc_basic_string;
	
	typedef	i3::rc_basic_string<char, char_traits<char>, i3::pool_allocator<char> >				rc_string;
	typedef i3::rc_basic_string<wchar_t, char_traits<wchar_t>, i3::pool_allocator<wchar_t> >	rc_wstring;
}
