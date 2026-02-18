#pragma once

//
// i3String의 Rabin-Karp의 일반화..
//

#include "../../itl/range/iterator.h"
#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/begin.h"
#include "../../itl/range/end.h"
#include "../../itl/range/literal_begin.h"
#include "../../itl/range/literal_end.h"

// 다른 알고리즘과 달리 메모리할당이 요구되지 않는 것 같다... (KMP, QSEARCH, BM은 모두 메모리할당이 요구된다..)
// 따라서, 일반함수로만 작성한다..

namespace i3
{
	// 이 구현함수 제외한 나머지는 모두 래핑처리한다..
	template<class SrcIt, class SearchIt>
	SrcIt rabin_karp_search(SrcIt src_f, SrcIt src_l, SearchIt search_f, SearchIt search_l)
	{
		if (src_f == src_l ) return src_l;
		if (search_f == search_l) return src_f;
		
		typedef typename i3::iterator_traits<SrcIt>::difference_type  diff_t;
		diff_t src_len = i3::distance(src_f, src_l);
		diff_t search_len = i3::distance(search_f, search_l);

		if ( src_len < search_len ) return src_l;
		
		diff_t d = 1;
		for (diff_t i = 1; i < search_len ; ++i)
			d <<= 1;

		diff_t hx = 0;	diff_t hy = 0;
		for (diff_t i = 0; i < search_len ; ++i)
		{	
			hx = ((hx<<1) + search_f[i]);
			hy = ((hy<<1) + src_f[i]);
		}
		
		const diff_t loop_len = src_len - search_len;
		for ( diff_t j = 0 ; j <= loop_len ; ++j )
		{
			if ( hx == hy && i3::equal(search_f, search_l, src_f + j) )
				return src_f + j;
					
			hy = ( (hy - src_f[j]*d)<<1) + src_f[j + search_len];	// 해시함수.
		}

		return src_l;
	}

	template<class SrcIt, class SearchRng> inline typename i3::enable_if_iterator<SrcIt,
	SrcIt>::type	rabin_karp_search(SrcIt src_f, SrcIt src_l, const SearchRng& search_rng)
	{
		return rabin_karp_search(src_f, src_l, i3::literal_begin(search_rng), i3::literal_end(search_rng) );
	}

	template<class SrcSeq, class SearchIt> inline typename i3::lazy_disable_if_iterator<SrcSeq,
	range_iterator<SrcSeq> >::type	rabin_karp_search(SrcSeq& src, SearchIt search_f, SearchIt search_l)
	{	
		return rabin_karp_search(i3::begin(src), i3::end(src), search_f, search_l);
	}
	
	template<class SrcSeq, class SearchRng> inline typename i3::lazy_disable_if_iterator<SrcSeq,
	range_iterator<SrcSeq> >::type rabin_karp_search(SrcSeq& src, const SearchRng& search_rng)
	{
		return rabin_karp_search(i3::begin(src), i3::end(src), 
			i3::literal_begin(search_rng), i3::literal_end(search_rng) );
	}

	template<class SrcLit, class SearchRng> inline typename i3::enable_if_iterator<SrcLit,
	SrcLit >::type rabin_karp_search(SrcLit src, const SearchRng& search_rng)
	{
		return rabin_karp_search(i3::literal_begin(src), i3::literal_end(src), 
			i3::literal_begin(search_rng), i3::literal_end(search_rng) );
	}
	
////////////////////////////////////
	template<class SrcIt, class SearchIt> inline
	int	rabin_karp_search_pos(SrcIt src_f, SrcIt src_l, SearchIt search_f, SearchIt search_l)
	{
		SrcIt res_it =	rabin_karp_search(src_f, src_l, search_f, search_l);
		return (res_it == src_l) ? -1 : (int)i3::distance(src_f, res_it);
	}
	
	template<class SrcIt, class SearchRng> inline typename i3::enable_if_iterator<SrcIt,
	int>::type	rabin_karp_search_pos(SrcIt src_f, SrcIt src_l, const SearchRng& search_rng)
	{
		SrcIt res_it = rabin_karp_search(src_f, src_l, search_rng);
		return (res_it == src_l) ? -1 : (int)i3::distance(src_f, res_it);
	}
	
	template<class SrcSeq, class SearchIt> inline typename i3::disable_if_iterator<SrcSeq,
	int>::type	rabin_karp_search_pos(SrcSeq& src, SearchIt search_f, SearchIt search_l)
	{
		typedef typename i3::range_iterator<SrcSeq>::type SrcIt;
		SrcIt src_f = i3::begin(src);	SrcIt src_l = i3::end(src);
		SrcIt res_it = rabin_karp_search(src_f, src_l, search_f, search_l);
		return ( res_it == src_l ) ? -1 : (int)i3::distance(src_f, res_it);
	}
		
	template<class SrcSeq, class SearchRng> inline typename i3::disable_if_iterator<SrcSeq,
	int>::type rabin_karp_search_pos(SrcSeq& src, const SearchRng& search_rng)
	{
		typedef typename i3::range_iterator<SrcSeq>::type SrcIt;
		SrcIt src_f = i3::begin(src);	SrcIt src_l = i3::end(src);
		SrcIt res_it = rabin_karp_search(src_f, src_l, search_rng);
		return ( res_it == src_l ) ? -1 : (int)i3::distance(src_f, res_it);
	}
	
	template<class SrcLit, class SearchRng> inline typename i3::enable_if_iterator<SrcLit,
	int >::type rabin_karp_search_pos(SrcLit src, const SearchRng& search_rng)
	{
		SrcLit lit_begin = i3::literal_begin(src);
		SrcLit lit_end = i3::literal_end(src);
		SrcLit res_it = rabin_karp_search(lit_begin, lit_end, search_rng);
		return ( res_it == lit_end  ) ? -1 : (int)i3::distance( lit_begin , res_it);
	}


}

