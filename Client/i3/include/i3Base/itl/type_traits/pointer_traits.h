#pragma once

#include "has_element_type.h"
#include "has_reference.h"
#include "has_difference_type.h"
#include "template_at.h"

#include "add_reference.h"
#include "type_rebinder.h"

#include "is_pointer_to_static_mem_fun_call_possible.h"
#include "../utility/addressof.h"

namespace i3
{
	namespace detail_pointer_traits
	{
		template<class T>
		struct get_element_type_if_has 
		{	
			typedef typename T::element_type type;	
		};
		
		template<class T>
		struct get_element_type_if_template
		{
			typedef typename template_at_c<T, 0>::type	type;
		};

		template<class T>
		struct get_element_type
		{
			typedef typename mpl::eval_if< has_element_type<T>,
											get_element_type_if_has<T>,
											get_element_type_if_template<T> >::type	type;
		};
		
		template<class T>
		struct get_difference_type_if_has { typedef typename T::difference_type type; };
		
		template<class T>
		struct get_difference_type
		{
			typedef typename mpl::eval_if<has_difference_type<T>,
											get_difference_type_if_has<T>,
											identity<std::ptrdiff_t> >::type		type;
		};

		template<class T>
		struct get_reference_if_has { typedef typename T::reference type; };

		template<class T, class Element>
		struct get_reference
		{
			typedef typename mpl::eval_if<has_reference<T>,
				get_reference_if_has<T>, add_reference<Element> >::type		type;
		};

		template<bool has_pointer_to>
		struct pointer_to_impl
		{
			template<class Ptr, class R, class Arg> __forceinline 
			static R call(Arg a) {  return Ptr::pointer_to(a);   }
		};

		template<>
		struct pointer_to_impl<false>
		{
			template<class Ptr, class R, class Arg> __forceinline
			static R call(Arg a) {  return R(i3::addressof(a));  }
		};
	}

	template<class Ptr>
	struct pointer_traits
	{
		typedef Ptr			pointer;
		typedef typename detail_pointer_traits::get_element_type<Ptr>::type	element_type;
		typedef typename detail_pointer_traits::get_difference_type<Ptr>::type difference_type;
		typedef typename detail_pointer_traits::get_reference<Ptr, element_type>::type reference;		
		
		template<class U>
		struct rebind_pointer 
		{
			typedef typename i3::type_rebinder<Ptr, U>::type	type;
		};
		
		static pointer pointer_to(reference r)
		{
			static const bool has_pointer_to = i3::is_pointer_to_static_mem_fun_call_possible<Ptr, pointer, reference>::value;
			return detail_pointer_traits::pointer_to_impl<has_pointer_to>::call<Ptr, pointer, reference>(r);
		}

		template<class UPtr>
		static pointer static_cast_from(const UPtr& u) {  return pointer_to(*static_cast<element_type*>(to_raw_pointer(u)));  }
		template<class UPtr>
		static pointer const_cast_from(const UPtr& u) { return pointer_to(*const_cast<element_type*>(to_raw_pointer(u))); }
		template<class UPtr>
		static pointer dynamic_cast_from(const UPtr& u) { return pointer_to(*dynamic_cast<element_type*>(to_raw_pointer(u))); }

		template<class U>
		static pointer static_cast_from(U* u) {  return pointer_to(*static_cast<element_type*>(u));  }
		template<class U>
		static pointer const_cast_from(U* u) { return pointer_to(*const_cast<element_type*>(u)); }
		template<class U>
		static pointer dynamic_cast_from(U* u) { return pointer_to(*dynamic_cast<element_type*>(u)); }

	};

	template<class Ptr> struct pointer_traits<const Ptr> : pointer_traits<Ptr> {};
	template<class Ptr> struct pointer_traits<volatile Ptr> : pointer_traits<Ptr> {};
	template<class Ptr> struct pointer_traits<const volatile Ptr> : pointer_traits<Ptr> {};
	template<class Ptr> struct pointer_traits<Ptr&> : pointer_traits<Ptr> {};		// 참조는 활용하지 않음.

	template<class T>
	struct pointer_traits<T*>
	{
		typedef T											element_type;
		typedef T*											pointer;
		typedef std::ptrdiff_t								difference_type;
		typedef typename i3::add_reference<T>::type			reference;		
		template<class U>
		struct rebind_pointer { typedef U*	type; };
		
		static pointer pointer_to(reference r) { return i3::addressof(r); }
		template<class UPtr>
		static pointer static_cast_from(const UPtr& u) {  return static_cast<pointer>( to_raw_pointer(u) );  }
		template<class UPtr>
		static pointer const_cast_from(const UPtr& u) { return const_cast<pointer>( to_raw_pointer(u) ); }
		template<class UPtr>
		static pointer dynamic_cast_from(const UPtr& u) { return dynamic_cast<pointer>(to_raw_pointer(u) ); }
		
		template<class U>
		static pointer static_cast_from(U* u) {  return static_cast<pointer>(u);  }
		template<class U>
		static pointer const_cast_from(U* u) { return const_cast<pointer>(u); }
		template<class U>
		static pointer dynamic_cast_from(U* u) { return dynamic_cast<pointer>(u); }

	};
		
	
	
}