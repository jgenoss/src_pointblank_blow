#pragma once

#include "../utility/result_of.h"
#include "../type_traits/is_convertible.h"
#include "../enable_if.h"
#include "binary_iterator_adaptor.h"


namespace i3
{
	
	// UnaryFn은 transform알고리즘에 쓰이던 인수 함수객체임..
	template<class BinaryFn, class It1, class It2, class Ref = use_default, class Val = use_default>
	class binary_transform_iterator;

	namespace detail
	{
		template<class BinaryFn, class It1, class It2, class Ref, class Val>
		struct make_binary_iterator_adaptor_for_binary_transform_iterator
		{
			typedef typename select_type_with_default<Ref, 
				i3::result_of< const BinaryFn( typename i3::iterator_traits<It1>::reference, 
				typename i3::iterator_traits<It2>::reference) > >::type target_reference;
			typedef typename select_type_with_default<Val, remove_reference<target_reference> >::type target_value_type;
			typedef binary_iterator_adaptor< binary_transform_iterator<BinaryFn, It1, It2, Ref, Val>, 
				It1, It2, target_value_type, use_default, target_reference> type;
		};
	}

	template<class BinaryFn, class It1, class It2, class Ref , class Val>
	class binary_transform_iterator : 
		public detail::make_binary_iterator_adaptor_for_binary_transform_iterator<BinaryFn, It1, It2, Ref, Val>::type
	{
		friend class iterator_core_access;		// 
		typedef typename detail::make_binary_iterator_adaptor_for_binary_transform_iterator<
			BinaryFn, It1, It2, Ref, Val>::type	super_t;
	public:
		binary_transform_iterator() {}
		binary_transform_iterator(const It1& i1, const It2 i2, BinaryFn f) : super_t(i1, i2), m_f(f) {}
		binary_transform_iterator(const It1& i1, const It2 i2) : super_t(i1, i2) { compile_assert_msg(is_class<BinaryFn>::value, this_cons_must_have_functor); }

		// 템플릿 복사 생성자..
		template<class B, class I1, class I2, class Rf, class V>
		binary_transform_iterator( const binary_transform_iterator<B, I1, I2, Rf, V>& rhs, 
			typename enable_if< is_convertible<I1, It1>, int>::type = 0,
			typename enable_if< is_convertible<I2, It2>, int>::type = 0,
			typename enable_if< is_convertible<B, BinaryFn>, int>::type = 0) : super_t(rhs.base1(), rhs.base2()), 
				m_f(rhs.functor()) {}
		BinaryFn functor() const { return m_f; }

	private:
		typename super_t::reference dereference() const { return m_f(*this->base1(), *this->base2()); }		// 이 구현이 핵심..노멀어댑터에서 이것이 덮어씌어짐..
		BinaryFn m_f;
	};

	template<class BinaryFn, class It1, class It2> inline
	binary_transform_iterator<BinaryFn, It1, It2> make_binary_transform_iterator(It1 i1, It2 i2, BinaryFn f)
	{
		return binary_transform_iterator<BinaryFn, It1, It2>(i1, i2, f);
	}

	template<class BinaryFn, class It1, class It2> inline typename enable_if<is_class<BinaryFn>,		// 이 경우, 함수객체의 템플릿 매개변수 요구
	binary_transform_iterator<BinaryFn, It1, It2> >::type	make_binary_transform_iterator(It1 i1, It2 i2)
	{
		return binary_transform_iterator<BinaryFn, It1, It2>(i1, i2);		
	}

	
}