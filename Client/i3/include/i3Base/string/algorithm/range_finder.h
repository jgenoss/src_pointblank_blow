#pragma once

//
//  range_finder의 경우 find 역할을 수행하지 않는다..
//  실제로 찾는게 없고, 미리 보관된 입력구간을 그대로 리턴한다...(외부에서 발견된 것을 그대로 넣는것으로 간주)
// 
#include "../../itl/range/iterator_range.h"

namespace i3
{
	template<class FwdIt1>
	struct range_finderF 
	{
		typedef FwdIt1									input_iterator_type;
		typedef iterator_range<input_iterator_type>		result_type;

		range_finderF(input_iterator_type f, input_iterator_type l) : m_Rng(f, l) {}
		range_finderF(const iterator_range<input_iterator_type>& rng) : m_Rng(rng) {}
		
		template<class FwdIt2>
		iterator_range<FwdIt2> operator()( FwdIt2, FwdIt2) const { return m_Rng; }
	private:
		iterator_range<input_iterator_type>	m_Rng;
	};


	template<class FwdIt> inline
	range_finderF<FwdIt> range_finder(FwdIt f, FwdIt l) 
	{
		return range_finderF<FwdIt>(f, l);
	}

	template<class FwdIt> inline
	range_finderF<FwdIt> range_finder(i3::iterator_range<FwdIt> iter_rng) 
	{
		return range_finderF<FwdIt>(iter_rng);
	}
}