#pragma once

#include "is_integral.h"
#include "is_enum.h"
#include "remove_cv.h"

namespace i3
{
	namespace detail
	{
		template <class T>
		struct is_unsigned_values
		{
			typedef typename remove_cv<T>::type no_cv_t;
			static const no_cv_t minus_one = static_cast<no_cv_t>(-1);
			static const no_cv_t zero = (static_cast<no_cv_t>(0));
		};

		template <class T>
		struct is_unsigned_helper
		{
		    static const bool value = ( is_unsigned_values<T>::minus_one > is_unsigned_values<T>::zero);
		};

		template <bool integral_type>
		struct is_unsigned_select_helper
		{
		   template <class T>
		   struct rebind {  typedef is_unsigned_helper<T> type; };
		};

		template <>
		struct is_unsigned_select_helper<false>
		{
		   template <class T>
		   struct rebind {  typedef false_type type;  };
		};

		template <class T>
		struct is_unsigned_imp
		{
		   typedef is_unsigned_select_helper< mpl::or_< is_integral<T>, is_enum<T> >::value > selector;
		   typedef typename selector::template rebind<T> binder;
		   typedef typename binder::type type;
		   static const bool value = type::value;
		};
	
	}
	
	template<class T> class is_unsigned : integral_constant<bool, detail::is_unsigned_imp<T>::value> {};

}
