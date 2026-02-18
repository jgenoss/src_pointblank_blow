#pragma once

#include "../type_traits/has_size_type.h"
#include "../type_traits/make_unsigned.h"

namespace i3
{
	namespace range_detail
	{
		template<class T> struct extract_size_type_if_has_size_fun { typedef typename T::size_type		type; 	};
		
		template<class T> struct extract_size_type_if_no_size_fun 
		{ 
			typedef typename i3::make_unsigned< typename range_difference<T>::type >::type type;
		};

		template<class T> struct extract_size_type 
		{
			typedef typename mpl::eval_if<	i3::has_size_type<T>,
											extract_size_type_if_has_size_fun<T>,
											extract_size_type_if_no_size_fun<T> >::type type;
		};
	}

	template<class T>
	struct range_size : range_detail::extract_size_type<T> {};
	
	template<class T>
	struct range_size<const T> : range_detail::extract_size_type<T> {};
	
//	template<class T>
//	struct range_size<T*> { typedef std::size_t type;  };    
}

