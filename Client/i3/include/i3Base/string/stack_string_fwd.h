#pragma once

#include "../itl/mpl/mpl_size_t.h"

namespace i3
{
	template<class> class  pool_allocator;
	template<class> struct char_traits;
	
	template<class BuffSize, class Ch, class traits = char_traits<Ch>, class Alloc = i3::pool_allocator<Ch> >
	class stack_basic_string;
	
	typedef	i3::stack_basic_string<mpl::size_t<128>, char, char_traits<char>, i3::pool_allocator<char> >				stack_string;
	typedef i3::stack_basic_string<mpl::size_t<128>, wchar_t, char_traits<wchar_t>, i3::pool_allocator<wchar_t> >	stack_wstring;

	template<size_t BuffSize>
	struct custom_stack_string 
	{
		typedef i3::stack_basic_string<mpl::size_t<BuffSize>, char, char_traits<char>, i3::pool_allocator<char> >	type;
	};

	template<size_t BuffSize>
	struct custom_stack_wstring 
	{
		typedef i3::stack_basic_string<mpl::size_t<BuffSize>, wchar_t, char_traits<wchar_t>, i3::pool_allocator<wchar_t> >	type;
	};


}
