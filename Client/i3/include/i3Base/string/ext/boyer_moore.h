#pragma once

#include "detail/bm_skiptable.h"

#include "../../itl/range/iterator.h"
#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/begin.h"
#include "../../itl/range/end.h"
#include "../../itl/range/literal_begin.h"
#include "../../itl/range/literal_end.h"
#include "../../itl/svector.h"

namespace i3
{
	template<class It>
	class boyer_moore
	{
		typedef typename i3::iterator_traits<It>::difference_type			difference_type;
		typedef typename i3::detail::get_bm_skiptable_type<It>::type		skiptable_type;
	public:
		boyer_moore(It f, It l) : m_first(f), m_length( i3::distance(f, l) ),
									m_skiptable( m_length, m_length),		// i3String코드를 따른다..
									m_suffixtable( m_length + 1, m_length) 
		{
			build_skiptable();
			build_suffixtable();
		}
		~boyer_moore() {}
		
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
		i3::svector<difference_type>	m_suffixtable;

		void build_skiptable() 
		{
			It f = m_first;
			for (difference_type i = 0; i < m_length - 1 ; ++i, ++f)
				m_skiptable.insert( *f, m_length - 1 - i );
		}
		void build_suffixtable()
		{
			if (m_length < 1 ) 
				return;
			i3::svector<difference_type>	suff( m_length + 1 );			// 임시컨테이너다.. (i3Memalloc과 동일역할을 수행한다.)

			suff[m_length - 1] = m_length;

			// suff 생성 블록..
			{
				difference_type f = 0;
				difference_type g = m_length - 1;
				
				for (difference_type i = m_length - 2 ; i >= 0 ; --i )
				{
					if ( i > g && suff[ i+m_length-1-f] < i-g )
					{
						suff[i] = suff[ i + m_length -1 -f];
					}
					else
					{
						if ( i < g ) g = i;
						f = i;
						while( g >= 0 && m_first[g] == m_first[g+m_length-1-f] )
							--g;
						suff[i] = f - g;
					}
				}
			}
			
			for ( difference_type i = m_length -1, j = 0; i >= 0; --i )
			{
				if ( suff[ i] == i + 1 )
				{
					for ( ; j < m_length - 1 -i ; ++j )
					{
						if ( m_suffixtable[j] == m_length )
							m_suffixtable[j] = m_length - 1 -i;
					}
				}
			}
			
			for ( difference_type i = 0; i <= m_length - 2 ; ++i )
			{
				m_suffixtable[ m_length - 1 - suff[i] ] = m_length - 1 -i;
			}
		}
		template<class SrcIt>
		SrcIt do_search(SrcIt src_f, SrcIt src_l) const
		{
			const difference_type loop_len = i3::distance(src_f, src_l) - m_length;

			for ( difference_type j = 0, i = 0 ; j <= loop_len ; )
			{
				for ( i = m_length - 1 ; i >= 0 && m_first[i] == src_f[i + j] ; --i) {}

				if ( i < 0 )
				{
					return src_f + j;
				}
				else
				{
					difference_type skip = m_skiptable[ src_f[i + j] ];
					difference_type acc  = skip - m_length + 1 + i;
					if ( m_suffixtable[i] > acc )
						j+= m_suffixtable[i];
					else
						j+= acc;
				}
			}
			
			return src_l;
		}
	};
	
	template<class SrcIt, class SearchIt> inline
	SrcIt	boyer_moore_search(SrcIt src_f, SrcIt src_l, SearchIt search_f, SearchIt search_l)
	{
		boyer_moore<SearchIt> bm( search_f, search_l);
		return bm(src_f, src_l);
	}
	
	template<class SrcIt, class SearchRng> inline typename i3::enable_if_iterator<SrcIt,
	SrcIt>::type	boyer_moore_search(SrcIt src_f, SrcIt src_l, const SearchRng& search_rng)
	{
		typedef typename i3::range_iterator<const SearchRng>::type SearchIt;
		boyer_moore<SearchIt> bm( i3::literal_begin(search_rng), i3::literal_end(search_rng));
		return bm(src_f, src_l);
	}
	
