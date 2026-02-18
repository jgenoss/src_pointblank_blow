#pragma once

namespace i3
{
	namespace detail
	{
		template<class T> struct floating_point_promotion {	typedef T type;	};
		template<> struct floating_point_promotion<float> {	typedef double type; };
		template<> struct floating_point_promotion<float const> { typedef double const type; };
		template<> struct floating_point_promotion<float volatile> { typedef double volatile type;	};
		template<> struct floating_point_promotion<float const volatile> {	typedef double const volatile type;	};
	}
	
	template<class T> floating_point_promotion : detail::floating_point_promotion<T> {};

}