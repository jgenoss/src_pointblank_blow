#pragma once

#include "iterator.h"

namespace i3
{
	namespace range_detail
	{
		template<class T> inline 
		typename range_iterator<T>::type range_end( T& t ) { return t.end(); }
		
		template< class It > inline 
		It range_end( const std::pair<It,It>& p ) {	return p.second; }

		template< class It > inline 
		It range_end( std::pair<It,It>& p ) { return p.second; }
		
		template< class It > inline 
		It range_end( const i3::pair<It,It>& p ) {	return p.second; }

		template< class It > inline 
		It range_end( i3::pair<It,It>& p ) { return p.second; }
		
		template< class T, std::size_t sz > inline 
		const T* range_end( const T (&a)[sz] ) { return a + sz; }

		template< class T, std::size_t sz > inline 
		T* range_end( T (&a)[sz] ) { return a + sz; }
	}

	namespace range_adl_barrier
	{
		template<class T> inline
		typename range_iterator<T>::type end(T& t) { using namespace range_detail; return range_end(t); }

		template<class T> inline
		typename range_iterator<const T>::type end(const T& t) { using namespace range_detail; return range_end(t); }

		template<class T> inline
		typename range_iterator<const T>::type const_end(const T& t) { return range_adl_barrier::end(t); }
	}

	using namespace range_adl_barrier;
}
