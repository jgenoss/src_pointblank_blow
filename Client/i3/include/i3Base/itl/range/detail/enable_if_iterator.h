#pragma once

#include "../../enable_if.h"
#include "../../type_traits/is_iterator.h"

namespace i3
{
	template<class It, class Ret = void> 
	struct enable_if_iterator : enable_if< i3::is_iterator<It>, Ret> {};
	template<class Seq, class Ret = void>
	struct disable_if_iterator : disable_if< i3::is_iterator<Seq>, Ret> {};
	
	template<class It, class T> 
	struct lazy_enable_if_iterator : lazy_enable_if< i3::is_iterator<It>, T> {};
	template<class Seq, class T>
	struct lazy_disable_if_iterator : lazy_disable_if< i3::is_iterator<Seq>, T> {};



	// 아래 2개는 같은 종류 템플릿 매개변수의 오버로딩함수버전을 만들때 쓴다..
	template<class It>
	struct iterator_return : enable_if< i3::is_iterator<It>, It> {};
	template<class Seq>
	struct non_iterator_return : lazy_disable_if< i3::is_iterator<Seq>, i3::add_reference<Seq> > {};

	template<class Unknown>
	struct check_iterator_return : mpl::eval_if<	i3::is_iterator<Unknown>, 
													mpl::identity<Unknown>, 
													i3::add_reference<Unknown> > {};

}
