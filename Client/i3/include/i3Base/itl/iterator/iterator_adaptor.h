#pragma once

#include "detail/make_iterator_facade_with_default.h"

namespace i3
{


	template<	class DerivedIt, class Base, 
				class Val = use_default, class Cat = use_default, class Ref = use_default, class Diff = use_default>
	class iterator_adaptor;


	template<class DerivedIt, class Base, class Val, class Cat, class Ref, class Diff>
	class iterator_adaptor : public make_iterator_facade_with_default<DerivedIt, Base, Val, Cat, Ref, Diff>::type
	{
		friend class iterator_core_access;
	protected:
		typedef typename make_iterator_facade_with_default<DerivedIt, Base, Val, Cat, Ref, Diff>::type	facade_type;
	public:
		typedef Base			base_type;			// Base가 base클래스가 아님..멤버임에 유의할 것..
		const Base&				base() const { return m_iter; }
	
		iterator_adaptor() {}
		explicit iterator_adaptor(const Base& i) : m_iter(i) {}

	protected:
		typedef iterator_adaptor			this_type;
		const Base&	base_reference() const { return m_iter; }					
		Base&		base_reference() { return m_iter; }

	private:
		typename facade_type::reference	dereference() const { return *m_iter; }		// CRTP 다형성 구현.
		template<class DerivedIt2, class Base2, class Val2, class Cat2, class Ref2, class Diff2>
		bool		equal( const iterator_adaptor<DerivedIt2,Base2,Val2,Cat2,Ref2,Diff2>& rhs) const {	return m_iter == rhs.base(); }
		
		void		advance(typename facade_type::difference_type d) 
		{ 
			compile_assert_msg( (is_convertible<typename facade_type::iterator_category, i3::random_access_iterator_tag>::value), iterator_category_not_compatible);
			m_iter += d;	
		}		// random_access_iterator만 가능.

		void		increment() { ++m_iter; }

		void decrement() // bid_iterator만 가능..
		{ 
			compile_assert_msg( (is_convertible<typename facade_type::iterator_category, i3::bidirectional_iterator_tag>::value), iterator_category_not_compatible);
			--m_iter; 
		}
		
		template<class DerivedIt2, class Base2, class Val2, class Cat2, class Ref2, class Diff2>
		typename facade_type::difference_type distance_to(const iterator_adaptor<DerivedIt2,Base2,Val2,Cat2,Ref2,Diff2>& rhs) const
		{
			compile_assert_msg( (is_convertible<typename facade_type::iterator_category, i3::random_access_iterator_tag>::value), iterator_category_not_compatible);
			return rhs.base() - m_iter;
		}

		Base	m_iter;
	};
}
