#pragma once

#include "../../itl/range/empty.h"
#include "../../itl/range/as_literal.h"
#include "../compare/is_equal.h"

namespace i3
{
	// Pred가 이항 연산.. 원본문자열과 비교문자열간 각 문자 비교에 의한 것..
	// 서브스트링과 일치하게 되면 일치하는 구간이 리턴됨..

	template<class SearchIt, class Pred2>
	struct first_finderF
	{
		typedef SearchIt			search_iterator_type;

		template< class SearchRng >
		first_finderF( const SearchRng& Search, Pred2 Comp) : m_Search(begin(Search), end(Search)), m_Comp(Comp) {}
		first_finderF( search_iterator_type SearchBegin, search_iterator_type SearchEnd, Pred2 Comp ) 
			: m_Search(SearchBegin, SearchEnd), m_Comp(Comp) {}
		
		template< class FwdIt >
		iterator_range<FwdIt> operator()(FwdIt Begin, FwdIt End ) const
		{
			typedef iterator_range<FwdIt>	result_type;
			typedef FwdIt					input_iterator_type;

			if( i3::empty(m_Search) )
				return result_type( End, End );

			for(input_iterator_type OuterIt=Begin; OuterIt!=End; ++OuterIt)
			{
				input_iterator_type InnerIt=OuterIt;
				search_iterator_type SubstrIt=m_Search.begin();
				for(; InnerIt!=End && SubstrIt!=m_Search.end(); ++InnerIt,++SubstrIt)
				{
					if( !( m_Comp(*InnerIt,*SubstrIt) ) )
						break;
				}
				
				if ( SubstrIt==m_Search.end() )			// 동일하면..구간 리턴..
					return result_type( OuterIt, InnerIt );
			}

			return result_type( End, End );		// 루프를 다 돌면...false구간 리턴..
		}

	private:
		iterator_range<search_iterator_type> m_Search;
		Pred2 m_Comp;

	};
	
	// first_finder의 목표는 서브스트링을 갖고 대상스트링의 동일문자열 발견 구간을 알려주는 것임...하나만 발견하면 종료된다..

	template<class Range> inline
	first_finderF< typename range_const_iterator<Range>::type, is_equal>
	first_finder( const Range& Search)
	{
		return first_finderF<typename range_const_iterator<Range>::type, is_equal>( as_literal(Search), is_equal() );
	}

	template<class Range, class Pred2> inline
	first_finderF< typename range_const_iterator<Range>::type, Pred2>
	first_finder( const Range& Search, Pred2 pr)
	{
		return first_finderF<typename range_const_iterator<Range>::type, Pred2>( as_literal(Search), pr );
	}


}
