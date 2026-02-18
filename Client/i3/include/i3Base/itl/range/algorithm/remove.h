#pragma once

#include "../../algorithm/remove.h"
#include "../generic_erase_iter_end.h"
#include "../begin.h"
#include "../end.h"
#include "../as_literal.h"
#include "../../type_traits/has_mutable_iterator.h"
#include "../../utility/copy_or_swap.h"

namespace i3
{
	//
	// erase기능이 들어가 있어야한다..
	// 리턴값은 void로 변경.. ( std::list::remove()를 기준으로 정함)
	
	//
	// 반복자 수정이 불가능한 경우엔 컨테이너를 바꿔서 처리한다..
	// 삭제과정이 있기 때문에, i3::vector를 대신 쓰고 다시 스왑한다..
	//

	namespace range
	{
		namespace detail
		{
			template<class FwdRange, class Val, bool has_mutable_iterator> 
			struct remove_impl;

			template<class FwdRange, class Val> 
			struct remove_impl<FwdRange, Val, true>
			{
				static void call(FwdRange& rng, const Val& v)
				{
					typedef typename i3::range_iterator<FwdRange>::type		iterator;
					iterator new_end = i3::remove(begin(rng), end(rng), v);
					i3::generic_erase_iter_end(rng, new_end);
				}
			};

			template<class FwdRange, class Val> 
			struct remove_impl<FwdRange, Val, false>
			{
				static void call(FwdRange& rng, const Val& v)
				{
					typedef typename i3::range_value<FwdRange>::type value_type;
					typedef i3::vector<value_type>			vector_type;

					vector_type tmp_vec( begin(rng), end(rng) );
					remove_impl<vector_type, Val, true>::call(tmp_vec, v);
					
					FwdRange tmp( tmp_vec.begin(), tmp_vec.end() );
					i3::copy_or_swap(rng, tmp);
				}
			};

		}

		template<class FwdRange, class Val> inline 
		typename i3::disable_if_iterator<FwdRange>::type
			remove( FwdRange& rng, const Val& v)
		{
			detail::remove_impl<
				FwdRange, Val, i3::has_mutable_iterator<FwdRange>::value
				>::call(rng, v);
		}
		

		template<class Lit, class Val> inline 
		typename i3::enable_if_iterator<Lit>::type
			remove( Lit lit, const Val& v)
		{
			i3::iterator_range<Lit> rng = i3::as_literal(lit);
			Lit new_end = i3::remove(begin(rng), end(rng), v);
			i3::generic_erase_iter_end(lit, new_end);
		}
		
		
	}
}

namespace i3range = i3::range;