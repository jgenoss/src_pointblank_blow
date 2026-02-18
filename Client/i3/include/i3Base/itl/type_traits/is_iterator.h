#pragma once

//  
//  i3::iterator_traits와의 호환성 여부를 볼것...
//  일단 iterator_category가 아예 정의되있지 않은 케이스면 iterator가 아닌것으로 간주..
//
#include "has_iterator_category.h"

namespace i3
{

	namespace detail
	{
		template<class T>
		struct is_iterator_impl : mpl::not_<is_same<typename iterator_traits<T>::iterator_category, mpl::na> > {};
	}

	template<class T>
	struct is_iterator : detail::is_iterator_impl<T> {};
}
