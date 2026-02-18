#pragma once

//
// dest의 원소 갯수 제약을 준 함수가 필요해서 몇개 추가해야한다..
// max_size는 버퍼사이즈가 아니라..원소최대 갯수임을 유의할 것....
//
#include "detail/enable_if_iterator.h"
#include "iterator.h"
#include "generic_size.h"
#include "generic_erase_iter_end.h"
#include "literal_begin.h"
#include "generic_insert_iterators.h"
#include "generic_size.h"

namespace i3
{

	namespace detail
	{
		template<class LitOrSeq, class InIt> 
		void generic_ninsert_iterators_impl(LitOrSeq ctn, std::size_t max_elem_size, typename range_iterator<LitOrSeq>::type pos, InIt first, InIt last)	
		{
			std::size_t orig_ctn_length = i3::generic_size( ctn);
			
			typedef typename range_iterator<LitOrSeq>::type		output_iterator;
		
			if ( orig_ctn_length > max_elem_size )		// 실제 길이보다 max_size가 작다면...줄이고 리턴..추가삽입은 불가능..
			{
				output_iterator max_elem_it = i3::literal_begin(ctn);
				i3::advance(max_elem_it, max_elem_size);
				i3::generic_erase_iter_end(ctn, max_elem_it);
				return;
			}

			typedef typename i3::iterator_difference<InIt>::type diff_type;
		
			if ( i3::distance(i3::literal_begin(ctn), pos) >= diff_type(max_elem_size) )		// 넣을수 없음...리턴..
				return;
			
			diff_type dist_for_insert = i3::distance(first, last);

			if ( orig_ctn_length + dist_for_insert > max_elem_size )
			{
				dist_for_insert = max_elem_size - orig_ctn_length;
				last = first;	i3::advance(last, dist_for_insert);			// last값을 max_elem_size에 맞춘다..
			}
			
			i3::generic_insert_iterators(ctn, pos, first, last);
		}
	}

	template<class Literal, class InIt> inline typename i3::enable_if_iterator<Literal>::type 
	generic_ninsert_iterators(Literal ctn, std::size_t max_elem_size, typename range_iterator<Literal>::type pos, InIt first, InIt last)
	{
		detail::generic_ninsert_iterators_impl<Literal, InIt>(ctn, max_elem_size, pos, first, last);
	}

	template<class Literal, class Range> inline typename i3::enable_if_iterator<Literal>::type 
	generic_ninsert_range(Literal ctn, std::size_t max_elem_size, typename range_iterator<Literal>::type pos, const Range& rng)
	{
		typedef typename get_iterator_range_type<const Range>::type iter_range_type;
		iter_range_type it_rng = i3::as_literal(rng);
		generic_ninsert_iterators(ctn, max_elem_size, pos, it_rng.begin(), it_rng.end());
	}


	template<class Seq, class InIt> inline typename i3::disable_if_iterator<Seq>::type 
	generic_ninsert_iterators(Seq& s, std::size_t max_elem_size, typename range_iterator<Seq>::type pos, InIt first, InIt last)
	{
		detail::generic_ninsert_iterators_impl<Seq&, InIt>(s, max_elem_size, pos, first, last);
	}

	template<class Seq, class Range> inline typename i3::disable_if_iterator<Seq>::type 
	generic_ninsert_range(Seq& s, std::size_t max_elem_size, typename range_iterator<Seq>::type pos, const Range& rng)
	{
		typedef typename get_iterator_range_type<const Range>::type iter_range_type;
		iter_range_type it_rng = i3::as_literal(rng);
		generic_ninsert_iterators(s, max_elem_size, pos, it_rng.begin(), it_rng.end());
	}
	
	
	
}