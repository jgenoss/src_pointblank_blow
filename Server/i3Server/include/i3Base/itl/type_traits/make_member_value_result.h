#pragma once

//
//  참조형 강제가 필요하지 않은 경우, 가장 적절한 타입을 선택하게 함..
//  참조형 강제가 아니므로, 가능하다면 기본형을 선호하게끔 처리하자...
//  함수형/함수참조형의 경우, 사이즈 계산이 되지 않는문제가 있어 이것을 필터링할것..

#include "add_const.h"
#include "is_object.h"


namespace i3
{
	namespace detail
	{
		template<class T> 
		struct is_larger_than_void_ptr : mpl::bool_< (sizeof(T)>sizeof(void*)) > {};
		
		template<class T>
		struct is_not_object : mpl::not_< is_object<T> >  {};		// 레퍼런스, void, 함수형 등은 걸러낸다..(레퍼런스의 경우, sizeof가 가능하지만)
	}

	template<class T>
	struct make_member_value_result : mpl::eval_if< mpl::and_< detail::is_not_object<T>,
														detail::is_larger_than_void_ptr<T> >,
											 add_reference<typename add_const<T>::type>,
											 mpl::identity<T> > {};
}

