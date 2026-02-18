#pragma once

#include "../type_traits/is_equal_to_comparable.h"

namespace i3
{
	namespace detail
	{
	//  넣을 함수 후보군은 함수포인터,함수멤버(함수)포인터, 함수객체이다...
	//  i3::function의 경우 == operator를 대신 사용하므로, 호환은 되나 고려 대상이 아닐것이다..

		template< bool has_equal_op>
		struct function_equal_dispatch
		{
			template< class F, class G > __forceinline static 
			bool	call( const F& f, const G& g ) { return f == g; }
		};
		
		template<>
		struct function_equal_dispatch<false>
		{
			template<class F, class G> __forceinline static 
			bool	call(const F&, const G&) { return typeid(F) == typeid(G); }

			template<class F> __forceinline static 
			bool	call(const F&, const F&) { return true; }
		};
	}
	
	template<class F, class G> inline 
	bool function_equal(const F& f, const G& g) 
	{
		static const bool has_equal_op = is_equal_to_comparable<F, G>::value;
		return detail::function_equal_dispatch<has_equal_op>::call(f, g);
	}

}
