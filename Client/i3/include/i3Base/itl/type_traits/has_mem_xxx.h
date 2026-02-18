
// no include guard
#ifndef has_mem_xxx_h
#define has_mem_xxx_h

#include "../preprocessor/tuple/pp_tuple_elem.h"
#include "../preprocessor/pp_cat.h"
#include "../type_traits/yes_no_type.h"
#include "../mpl/mpl_bool.h"
#include "../mpl/mpl_and.h"
#include "../type_traits/is_class.h"

#endif 

#define		pp_trait_name		pp_tuple_elem(2, 0, has_mem_xxx)
#define		pp_member_name		pp_tuple_elem(2, 1, has_mem_xxx)

#pragma warning(push)
#pragma warning(disable : 4510 4610 4624)

namespace i3
{
	namespace detail
	{
		template<class T>
		struct pp_cat(pp_trait_name,_impl)
		{
			struct base_mixin { void pp_member_name(){} };
			struct functor_wrap : public T, public base_mixin {};
			
			template<class U, U t> class helper;
			template<class U> 
			static no_type	test(U*, helper<void (base_mixin::*)(), &U::pp_member_name>* = 0);
			static yes_type test(...);
			static const bool value = (sizeof(yes_type) == sizeof(test(static_cast<functor_wrap*>(0))));
			typedef mpl::bool_<value>		type;
		};
	
	}

	template<class T>
	struct pp_trait_name : mpl::and_< is_class_or_union<T>, detail::pp_cat(pp_trait_name,_impl)<T> > {};
}


#pragma warning(pop)


#undef pp_member_name
#undef pp_trait_name
#undef has_mem_xxx
