#pragma once

//
// 우선 boost의 kmp를 발췌해서 수정후 올림..
// 그다음에 i3String버전에 인터페이스를 맞춘다..
//
#include "../../itl/range/iterator.h"
#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/begin.h"
#include "../../itl/range/end.h"
#include "../../itl/range/literal_begin.h"
#include "../../itl/range/literal_end.h"
#include "../../itl/svector.h"

namespace i3
{
	template <typename patIter>
    class kmp 
	{
        typedef typename i3::iterator_traits<patIter>::difference_type difference_type;
    public:
        kmp ( patIter first, patIter last ) :	pat_first ( first ), pat_last ( last ), 
												k_pattern_length ( i3::distance ( pat_first, pat_last )),
												skip_ ( k_pattern_length + 1 ) 
		{
            init_skip_table ( pat_first, pat_last );
		}
            
        ~kmp () {}
        
       template <typename corpusIter>
       corpusIter operator () ( corpusIter corpus_first, corpusIter corpus_last ) const 
	   {
            compile_assert(( i3::is_same<
                typename i3::iterator_traits<patIter>::value_type, 
                typename i3::iterator_traits<corpusIter>::value_type>::value ));
            if ( corpus_first == corpus_last ) return corpus_last;  // if nothing to search, we didn't find it!
            if ( pat_first == pat_last )       return corpus_first; // empty pattern matches at start

            const difference_type k_corpus_length = i3::distance ( corpus_first, corpus_last );
        //  If the pattern is larger than the corpus, we can't find it!
            if ( k_corpus_length < k_pattern_length ) 
                return corpus_last;

            return do_search   ( corpus_first, corpus_last, k_corpus_length );
		}
		
		template <typename Range>
		typename i3::range_iterator<Range>::type operator () ( Range &r ) const 
		{
            return (*this) (i3::begin(r), i3::end(r));
        }

    private:
        patIter pat_first, pat_last;
        const difference_type k_pattern_length;
        i3::svector <difference_type> skip_;

		template <typename corpusIter>
        corpusIter do_search ( corpusIter corpus_first, corpusIter corpus_last, 
			difference_type k_corpus_length ) const 
		{
            difference_type match_start = 0;  // position in the corpus that we're matching
            
//  At this point, we know:
//          k_pattern_length <= k_corpus_length
//          for all elements of skip, it holds -1 .. k_pattern_length
//      
//          In the loop, we have the following invariants
//              idx is in the range 0 .. k_pattern_length
//              match_start is in the range 0 .. k_corpus_length - k_pattern_length + 1

            const difference_type last_match = k_corpus_length - k_pattern_length;
            difference_type idx = 0;          // position in the pattern we're comparing

            while ( match_start <= last_match ) 
			{
                while ( pat_first [ idx ] == corpus_first [ match_start + idx ] ) 
				{
                    if ( ++idx == k_pattern_length )
                        return corpus_first + match_start;
				}
            //  Figure out where to start searching again
           //   assert ( idx - skip_ [ idx ] > 0 ); // we're always moving forward
                match_start += idx - skip_ [ idx ];
                idx = skip_ [ idx ] >= 0 ? skip_ [ idx ] : 0;
           //   assert ( idx >= 0 && idx < k_pattern_length );
            }
        //  We didn't find anything
			return corpus_last;
		}
    
		void init_skip_table ( patIter first, patIter last ) 
		{
            const difference_type count = i3::distance ( first, last );
    
            int j;
            skip_ [ 0 ] = -1;
            for ( int i = 1; i <= count; ++i ) 
			{
                j = skip_ [ i - 1 ];
                while ( j >= 0 ) 
				{
                    if ( first [ j ] == first [ i - 1 ] )
                        break;
                    j = skip_ [ j ];
                }
                skip_ [ i ] = j + 1;
			}
		}
	};
	
	template<class SrcIt, class SearchIt> inline
	SrcIt	kmp_search(SrcIt src_f, SrcIt src_l, SearchIt search_f, SearchIt search_l)
	{
		kmp<SearchIt> k(search_f, search_l);
		return k(src_f, src_l);
	}
	
	template<class SrcIt, class SearchRng> inline typename i3::enable_if_iterator<SrcIt,
	SrcIt>::type	kmp_search(SrcIt src_f, SrcIt src_l, const SearchRng& search)
	{
		typedef typename i3::range_iterator<const SearchRng>::type	SearchIt;
		kmp<SearchIt> k( i3::literal_begin(search), i3::literal_end(search) );
		return k(src_f, src_l);
	}
	
