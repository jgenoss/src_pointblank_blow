#pragma once

namespace i3
{
	class iterator_core_access
	{
		template<class It, class Val, class Cat, class Ref, class Diff> friend class iterator_facade;

		template<	class It1, class Val1, class Cat1, class Ref1, class Diff1,
		class It2, class Val2, class Cat2, class Ref2, class Diff2 > friend typename detail::enable_if_interoperable<It1, It2,
			bool>::type operator==(	const iterator_facade<It1, Val1, Cat1, Ref1, Diff1>& lhs, 
			const iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& rhs );
		template<	class It1, class Val1, class Cat1, class Ref1, class Diff1,
		class It2, class Val2, class Cat2, class Ref2, class Diff2 > friend typename detail::enable_if_interoperable<It1, It2,
			bool>::type operator!=(	const iterator_facade<It1, Val1, Cat1, Ref1, Diff1>& lhs, 
			const iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& rhs );		
		template<	class It1, class Val1, class Cat1, class Ref1, class Diff1,
		class It2, class Val2, class Cat2, class Ref2, class Diff2 > friend typename detail::enable_if_interoperable<It1, It2,
			bool>::type operator<(	const iterator_facade<It1, Val1, Cat1, Ref1, Diff1>& lhs, 
			const iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& rhs );			
		template<	class It1, class Val1, class Cat1, class Ref1, class Diff1,
		class It2, class Val2, class Cat2, class Ref2, class Diff2 > friend typename detail::enable_if_interoperable<It1, It2,
			bool>::type operator>(	const iterator_facade<It1, Val1, Cat1, Ref1, Diff1>& lhs, 
			const iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& rhs );			
		template<	class It1, class Val1, class Cat1, class Ref1, class Diff1,
		class It2, class Val2, class Cat2, class Ref2, class Diff2 > friend typename detail::enable_if_interoperable<It1, It2,
			bool>::type operator<=(	const iterator_facade<It1, Val1, Cat1, Ref1, Diff1>& lhs, 
			const iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& rhs );			
		template<	class It1, class Val1, class Cat1, class Ref1, class Diff1,
		class It2, class Val2, class Cat2, class Ref2, class Diff2 > friend typename detail::enable_if_interoperable<It1, It2,
			bool>::type operator>=(	const iterator_facade<It1, Val1, Cat1, Ref1, Diff1>& lhs, 
			const iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& rhs );				
		template<	class It1, class Val1, class Cat1, class Ref1, class Diff1,
		class It2, class Val2, class Cat2, class Ref2, class Diff2 > friend 
			typename detail::choose_difference_type<It1, It2>::type operator-(	
			const iterator_facade<It1, Val1, Cat1, Ref1, Diff1>& lhs, const iterator_facade<It2, Val2, Cat2, Ref2, Diff2>& rhs );				

		template<	class It, class Val, class Cat, class Ref, class Diff> friend inline
			It			operator+( const iterator_facade<It, Val, Cat, Ref, Diff>& , typename It::difference_type );

		template<	class It, class Val, class Cat, class Ref, class Diff> friend inline
			It			operator+( typename It::difference_type, const iterator_facade<It, Val, Cat, Ref, Diff>&);

		template<class Facade> inline
			static typename Facade::reference dereference(const Facade& f) {  return f.dereference();  }
		template<class Facade> inline
			static void	increment(Facade& f) { f.increment(); }
		template<class Facade> inline
			static void	decrement(Facade& f) { f.decrement(); }
		template<class Facade1, class Facade2> inline
			static bool	equal(const Facade1& f1, const Facade2& f2, mpl::true_) { return f1.equal(f2); }
		template<class Facade1, class Facade2> inline
			static bool	equal(const Facade1& f1, const Facade2& f2, mpl::false_) { return f2.equal(f1); }
		template <class Facade> inline
			static void advance(Facade& f, typename Facade::difference_type n) {	f.advance(n);	}

		template <class Facade1, class Facade2> inline
			static typename Facade1::difference_type distance_from(const Facade1& f1, const Facade2& f2, mpl::true_) { return -f1.distance_to(f2); }
		template <class Facade1, class Facade2> inline
			static typename Facade2::difference_type distance_from(const Facade1& f1, const Facade2& f2, mpl::false_){ return f2.distance_to(f1); }

		template <class It, class Val, class Cat, class Ref, class Diff> inline
			static It& derived(iterator_facade<It,Val,Cat,Ref,Diff>& facade) {	return *static_cast<It*>(&facade); }

		template <class It, class Val, class Cat, class Ref, class Diff> inline
			static const It& derived(const iterator_facade<It,Val,Cat,Ref,Diff>& facade) { return *static_cast<const It*>(&facade); }

		iterator_core_access();
	};	
	
	
}