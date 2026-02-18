#pragma once

#include "remove_cv.h"
#include "is_class.h"
#include "ice_or.h"

namespace i3
{

//  is_empty 구현 (empty class 여부 확인)
	namespace detail
	{
#pragma warning(push)
#pragma warning(disable:4624)
		template<class T>
		struct empty_helper_t1 : public T 
		{
			int i[256];
			empty_helper_t1();
		private:
			empty_helper_t1(const empty_helper_t1&);
			empty_helper_t1& operator=(const empty_helper_t1&);
		};
		struct empty_helper_t2 { int i[256]; };
#pragma warning(pop)

		template<class T, bool is_class_type> struct empty_helper
		{	static const bool value = (sizeof(empty_helper_t1<T>) == sizeof(empty_helper_t2));	};

		template<class T> struct empty_helper<T, false> {	static const bool value = false; };
		
		template<class T>
		struct is_empty_impl
		{
			typedef typename remove_cv<T>::type		cvt;
			static const bool value = ice_or< empty_helper<cvt, is_class<T>::value >::value, __is_empty(cvt) >::value;
		};
		
	
	}

	template<class T>
	struct is_empty : integral_constant<bool, detail::is_empty_impl<T>::value> {};
	template<> struct is_empty<void> : false_type {}; 
	template<> struct is_empty<void const> : false_type {}; 
	template<> struct is_empty<void volatile> : false_type {}; 
	template<> struct is_empty<void const volatile> : false_type {}; 
//


}