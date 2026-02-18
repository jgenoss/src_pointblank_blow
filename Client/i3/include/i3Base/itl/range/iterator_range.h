#pragma once

#include "../utility/safe_bool.h"
#include "begin.h"
#include "end.h"
#include "difference_type.h"
#include "reverse_iterator.h"
#include "rbegin.h"
#include "rend.h"
#include "detail/enable_if_iterator.h"

namespace i3
{
	template<class It>
	class iterator_range
	{
	public:
		typedef iterator_range<It>						this_type;
		typedef safe_bool<this_type>					safe_bool_t;
		typedef typename iterator_value<It>::type		value_type;
		typedef typename iterator_difference<It>::type	difference_type;
		typedef std::size_t								size_type;
		typedef typename iterator_reference<It>::type	reference;
		typedef It										iterator;
		typedef It										const_iterator;
		// 생성자
		iterator_range() {}
		iterator_range(It begin, It end) : m_begin(begin), m_end(end) {}
		
		template<class Range>
		iterator_range(const Range& r) : m_begin(static_cast<It>(i3::begin(r) ) ), m_end( static_cast<It>(i3::end(r))) {}
		template<class Range>
		iterator_range(Range& r) : m_begin(static_cast<It>(i3::begin(r) ) ), m_end( static_cast<It>(i3::end(r))) {}

		// 배정연산자
		this_type& operator=(const this_type& rhs) { m_begin = rhs.m_begin; m_end = rhs.m_end; return *this; }
		template<class It2>
		this_type& operator=(const iterator_range<It2>& rhs) { m_begin = rhs.begin(); m_end = rhs.end();  return *this; }
		template<class FwdRange>
		this_type& operator=(FwdRange& r) { m_begin = static_cast<It>(i3::begin(r));	m_end = static_cast<It>(i3::end(r)); return *this; }
		template<class FwdRange>
		this_type& operator=(const FwdRange& r) { m_begin = static_cast<It>(i3::begin(r));	m_end = static_cast<It>(i3::end(r)); return *this; }
		
		this_type& assign( iterator f, iterator l) { m_begin = f;  m_end = l; return *this; }
		
		template<class It2> typename i3::enable_if_iterator<It2,
		this_type&>::type assign(It2 f, It2 l) {  m_begin = static_cast<It>(f); m_end = static_cast<It>(l); return *this; }

		// 접근자
		It				begin()	const { return m_begin; }
		It				end()	const { return m_end; }

		// 컨테이너멤버함수
		difference_type	size()  const { return m_end - m_begin; }	// 이건 모든 It에 적용되지 않음..(random_access iter수준에서 가능)
		bool			empty() const { return m_begin == m_end; }	// 이건 모든 It에 적용 가능..

		operator typename safe_bool_t::type() const { return safe_bool_t::call( m_begin != m_end ); }
		bool			operator!() const { return empty(); }
		
		reference		front() const	{ return *m_begin; }
		reference		back() const	{ It end_m_1(m_end);	return *(--end_m_1); }
		void			pop_front()		{ ++m_begin; }		// 실제로 제거되는게 아니라 범위만 조정된다.
		void			pop_back()		{ --m_end; }		// 실제로 제거되는게 아니라 범위만 조정된다.
		
		reference		operator[](difference_type idx) const { return m_begin[idx]; }	// random_access_iter만 가능할것이다.

		void			clear() { m_begin = m_end; }

	private:	typedef typename mpl::if_<is_abstract<value_type>, reference, value_type>::type		return_abstract_value_type; 
	public:		return_abstract_value_type operator()(difference_type idx) const { return m_begin[idx]; }	
		
		this_type&		advance_begin(difference_type d) { i3::advance(m_begin, d); return *this; }
		this_type&		advance_end(difference_type d) { i3::advance(m_end, d); return *this; }
		void			set_begin(It beg) { m_begin = beg; }
		void			set_end(It end) { m_end = end; }

	private:
		It	m_begin, m_end;
	};

	typedef iterator_range<char*>			char_range;
	typedef iterator_range<const char*>		const_char_range;
	typedef iterator_range<wchar_t*>		wchar_range;
	typedef iterator_range<const wchar_t*>	const_wchar_range;
}

#include "iterator_range_operator.h"

namespace i3
{
	// 인수 추정 때문에 make함수가 선호된다..
	template<class It> inline 
	iterator_range<It> make_iterator_range(It f, It l) {	return iterator_range<It>(f, l); }
	
	template<class FwdRange> inline 
	iterator_range<typename range_iterator<FwdRange>::type> make_iterator_range(FwdRange& r) 
	{	
		return iterator_range<typename range_iterator<FwdRange>::type>(r); 
	}

	template<class FwdRange> inline 
	iterator_range<typename range_iterator<const FwdRange>::type> make_iterator_range(const FwdRange& r) 
	{	
		return iterator_range<typename range_iterator<const FwdRange>::type>(r); 
	}

	namespace range_detail
	{
		template< class Range > inline 
		iterator_range< typename range_iterator<Range>::type >
		make_range_impl( Range& r,	typename range_difference<Range>::type dist_begin, 
									typename range_difference<Range>::type dist_end )
		{
			typename range_iterator<Range>::type b = i3::begin(r);
			typename range_iterator<Range>::type e = i3::end(r);
			i3::advance(b, dist_begin);
			i3::advance(e, dist_end);
			return make_iterator_range(b, e);
		}
	}

	template<class FwdRange> inline 
	iterator_range<typename range_iterator<FwdRange>::type> make_iterator_range(FwdRange& r, 
		typename range_difference<FwdRange>::type dist_begin, typename range_difference<FwdRange>::type dist_end) 		
	{
		return range_detail::make_range_impl(r, dist_begin, dist_end);
	}

	template<class FwdRange> inline 
	iterator_range<typename range_iterator<const FwdRange>::type> make_iterator_range(const FwdRange& r, 
		typename range_difference<FwdRange>::type dist_begin, typename range_difference<FwdRange>::type dist_end) 	 	
	{
		return range_detail::make_range_impl(r, dist_begin, dist_end);
	}

// 이하부터 다음 함수/메타함수 는 확장형..
	template<class FwdRange>
	struct get_iterator_range_type
	{
		typedef i3::iterator_range< typename range_iterator<FwdRange>::type > type;
	};
	
	template<class FwdRange>
	struct get_const_iterator_range_type
	{
		typedef i3::iterator_range< typename range_const_iterator<FwdRange>::type > type;
	};

	template<class FwdRange>
	struct get_reverse_iterator_range_type
	{
		typedef i3::iterator_range<typename range_reverse_iterator<FwdRange>::type> type;
	};

	
	template<class FwdRange> inline
	iterator_range<typename range_reverse_iterator<FwdRange>::type> make_reverse_iterator_range(FwdRange& r)
	{
		return iterator_range<typename range_reverse_iterator<FwdRange>::type>(i3::rbegin(r), i3::rend(r)); 
	}
	
	template<class FwdRange> inline
	iterator_range<typename range_reverse_iterator<const FwdRange>::type> make_reverse_iterator_range(const FwdRange& r)
	{
		return iterator_range<typename range_reverse_iterator<const FwdRange>::type>(i3::rbegin(r), i3::rend(r)); 
	}
}
