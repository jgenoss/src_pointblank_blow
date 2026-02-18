#pragma once

#include "../type_traits/has_iterators_insert_mem_fun.h"
#include "detail/enable_if_iterator.h"
#include "iterator.h"
#include "value_type.h"
#include "../utility/copy_or_swap.h"


// 반복자 3개짜리 insert를 일반화시킴..
// 대다수 멤버함수 insert 3반복자 버전은 리턴값이 없음..
//
namespace i3
{
	
	
	template<class Literal, class InIt> inline typename i3::enable_if_iterator<Literal>::type 
	generic_insert_iterators(Literal ctn, typename range_iterator<Literal>::type pos, InIt first, InIt last)
	{
		typedef Literal										output_iterator;
		typedef typename i3::iterator_value<Literal>::type	value_type;
		// begin~pos까지는 변경 없음..
		// pos+insert_len 뒷부분으로 카피가 필요함...겹치는 영역이 있을수 있어 copy_backward()를 써야한다..
		
		output_iterator orig_end_it = ctn;
		i3::advance(orig_end_it, i3::char_traits<value_type>::length( &(*ctn) ) );
		
		output_iterator new_end_it = orig_end_it;
		i3::advance(new_end_it, i3::distance(first, last) );

		*new_end_it = value_type();			// 널값을 미리 넣음..
		i3::copy_backward( pos, orig_end_it, new_end_it);
		i3::copy( first, last, pos );
	}

	template<class Literal, class Range> inline typename i3::enable_if_iterator<Literal>::type 
	generic_insert_range(Literal ctn, typename range_iterator<Literal>::type pos, const Range& rng)
	{
		typedef typename get_iterator_range_type<const Range>::type iter_range_type;
		iter_range_type it_rng = i3::as_literal(rng);
		generic_insert_iterators(ctn, pos, it_rng.begin(), it_rng.end());
	}

	//
	// insert(iter, iter, iter)가 필요함..없다면, copy_swap으로 대신 처리한다..
	//

	namespace detail
	{
		template<class Seq, class InIt, bool has_insert>
		struct generic_insert_iterators_impl;

		template<class Seq, class InIt>
		struct generic_insert_iterators_impl<Seq, InIt, true>
		{
			static inline void call(Seq& s, typename range_iterator<Seq>::type pos, InIt first, InIt last)
			{
				return s.insert(pos, first, last);			// 여기로 오면 제일 좋다..
			}
		};

		template<class Seq, class InIt>
		struct generic_insert_iterators_impl<Seq, InIt, false>
		{
			static inline void call(Seq& s, typename range_iterator<Seq>::type pos, InIt first, InIt last)
			{
				// 컨테이너 자체가 삽입 불가능한 것일 가능성이 높다.  다른 종류의 컨테이너를 동원한다.
				typedef typename i3::range_value<Seq>::type			value_type;
						
				i3::vector<value_type> tmp_vec(i3::begin(s), pos );	// allocator도 필요할수 있지만, 우선 이렇게만 처리..
				tmp_vec.insert(tmp_vec.end(), first, last);
				tmp_vec.insert(tmp_vec.end(), pos, i3::end(s) ); 

				Seq tmp( tmp_vec.begin(), tmp_vec.end() );
				i3::copy_or_swap(s, tmp);
			}
		};

	}


	template<class Seq, class InIt> inline typename i3::disable_if_iterator<Seq>::type 
	generic_insert_iterators(Seq& s, typename range_iterator<Seq>::type pos, InIt first, InIt last)
	{
		detail::generic_insert_iterators_impl<Seq, InIt, 
			i3::has_iterators_insert_mem_fun<Seq>::value>::call(s, pos, first, last);
	}

	template<class Seq, class Range> inline typename i3::disable_if_iterator<Seq>::type 
	generic_insert_range(Seq& s, typename range_iterator<Seq>::type pos, const Range& rng)
	{
		typedef typename get_iterator_range_type<const Range>::type iter_range_type;
		iter_range_type it_rng = i3::as_literal(rng);
		generic_insert_iterators(s, pos, it_rng.begin(), it_rng.end());
	}
	
	
	
}