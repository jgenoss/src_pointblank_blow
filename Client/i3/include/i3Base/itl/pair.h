#pragma once

#include "swap.h"

#pragma warning(push)
#pragma warning( disable : 4512 )

namespace i3
{	
	
	template<class T1, class T2>
	struct pair
	{	
		typedef pair<T1, T2>	my_type;
		typedef T1				first_type;
		typedef T2				second_type;

		pair() : first(T1()), second(T2())	{}
		pair(const T1& _Val1, const T2& _Val2) : first(_Val1), second(_Val2) { }
		
		template< template<class, class> class Ctn, class Other1, class Other2>					// 이 함수를 통해서 std::pair의 암시적호환이 됨..
		pair(const Ctn<Other1, Other2>& _Right) : first(_Right.first), second(_Right.second) {} 
		
		template<class _Other1,	class _Other2>
		pair(const pair<_Other1, _Other2>& _Right) : first(_Right.first), second(_Right.second) {}

		template<class _Other1,	class _Other2>
		pair&	operator=(const pair<_Other1, _Other2>& _Right)	{ first = _Right.first;		second = _Right.second; return *this; }

//		pair(const pair& _Right) : first(_Right.first), second(_Right.second) {}
//		pair&	operator=(const pair& _Right)	{ first = _Right.first;		second = _Right.second; return *this; }


		void swap(my_type& _Right)
		{	// exchange contents with _Right
			if (this != &_Right)
			{	// different, worth swapping
				i3::swap(first, _Right.first);
				i3::swap(second, _Right.second);
			}
		}

		T1 first;	// the first stored value
		T2 second;	// the second stored value
	};

		
	template<class T1,	class T2> inline
	void swap(pair<T1, T2>& _Left, pair<T1, T2>& _Right) {	_Left.swap(_Right);	}

	template<class T1, class T2> inline
	bool operator==(const pair<T1, T2>& _Left, const pair<T1, T2>& _Right)
	{	
		return (_Left.first == _Right.first && _Left.second == _Right.second);
	}

	template<class T1, class T2> inline
	bool operator!=(const pair<T1, T2>& _Left, const pair<T1, T2>& _Right)
	{	
		return (!(_Left == _Right));
	}

	template<class T1, class T2> inline
	bool operator<(const pair<T1, T2>& _Left, const pair<T1, T2>& _Right)
	{	
		return (_Left.first < _Right.first || !(_Right.first < _Left.first) && _Left.second < _Right.second);
	}

	template<class T1,	class T2> inline 
	bool operator>(const pair<T1, T2>& _Left, const pair<T1, T2>& _Right)
	{	
		return (_Right < _Left);
	}

	template<class T1,	class T2> inline
	bool operator<=(const pair<T1, T2>& _Left, const pair<T1, T2>& _Right)
	{	
		return (!(_Right < _Left));
	}

	template<class T1, class T2> inline
	bool operator>=(const pair<T1, T2>& _Left, const pair<T1, T2>& _Right)
	{	
		return (!(_Left < _Right));
	}

	template<class T1, class T2> inline
	pair<T1, T2> make_pair(T1 _Val1, T2 _Val2)
	{	
		return (pair<T1, T2>(_Val1, _Val2));
	}

	
	
	
}

#pragma warning(pop)