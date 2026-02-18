#pragma once

// shared_ptr만의 고유한 특질함수로 다른 루틴에서는 응용되지 않을 것으로 판단된다..

namespace i3
{
	namespace detail
	{
		template<class T> struct shared_ptr_add_reference { typedef T& type;  };
		template<> struct shared_ptr_add_reference<void> { typedef void type; };
		template<> struct shared_ptr_add_reference<void const> { typedef void type;  };
		template<> struct shared_ptr_add_reference<void volatile> {	typedef void type; };
		template<> struct shared_ptr_add_reference<void const volatile> { typedef void type; };
	}
}

