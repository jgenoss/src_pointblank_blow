#pragma once

#include "../../itl/range/iterator_range.h"
#include "../../itl/algorithm/find_if.h"

namespace i3
{
	// 클래스는 반복자 2개의 이항, Pred1은 값에 대한 단항
#pragma warning(push)
#pragma warning(disable : 4512)

	template<class Pred1>
	struct token_finderF
	{
		token_finderF(Pred1 pr, bool compress_on = false ) : m_pr(pr), m_compress(compress_on) {}
		Pred1 m_pr;		bool m_compress;

		template<class FwdIt> 
		iterator_range<FwdIt> operator()(FwdIt f, FwdIt l) const
		{
			typedef iterator_range<FwdIt>	result_type;
			
			FwdIt it = i3::find_if(f, l, m_pr);

			if (it == l) return result_type(l, l);
			
			FwdIt it2 = it;
			if ( !m_compress ) return result_type(it, ++it2);

			while( it2 != l && m_pr(*it2) ) ++it2;
			return result_type(it, it2);
		}
		
	};

#pragma warning(pop)

	template<class Pred1> inline
	token_finderF<Pred1> token_finder(Pred1 pr, bool compress_on = false) 
	{
		return token_finderF<Pred1>(pr, compress_on);
	}

}
