#pragma once

namespace i3
{
	template <class T> struct remove_reference {	typedef T type; 	};
	template <class T> struct remove_reference<T&> { typedef T type; 	};
}
