#pragma once

namespace i3
{
	template<class T> struct forward_type { typedef const T& type; };
	template<class T> struct forward_type<rv<T> > { typedef T type; };

	template<class T> struct is_rvalue_reference : false_type {};
	template<class T> struct is_rvalue_reference< rv<T>& > : true_type {};
	template<class T> struct is_rvalue_reference< const rv<T>& > : true_type {};

	namespace detail_move
	{
		template<class T,	bool emul = has_move_emulation_enabled<T>::value,
							bool rv   = detail::is_rv<T>::value >
		struct add_rvalue_reference_impl { typedef T type; };

		template<class T, bool emul>
		struct add_rvalue_reference_impl<T, emul, true> { typedef T& type; };	// rv<T2>& ..
		
		template<class T, bool rv>
		struct add_rvalue_reference_impl<T, true, rv> { typedef rv<T>& type; };
	}

	template<class T>
	struct add_rvalue_reference : detail_move::add_rvalue_reference_impl<T> {};

	template<class T> struct add_rvalue_reference<T&> { typedef T& type; }

	template<class T> struct remove_rvalue_reference { typedef T type; };

	template<class T> struct remove_rvalue_reference< rv<T> >                { typedef T type; };
	template<class T> struct remove_rvalue_reference< const rv<T> >          { typedef T type; };
	template<class T> struct remove_rvalue_reference< volatile rv<T> >       { typedef T type; };
	template<class T> struct remove_rvalue_reference< const volatile rv<T> > { typedef T type; };
	template<class T> struct remove_rvalue_reference< rv<T>& >               { typedef T type; };
	template<class T> struct remove_rvalue_reference< const rv<T>& >         { typedef T type; };
	template<class T> struct remove_rvalue_reference< volatile rv<T>& >      { typedef T type; };
	template<class T> struct remove_rvalue_reference< const volatile rv<T>& >{ typedef T type; };
	
	


}