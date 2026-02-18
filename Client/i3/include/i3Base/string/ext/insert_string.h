#pragma once

#include "../../itl/undef_minmax_begin.h"
#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/value_type.h"
#include "../../itl/range/iterator_range.h"

#include "../stack_string.h"
#include "generic_string_size.h"
#include "is_i3_string.h"
#include "is_stack_string.h"

namespace i3
{

// 컨테이너와 구문자열이 모두 인수로 잘들어가도록 처리해야한다..	
// 변환까지 지원될 필요가 없기 때문에, char <-> wchar_t간 인수가 되면
// 컴파일 오류가 나도록 조정..


	// i3::iterator_traits문제를 해결하기 전에는 std::string을 쓰기가 좀 곤란하다..
/*
	template<class T>
	struct is_std_basic_string_container : i3::is_template_same< 
		std::basic_string<mpl_, mpl_, mpl_>, T> {};
*/	

	template<class T>
	struct is_insert_friendly_string_container : 
				mpl::or_<	is_i3_basic_string<T>,
				is_stack_basic_string<T> >::type {};
			//	, is_std_basic_string_container<T> >::type {};
	
	namespace detail
	{
		struct insert_string_friendly_tag {};
		struct insert_string_unfriendly_tag{};

		template<class DestRange, class SrcRange> inline
		DestRange& insert_string(DestRange& dst, size_t dst_pos, const SrcRange& src, 
								insert_string_friendly_tag )
		{
			return dst.insert( dst_pos, &(*src.begin()), src.size() );
		}

		template<class DestRange, class SrcRange> inline
		DestRange& insert_string(DestRange& dst, size_t dst_pos, const SrcRange& src, 
				 insert_string_unfriendly_tag )
		{
			// 임시객체로 처리 후 대입 처리..
		
			typedef typename i3::range_value<DestRange>::type		dest_value_type;
			typedef i3::stack_basic_string< mpl::size_t<128>, dest_value_type > sstring_type;

			sstring_type tmp_ctn = sstring_type( i3::begin(dst), i3::end(dst) );
			tmp_ctn.insert( dst_pos, &(*src.begin()), src.size() );

			return dst = tmp_ctn;
		}

	}

	template<class DestRange, class SrcRange> inline
	typename i3::non_iterator_return<DestRange>::type insert_string(
		DestRange& dst, size_t dst_pos, const SrcRange& src,  size_t src_len /*= std::numeric_limits<size_t>::max()*/ )
	{
		typedef typename i3::range_value<SrcRange>::type		src_value_type;
		typedef typename i3::range_value<DestRange>::type		dest_value_type;
		compile_assert_pred( (i3::is_same<src_value_type, dest_value_type>) );

		// 문자열 조작과정이기 때문에, DestRange를 직접 쓰기보다는 문자열 조작과정이 용이한 컨테이너를
		// 선택한다.  ( i3::basic_string / i3::stack_basic_string 중 하나 )

		typedef typename mpl::if_< 
			is_insert_friendly_string_container<DestRange>, 
			detail::insert_string_friendly_tag,
			detail::insert_string_unfriendly_tag>::type		dispatch_tag;
		
		if ( src_len == size_t(std::numeric_limits<size_t>::max()) )
		{
			src_len = i3::generic_string_size(src);
		}

		typedef typename i3::range_const_iterator<SrcRange>::type		src_iterator;
		typedef i3::iterator_range<src_iterator>		src_iter_range;

		return detail::insert_string(dst, dst_pos, src_iter_range( &src[0], &src[0] + src_len),  dispatch_tag());
	}

	// generic_string_length 함수가 필요하다..

	// 구 문자열은 아래 함수가 돌도록..
	template<class DestIter, class SrcRange> inline
	typename i3::iterator_return<DestIter>::type insert_string(
		DestIter dst, size_t dst_pos, const SrcRange& src, size_t src_len /*= std::numeric_limits<size_t>::max()*/ )
	{
		typedef typename i3::range_value<SrcRange>::type		src_value_type;
		typedef typename i3::iterator_value<DestIter>::type		dest_value_type;

		if ( src_len == size_t(std::numeric_limits<size_t>::max()) )
		{
			src_len = i3::generic_string_size(src);
		}
		
		typedef typename i3::range_const_iterator<SrcRange>::type		src_iterator;
		typedef i3::iterator_range<src_iterator>		src_iter_range;

		src_iter_range s_r = src_iter_range( &src[0], &src[0] + src_len);

		// 이부분은 문제가 없다..(컨테이너가 아니므로 문자열 포인터로 간주할수 있다.)
		std::size_t dst_orig_len = i3::char_traits<dest_value_type>::length( &(*dst) );
		
		// 문자열의 경우 관례적으로 널값을 마지막에 넣어준다.
		*(dst + dst_orig_len+src_len) = dest_value_type();
		//
		// 소스영역과 복사영역이 겹쳐지는 부분이 있기 때문에 거꾸로 접근해야만..한다....
		i3::copy_backward( dst + dst_pos, dst + dst_orig_len, dst + dst_orig_len + src_len);
		//
		i3::copy( s_r.begin(), s_r.end(), dst + dst_pos);
		//
		
		return dst;
	}

	
	
}

#include "../../itl/undef_minmax_end.h"