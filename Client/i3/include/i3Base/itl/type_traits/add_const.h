#pragma once

namespace i3
{
	template <class T> struct add_const			 { typedef T const	type; };
	template <class T> struct add_const<T&>		 { typedef T&		type; };	//참조에 막바로 붙지 않음.
}
