#pragma once


//
// i3String에는 없는 boyer-moore-horspool 매칭 알고리즘이고, 부스트에서 갖고와
// 적절이 고친다...
// 
// 원문 출처는 http://www-igm.univ-mlv.fr/%7Elecroq/string/node18.html
//
// 전체적으로 볼때 스킵테이블과 문자열 비교의 미묘한 차이를 빼면..
// Q-Search와 거의 같아보인다...
//

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
	class bmh
	{
		typedef typename i3::iterator_traits<It>::difference_type			difference_type;
		typedef typename i3::detail::get_bm_skiptable_type<It>::type		skiptable_type;
	public:
		bmh(It f, It l) : m_first(f), m_length( i3::distance(f, l) ),
									m_skiptable( m_length, m_length )	// i3String코드를 따른다..
		{
			build_skiptable();
		}
		~bmh() {}
		
		template<class SrcIt>
		SrcIt		operator()( SrcIt src_f, SrcIt src_l ) const
		{
			if ( src_f == src_l ) return src_l;
			if ( m_length == 0 ) return src_f;
			const difference_type src_len = i3::distance(src_f, src_l);
			if ( src_len < m_length ) return src_l;
			return do_search(src_f, src_l);
		}

	private:
		It								m_first;
		const difference_type			m_length;		// 선언 순서가 테이블보다 앞에 있어야함.
		skiptable_type					m_skiptable;
	
		void build_skiptable() 
		{
			It f = m_first;
			for (difference_type i = 0; i < m_length - 1; ++i, ++f)
				m_skiptable.insert( *f, m_length - 1 - i );
		}
	
		template<class SrcIt>
		SrcIt do_search(SrcIt src_f, SrcIt src_l) const
		{
			SrcIt curPos = src_f;
			const SrcIt lastPos = src_l - m_length;
			
			while (curPos <= lastPos )
			{
				// 결과는 전체equal과 같지만, 끝문자만 먼저 조사함..
				if ( m_first[m_length - 1] == curPos[m_length - 1]  
					&& i3::equal(m_first, m_first + m_length - 1, curPos) )
					return curPos;
				curPos += m_skiptable[ curPos[m_length - 1] ];
			}

			return src_l;
		}
	};
	
	template<class SrcIt, class SearchIt> inline
	SrcIt	bmh_search(SrcIt src_f, SrcIt src_l, SearchIt search_f, SearchIt search_l)
	{
		bmh<SearchIt> bm( search_f, search_l);
		return bm(src_f, src_l);
	}
	
	template<class SrcIt, class SearchRng> inline typename i3::enable_if_iterator<SrcIt,
	SrcIt>::type	bmh_search(SrcIt src_f, SrcIt src_l, const SearchRng& search_rng)
	{
		typedef typename i3::range_iterator<const SearchRng>::type SearchIt;
		bmh<SearchIt> bm( i3::literal_begin(search_rng), i3::literal_end(search_rng));
		return bm(src_f, src_l);
	}
	
	template<class SrcSeq, class SearchIt> inline typename i3::lazy_disable_if_iterator<SrcSeq,
	range_iterator<SrcSeq> >::type	bmh_search(SrcSeq& src, SearchIt search_f, SearchIt search_l)
	{
		bmh<SearchIt> bm( search_f, search_l);
		return bm(begin(src), end(src) );
	}
		
	template<class SrcSeq, class SearchRng> inline typename i3::lazy_disable_if_iterator<SrcSeq,
	range_iterator<SrcSeq> >::type bmh_search(SrcSeq& src, const SearchRng& search_rng)
	{
		typedef typename i3::range_iterator<const SearchRng>::type SearchIt;
		bmh<SearchIt> bm( i3::literal_begin(search_rng), i3::literal_end(search_rng));
		return bm(begin(src), end(src));
	}
	
	template<class SrcLit, class SearchRng> inline typename i3::enable_if_iterator<SrcLit,
	SrcLit >::type bmh_search(SrcLit src, const SearchRng& search_rng)
	{
		typedef typename i3::range_iterator<const SearchRng>::type SearchIt;
		bmh<SearchIt> bm( i3::literal_begin(search_rng), i3::literal_end(search_rng));
		return bm(i3::literal_begin(src), i3::literal_end(src));
	}

	// i3String버전의 경우 인덱스를 리턴한다...그쪽 함수를 따로 마련..
	/////////////////////////////////////////////////////////////////////////////////////////

	template<class SrcIt, class SearchIt> inline
	int	bmh_search_pos(SrcIt src_f, SrcIt src_l, SearchIt search_f, SearchIt search_l)
	{
		SrcIt res_it =	bmh_search(src_f, src_l, search_f, search_l);
		return (res_it == src_l) ? -1 : (int)i3::distance(src_f, res_it);
	}
	
	template<class SrcIt, class SearchRng> inline typename i3::enable_if_iterator<SrcIt,
	int>::type	bmh_search_pos(SrcIt src_f, SrcIt src_l, const SearchRng& search_rng)
	{
		SrcIt res_it = bmh_search(src_f, src_l, search_rng);
		return (res_it == src_l) ? -1 : (int)i3::distance(src_f, res_it);
	}
	
	template<class SrcSeq, class SearchIt> inline typename i3::disable_if_iterator<SrcSeq,
	int>::type	bmh_search_pos(SrcSeq& src, SearchIt search_f, SearchIt search_l)
	{
		typedef typename i3::range_iterator<SrcSeq>::type SrcIt;
		SrcIt res_it = bmh_search(src, search_f, search_l);
		return ( res_it == i3::end(src) ) ? -1 : (int)i3::distance(i3::begin(src), res_it);
	}
		
	template<class SrcSeq, class SearchRng> inline typename i3::disable_if_iterator<SrcSeq,
	int>::type bmh_search_pos(SrcSeq& src, const SearchRng& search_rng)
	{
		typedef typename i3::range_iterator<SrcSeq>::type SrcIt;
		SrcIt res_it = bmh_search(src, search_rng);
		return ( res_it == i3::end(src) ) ? -1 : (int)i3::distance(i3::begin(src), res_it);
	}
	
	template<class SrcLit, class SearchRng> inline typename i3::enable_if_iterator<SrcLit,
	int >::type bmh_search_pos(SrcLit src, const SearchRng& search_rng)
	{
		SrcLit lit_begin = i3::literal_begin(src);
		SrcLit lit_end = i3::literal_end(src);
		SrcLit res_it = bmh_search(lit_begin, lit_end, search_rng);
		return ( res_it == lit_end  ) ? -1 : (int)i3::distance( lit_begin , res_it);
	}

	// bm객체는 재활용될수도 있기 때문에, boost에 있는 make_boyer_moore도 따서 쓴다..
	template<class Rng> inline
	bmh<typename range_iterator<const Rng>::type> make_bmh(const Rng& r)
	{
		return bmh< typename range_iterator<const Rng>::type>( literal_begin(r), literal_end(r) );
	}
	
	template<class Rng> inline
	bmh<typename range_iterator<const Rng>::type> make_bmh(Rng& r)
	{
		return bmh< typename range_iterator<Rng>::type>( literal_begin(r), literal_end(r) );
	}


}
