#pragma once


namespace i3
{
	// remove_pointer 
	template <class T> struct remove_pointer	 {	typedef T type; };
	template <class T> struct remove_pointer<T*> {	typedef T type; };
	template <class T> struct remove_pointer<T* const> {	typedef T type; };
	template <class T> struct remove_pointer<T* volatile> {	typedef T type; };
	template <class T> struct remove_pointer<T* const volatile> {	typedef T type; };		// 포인터가 사라지므로, 포인터의 cv도 제거..
	
}
