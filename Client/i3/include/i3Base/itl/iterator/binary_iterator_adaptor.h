#pragma once

//
//
// 알고리즘 함수 중에 반복자 2개를 동시에 운용해야될 경우가 좀 있어서 어댑터 원형을 따로 하나 추가해야됨..
// 반복자 둘 중 어느것이 디폴트인지 알수가 없지만, 우선 첫번째 것을 기준으로 한다.. 
//


#include "detail/make_iterator_facade_with_default.h"


namespace i3
{

	template<	class DerivedIt, class Base1, class Base2, 
		class Val = use_default, class Cat = use_default, class Ref = use_default, class Diff = use_default>
	class binary_iterator_adaptor;


	template<class DerivedIt, class Base1, class Base2, class Val, class Cat, class Ref, class Diff>
	class binary_iterator_adaptor : public make_iterator_facade_with_default<DerivedIt, Base1, Val, Cat, Ref, Diff>::type
	{
		friend class iterator_core_access;
	protected:
		typedef typename make_iterator_facade_with_default<DerivedIt, Base1, Val, Cat, Ref, Diff>::type	facade_type;
	public:
		typedef Base1			base_type;
		typedef Base1			base1_type;			// Base가 base클래스가 아님..멤버임에 유의할 것..
		typedef Base2			base2_type;

		const	Base1&			base() const { return m_iter1; }
		const	Base1&			base1() const { return m_iter1; }
		const	Base2&			base2() const { return m_iter2; }


		binary_iterator_adaptor() {}
		binary_iterator_adaptor(const Base1& i1, const Base2& i2) : m_iter1(i1), m_iter2(i2) {}

	protected:
		typedef binary_iterator_adaptor			this_type;
		const Base1&	base_reference() const	{ return m_iter1; }					
		Base1&			base_reference()		{ return m_iter1; }

		const Base1&	base1_reference() const	{ return m_iter1; }					
		Base1&			base1_reference()		{ return m_iter1; }
		const Base2&	base2_reference() const	{ return m_iter2; }					
		Base2&			base2_reference()		{ return m_iter2; }

	private:
		typename facade_type::reference	dereference() const { return *m_iter1; }		// CRTP 다형성 구현.
		template<class D, class B1, class B2, class V, class Ct, class Rf, class Df>
		bool		equal( const binary_iterator_adaptor<D,B1,B2,V,Ct,Rf,Df>& rhs) const 
		{	
			return m_iter1 == rhs.base1() && m_iter2 == rhs.base2(); 
		}

		void		advance(typename facade_type::difference_type d) 
		{ 
			compile_assert_msg( (is_convertible<typename facade_type::iterator_category, i3::random_access_iterator_tag>::value), iterator1_category_not_compatible);
			compile_assert_msg( (is_convertible<typename iterator_traits<base2_type>::iterator_category, i3::random_access_iterator_tag>::value), iterator2_category_not_compatible);
			m_iter1 += d;	
			m_iter2 += d;
		}		// random_access_iterator만 가능.

		void		increment() { ++m_iter1; ++m_iter2; }

		void		decrement() // bid_iterator만 가능..
		{ 
			compile_assert_msg( (is_convertible<typename facade_type::iterator_category, i3::bidirectional_iterator_tag>::value), iterator1_category_not_compatible);
			compile_assert_msg( (is_convertible<typename iterator_traits<base2_type>::iterator_category, i3::bidirectional_iterator_tag>::value), iterator2_category_not_compatible);

			--m_iter1; 
			--m_iter2;
		}

		template<class D, class B1, class B2, class V, class Ct, class Rf, class Df>
		typename facade_type::difference_type distance_to(const binary_iterator_adaptor<D,B1,B2,V,Ct,Rf,Df>& rhs) const
		{
			compile_assert_msg( (is_convertible<typename facade_type::iterator_category, i3::random_access_iterator_tag>::value), iterator1_category_not_compatible);
			compile_assert_msg( (is_convertible<typename iterator_traits<base2_type>::iterator_category, i3::random_access_iterator_tag>::value), iterator2_category_not_compatible);
			return rhs.base() - m_iter1;
		}

		Base1	m_iter1;
		Base2	m_iter2;
	};
}
