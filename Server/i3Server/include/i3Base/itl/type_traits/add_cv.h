#pragma once

namespace i3
{
	// add_cv
	template <class T> struct add_cv				{ typedef T const volatile type; };
	template <class T> struct add_cv<T&>			{ typedef T&			type; };	//참조에 막바로 붙지 않음.
}

