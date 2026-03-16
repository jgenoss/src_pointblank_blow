#pragma once

#include "is_signed.h"
#include "is_integral.h"
#include "is_enum.h"
#include "is_same.h"
#include "remove_cv.h"
#include "is_const.h"
#include "is_volatile.h"
#include "add_const.h"
#include "add_volatile.h"

#include "../mpl/mpl_if.h"
#include "../mpl/mpl_logical.h"
#include "../template_utility/compile_assert.h"

namespace i3
{
	namespace detail
	{
		
		template <class T>
		struct make_signed_imp
		{
			typedef typename remove_cv<T>::type t_no_cv;

			compile_assert_pred((mpl::or_<is_integral<T>, is_enum<T>));
			compile_assert_pred_not(( is_same< t_no_cv, bool> ));

		
			typedef typename mpl::if_< mpl::and_<	is_signed<T>, is_integral<T>, 
													mpl::not_<is_same<t_no_cv, char> >,
													mpl::not_<is_same<t_no_cv, wchar_t> >,
													mpl::not_<is_same<t_no_cv, bool> > >, T, 
					typename mpl::if_< mpl::and_<	is_integral<T>, 
													mpl::not_<is_same<t_no_cv, char> >,
													mpl::not_<is_same<t_no_cv, wchar_t> >,
													mpl::not_<is_same<t_no_cv, bool> > >,
					typename mpl::if_< is_same<t_no_cv, unsigned char>, signed char,
					typename mpl::if_< is_same<t_no_cv, unsigned short>, signed short,
					typename mpl::if_< is_same<t_no_cv, unsigned int>, int,
					typename mpl::if_< is_same<t_no_cv, unsigned long>, long, long long>::type >::type>::type>::type,
					typename mpl::if_c< sizeof(t_no_cv) == sizeof(unsigned char), signed char,
					typename mpl::if_c< sizeof(t_no_cv) == sizeof(unsigned short), signed short,
					typename mpl::if_c< sizeof(t_no_cv) == sizeof(unsigned int), int,
					typename mpl::if_c< sizeof(t_no_cv) == sizeof(unsigned long), long, long long>::type>::type>::type>::type>::type>::type 
					base_integer_type;
			
		   
			typedef typename mpl::if_<is_const<T>, typename add_const<base_integer_type>::type, base_integer_type>::type const_base_integer_type;
			typedef typename mpl::if_<is_volatile<T>, typename add_volatile<const_base_integer_type>::type, const_base_integer_type>::type type;
		};
	}

	template<class T> struct make_signed { typename detail::make_signed_imp<T>::type type; };

}
