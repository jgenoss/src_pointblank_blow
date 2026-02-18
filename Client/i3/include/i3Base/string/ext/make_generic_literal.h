#pragma once

//
// 리터럴 일반화가 꽤 까다로운 문제이고, 일단은 매크로로 처리하는게 좋겠다..
// http://stackoverflow.com/questions/4261673/templates-and-string-literals-and-unicode 
// 우선 위와같이 찾아서 참고한다..

#define make_generic_literal(TYPE, x)		select_literal<TYPE>(x, L##x)

namespace i3
{
	namespace detail
	{
		template<size_t SizeOfType>
		struct select_literal_impl 
		{
			template<class Ch> inline static 
			const Ch*	call(const char* ansi, const wchar_t* wide) { return reinterpret_cast<const Ch*>(wide); }
		};
		
		template<>
		struct select_literal_impl<1>
		{
			template<class Ch> inline static
			const Ch*	call(const char* ansi, const wchar_t* wide) { return reinterpret_cast<const Ch*>(ansi); }
		};
	};
	
	template<class Ch> inline
	const Ch*	select_literal(const char* ansi, const wchar_t* wide) 
	{
		return detail::select_literal_impl<sizeof(Ch)>::call<Ch>(ansi, wide);
	}
}
