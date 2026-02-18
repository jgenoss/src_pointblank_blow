#pragma once

//
// i3String의 Quick Search알고리즘(Q_search함수)의 일반화..
// http://www-igm.univ-mlv.fr/~lecroq/string/node19.html#SECTION00190

#include "detail/bm_skiptable.h"

#include "../../itl/range/iterator.h"
#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/begin.h"
#include "../../itl/range/end.h"
#include "../../itl/range/literal_begin.h"
#include "../../itl/range/literal_end.h"

namespace i3
{
	template<class It>
	class bmq
	{
		typedef typename i3::iterator_traits<It>::difference_type			difference_type;
		typedef typename i3::detail::get_bm_skiptable_type<It>::type		skiptable_type;
	public:
		bmq(It f, It l) : m_first(f), m_length( i3::distance(f, l) ),
									m_skiptable( m_length, m_length+1)	// i3String코드를 따른다..
		{
			build_skiptable();
		}
		~bmq() {}
		
		template<class SrcIt>
		SrcIt		operator()( SrcIt src_f, SrcIt src_l ) const
		{
			if ( src_f == src_l ) return src_l;
			if ( m_length == 0 ) return src_f;
			const difference_type src_len = i3::distance(src_f, src_l);
			if ( src_len < m_length ) return src_l;
			return do_search(src_f, src_l);
		}

		template<class Rng>
		typename range_iterator<Rng>::type operator()( Rng& r) const
		{
			return (*this)( begin(r), end(r) );
		}

	private:
		It								m_first;
		const difference_type			m_length;		// 선언 순서가 테이블보다 앞에 있어야함.
		skiptable_type					m_skiptable;
	
		void build_skiptable() 
		{
			It f = m_first;
			for (difference_type i = 0; i < m_length ; ++i, ++f)
				m_skiptable.insert( *f, m_length - i );
		}
	
		template<class SrcIt>
		SrcIt do_search(SrcIt src_f, SrcIt src_l) const
		{
			const difference_type loop_len = i3::distance(src_f, src_l) - m_length;
			
			for ( difference_type j = 0 ; j <= loop_len ; )
			{
				if ( i3::equal( m_first, m_first + m_length, src_f + j) )
					return src_f + j;
		
				j+= m_skiptable[ src_f[j + m_length] ];
			}
			
			return src_l;
		}
	};
	
	template<class SrcIt, class SearchIt> inline
	SrcIt	bmq_search(SrcIt src_f, SrcIt src_l, SearchIt search_f, SearchIt search_l)
	{
		bmq<SearchIt> bm( search_f, search_l);
		return bm(src_f, src_l);
	}
	
	template<class SrcIt, class SearchRng> inline typename i3::enable_if_iterator<SrcIt,
	SrcIt>::type	bmq_search(SrcIt src_f, SrcIt src_l, const SearchRng& search_rng)
	{
		typedef typename i3::range_iterator<const SearchRng>::type SearchIt;
		bmq<SearchIt> bm( i3::literal_begin(search_rng), i3::literal_end(search_rng));
		return bm(src_f, src_l);
	}
	
	template<class SrcSeq, class SearchIt> inline typename i3::lazy_disable_if_iterator<SrcSeq,
	range_iterator<SrcSeq> >::type	bmq_search(SrcSeq& src, SearchIt search_f, SearchIt search_l)
	{
		bmq<SearchIt> bm( search_f, search_l);
		return bm(begin(src), end(src) );
	}
		
	template<class SrcSeq, class SearchRng> inline typename i3::lazy_disable_if_iterator<SrcSeq,
	range_iterator<SrcSeq> >::type bmq_search(SrcSeq& src, const SearchRng& search_rng)
	{
		typedef typename i3::range_iterator<const SearchRng>::type SearchIt;
		bmq<SearchIt> bm( i3::literal_begin(search_rng), i3::literal_end(search_rng));
		return bm(begin(src), end(src));
	}
	
	template<class SrcLit, class SearchRng> inline typename i3::enable_if_iterator<SrcLit,
	SrcLit >::type bmq_search(SrcLit src, const SearchRng& search_rng)
	{
		typedef typename i3::range_iterator<const SearchRng>::type SearchIt;
		bmq<SearchIt> bm( i3::literal_begin(search_rng), i3::literal_end(search_rng));
		return bm(i3::literal_begin(src), i3::literal_end(src));
	}

	// i3String버전의 경우 인덱스를 리턴한다...그쪽 함수를 따로 마련..
	/////////////////////////////////////////////////////////////////////////////////////////

	template<class SrcIt, class SearchIt> inline
	int	bmq_search_pos(SrcIt src_f, SrcIt src_l, SearchIt search_f, SearchIt search_l)
	{
		SrcIt res_it =	bmq_search(src_f, src_l, search_f, search_l);
		return (res_it == src_l) ? -1 : (int)i3::distance(src_f, res_it);
	}
	
	template<class SrcIt, class SearchRng> inline typename i3::enable_if_iterator<SrcIt,
	int>::type	bmq_search_pos(SrcIt src_f, SrcIt src_l, const SearchRng& search_rng)
	{
		SrcIt res_it = bmq_search(src_f, src_l, search_rng);
		return (res_it == src_l) ? -1 : (int)i3::distance(src_f, res_it);
	}
	
	template<class SrcSeq, class SearchIt> inline typename i3::disable_if_iterator<SrcSeq,
	int>::type	bmq_search_pos(SrcSeq& src, SearchIt search_f, SearchIt search_l)
	{
		typedef typename i3::range_iterator<SrcSeq>::type SrcIt;
		SrcIt res_it = bmq_search(src, search_f, search_l);
		return ( res_it == i3::end(src) ) ? -1 : (int)i3::distance(i3::begin(src), res_it);
	}
		
	template<class SrcSeq, class SearchRng> inline typename i3::disable_if_iterator<SrcSeq,
	int>::type bmq_search_pos(SrcSeq& src, const SearchRng& search_rng)
	{
		typedef typename i3::range_iterator<SrcSeq>::type SrcIt;
		SrcIt res_it = bmq_search(src, search_rng);
		return ( res_it == i3::end(src) ) ? -1 : (int)i3::distance(i3::begin(src), res_it);
	}
	
	template<class SrcLit, class SearchRng> inline typename i3::enable_if_iterator<SrcLit,
	int >::type bmq_search_pos(SrcLit src, const SearchRng& search_rng)
	{
		SrcLit lit_begin = i3::literal_begin(src);
		SrcLit lit_end = i3::literal_end(src);
		SrcLit res_it = bmq_search(lit_begin, lit_end, search_rng);
		return ( res_it == lit_end  ) ? -1 : (int)i3::distance( lit_begin , res_it);
	}

	// bm객체는 재활용될수도 있기 때문에, boost에 있는 make_boyer_moore도 따서 쓴다..
	template<class Rng> inline
	bmq<typename range_iterator<const Rng>::type> make_bmq(const Rng& r)
	{
		return bmq< typename range_iterator<const Rng>::type>( literal_begin(r), literal_end(r) );
	}
	
	template<class Rng> inline
	bmq<typename range_iterator<const Rng>::type> make_bmq(Rng& r)
	{
		return bmq< typename range_iterator<Rng>::type>( literal_begin(r), literal_end(r) );
	}


}
