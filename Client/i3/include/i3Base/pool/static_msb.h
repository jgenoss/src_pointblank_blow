#pragma once

namespace i3
{
		
	namespace detail
	{
		template<DWORD arg, DWORD n, bool satisfy>		// 큰 수에서 작은 수로 감소시킬것..
		struct static_msb32_helper {	static const DWORD  value = n; 	};

		template<DWORD arg, DWORD n>
		struct static_msb32_helper<arg, n, false> {	static const DWORD value = static_msb32_helper<arg, n - 1, ( arg & ( 1 << (n - 1) ) ) != 0 >::value; };

		template<DWORD n, bool satisfy>
		struct static_msb32_helper<0, n, satisfy> {	static const DWORD value = 0; };
	}
		
	template<DWORD arg>
	struct	static_msb32
	{
		static const DWORD value = detail::static_msb32_helper<arg, 31, ( arg & ( 1 << 31 ) ) != 0>::value;
	};
	
	template<>
	struct static_msb32<0> {	static const DWORD value = 0; };		// 올바른 값이 아님..
		
}