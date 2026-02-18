#pragma once

// 무식한 문자열검색.
//
// http://www-igm.univ-mlv.fr/~lecroq/string/node3.html#SECTION0030
//
// i3String.cpp에 존재하기 때문에 호환성을 위해 넣음..
//
// 퍼포먼스 테스트를 제외하고는 쓸모는 없다..
//

#include "../../itl/range/iterator.h"
#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/begin.h"
#include "../../itl/range/end.h"
#include "../../itl/range/literal_begin.h"
#include "../../itl/range/literal_end.h"

namespace i3
{
	// 이 구현함수 제외한 나머지는 모두 래핑처리한다..
	template<class SrcIt, class SearchIt>
	SrcIt bf_string_search(SrcIt src_f, SrcIt src_l, SearchIt search_f, SearchIt search_l)
	{
		if (src_f == src_l ) return src_l;
		if (search_f == search_l) return src_f;
		
		typedef typename i3::iterator_traits<SrcIt>::difference_type  diff_t;
		diff_t src_len = i3::distance(src_f, src_l);
		diff_t search_len = i3::distance(search_f, search_l);

		if ( src_len < search_len ) return src_l;
		
		const diff_t loop_len = src_len - search_len;

		for ( diff_t j = 0 ; j <= loop_len ; ++j )
		{
			diff_t i;
			for ( i = 0; i < search_len && search_f[i] == src_f[i+j]; ++i){}
			if ( i >= search_len)
				return src_f + j;	
		}

		return src_l;
	}

	template<class SrcIt, class SearchRng> inline typename i3::enable_if_iterator<SrcIt,
	SrcIt>::type	bf_string_search(SrcIt src_f, SrcIt src_l, const SearchRng& search_rng)
	{
		return bf_string_search(src_f, src_l, i3::literal_begin(search_rng), i3::literal_end(search_rng) );
	}

	template<class SrcSeq, class SearchIt> inline typename i3::lazy_disable_if_iterator<SrcSeq,
	range_iterator<SrcSeq> >::type	bf_string_search(SrcSeq& src, SearchIt search_f, SearchIt search_l)
	{	
		return bf_string_search(i3::begin(src), i3::end(src), search_f, search_l);
	}
	
	template<class SrcSeq, class SearchRng> inline typename i3::lazy_disable_if_iterator<SrcSeq,
	range_iterator<SrcSeq> >::type bf_string_search(SrcSeq& src, const SearchRng& search_rng)
	{
		return bf_string_search(i3::begin(src), i3::end(src), 
			i3::literal_begin(search_rng), i3::literal_end(search_rng) );
	}

	template<class SrcLit, class SearchRng> inline typename i3::enable_if_iterator<SrcLit,
	SrcLit >::type bf_string_search(SrcLit src, const SearchRng& search_rng)
	{
		return bf_string_search(i3::literal_begin(src), i3::literal_end(src), 
			i3::literal_begin(search_rng), i3::literal_end(search_rng) );
	}
	
////////////////////////////////////
	template<class SrcIt, class SearchIt> inline
	int	bf_string_search_pos(SrcIt src_f, SrcIt src_l, SearchIt search_f, SearchIt search_l)
	{
		SrcIt res_it =	bf_string_search(src_f, src_l, search_f, search_l);
		return (res_it == src_l) ? -1 : (int)i3::distance(src_f, res_it);
	}
	
	template<class SrcIt, class SearchRng> inline typename i3::enable_if_iterator<SrcIt,
	int>::type	bf_string_search_pos(SrcIt src_f, SrcIt src_l, const SearchRng& search_rng)
	{
		SrcIt res_it = bf_string_search(src_f, src_l, search_rng);
		return (res_it == src_l) ? -1 : (int)i3::distance(src_f, res_it);
	}
	
	template<class SrcSeq, class SearchIt> inline typename i3::disable_if_iterator<SrcSeq,
	int>::type	bf_string_search_pos(SrcSeq& src, SearchIt search_f, SearchIt search_l)
	{
		typedef typename i3::range_iterator<SrcSeq>::type SrcIt;
		SrcIt src_f = i3::begin(src);	SrcIt src_l = i3::end(src);
		SrcIt res_it = bf_string_search(src_f, src_l, search_f, search_l);
		return ( res_it == src_l ) ? -1 : (int)i3::distance(src_f, res_it);
	}
		
	template<class SrcSeq, class SearchRng> inline typename i3::disable_if_iterator<SrcSeq,
	int>::type bf_string_search_pos(SrcSeq& src, const SearchRng& search_rng)
	{
		typedef typename i3::range_iterator<SrcSeq>::type SrcIt;
		SrcIt src_f = i3::begin(src);	SrcIt src_l = i3::end(src);
		SrcIt res_it = bf_string_search(src_f, src_l, search_rng);
		return ( res_it == src_l ) ? -1 : (int)i3::distance(src_f, res_it);
	}
	
	template<class SrcLit, class SearchRng> inline typename i3::enable_if_iterator<SrcLit,
	int >::type bf_string_search_pos(SrcLit src, const SearchRng& search_rng)
	{
		SrcLit lit_begin = i3::literal_begin(src);
		SrcLit lit_end = i3::literal_end(src);
		SrcLit res_it = bf_string_search(lit_begin, lit_end, search_rng);
		return ( res_it == lit_end  ) ? -1 : (int)i3::distance( lit_begin , res_it);
	}
	
	
}