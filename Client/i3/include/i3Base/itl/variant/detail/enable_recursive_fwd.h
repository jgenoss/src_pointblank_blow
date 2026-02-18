#pragma once

namespace i3
{
	namespace detail { namespace variant 
	{
		template<class T> struct recursive_flag {	typedef T type;	};

		template<class T> struct is_recursive_flag : mpl::false_ {};
		template<class T> struct is_recursive_flag< recursive_flag<T> > : mpl::true_ {};


		template<class T, class RecursiveVariant, class NoWrapper = mpl::false_>
		struct enable_recursive;

		template<class RecursiveVariant, class NoWrapper = mpl::false_>
		struct quoted_enable_recursive;


	} }
}