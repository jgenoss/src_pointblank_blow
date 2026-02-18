#pragma once

// nreplace 대체함수가 아니라서 nreplace_range라는 이름이 붙음..
// 
// replace를 수행하기 전에, max_size에 맞춰 구간값을 조정한다...
//
// 원본구간을 대체 구간으로 변경한다..  
// (replace알고리즘 함수의 경우는 대체값으로만 변경 가능하다.)
#include "detail/enable_if_iterator.h"
#include "iterator.h"
#include "generic_erase_iter_end.h"
#include "generic_replace_range.h"
#include "generic_size.h"

namespace i3
{

	namespace detail
	{
		template<class LitOrSeq, class FwdIt> 
		void generic_nreplace_range_impl(	LitOrSeq ctn, std::size_t max_elem_size, typename range_iterator<LitOrSeq>::type from, 
											typename range_iterator<LitOrSeq>::type to, FwdIt f, FwdIt l)
		{
			std::size_t orig_ctn_length = i3::generic_size( ctn);
			
			typedef typename range_iterator<LitOrSeq>::type		output_iterator;

			output_iterator ctn_begin = i3::literal_begin(ctn);

			if ( orig_ctn_length > max_elem_size )		// 실제 길이보다 max_size가 작다면...줄이고 리턴..추가삽입은 불가능..
			{
				output_iterator max_elem_it = ctn_begin;
				i3::advance(max_elem_it, max_elem_size);
				i3::generic_erase_iter_end(ctn, max_elem_it);
				return;
			}
			
			typedef typename i3::iterator_difference<output_iterator>::type out_diff_type;
		
			if ( i3::distance(ctn_begin, from) >= out_diff_type(max_elem_size) )		// 넣을수 없음...리턴..
				return;
						
			out_diff_type dist_for_to = i3::distance(ctn_begin, to);

			if ( dist_for_to > out_diff_type(max_elem_size) )
			{
				to = ctn_begin;		i3::advance(to, max_elem_size);		// to가 넘치면 자름..
			}
			
			typedef typename i3::iterator_difference<FwdIt>::type in_diff_type;
			
			out_diff_type dist_from_to = i3::distance(from, to);
			in_diff_type  dist_f_l     = i3::distance(f, l);

			if ( orig_ctn_length + dist_f_l - dist_from_to > max_elem_size ) // 결과물이 넘칠것같다면.
			{
				dist_f_l = max_elem_size + dist_from_to - orig_ctn_length ;
				l = f;		i3::advance(l, dist_f_l);
			}

			i3::generic_replace_range(ctn, from, to, f, l);
		}
	}
	
	template<class Lit, class FwdIt> inline typename i3::enable_if_iterator<Lit>::type
	generic_nreplace_range( Lit lit, std::size_t max_elem_size, typename range_iterator<Lit>::type from, typename range_iterator<Lit>::type to,
							FwdIt f, FwdIt l)
	{
		detail::generic_nreplace_range_impl<Lit, FwdIt>( lit, max_elem_size, from, to, f, l);
	}
	

	template<class Seq, class FwdIt> inline typename i3::disable_if_iterator<Seq>::type
	generic_nreplace_range( Seq& s, std::size_t max_elem_size, typename range_iterator<Seq>::type from, typename range_iterator<Seq>::type to,
		FwdIt f, FwdIt l)
	{
		detail::generic_nreplace_range_impl<Seq&, FwdIt>(s, max_elem_size, from, to, f, l);
	}


}
