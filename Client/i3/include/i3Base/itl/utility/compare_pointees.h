#pragma once


//  포인터 참조시 널값이 포함된 것을 함께 고려한 비교함수이다...
//  널체크가 필요없다면 쓰지말것..

namespace i3
{
	template<class T> inline
	bool equal_pointees( const T* x, const T* y) { return (!x) != (!y) ? false : ( !x ? true : *x == *y ) ; }

	template<class Opt> inline typename i3::disable_if<i3::is_pointer<Opt>,
	bool>::type equal_pointees( const Opt& x, const Opt& y) { return (!x) != (!y) ? false : ( !x ? true : *x == *y ) ; }

	template<class T> inline
	bool less_pointees( const T* x, const T* y) { return !y ? false : ( !x ? true : *x < *y ) ; }

	template<class Opt> inline typename i3::disable_if<i3::is_pointer<Opt>,
	bool>::type less_pointees( const Opt& x, const Opt& y) { return !y ? false : ( !x ? true : *x < *y ) ; }
	

	template<class T>
	struct equal_pointees_t : i3::binary_function<T,T,bool>
	{
		bool operator() ( const T& x, const T& y ) const { return equal_pointees(x,y) ; }
	};
	
	template<class T>
	struct equal_pointees_t<T*> : i3::binary_function<T*,T*,bool>
	{
		bool operator() ( const T* x, const T* y ) const { return equal_pointees(x,y) ; }
	};

	template<class T>
	struct less_pointees_t : i3::binary_function<T,T,bool>
	{
		bool operator() ( const T& x, const T& y ) const { return less_pointees(x,y) ; }
	};

	template<class T>
	struct less_pointees_t<T*> : i3::binary_function<T*,T*,bool>
	{
		bool operator() ( const T* x, const T* y ) const { return less_pointees(x,y) ; }
	};


}	