	template<class SrcSeq, class SearchIt> inline typename i3::lazy_disable_if_iterator<SrcSeq,
	range_iterator<SrcSeq> >::type	boyer_moore_search(SrcSeq& src, SearchIt search_f, SearchIt search_l)
	{
		boyer_moore<SearchIt> bm( search_f, search_l);
		return bm(begin(src), end(src) );
	}
		
	template<class SrcSeq, class SearchRng> inline typename i3::lazy_disable_if_iterator<SrcSeq,
	range_iterator<SrcSeq> >::type boyer_moore_search(SrcSeq& src, const SearchRng& search_rng)
	{
		typedef typename i3::range_iterator<const SearchRng>::type SearchIt;
		boyer_moore<SearchIt> bm( i3::literal_begin(search_rng), i3::literal_end(search_rng));
		return bm(begin(src), end(src));
	}
	
	template<class SrcLit, class SearchRng> inline typename i3::enable_if_iterator<SrcLit,
	SrcLit >::type boyer_moore_search(SrcLit src, const SearchRng& search_rng)
	{
		typedef typename i3::range_iterator<const SearchRng>::type SearchIt;
		boyer_moore<SearchIt> bm( i3::literal_begin(search_rng), i3::literal_end(search_rng));
		return bm(i3::literal_begin(src), i3::literal_end(src));
	}

	// i3String버전의 경우 인덱스를 리턴한다...그쪽 함수를 따로 마련..
	/////////////////////////////////////////////////////////////////////////////////////////

	template<class SrcIt, class SearchIt> inline
	int	boyer_moore_search_pos(SrcIt src_f, SrcIt src_l, SearchIt search_f, SearchIt search_l)
	{
		SrcIt res_it =	boyer_moore_search(src_f, src_l, search_f, search_l);
		return (res_it == src_l) ? -1 : (int)i3::distance(src_f, res_it);
	}
	
	template<class SrcIt, class SearchRng> inline typename i3::enable_if_iterator<SrcIt,
	int>::type	boyer_moore_search_pos(SrcIt src_f, SrcIt src_l, const SearchRng& search_rng)
	{
		SrcIt res_it = boyer_moore_search(src_f, src_l, search_rng);
		return (res_it == src_l) ? -1 : (int)i3::distance(src_f, res_it);
	}
	
	template<class SrcSeq, class SearchIt> inline typename i3::disable_if_iterator<SrcSeq,
	int>::type	boyer_moore_search_pos(SrcSeq& src, SearchIt search_f, SearchIt search_l)
	{
		typedef typename i3::range_iterator<SrcSeq>::type SrcIt;
		SrcIt res_it = boyer_moore_search(src, search_f, search_l);
		return ( res_it == i3::end(src) ) ? -1 : (int)i3::distance(i3::begin(src), res_it);
	}
		
	template<class SrcSeq, class SearchRng> inline typename i3::disable_if_iterator<SrcSeq,
	int>::type boyer_moore_search_pos(SrcSeq& src, const SearchRng& search_rng)
	{
		typedef typename i3::range_iterator<SrcSeq>::type SrcIt;
		SrcIt res_it = boyer_moore_search(src, search_rng);
		return ( res_it == i3::end(src) ) ? -1 : (int)i3::distance(i3::begin(src), res_it);
	}
	
	template<class SrcLit, class SearchRng> inline typename i3::enable_if_iterator<SrcLit,
	int >::type boyer_moore_search_pos(SrcLit src, const SearchRng& search_rng)
	{
		SrcLit lit_begin = i3::literal_begin(src);
		SrcLit lit_end = i3::literal_end(src);
		SrcLit res_it = boyer_moore_search(lit_begin, lit_end, search_rng);
		return ( res_it == lit_end  ) ? -1 : (int)i3::distance( lit_begin , res_it);
	}

	// bm객체는 재활용될수도 있기 때문에, boost에 있는 make_boyer_moore도 따서 쓴다..
	template<class Rng> inline
	boyer_moore<typename range_iterator<const Rng>::type> make_boyer_moore(const Rng& r)
	{
		return boyer_moore< typename range_iterator<const Rng>::type>( literal_begin(r), literal_end(r) );
	}
	
	template<class Rng> inline
	boyer_moore<typename range_iterator<const Rng>::type> make_boyer_moore(Rng& r)
	{
		return boyer_moore< typename range_iterator<Rng>::type>( literal_begin(r), literal_end(r) );
	}


}