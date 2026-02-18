#pragma once

#include "../utility/result_of.h"
#include "../type_traits/is_convertible.h"
#include "../enable_if.h"
#include "iterator_adaptor.h"

namespace i3
{
	// UnaryFn은 transform알고리즘에 쓰이던 인수 함수객체임..
	template<class UnaryFn, class It, class Ref = use_default, class Val = use_default>
	class transform_iterator;
	
	namespace detail
	{
		template<class UnaryFn, class It, class Ref, class Val>
		struct make_iterator_adaptor_for_transform_iterator
		{
			typedef typename select_type_with_default<Ref, 
				i3::result_of< const UnaryFn( typename i3::iterator_traits<It>::reference ) > >::type target_reference;
			typedef typename select_type_with_default<Val, remove_reference<target_reference> >::type target_value_type;
			typedef iterator_adaptor< transform_iterator<UnaryFn, It, Ref, Val>, It, target_value_type, use_default, target_reference> type;
		};
	}

	template<class UnaryFn, class It, class Ref , class Val>
	class transform_iterator : public detail::make_iterator_adaptor_for_transform_iterator<UnaryFn, It, Ref, Val>::type
	{
		friend class iterator_core_access;		// 
		typedef typename detail::make_iterator_adaptor_for_transform_iterator<UnaryFn, It, Ref, Val>::type	super_t;
	public:
		transform_iterator() {}
		transform_iterator(const It& i, UnaryFn f) : super_t(i), m_f(f) {}
		explicit transform_iterator(const It& i) : super_t(i) { compile_assert_msg(is_class<UnaryFn>::value, this_cons_must_have_functor); }
		
		// 템플릿 복사 생성자..
		template<class UnaryFn2, class It2, class Ref2, class Val2>
		transform_iterator( const transform_iterator<UnaryFn2, It2, Ref2, Val2>& rhs, typename enable_if< is_convertible<It2, It>, int>::type = 0,
			typename enable_if< is_convertible<UnaryFn2, UnaryFn>, int>::type = 0) : super_t(rhs.base()), m_f(rhs.functor()) {}
		UnaryFn functor() const { return m_f; }

	private:
		typename super_t::reference dereference() const { return m_f(*this->base()); }		// 이 구현이 핵심..노멀어댑터에서 이것이 덮어씌어짐..
		UnaryFn m_f;
	};
	
	template<class UnaryFn, class It> inline
	transform_iterator<UnaryFn, It> make_transform_iterator(It i, UnaryFn f)
	{
		return transform_iterator<UnaryFn, It>(i, f);
	}

	template<class UnaryFn, class It> inline typename enable_if<is_class<UnaryFn>,		// 이 경우, 함수객체의 템플릿 매개변수 요구
	transform_iterator<UnaryFn, It> >::type	make_transform_iterator(It i)
	{
		return transform_iterator<UnaryFn, It>(i);		
	}
	
}
