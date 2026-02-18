#pragma once

// 호환성을 위해 작성하지만 쓰이는 곳은 없는것 같다..
#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/algorithm/find.h"
#include "../../itl/range/generic_erase_begin_iter.h"

namespace i3
{
		
	namespace detail
	{
		template<class LIT_OR_SEQ>
		void remove_drivename_impl( LIT_OR_SEQ path)
		{
			typedef typename get_iterator_range_type<LIT_OR_SEQ>::type range_type;

			range_type	lit_rng = i3::as_literal(path);

			typedef typename range_type::iterator	iterator;

			iterator iter = i3::range::find(lit_rng, ':');
			
			if ( iter == lit_rng.end()) return;

			++iter;
			i3::generic_erase_begin_iter( path, iter);
		}
	}

	template< class Lit> inline typename i3::enable_if_iterator<Lit>::type
	remove_drivename(Lit inout)
	{
		detail::remove_drivename_impl<Lit>(inout);
	}

	template< class Seq> inline typename i3::disable_if_iterator<Seq>::type
	remove_drivename(Seq& inout)
	{
		detail::remove_drivename_impl<Seq&>(inout);
	}


}