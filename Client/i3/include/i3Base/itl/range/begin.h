#pragma once

#include "iterator.h"

namespace i3
{

	namespace range_detail
	{
		template<class T> inline
		typename range_iterator<T>::type range_begin( T& t) { return t.begin();	}
		
		template< class It > inline
		It range_begin( const std::pair<It,It>& p ) {	return p.first; }

		template< class It > inline
		It range_begin( std::pair<It,It>& p ) { return p.first; }

		template< class It > inline
		It range_begin( const i3::pair<It,It>& p ) {	return p.first; }

		template< class It > inline
		It range_begin( i3::pair<It,It>& p ) { return p.first; }

		template< class T, std::size_t sz > inline 
		const T* range_begin( const T (&a)[sz] ) {	return a; }

		template< class T, std::size_t sz > inline 
		T* range_begin( T (&a)[sz] ) { return a; }

	}
	
	namespace range_adl_barrier
	{
		template<class T> inline
		typename range_iterator<T>::type begin(T& t) { using namespace range_detail; return range_begin(t); }

		template<class T> inline
		typename range_iterator<const T>::type begin(const T& t) { using namespace range_detail; return range_begin(t); }

		template<class T> inline
		typename range_iterator<const T>::type const_begin(const T& t) { return range_adl_barrier::begin(t); }
	}

	using namespace range_adl_barrier;
}
