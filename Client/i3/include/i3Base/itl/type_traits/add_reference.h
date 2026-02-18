#pragma once

namespace i3
{
	// add_reference
	template <class T>	struct add_reference {	typedef T& type;  };
	template <class T>	struct add_reference<T&> {	typedef T& type;	};
	template <>			struct add_reference<void> { typedef void type; };		// null_type 대신에 그냥 void사용이 boost표준..
	template <>			struct add_reference<void const> { typedef void const type; };
	template <>			struct add_reference<void volatile> { typedef void volatile type; };
	template <>			struct add_reference<void const volatile> { typedef void const volatile type; };
}
