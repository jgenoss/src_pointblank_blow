
#ifndef has_xxx_h
#define has_xxx_h
#include "../mpl/mpl_bool.h"
#include "../mpl/mpl_identity.h"
#endif  // has_xxx_h

// no include guard
#define		pp_trait_name		pp_tuple_elem(2, 0, has_xxx)
#define		pp_type_name		pp_tuple_elem(2, 1, has_xxx)


namespace i3
{
	template<class T> 
	struct pp_trait_name 
	{ 
		struct impl 
		{	
			template<class U> 
			static yes_type test( i3::identity<U> const volatile*, i3::identity<typename U::pp_type_name>* = 0 ); 
			static no_type	test(...);	
		}; 

		enum { value = ( sizeof( impl::test(static_cast<i3::identity<T>*>(0))) == sizeof(yes_type) ) }; 
		typedef i3::mpl::bool_<value>		type; 
	};
}


#undef pp_type_name
#undef pp_trait_name
#undef has_xxx
