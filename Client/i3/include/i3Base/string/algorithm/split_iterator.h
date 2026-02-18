#pragma once

#include "detail/find_iterator_base.h"
#include "../../itl/iterator/iterator_facade.h"
//
//	eof처리 부분과 디폴트 생성자를 제거한다..
//  Finder부분을 템플릿 매개변수로 추가한다.

namespace i3
{
////
	template<class It, class FindFn>
	class split_iterator :	public iterator_facade<	split_iterator<It, FindFn>,
													const iterator_range<It>,
													forward_iterator_tag >,
							private  detail::find_iterator_base<It, FindFn>
	{
		friend class iterator_core_access;
	private:
		typedef detail::find_iterator_base<It, FindFn>	base_type;		// 2개의 기본클래스 중 이것이 선택됨..
		typedef It										input_iterator_type;
		typedef iterator_range<It>						match_type;
	public:
		
		split_iterator(FindFn finder) : base_type(finder) {}		// 디폴트대신 이것을 쓴다..finder이외는 미초기화상태..

		split_iterator( const split_iterator& rhs) : base_type(rhs), m_match(rhs.m_match),
													m_next(rhs.m_next), m_end(rhs.m_end) {}

		split_iterator( It begin, It end, FindFn finder) : base_type(finder), m_match(begin, begin),
													m_next(begin), m_end(end)
		{
			if (begin != end) increment();
		}
		
		// split_iterator의 end()를 만들때는 이것을 쓴다..(기존버전과는 크게 차이가 있음)
		split_iterator( It end, FindFn finder) : base_type(finder), m_match(end, end), m_next(end), m_end(end)
		{
		}

		template<class Range>
		split_iterator( Range& Col, FindFn finder) : base_type(finder)
		{
			iterator_range<typename range_iterator<Range>::type > lit_col( i3::as_literal(Col));
			m_match = make_iterator_range( i3::begin(lit_col), i3::begin(lit_col));		//beg,beg
			m_next  = i3::begin(lit_col);
			m_end   = i3::end(lit_col);

			if (m_next != m_end) increment();
		}
	private:			// facade와 연결되는 템플릿 다형성 함수
		const match_type&	dereference() const { return m_match; }			// 찾아낸 결과 구간 리턴..
		void				increment()										// 반복자 증가할때 내부 계산한다..
		{
			if ( m_next == m_end ) // 종료단계로 m_match를 바꾸고 끝낸다..
			{
				m_match = match_type( m_end, m_end );
			}
			else
			{
				match_type match_res = base_type::call_finder( m_next, m_end);
				m_match = match_type( m_next, match_res.begin() );				// 토크닝된 단어 구간..
				m_next  = match_res.end();
			}
		}
		
		// 미초기화된 것의 경우엔 equal사용을 해선 안되지만, end()의 경우는 괜찮다...루프 종료조건에 쓰일것임..
		bool equal(const split_iterator& rhs) const 
		{
			return m_match.begin() == rhs.m_match.begin() && m_match.end() == rhs.m_match.end();  // && m_next == rhs.m_next && m_end == rhs.m_end ;
		}

		match_type				m_match;
		input_iterator_type		m_next, m_end;
	};
	

	// 
	// 인수추론위한 함수.
	//
	template<class Range, class Finder> inline
	split_iterator<typename range_iterator<Range>::type, Finder> make_split_iterator(Range& Collection, Finder fnd)
	{
		return split_iterator<typename range_iterator<Range>::type, Finder>(Collection, fnd); 
	}
	
	
////
}