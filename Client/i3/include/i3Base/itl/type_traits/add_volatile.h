#pragma once

namespace i3
{
	template <class T> struct add_volatile				{ typedef T volatile	type; };
	template <class T> struct add_volatile<T&>			{ typedef T&			type; };	//참조에 막바로 붙지 않음.
}
