#pragma once


#include "is_i3_string.h"
#include "is_stack_string.h"
#include "../../itl/range/detail/enable_if_iterator.h"

namespace i3
{
	
	template<class T>
	struct is_erase_friendly_string_container : 
		mpl::or_< i3::is_i3_basic_string<T>, i3::is_stack_basic_string<T> > {};

	namespace detail
	{
		struct erase_string_friendly_tag {};
		struct erase_string_unfriendly_tag{};

		template<class DestRange> inline
			DestRange& erase_string(DestRange& dst, size_t pos, size_t num, erase_string_friendly_tag )
		{
			return dst.erase( pos, num);
		}

		template<class DestRange> inline
			DestRange& erase_string(DestRange& dst,size_t pos, size_t num, erase_string_unfriendly_tag) 
		{
			// 임시객체로 처리 후 대입 처리..
			typedef typename i3::range_value<DestRange>::type		dest_value_type;
			typedef i3::stack_basic_string< mpl::size_t<128>, dest_value_type > sstring_type;
			sstring_type tmp_ctn = sstring_type( i3::begin(dst), i3::end(dst) );
			tmp_ctn.erase( pos, num );
			return dst = tmp_ctn;
		}

	}

	template<class DestRange> inline
	typename i3::non_iterator_return<DestRange>::type erase_string(
		DestRange& dst, size_t pos, size_t num /*= 1*/ )
	{
		typedef typename mpl::if_< 
			is_erase_friendly_string_container<DestRange>, 
			detail::erase_string_friendly_tag,
			detail::erase_string_unfriendly_tag>::type		dispatch_tag;

		return detail::erase_string(dst, pos, num, dispatch_tag());
	}

	template<class DestIter> inline
	typename i3::iterator_return<DestIter>::type erase_string(
		DestIter dst, size_t pos, size_t num /*= 1*/ )
	{
		typedef typename i3::iterator_value<DestIter>::type		dest_value_type;
		std::size_t dst_orig_len = i3::char_traits<dest_value_type>::length( &(*dst) );

		DestIter next_it = i3::copy( dst + pos + num, dst + dst_orig_len, dst + pos);
		*next_it = dest_value_type();

		return dst;
	}

	
}
