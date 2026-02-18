#pragma once

#include "../../type_traits/has_push_back_mem_fun.h"
#include "../../type_traits/has_iter_value_insert_mem_fun.h"
#include "../../type_traits/is_iterator.h"
#include "../detail/enable_if_iterator.h"

namespace i3
{
	//
	// Range에서 배열과 반복자의 경우는 모두 이터레이터로 간주...
	// 나머지는 컨테이너인데, 비어있는것으로 가정하고 들어가므로, 
	// begin()을 찾고, 미리 확보된것을 가정하고 이 이터레이터를 활용한다..... (이 경우 inserter동작이 없음)
	//
	// insert와 push_back지원여부를 보고 컴파일 분기하고 i3::inserter를 쓴다..
	template<class T>
	struct get_generic_inserter_type
	{
		typedef typename mpl::multi_if<	i3::is_iterator<T>, T, 
										i3::has_push_back_mem_fun<T>, i3::back_insert_iterator<T>,
										i3::has_iter_value_insert_mem_fun<T>, i3::insert_iterator<T>, 
										i3::null_type >::type	type;
	};
	
	

	template<class It> inline
	typename i3::enable_if_iterator<It, It>::type generic_inserter(It it) { return it; }	
	 
	namespace detail
	{
		template<class InserterType>
		struct dispatch_generic_inserter;

		template<class Ctn>
		struct dispatch_generic_inserter< i3::back_insert_iterator<Ctn> >
		{
			static i3::back_insert_iterator<Ctn> call(Ctn& ctn) { return i3::back_inserter(ctn); }
		};

		template<class Ctn>
		struct dispatch_generic_inserter< i3::insert_iterator<Ctn> >
		{
			static i3::insert_iterator<Ctn>	call(Ctn& ctn) { return i3::inserter(ctn, ctn.end()); }
		};
		
		template<class ErrorCtn>
		struct dispatch_generic_inserter
		{
			static i3::null_type	call(ErrorCtn& ctn) { compile_assert_msg(false, container_must_have_push_back_or_insert); }
		};

	}
		
	template<class Ctn> inline
	typename i3::lazy_disable_if_iterator<Ctn, get_generic_inserter_type<Ctn> >::type generic_inserter(Ctn& ctn)
	{
		typedef typename get_generic_inserter_type<Ctn>::type	generic_inserter_type;
		return detail::dispatch_generic_inserter< generic_inserter_type >::call(ctn);
	}
	
	
}