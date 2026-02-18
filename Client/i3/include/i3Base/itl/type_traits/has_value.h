#pragma once

#include "../mpl/mpl_long.h"
#include "../mpl/mpl_ntype.h"

namespace i3
{
	
	template<class T>
	struct has_integral_value
	{
		struct impl 
		{	
			template<class U> static yes_type test( identity<U> const volatile*, mpl::long_<static_cast<long>(U::value) >* = 0 ); 
			static no_type	test(...);	
		};

		enum { value = ( sizeof( impl::test(static_cast<identity<T>*>(0))) == sizeof(yes_type) ) };
		typedef mpl::bool_<value> type;
	};

	template<class T>
	struct has_value
	{
		struct impl 
		{	
			template<class U> static yes_type test1(identity<U> const volatile*, mpl::ntype< typename U::value_type, U::value>* = 0);
			static no_type  test1(...);
			
			template<class U> static yes_type test2( identity<U> const volatile*, mpl::ntype< typename U::type, U::value>* = 0);
			static no_type  test2(...);
		}; 

		enum 
		{ 
			value = has_integral_value<T>::value || 
					( sizeof( impl::test1(static_cast<identity<T>*>(0))) == sizeof(yes_type) ) || 
		            ( sizeof( impl::test2(static_cast<identity<T>*>(0))) == sizeof(yes_type) ) 
		};

		typedef mpl::bool_<value> type;
	};
			
		
	
}