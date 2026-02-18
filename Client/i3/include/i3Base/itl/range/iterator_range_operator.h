#pragma once

#include "algorithm/equal.h"

namespace i3
{
	template< class It1, class It2 > inline 
	bool operator==( const iterator_range<It1>& l, const iterator_range<It2>& r) {	return range::equal( l, r ); }

	template< class It, class FwdRange > inline 
	bool operator==( const iterator_range<It>& l, const FwdRange& r) {	return range::equal( l, r ); }

	template< class It, class FwdRange > inline 
	bool operator==( const FwdRange& l,	const iterator_range<It>& r ) { return range::equal( l, r ); }

	template< class It1, class It2 > inline 
	bool operator!=( const iterator_range<It1>& l, const iterator_range<It2>& r) {	return !range::equal( l, r ); }

	template< class It, class FwdRange > inline 
	bool operator!=( const iterator_range<It>& l, const FwdRange& r)  {	return !range::equal( l, r ); }

	template< class It, class FwdRange > inline
	bool operator!=( const FwdRange& l, const iterator_range<It>& r ) {	return !range::equal( l, r ); }
	
	template< class It1, class It2 > inline 
	bool operator<( const iterator_range<It1>& l, const iterator_range<It2>& r) 
	{	
		return i3::lexicographical_compare(i3::begin(l), i3::end(l), i3::begin(r), i3::end(r) );
	}

	template< class It, class FwdRange > inline 
	bool operator<( const iterator_range<It>& l, const FwdRange& r) 
	{	
		return i3::lexicographical_compare(i3::begin(l), i3::end(l), i3::begin(r), i3::end(r) );
	}	

	template< class It, class FwdRange > inline 
	bool operator<( const FwdRange& l,	const iterator_range<It>& r ) 
	{ 
		return i3::lexicographical_compare(i3::begin(l), i3::end(l), i3::begin(r), i3::end(r) );
	}
	
	template< class It1, class It2 > inline 
	bool operator<=( const iterator_range<It1>& l, const iterator_range<It2>& r) {	return !(r < l); }

	template< class It, class FwdRange > inline 
	bool operator<=( const iterator_range<It>& l, const FwdRange& r) {	return !(r < l); }
	
	template< class It, class FwdRange > inline 
	bool operator<=( const FwdRange& l,	const iterator_range<It>& r ) {	return !(r < l); }
	
	template< class It1, class It2 > inline 
	bool operator>( const iterator_range<It1>& l, const iterator_range<It2>& r) {	return (r < l); }

	template< class It, class FwdRange > inline 
	bool operator>( const iterator_range<It>& l, const FwdRange& r) {	return (r < l); }

	template< class It, class FwdRange > inline 
	bool operator>( const FwdRange& l,	const iterator_range<It>& r ) {	return (r < l); }

	template< class It1, class It2 > inline 
	bool operator>=( const iterator_range<It1>& l, const iterator_range<It2>& r) {	return !(l < r); }

	template< class It, class FwdRange > inline 
	bool operator>=( const iterator_range<It>& l, const FwdRange& r) {	return !(l < r); }

	template< class It, class FwdRange > inline 
	bool operator>=( const FwdRange& l,	const iterator_range<It>& r ) {	return !(l < r); }

}

