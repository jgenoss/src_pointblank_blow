#pragma once

#include "first_finder.h"

namespace i3
{
	// Pred2의 좌항은 원본문자열/ 우항은 검색문자열..

	template<class SearchIt, class Pred2>
	struct last_finderF
	{
		typedef SearchIt		search_iterator_type;
		typedef first_finderF<search_iterator_type, Pred2> first_finder_type;

		template<class SearchRng>
		last_finderF( const SearchRng& search, Pred2 pr) : m_search( begin(search), end(search) ), m_pr(pr) {}
		last_finderF( search_iterator_type beg, search_iterator_type end, Pred2 pr) : m_search(beg, end), m_pr(pr) {}
		
		template<class FwdIt>
		iterator_range<FwdIt> operator()(FwdIt beg, FwdIt end) const
		{
			typedef iterator_range<FwdIt>	result_type;
			if ( i3::empty(m_search) ) 
				return result_type(end, end);

			typedef typename iterator_category<FwdIt>::type			category;
			return findit(beg, end, category() );
		}
	private:
		template<class FwdIt>			// 처음부터 끝까지 찾는 버전..
		iterator_range<FwdIt> findit(FwdIt beg, FwdIt end, i3::forward_iterator_tag) const 
		{
			typedef FwdIt input_iterator_type;
            typedef iterator_range<FwdIt> result_type;

			first_finder_type first_finder(m_search.begin(), m_search.end(), m_pr );

            result_type M= first_finder( beg, end );
            result_type Last= M;

            while( M )				// safe_bool이 존재하기 때문에 가능..
            {
				Last = M;
				M = first_finder( i3::end(M), end );
            }

            return Last;			
		}
		
		template<class FwdIt>		// 뒤에서부터 찾는 버전..
		iterator_range<FwdIt> findit(FwdIt beg, FwdIt end, i3::bidirectional_iterator_tag) const 
		{
			
            typedef FwdIt					input_iterator_type;
			typedef iterator_range<FwdIt>	result_type;
           
            for(input_iterator_type OuterIt= end; OuterIt != beg; )
            {
				input_iterator_type OuterIt2=--OuterIt;

				input_iterator_type InnerIt=OuterIt2;
				search_iterator_type SubstrIt=m_search.begin();
				for(; InnerIt!=end && SubstrIt!=m_search.end(); ++InnerIt,++SubstrIt)
				{
					if( !( m_pr(*InnerIt,*SubstrIt) ) )
						break;
				}

				if( SubstrIt == m_search.end() )
					return result_type( OuterIt2, InnerIt );
            }

            return result_type( end, end );
		}
	private:
		iterator_range<search_iterator_type> m_search;
		Pred2 m_pr;
	};

	
	template<class Range> inline
	last_finderF<typename range_const_iterator<Range>::type, is_equal>
	last_finder( const Range& search)
	{
		return last_finderF<typename range_const_iterator<Range>::type, is_equal>(as_literal(search), is_equal());
	}

	template<class Range, class Pred2> inline
	last_finderF<typename range_const_iterator<Range>::type, Pred2>
	last_finder( const Range& search, Pred2 pr)
	{
		return last_finderF<typename range_const_iterator<Range>::type, Pred2>(as_literal(search), pr);
	}

}