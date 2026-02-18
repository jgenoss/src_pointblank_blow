#pragma once

#include "is_base_of.h"
#include "../mpl/mpl_and.h"
#include "../mpl/mpl_not.h"

namespace i3
{
	namespace detail
	{
#pragma warning( push )
#pragma warning( disable : 4584 )

		template<class Base, class Derived, class tag>
		struct is_virtual_base_of_impl {	static const bool value = false;	};

		template<class Base, class Derived>
		struct is_virtual_base_of_impl<Base, Derived, mpl::true_>
		{
			struct X : Derived, virtual Base 
			{
			   X();	   X(const X&);	   X& operator=(const X&);	   ~X()throw();
			};
			struct Y : Derived 
			{
			   Y();	   Y(const Y&);	   Y& operator=(const Y&);	   ~Y()throw();
			};
			static const bool value = (sizeof(X)==sizeof(Y));
		};

		template<class Base, class Derived>
		struct is_virtual_base_of_impl2
		{
			typedef typename mpl::and_<is_base_of<Base, Derived>, mpl::not_<is_same<Base, Derived> > >::type tag_type;
			typedef is_virtual_base_of_impl<Base, Derived, tag_type> imp;
			static const bool value = imp::value;
		};
#pragma warning( pop )

	}
	
	template<class Base, class Derived> struct is_virtual_base_of : 
		integral_constant<bool, detail::is_virtual_base_of_impl2<Base,Derived>::value> {};
	
	template<class base, class derived> struct is_virtual_base_of<base&, derived> : false_type {}; 
	template<class base, class derived> struct is_virtual_base_of<base, derived&> : false_type {}; 
	template<class base, class derived> struct is_virtual_base_of<base&, derived&> : false_type {}; 


}