#pragma once

#include "../../algorithm/remove_if.h"
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
	

	namespace range
	{

		namespace detail
		{
			template<class FwdRange, class Pred1, bool has_mutable_iterator> 
			struct remove_if_impl;

			template<class FwdRange, class Pred1> 
			struct remove_if_impl<FwdRange, Pred1, true>
			{
				static void call(FwdRange& rng, Pred1 pr)
				{
					typedef typename i3::range_iterator<FwdRange>::type		iterator;
					iterator new_end = i3::remove_if(begin(rng), end(rng), pr);
					i3::generic_erase_iter_end(rng, new_end);
				}
			};

			template<class FwdRange, class Pred1> 
			struct remove_if_impl<FwdRange, Pred1, false>
			{
				static void call(FwdRange& rng, Pred1 pr)
				{
					typedef typename i3::range_value<FwdRange>::type value_type;
					typedef i3::vector<value_type>			vector_type;

					vector_type tmp_vec( begin(rng), end(rng) );
					remove_if_impl<vector_type, Pred1, true>::call(tmp_vec, pr);

					FwdRange tmp( tmp_vec.begin(), tmp_vec.end() );
					i3::copy_or_swap(rng, tmp);
				}
			};

		}

		template<class FwdRange, class Pred1> inline 
		typename i3::disable_if_iterator<FwdRange>::type
			remove_if( FwdRange& rng, Pred1 pr)
		{
			detail::remove_if_impl<
				FwdRange, Pred1, i3::has_mutable_iterator<FwdRange>::value 
			>::call(rng, pr);
		}

		template<class Lit, class Pred1> inline 
		typename i3::enable_if_iterator<Lit>::type
			remove_if( Lit lit, Pred1 pr)
		{
			i3::iterator_range<Lit> rng = i3::as_literal(lit);
			Lit new_end = i3::remove_if(begin(rng), end(rng), pr);
			i3::generic_erase_iter_end(lit, new_end);
		}
	}
}

namespace i3range = i3::range;