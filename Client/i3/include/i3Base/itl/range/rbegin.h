#pragma once

#include "end.h"
#include "reverse_iterator.h"

namespace i3
{
	template<class T> inline
	typename range_reverse_iterator<T>::type rbegin(T& t)
	{
		return typename range_reverse_iterator<T>::type( i3::end(t) );
	}
	
	template<class T> inline
	typename range_reverse_iterator<const T>::type rbegin(const T& t)
	{
		return typename range_reverse_iterator<const T>::type( i3::end(t) );
	}
	
	template<class T> inline
	typename range_reverse_iterator<const T>::type const_rbegin(const T& t)
	{
		return i3::rbegin(t);
	}
}
