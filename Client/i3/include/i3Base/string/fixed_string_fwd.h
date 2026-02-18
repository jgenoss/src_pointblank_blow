#pragma once


namespace i3
{
	template<class> class  pool_allocator;
	template<class> struct char_traits;

	template<class Ch, class Traits = i3::char_traits<Ch>, class Alloc = i3::pool_allocator<Ch> >
	class fixed_basic_string;

	typedef	i3::fixed_basic_string<char>			fixed_string;
	typedef i3::fixed_basic_string<wchar_t>			fixed_wstring;
}
