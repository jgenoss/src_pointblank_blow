#pragma once

#include "../../algorithm/replace.h"
#include "../../type_traits/has_mutable_iterator.h"
#include "../../utility/copy_or_swap.h"
#include "../detail/enable_if_iterator.h"

// iterator가 mutable이 아니면 실행이 안될수 있으므로 좀더 변경..

namespace i3
{
	namespace range
	{
		namespace detail
		{
			template<class FwdRange, class Val, bool mutable_iterator>
			struct replace_impl;

			template<class FwdRange, class Val>
			struct replace_impl<FwdRange, Val, true>
			{
				static void call( FwdRange& rng, const Val& old_v, const Val& new_v)
				{
					i3::replace(begin(rng), end(rng), old_v, new_v );
				}
			};

			template<class FwdRange, class Val>
			struct replace_impl<FwdRange, Val, false>
			{
				static void call( FwdRange& rng, const Val& old_v, const Val& new_v)
				{
					typedef typename i3::range_value<FwdRange>::type value_type;
					typedef i3::vector<value_type>			vector_type;

					vector_type tmp_vec( begin(rng), end(rng) );
					replace_impl<vector_type, Val, true>::call(tmp_vec, old_v, new_v);

					FwdRange tmp( tmp_vec.begin(), tmp_vec.end() );
					i3::copy_or_swap(rng, tmp);
				}
			};
		}


		template<class FwdRange, class Val> inline typename i3::disable_if_iterator<FwdRange,
		FwdRange&>::type replace( FwdRange& rng, const Val& old_v, const Val& new_v)
		{
			detail::replace_impl<FwdRange, Val, i3::has_mutable_iterator<FwdRange>::value>::call(rng, old_v, new_v );
			return rng;
		}		
		
		template<class Lit, class Val> inline typename i3::enable_if_iterator<Lit,
		Lit>::type replace( Lit lit, const Val& old_v, const Val& new_v)
		{
			i3::iterator_range<Lit> rng = i3::as_literal(lit);
			i3::replace( begin(rng), end(rng), old_v, new_v );
			return lit;
		}		

	}
}


namespace i3range = i3::range;
