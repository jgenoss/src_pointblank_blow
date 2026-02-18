#pragma once

#include "algorithm_util.h"

namespace i3
{

	namespace vector_set_util
	{
		// linear한 것은 상등 비교를 통해서 처리..
		// find기반이기때문에 multi index_of에서는 인덱스 하나만 리턴된다..
		// find기반이기때문에 multi remove에서는 요소 1개만 삭제된다..

		template<class K, class P, class A, class U>
		typename i3::vector_set<K, P, A>::size_type	linear_index_of( const i3::vector_set<K, P, A>& ctn, const U& obj);

		template<class K, class P, class A, class U>
		typename i3::vector_multiset<K, P, A>::size_type linear_1st_index_of( const i3::vector_multiset<K, P, A>& ctn, const U& obj);

		template<class K, class P, class A, class U>
		bool									linear_remove( i3::vector_set<K, P, A>& ctn, const U& obj);
		
		template<class K, class P, class A, class U>
		bool									linear_remove_1st( i3::vector_multiset<K, P, A>& ctn, const U& obj);

		//
		// binary한 것은 동등 비교를 통해서 처리..(동등 비교의 경우, 객체의 동등성은 <로만 처리됨)
		//

		template<class K, class P, class A, class U>
		typename i3::vector_set<K, P, A>::size_type	binary_index_of( const i3::vector_set<K, P, A>& ctn, const U& obj);

		template<class K, class P, class A, class U>
		typename i3::vector_multiset<K, P, A>::size_type binary_1st_index_of( const i3::vector_multiset<K, P, A>& ctn, const U& obj);

		template<class K, class P, class A, class U>
		bool									binary_remove( i3::vector_set<K, P, A>& ctn, const U& obj);

		template<class K, class P, class A, class U>
		bool									binary_remove_1st( i3::vector_multiset<K, P, A>& ctn, const U& obj);

		template<class K, class P, class A, class T, class Pred> 
		i3::pair<typename i3::vector_set<K,P,A>::const_iterator, bool> binary_find(const i3::vector_set<K, P, A>& ctn, const T& Val, Pred Pr );

		template<class K, class P, class A, class T> 
		i3::pair<typename i3::vector_set<K,P,A>::const_iterator, bool> binary_find(const i3::vector_set<K, P, A>& ctn, const T& Val );

		template<class K, class P, class A, class T, class Pred> 
		i3::pair<typename i3::vector_multiset<K,P,A>::const_iterator, bool> binary_find(const i3::vector_multiset<K, P, A>& ctn, const T& Val, Pred Pr );

		template<class K, class P, class A, class T> 
		i3::pair<typename i3::vector_multiset<K,P,A>::const_iterator, bool> binary_find(const i3::vector_multiset<K, P, A>& ctn, const T& Val );



	}

	namespace vsu = vector_set_util;

}

#include "detail/vector_set_util.inl"