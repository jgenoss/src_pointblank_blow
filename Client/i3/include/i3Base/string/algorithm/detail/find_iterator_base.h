#pragma once

//
//  boost버전의 find_iterator는 Functor타입대신에 내부멤버에 function컨테이너 멤버변수가 쓰이는데...명백한 낭비이다..
//  그냥 클래스 템플릿 매개변수를 추가해서 변경...
//  기본생성자는 제거함..
//
#include "../../../itl/range/iterator_range.h"

namespace i3
{
	namespace detail
	{
		template<class It, class FindFn>
		class find_iterator_base
		{
		protected:
			typedef It						input_iterator_type;	// 소스의 반복자..
			typedef iterator_range<It>		match_type;				// 찾아낸 반복자 구간..
			typedef FindFn					finder_type;			// 함수포인터나 객체..(부스트는 function사용)
			
			find_iterator_base(const find_iterator_base& rhs) : m_finder(rhs.m_finder) {}
			find_iterator_base( finder_type f) : m_finder(f) {}

			match_type	call_finder(input_iterator_type first, input_iterator_type last) const
			{
				return m_finder(first, last);
			}
		private:
			finder_type						m_finder;
		};
	}

}
