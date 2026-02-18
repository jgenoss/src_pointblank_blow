#pragma once

#include "begin.h"
#include "reverse_iterator.h"

namespace i3
{
	template<class T> inline
	typename range_reverse_iterator<T>::type rend(T& t)
	{
		return typename range_reverse_iterator<T>::type( i3::begin(t) );
	}

	template<class T> inline
	typename range_reverse_iterator<const T>::type rend(const T& t)
	{
		return typename range_reverse_iterator<const T>::type( i3::begin(t) );
	}

	template<class T> inline
	typename range_reverse_iterator<const T>::type const_rend(const T& t)
	{
		return i3::rend(t);
	}
}
