#pragma once

#include <new>
#include "yes_no_type.h"
#include "ice_or.h"

namespace i3
{
	namespace detail
	{
		// 이 new operator를 구하는 루틴은 다른 부분에도 응용할수 있는 여지가 많으므로, 정리하고 분석하면 좋음...
		template <class T>
		struct has_new_operator_impl
		{
			template <class U, U x> struct test;

			template<class U> static yes_type check_sig1(U*, test<void* (*)(std::size_t), &U::operator new >* = NULL	);
			template<class U> static no_type  check_sig1(...);
			
			template<class U> static yes_type check_sig2(U*, test<void* (*)(std::size_t, const std::nothrow_t&), &U::operator new>* = NULL );
			template<class U> static no_type  check_sig2(...);
			
			template<class U> static yes_type check_sig3(U*, test<void* (*)(std::size_t, void*), &U::operator new>* = NULL );
			template<class U> static no_type  check_sig3(...);
			
			template<class U> static yes_type check_sig4(U*, test<void* (*)(std::size_t), &U::operator new[]>* = NULL );
			template<class U> static no_type  check_sig4(...);

			template<class U> static yes_type check_sig5(U*, test<void* (*)(std::size_t, const std::nothrow_t&), &U::operator new[]>* = NULL);
			template<class U> static no_type  check_sig5(...);

			template<class U> static yes_type check_sig6(U*, test<void* (*)(std::size_t, void*), &U::operator new[]>* = NULL);
			template<class U> static no_type  check_sig6(...);
			
			static const bool	value = ice_or< sizeof(check_sig1<T>(0)) == sizeof(yes_type), sizeof(check_sig2<T>(0)) == sizeof(yes_type),  
								sizeof(check_sig3<T>(0)) == sizeof(yes_type), sizeof(check_sig4<T>(0)) == sizeof(yes_type),  
								sizeof(check_sig5<T>(0)) == sizeof(yes_type), sizeof(check_sig6<T>(0)) == sizeof(yes_type)>::value;
		};
	}

	template <class T>
	struct has_new_operator : integral_constant< bool, detail::has_new_operator_impl<T>::value> {};
	
}