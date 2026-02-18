#pragma once

#include "../../algorithm/replace_if.h"
#include "../../type_traits/has_mutable_iterator.h"
#include "../../utility/copy_or_swap.h"
#include "../detail/enable_if_iterator.h"

namespace i3
{
	namespace range
	{

		namespace detail
		{
			template<class FwdRange, class Pred1, class Val, bool mutable_iterator>
			struct replace_if_impl;

			template<class FwdRange, class Pred1, class Val>
			struct replace_if_impl<FwdRange, Pred1, Val, true>
			{
				static void call( FwdRange& rng, Pred1 pr, const Val& new_v)
				{
					i3::replace_if(begin(rng), end(rng), pr, new_v );
				}
			};

			template<class FwdRange, class Pred1, class Val>
			struct replace_if_impl<FwdRange, Pred1, Val, false>
			{
				static void call( FwdRange& rng, Pred1 pr, const Val& new_v)
				{
					typedef typename i3::range_value<FwdRange>::type value_type;
					typedef i3::vector<value_type>			vector_type;

					vector_type tmp_vec( begin(rng), end(rng) );
					replace_if_impl<vector_type, Val, true>::call(tmp_vec, pr, new_v);

					FwdRange tmp( tmp_vec.begin(), tmp_vec.end() );
					i3::copy_or_swap(rng, tmp);
				}
			};
		}


		template<class FwdRange, class Pred1, class Val> inline typename i3::disable_if_iterator<FwdRange,
			FwdRange&>::type replace_if( FwdRange& rng, Pred1 pr, const Val& new_v)
		{
			detail::replace_if_impl<FwdRange, Val, i3::has_mutable_iterator<FwdRange>::value>::call(rng, pr, new_v );
			return rng;
		}		

		template<class Lit, class Pred1, class Val> inline typename i3::enable_if_iterator<Lit,
			Lit>::type replace_if( Lit lit, Pred1 pr, const Val& new_v)
		{
			i3::iterator_range<Lit> rng = i3::as_literal(lit);
			i3::replace_if( begin(rng), end(rng), pr, new_v );
			return lit;
		}		



	}
}

namespace i3range = i3::range;