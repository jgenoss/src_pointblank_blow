#pragma once

#include "is_unsigned.h"
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
		struct make_unsigned_imp
		{
			typedef typename remove_cv<T>::type t_no_cv;

			compile_assert_pred((mpl::or_<is_integral<T>, is_enum<T>));
			compile_assert_pred_not(( is_same< t_no_cv, bool> ));

		
			typedef typename mpl::if_< mpl::and_<	is_unsigned<T>, is_integral<T>, 
													mpl::not_<is_same<t_no_cv, char> >,
													mpl::not_<is_same<t_no_cv, wchar_t> >,
													mpl::not_<is_same<t_no_cv, bool> > >, T, 
					typename mpl::if_< mpl::and_<	is_integral<T>, 
													mpl::not_<is_same<t_no_cv, char> >,
													mpl::not_<is_same<t_no_cv, wchar_t> >,
													mpl::not_<is_same<t_no_cv, bool> > >,
					typename mpl::if_< is_same<t_no_cv, signed char>, unsigned char,
					typename mpl::if_< is_same<t_no_cv, short>, unsigned short,
					typename mpl::if_< is_same<t_no_cv, int>, unsigned int,
					typename mpl::if_< is_same<t_no_cv, long>, unsigned long, unsigned long long>::type >::type>::type>::type,
					typename mpl::if_c< sizeof(t_no_cv) == sizeof(unsigned char), unsigned char,
					typename mpl::if_c< sizeof(t_no_cv) == sizeof(unsigned short), unsigned short,
					typename mpl::if_c< sizeof(t_no_cv) == sizeof(unsigned int), unsigned int,
					typename mpl::if_c< sizeof(t_no_cv) == sizeof(unsigned long), unsigned long, unsigned long long>::type>::type>::type>::type>::type>::type 
					base_integer_type;
			
		   
			typedef typename mpl::if_<is_const<T>, typename add_const<base_integer_type>::type, base_integer_type>::type const_base_integer_type;
			typedef typename mpl::if_<is_volatile<T>, typename add_volatile<const_base_integer_type>::type, const_base_integer_type>::type type;
		};
	}

	template<class T> struct make_unsigned { typename detail::make_unsigned_imp<T>::type type; };

}
