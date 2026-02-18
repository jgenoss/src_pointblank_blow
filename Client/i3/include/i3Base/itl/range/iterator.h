#pragma once

#include "../type_traits/has_iterator.h"
#include "../type_traits/has_const_iterator.h"
#include "../type_traits/remove_const.h"
#include "../mpl/mpl_if.h"

namespace i3
{
	namespace range_detail
	{
		template<class T> struct extract_iterator_impl { typedef typename T::iterator		type; 	};
		template<class T> struct extract_const_iterator_impl { typedef typename T::const_iterator		type; 	};

		template<class T> struct extract_iterator : i3::lazy_enable_if<i3::has_iterator<T>, extract_iterator_impl<T> >
		{
		};

		template<class T> struct extract_const_iterator : i3::lazy_enable_if<i3::has_const_iterator<T>, extract_const_iterator_impl<T> >
		{
		};
	}
}

//#define extract_type_xxx ( range_detail::extract_iterator, iterator)
//#include "../type_traits/extract_type_xxx.h"
//#define extract_type_xxx ( range_detail::extract_const_iterator, const_iterator)
//#include "../type_traits/extract_type_xxx.h"

namespace i3
{
	template<class T>
	struct range_mutable_iterator : range_detail::extract_iterator<T> {};

	template< class It>
	struct range_mutable_iterator< std::pair<It, It> > { typedef It type;	};

	template< class It>
	struct range_mutable_iterator< i3::pair<It, It> > { typedef It type;	};

	template< class T, std::size_t sz >
	struct range_mutable_iterator< T[sz] > {	typedef T* type; };
	
	template<class T>
	struct range_const_iterator : range_detail::extract_const_iterator<T> {};

	template< class It>
	struct range_const_iterator< std::pair<It, It> > { typedef It type;	};

	template< class It>
	struct range_const_iterator< i3::pair<It, It> > { typedef It type;	};

	template< class T, std::size_t sz >
	struct range_const_iterator< T[sz] > {	typedef const T* type; };

	template< class T>
	struct range_mutable_iterator<T*>	{ typedef T* type; };					// T포인터형인데 너무 일반화된것 같다..
	template< class T>
	struct range_const_iterator<T*>		{ typedef const T*	type; };			// 
	


	template<class T>
	struct range_iterator 
	{
		typedef typename mpl::eval_if<	is_const<T>, 
										range_const_iterator< typename remove_const<T>::type >,
										range_mutable_iterator<T> >::type	type;
	};

}