	template<class SrcSeq, class SearchIt> inline typename i3::lazy_disable_if_iterator<SrcSeq,
	range_iterator<SrcSeq> >::type kmp_search(SrcSeq& src, SearchIt search_f, SearchIt search_l)
	{
		kmp<SearchIt> k(search_f, search_l);
		return k(i3::begin(src), i3::end(src) );
	}
	
	template<class SrcSeq, class SearchRng> inline typename i3::lazy_disable_if_iterator<SrcSeq,
	range_iterator<SrcSeq> >::type kmp_search(SrcSeq& src, const SearchRng& search)
	{
		typedef typename i3::range_iterator<const SearchRng>::type	SearchIt;
		kmp<SearchIt> k(i3::literal_begin(search), i3::literal_end(search));
		return k(i3::begin(src), i3::end(src));
	}

	template<class SrcLit, class SearchRng> inline typename i3::enable_if_iterator<SrcLit,
	SrcLit>::type kmp_search(SrcLit src, const SearchRng& search)
	{
		typedef typename i3::range_iterator<const SearchRng>::type	SearchIt;
		kmp<SearchIt> k(i3::literal_begin(search), i3::literal_end(search));
		return k(i3::literal_begin(src), i3::literal_end(src));
	}


	// i3String버전의 경우 인덱스를 리턴한다...그쪽 함수를 따로 마련..
	template<class SrcIt, class SearchIt> inline
	int	kmp_search_pos(SrcIt src_f, SrcIt src_l, SearchIt search_f, SearchIt search_l)
	{
		SrcIt res_it =	kmp_search(src_f, src_l, search_f, search_l);
		return (res_it == src_l) ? -1 : (int)i3::distance(src_f, res_it);
	}
	
	template<class SrcIt, class SearchRng> inline typename i3::enable_if_iterator<SrcIt,
	int>::type	kmp_search_pos(SrcIt src_f, SrcIt src_l, const SearchRng& search)
	{
		SrcIt res_it = kmp_search(src_f, src_l, search);
		return (res_it == src_l) ? -1 : (int)i3::distance(src_f, res_it);
	}
	
	template<class SrcSeq, class SearchIt> inline typename i3::disable_if_iterator<SrcSeq,
	int>::type kmp_search_pos(SrcSeq& src, SearchIt search_f, SearchIt search_l)
	{
		typedef typename i3::range_iterator<SrcSeq>::type SrcIt;
		SrcIt res_it = kmp_search(src, search_f, search_l);
		return ( res_it == i3::end(src) ) ? -1 : (int)i3::distance(i3::begin(src), res_it);
	}
	
	template<class SrcSeq, class SearchRng> inline typename i3::disable_if_iterator<SrcSeq,
	int >::type kmp_search_pos(SrcSeq& src, const SearchRng& search)
	{
		typedef typename i3::range_iterator<SrcSeq>::type SrcIt;
		SrcIt res_it = kmp_search(src, search);
		return ( res_it == i3::end(src) ) ? -1 : (int)i3::distance(i3::begin(src), res_it);
	}
	
	template<class SrcLit, class SearchRng> inline typename i3::enable_if_iterator<SrcLit,
	int>::type kmp_search_pos(SrcLit src, const SearchRng& search)
	{
		SrcLit lit_begin = i3::literal_begin(src);
		SrcLit lit_end = i3::literal_end(src);
		SrcLit res_it = kmp_search(lit_begin, lit_end, search);
		return ( res_it == lit_end  ) ? -1 : (int)i3::distance( lit_begin , res_it);
	}


	// kmp객체는 재활용될수도 있기 때문에, boost에 있는 make_kmp도 따서 쓴다..
	template<class Rng> inline
	kmp<typename range_iterator<const Rng>::type> make_kmp(const Rng& r)
	{
		return kmp< typename range_iterator<const Rng>::type>( literal_begin(r), literal_end(r) );
	}
	
	template<class Rng> inline
	kmp<typename range_iterator<const Rng>::type> make_kmp(Rng& r)
	{
		return kmp< typename range_iterator<Rng>::type>( literal_begin(r), literal_end(r) );
	}

}