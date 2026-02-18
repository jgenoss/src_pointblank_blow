#pragma once

#include "../mpl/mpl_apply.h"
#include "../mpl/mpl_identity.h"

#include "../type_traits/add_pointer.h"

namespace i3
{
	namespace ft
	{
		using mpl::placeholders::_;
		
		template<class T, class L>
		struct class_transform {	typedef typename mpl::apply1<L,T>::type type;	};		// L람다함수에 T인자를 실행
		
		template<class T> 
		struct class_transform< T, mpl::identity<_> > { typedef T type; };					// 자명함수들은 간단하게 리턴시킨다..

		template<class T> 
		struct class_transform< T, add_reference<_> > { typedef T& type; };					// 항상, nonref라는 가정..

		template<class T> 
		struct class_transform< T, add_pointer<_> >	{ typedef T* type; };

		template<class T> 
		struct class_transform< T, remove_cv<_> > { typedef typename remove_cv<T>::type type; };

		template<class T> 
		struct class_transform< T, add_reference< remove_cv<_> > > { typedef typename remove_cv<T>::type& type; };

		template<class T> 
		struct class_transform< T, add_pointer< remove_cv<_> > > { typedef typename remove_cv<T>::type* type; };

		template<class T, class U> 
		struct class_transform< T, mpl::always<U> > { typedef U type; };
	}
}
