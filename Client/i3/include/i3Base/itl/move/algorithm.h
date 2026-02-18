#pragma once

#include "../iterator/iterator_traits.h"

namespace i3
{
	template<class InIt, class OutIt> inline
	OutIt move(InIt f, InIt l, OutIt res)
	{
		for ( ; f != l ; ++f, ++res ) 
			*res = i3::move(*f);
		return res;
	}
	
	template<class BidIt, class OutBidIt> inline
	OutBidIt move_backward(BidIt f, BidIt l, OutBidIt res)
	{
		while (f != l)
		{
			--l;	--res;
			*res = i3::move(*l);
		}
		return res;
	}

#pragma push_macro("new")

	template<class InIt, class OutIt>
	OutIt uninitialized_move(InIt f, InIt l, OutIt res)
	{
		typedef typename i3::iterator_value<InIt>::type	input_value_type;

		while( f != l )
		{
			void* addr = static_cast<void*>( i3::addressof(*res));
			::new (addr) input_value_type(i3::move(*f));
			++f;	++res;
		}

		return res;
	}

#pragma pop_macro("new")
	
	template<class InIt, class OutIt> inline typename i3::enable_if< is_move_iterator<InIt>,
	OutIt>::type uninitialized_copy_or_move( InIt f, InIt l, OutIt res)
	{
		return i3::uninitialized_move(f, l, res);
	}

	template<class InIt, class OutIt> inline typename i3::disable_if< is_move_iterator<InIt>,
	OutIt>::type uninitialized_copy_or_move( InIt f, InIt l, OutIt res)
	{
		return i3::uninitialized_copy(f, l, res);
	}

	template<class InIt, class OutIt> inline typename i3::enable_if< is_move_iterator<InIt>,
	OutIt>::type copy_or_move( InIt f, InIt l, OutIt res)
	{
		return i3::move(f, l, res);
	}

	template<class InIt, class OutIt> inline typename i3::disable_if< is_move_iterator<InIt>,
	OutIt>::type copy_or_move( InIt f, InIt l, OutIt res)
	{
		return i3::copy(f, l, res);
	}

}