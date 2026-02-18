#pragma once

#include "detail/operator_proxy.h"
#include "detail/iterator_core_access.h"
#include "../type_traits/add_pointer.h"

namespace i3
{

	// CRTP + Val + Cat 매개변수..
	template<class DerivedIt, class Val, class Cat, class Ref = Val&, class Diff = std::ptrdiff_t>
	class iterator_facade
	{
		DerivedIt& derived() { return *static_cast<DerivedIt*>(this); }
		const DerivedIt& derived() const { return *static_cast<const DerivedIt*>(this); }
	public:
		typedef Cat										iterator_category;
		typedef typename remove_const<Val>::type		value_type;
		typedef typename mpl::eval_if< is_const<Val>, add_pointer<const value_type>, add_pointer<value_type> >::type associated_pointer;
		typedef detail::operator_arrow_dispatch<Ref, associated_pointer> operator_arrow_dispatch_type;
		typedef typename operator_arrow_dispatch_type::result_type		pointer;
		typedef Ref														reference;
		typedef	Diff													difference_type;
		typedef iterator_facade											this_type;
		
		reference operator*() const { return iterator_core_access::dereference(derived()); }		// 대부분 사용자정의 템플릿정적다형성 처리.(CRTP)
		pointer   operator->() const { return operator_arrow_dispatch_type::apply(*derived()); }
		typename detail::operator_brackets_result<DerivedIt, Val>::type operator[](difference_type d) const
		{
			typedef detail::use_operator_brackets_proxy<Val, Ref> use_proxy;
			return detail::make_operator_brackets_result<DerivedIt>(derived() + d, use_proxy());
		}
		DerivedIt& operator++() { iterator_core_access::increment(derived()); return derived(); }
		DerivedIt& operator--() { iterator_core_access::decrement(derived()); return derived(); }
		DerivedIt  operator--(int) { DerivedIt tmp(derived()); --*this; return tmp; }
		DerivedIt& operator+=(difference_type d) { iterator_core_access::advance(derived(), d); return derived(); }
		DerivedIt& operator-=(difference_type d) { iterator_core_access::advance(derived(), -d); return derived(); }
		DerivedIt  operator-(difference_type d) const { DerivedIt res(derived()); return res -= d; }
	};
	
	// ++ 후증가 함수가 일반함수로 빠지고 필요하면 프록시클래스 정적 분기 처리..
	template<class It2, class Val2, class Cat2, class Ref2, class Diff2> inline
	typename detail::postfix_increment_result<It2, Val2, Ref2, Cat2>::type operator++( iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& i, int)
	{
		typename detail::postfix_increment_result<It2, Val2, Ref2, Cat2>::type tmp( *static_cast<It2*>(&i));
		++i; 
		return tmp;
	}
		
	template<	class It1, class Val1, class Cat1, class Ref1, class Diff1,
				class It2, class Val2, class Cat2, class Ref2, class Diff2 > inline typename detail::enable_if_interoperable<It1, It2,
	bool>::type operator==(	const iterator_facade<It1, Val1, Cat1, Ref1, Diff1>& lhs, 
							const iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& rhs )
	{
		return iterator_core_access::equal( *static_cast<const It1*>(&lhs),   *static_cast<const It2*>(&rhs), is_convertible<It2, It1>() );
	}
	
	template<	class It1, class Val1, class Cat1, class Ref1, class Diff1,
				class It2, class Val2, class Cat2, class Ref2, class Diff2 > inline typename detail::enable_if_interoperable<It1, It2,
	bool>::type operator!=(	const iterator_facade<It1, Val1, Cat1, Ref1, Diff1>& lhs, 
							const iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& rhs )
	{
		return !iterator_core_access::equal( *static_cast<const It1*>(&lhs),   *static_cast<const It2*>(&rhs), is_convertible<It2, It1>() );
	}	
	
	template<	class It1, class Val1, class Cat1, class Ref1, class Diff1,
				class It2, class Val2, class Cat2, class Ref2, class Diff2 > inline typename detail::enable_if_interoperable<It1, It2,
	bool>::type operator<(	const iterator_facade<It1, Val1, Cat1, Ref1, Diff1>& lhs, 
							const iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& rhs )
	{
		return 0 > iterator_core_access::distance_from( *static_cast<const It1*>(&lhs),   *static_cast<const It2*>(&rhs), is_convertible<It2, It1>() );
	}
	
	template<	class It1, class Val1, class Cat1, class Ref1, class Diff1,
				class It2, class Val2, class Cat2, class Ref2, class Diff2 > inline typename detail::enable_if_interoperable<It1, It2,
	bool>::type operator>(	const iterator_facade<It1, Val1, Cat1, Ref1, Diff1>& lhs, 
							const iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& rhs )
	{
		return 0 < iterator_core_access::distance_from( *static_cast<const It1*>(&lhs),   *static_cast<const It2*>(&rhs), is_convertible<It2, It1>() );
	}	

	template<	class It1, class Val1, class Cat1, class Ref1, class Diff1,
				class It2, class Val2, class Cat2, class Ref2, class Diff2 > inline typename detail::enable_if_interoperable<It1, It2,
	bool>::type operator<=(	const iterator_facade<It1, Val1, Cat1, Ref1, Diff1>& lhs, 
							const iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& rhs )
	{
		return 0 >= iterator_core_access::distance_from( *static_cast<const It1*>(&lhs),   *static_cast<const It2*>(&rhs), is_convertible<It2, It1>() );
	}	
	
	template<	class It1, class Val1, class Cat1, class Ref1, class Diff1,
				class It2, class Val2, class Cat2, class Ref2, class Diff2 > inline typename detail::enable_if_interoperable<It1, It2,
	bool>::type operator>=(	const iterator_facade<It1, Val1, Cat1, Ref1, Diff1>& lhs, 
							const iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& rhs )
	{
		return 0 <= iterator_core_access::distance_from( *static_cast<const It1*>(&lhs),   *static_cast<const It2*>(&rhs), is_convertible<It2, It1>() );
	}	
	
	template<	class It1, class Val1, class Cat1, class Ref1, class Diff1,
				class It2, class Val2, class Cat2, class Ref2, class Diff2 > inline 
	typename detail::choose_difference_type<It1, It2>::type operator-(	
					const iterator_facade<It1, Val1, Cat1, Ref1, Diff1>& lhs, const iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& rhs )
	{
		return iterator_core_access::distance_from( *static_cast<const It1*>(&lhs),   *static_cast<const It2*>(&rhs), is_convertible<It2, It1>() );
	}

	template<	class It, class Val, class Cat, class Ref, class Diff> inline
	It			operator+( const iterator_facade<It, Val, Cat, Ref, Diff>& i, typename It::difference_type d)
	{
		It tmp(static_cast<const It&>(i));
		return tmp += d;
	}

	template<	class It, class Val, class Cat, class Ref, class Diff> inline
	It			operator+(typename It::difference_type d, const iterator_facade<It, Val, Cat, Ref, Diff>& i)
	{
		It tmp(static_cast<const It&>(i));
		return tmp += d;
	}	

}
