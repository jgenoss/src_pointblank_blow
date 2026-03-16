#pragma once

namespace i3
{
	template<class T>
	T		align_down(T value, uintptr_t align);
	template<class T>
	T		align_up(T value, uintptr_t align);
	template<class T>
	bool	is_align(T value, uintptr_t align);

	template<size_t arg, size_t align>
	struct static_align_down {  static const size_t value =  uintptr_t(arg) & ~(align - 1);   };
	template<size_t arg, size_t align>
	struct static_align_up {  static const size_t value =  ( uintptr_t(arg) + align - 1 ) & ~(align - 1);   };


	template<class T> __forceinline
	T		align_down(T value, uintptr_t align) {	return T( uintptr_t(value) & ~(align - 1) ); }

	template<class T> __forceinline
	T		align_up(T value, uintptr_t align) { return T( ( uintptr_t(value) + align - 1 ) & ~(align - 1) ); }

	template<class T> __forceinline
	bool	is_align(T value, uintptr_t align) { return ( uintptr_t(value) & (align - 1) ) == 0; }
	
}