#pragma once

namespace i3
{
	template<class T> class recursive_wrapper;
	
	namespace detail
	{
		template<class T> struct is_recursive_wrapper_impl : mpl::false_ {};
		template<class T> struct is_recursive_wrapper_impl< recursive_wrapper<T> > : mpl::true_ {};
	}
			
	template<class T> struct is_recursive_wrapper : integral_constant<bool, detail::is_recursive_wrapper_impl<T>::value> {};

	template<class T> struct unwrap_recursive { typedef T type; };
	template<class T> struct unwrap_recursive<recursive_wrapper<T> > { typedef T type; };
	
	
}