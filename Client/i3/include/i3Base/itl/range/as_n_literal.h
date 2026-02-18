#pragma once

#include "as_literal.h"

namespace i3
{
	namespace range_detail
	{
		template<class T> inline
		iterator_range<T*> make_n_range(T* const r, size_t n, arg_true_type)							// 암시적 형변환에 의한 인수추론은 되지 않는다.
		{
			typedef typename i3::remove_cv<T>::type non_cvT;
			const size_t len = i3::char_traits<non_cvT>::length(r);
			const size_t n_len = (n < len) ? n : len;
			return iterator_range<T*>(r, r + n_len);
		}

		template<class T> inline
		iterator_range<typename range_iterator<T>::type > make_n_range(T& r, size_t n, arg_false_type)
		{
			typedef typename range_iterator<T>::type iterator;
			iterator it_beg = i3::begin(r);
			iterator it_end = i3::end(r);
			const size_t len = i3::distance(it_beg, it_end);

			if (n >= len)
				return iterator_range<iterator>(it_beg, it_end);
			
			it_end = it_beg;	i3::advance(it_end, n);
			return iterator_range<iterator>(it_beg, it_end);
		}

	}

	template<class Range> inline	typename range_detail::disable_if_c_str< Range,
	iterator_range<typename range_iterator<Range>::type> >::type as_n_literal(Range& r, size_t n)
	{
		typedef typename range_iterator<Range>::type iterator;
		iterator it_beg = i3::begin(r);
		iterator it_end = i3::end(r);
		const size_t len = i3::distance(it_beg, it_end);

		if (n >= len)
			return iterator_range<iterator>(it_beg, it_end);

		it_end = it_beg;	i3::advance(it_end, n);
		return iterator_range<iterator>(it_beg, it_end);
	}

	template<class Range> inline	typename range_detail::disable_if_c_str< const Range,
	iterator_range<typename range_iterator<const Range>::type> >::type as_n_literal(const Range& r, size_t n)
	{
		typedef typename range_iterator<const Range>::type iterator;
		iterator it_beg = i3::begin(r);
		iterator it_end = i3::end(r);
		const size_t len = i3::distance(it_beg, it_end);

		if (n >= len)
			return iterator_range<iterator>(it_beg, it_end);

		it_end = it_beg;	i3::advance(it_end, n);
		return iterator_range<iterator>(it_beg, it_end);
	}

	template<class Range> inline	typename range_detail::enable_if_c_str< Range,
	iterator_range<typename range_iterator<Range>::type> >::type as_n_literal(Range& r, size_t n)
	{
		return range_detail::make_n_range(r, n, arg_true);
	}

	template<class Range> inline	typename range_detail::enable_if_c_str< const Range,
	iterator_range<typename range_iterator<const Range>::type> >::type as_n_literal(const Range& r, size_t n)
	{
		return range_detail::make_n_range(r, arg_true);
	}

	template<class T> inline
	iterator_range<T*> as_n_literal(T* r, size_t n)
	{
		typedef typename i3::remove_cv<T>::type non_cvT;
		const size_t len = i3::char_traits<non_cvT>::length(r);
		const size_t n_len = (n < len) ? n : len;
		return iterator_range<T*>(r, r + n_len);
	}


	//// 이하부터 확장형
	
}