#pragma once

//
// type_traits처럼 처리하려다가 포기하고, 함수 오버로딩으로 좀더 쉽게 처리..
// 우선 낙관적으로 대체로 swap이 복사보다 유리하다고 가정한다...
//
#include "../../string/stack_string_fwd.h"



namespace i3
{
	//
	// 컨테이너 내부에 큰 배열이 있다면 대입처리..  향후, swap_non_friendly_type은 copy_or_swap을 재정의해서 대입하는쪽으로 
	// 처리..
	//
	template<class B, class C, class T, class A> __forceinline
	void copy_or_swap( stack_basic_string<B,C,T,A>& lhs, stack_basic_string<B,C,T,A>& rhs) { lhs = rhs; }
	//
	
	namespace detail
	{
		template<class T> __forceinline
		void copy_or_swap_impl( T& lhs, T& rhs)
		{
			i3::swap(lhs, rhs);	
		}
		
		template<class T> __forceinline
		void copy_or_swap_impl(T*& lhs, T*& rhs)
		{
			lhs = rhs;								// 포인터의 경우 간단이 대입..
		}

		template<class T, std::size_t N> __forceinline
		void copy_or_swap_impl(T (& lhs)[N], T (& rhs)[N])
		{
			i3::copy(rhs, rhs+N, lhs);			// 복사로 하고, copy 알고리즘을 대신 쓴다.. (경우에 따라 memcpy로 진행됨)
		}
	}
	
	template<class T> __forceinline
	void	copy_or_swap(T& lhs , T& rhs )
	{
		detail::copy_or_swap_impl(lhs, rhs);
	}
	
	// 배열과 포인터의 경우는 모두 swap에 불리한 것으로 간주한다.
	
}
