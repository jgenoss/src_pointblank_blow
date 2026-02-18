#pragma once

namespace i3
{
	
	namespace detail
	{
		// unqualified_type = cv가 제거된 순타입을 의미..
		template<class T> struct cv_traits_imp_ptr {};
		template<class T> struct cv_traits_imp_ptr<T*> 
		{ 
			static const bool is_const = false; static const bool is_volatile = false;	typedef T	unqualified_type;	
		};
		template<class T> struct cv_traits_imp_ptr<const T*> 
		{ 
			static const bool is_const = true;	static const bool is_volatile = false;	typedef T	unqualified_type;	
		};
		template<class T> struct cv_traits_imp_ptr<volatile T*> 
		{ 
			static const bool is_const = false; static const bool is_volatile = true;	typedef T	unqualified_type;	
		};
		template<class T> struct cv_traits_imp_ptr<const volatile T*> 
		{ 
			static const bool is_const = true;	static const bool is_volatile = true;	typedef T	unqualified_type;	
		};
	}
	


}