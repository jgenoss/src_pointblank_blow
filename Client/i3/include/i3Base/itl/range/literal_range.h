#pragma once

#include "../utility/safe_bool.h"

#include "value_type.h"
//#include "../type_traits/is_same.h"

#include "literal_begin.h"
#include "literal_end.h"
#include "difference_type.h"
#include "detail/enable_if_iterator.h"

namespace i3
{
	namespace detail_literal_range
	{
		template<class T, class Ch>
		struct has_same_value : i3::is_same<typename i3::range_value<T>::type, Ch> {};

		template<class T, class Ch>
		struct enable_if_unary_cons : i3::enable_if<mpl::and_< mpl::not_< i3::is_integral<T> >, 
			has_same_value<T, Ch> > > {};
	}

	template<class It>
	class literal_base_range
	{
	public:
		typedef literal_base_range<It>					this_type;
		typedef safe_bool<this_type>					safe_bool_t;
		typedef typename iterator_value<It>::type		value_type;
		typedef typename iterator_difference<It>::type	difference_type;
		typedef std::size_t								size_type;
		typedef typename iterator_reference<It>::type	reference;
		typedef It										iterator;
		typedef It										const_iterator;
		
		// 생성자
		literal_base_range() : m_begin(It()), m_end(It()) {}
		literal_base_range(It begin, It end) : m_begin(begin), m_end(end) {}

		template<class Range> 
		literal_base_range(const Range& r, typename detail_literal_range::enable_if_unary_cons<Range, value_type>::type* = 0) 
			: m_begin(static_cast<It>(i3::literal_begin(r) ) ), m_end( static_cast<It>(i3::literal_end(r))) {}
		template<class Range>
		literal_base_range(Range& r, typename detail_literal_range::enable_if_unary_cons<Range, value_type>::type* = 0) 
			: m_begin(static_cast<It>(i3::literal_begin(r) ) ), m_end( static_cast<It>(i3::literal_end(r))) {}

		// 배정연산자
		this_type& operator=(const this_type& rhs) { m_begin = rhs.m_begin; m_end = rhs.m_end; return *this; }
		template<class It2>
		this_type& operator=(const literal_base_range<It2>& rhs) { m_begin = rhs.begin(); m_end = rhs.end();  return *this; }
		template<class FwdRange>
		this_type& operator=(FwdRange& r) { m_begin = static_cast<It>(i3::literal_begin(r));	m_end = static_cast<It>(i3::literal_end(r)); return *this; }
		template<class FwdRange>
		this_type& operator=(const FwdRange& r) { m_begin = static_cast<It>(i3::literal_begin(r));	m_end = static_cast<It>(i3::literal_end(r)); return *this; }


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

		const value_type*	c_str() const { return &*m_begin; }
		
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
	
	typedef i3::literal_base_range<char*>			mutable_literal_range;
	typedef i3::literal_base_range<const char*>		literal_range;
	typedef i3::literal_base_range<wchar_t*>		mutable_wliteral_range;
	typedef i3::literal_base_range<const wchar_t*>	wliteral_range;
	
}