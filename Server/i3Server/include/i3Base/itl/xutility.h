#pragma once

#include "iterator.h"
#include "type_traits/is_base_of.h"
#include "type_traits/has_trivial_assign.h"

#include "undef_minmax_begin.h"

namespace i3
{
	
	// POINTER ITERATOR TAGS
	struct nonscalar_ptr_iterator_tag { };	// pointer to unknown type
	struct scalar_ptr_iterator_tag { };		// pointer to scalar type
	
	namespace detail
	{
		// T1이 Input / T2가 Output쪽임..
		template<class Input, class Output, bool is_derived_of_iterator1, bool is_derived_of_iterator2>
		struct pointer_category_helper_iterator_check;
	
		template<class Input,	class Output>
		struct pointer_category_helper	
		{		
			enum{    
					is_derived_of_iterator1 = is_base_of<iterator_base, Input>::value,
					is_derived_of_iterator2 = is_base_of<iterator_base, Output>::value
				};
			
			typedef typename pointer_category_helper_iterator_check
				<Input, Output, is_derived_of_iterator1, is_derived_of_iterator2>::pointer_category 
				pointer_category;
		};
		
		template<class Input, class Output, bool is_derived_of_iterator1, bool is_derived_of_iterator2>
		struct pointer_category_helper_iterator_check
		{
			typedef typename Input::inner_type		inner_type_input;
			typedef typename Output::inner_type		inner_type_output;
			typedef typename pointer_category_helper<inner_type_input, inner_type_output>::pointer_category    pointer_category;	
		};

		template<class Input, class Output, bool is_derived_of_iterator1>
		struct pointer_category_helper_iterator_check<Input, Output, is_derived_of_iterator1, false>
		{
			typedef typename Input::inner_type		inner_type_input;
			typedef typename pointer_category_helper< inner_type_input, Output>::pointer_category pointer_category;
		};

		template<class Input, class Output, bool is_derived_of_iterator2>
		struct pointer_category_helper_iterator_check<Input, Output, false, is_derived_of_iterator2>
		{
			typedef typename Output::inner_type		inner_type_output;
			typedef typename pointer_category_helper< Input, inner_type_output>::pointer_category pointer_category;
		};
		
		template<class Input, class Output>
		struct pointer_category_helper_iterator_check<Input, Output, false, false>
		{
			typedef nonscalar_ptr_iterator_tag	pointer_category;
		};

//		template<class T1, class T2>
//		struct pointer_category_helper<T1*, T2*>
//		{
//			typedef nonscalar_ptr_iterator_tag pointer_category;
//		};

		template<class T1>	struct pointer_category_helper<T1, undefined_inner_type> {	typedef nonscalar_ptr_iterator_tag pointer_category; };
		template<class T2>	struct pointer_category_helper<undefined_inner_type, T2> {	typedef nonscalar_ptr_iterator_tag pointer_category; };
		template<>			struct pointer_category_helper<undefined_inner_type, undefined_inner_type> {	typedef nonscalar_ptr_iterator_tag pointer_category; };

		template<>	struct pointer_category_helper<bool *, bool *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const bool *, bool *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<char *, char *>	{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const char *, char *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<signed char *, signed char *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const signed char *, signed char *> {	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<unsigned char *, unsigned char *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const unsigned char *, unsigned char *>{	typedef scalar_ptr_iterator_tag pointer_category;	};

#ifdef _NATIVE_WCHAR_T_DEFINED
		template<>	struct pointer_category_helper<wchar_t *, wchar_t *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const wchar_t *, wchar_t *>	{	typedef scalar_ptr_iterator_tag pointer_category;	};
#endif

		template<>	struct pointer_category_helper<short *, short *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const short *, short *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<unsigned short *, unsigned short *>{	typedef scalar_ptr_iterator_tag pointer_category; };
		template<>	struct pointer_category_helper<const unsigned short *, unsigned short *>{	typedef scalar_ptr_iterator_tag pointer_category; };
		template<>	struct pointer_category_helper<int *, int *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const int *, int *>{	typedef scalar_ptr_iterator_tag pointer_category;  };
		template<>	struct pointer_category_helper<unsigned int *, unsigned int *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const unsigned int *, unsigned int *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<long *, long *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const long *, long *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<unsigned long *, unsigned long *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const unsigned long *, unsigned long *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<float *, float *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const float *, float *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<double *, double *>	{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const double *, double *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<long double *, long double *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const long double *, long double *>	{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<long long *, long long *>	{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const long long *, long long *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<unsigned long long *, unsigned long long *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const unsigned long long *, unsigned long long *>{	typedef scalar_ptr_iterator_tag pointer_category; };
		template<class T>	struct pointer_category_helper<T **, T **>	{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<class T>	struct pointer_category_helper<T **, const T **>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<class T>	struct pointer_category_helper<T *const *, T **>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<class T>	struct pointer_category_helper<T *const *, const T **>{	typedef scalar_ptr_iterator_tag pointer_category; };
	


		template<class Cat1, class Cat2> struct Iter_random_helper {	typedef forward_iterator_tag Iter_random_cat;	};

		template<>	struct Iter_random_helper<random_access_iterator_tag, random_access_iterator_tag>
		{
			typedef random_access_iterator_tag Iter_random_cat;
		};
		
		template<class Iter1, class Iter2> inline	
		typename Iter_random_helper<	
									typename iterator_traits<Iter1>::iterator_category,
									typename iterator_traits<Iter2>::iterator_category
									>::Iter_random_cat
		Iter_random(const Iter1&, const Iter2&)
		{	// return category from iterator argument
			typename Iter_random_helper<
				iterator_traits<Iter1>::iterator_category, 
				iterator_traits<Iter2>::iterator_category>::Iter_random_cat Cat;
			return (Cat);
		}
		

		template<class ForwardIterator, class T> inline
		void  Fill(ForwardIterator First, ForwardIterator Last, const T& Val)
		{	
			for (; First != Last; ++First)
				*First = Val;
		}	
		inline void  Fill( char *First, char *Last, int Val){	::memset(First, Val, Last - First);	}
		inline void  Fill( signed char *First, signed char *Last, int Val)	{	::memset(First, Val, Last - First);	}
		inline void  Fill( unsigned char *First, unsigned char *Last, int Val)	{	::memset(First, Val, Last - First);	}

		template<class OutIt, class Diff,	class T> inline
		void  Fill_n(OutIt First, Diff Count, const T& Val)
		{	
			for (; 0 < Count; --Count, ++First)
				*First = Val;
		}
		inline void Fill_n( char * First, size_t Count, int Val)	{	::memset(First, Val, Count);  }
		inline void Fill_n( signed char * First, size_t Count, int Val)	{	::memset(First, Val, Count); }
		inline void Fill_n( unsigned char * First, size_t Count, int Val){	::memset(First, Val, Count); }
		

		template<class InputIterator, class OutputIterator>	inline
		OutputIterator  copy_opt(InputIterator First, InputIterator Last, OutputIterator Dest, const mpl::false_&) //nonscalar_ptr_iterator_tag )
		{	// copy [_First, _Last) to [_Dest, ...), arbitrary iterators
			for (; First != Last; ++Dest, ++First)
				*Dest = *First;
			return Dest;
		}

		template<class InputIterator, class OutputIterator>	inline
		OutputIterator copy_opt(InputIterator First, InputIterator Last, OutputIterator Dest, const mpl::true_&)	//scalar_ptr_iterator_tag)
		{	// copy [_First, _Last) to [_Dest, ...), pointers to scalars

			ptrdiff_t Off = Last - First;	// NB: non-overlapping move
			// if _OutIt is range checked, this will make sure there is enough space for the memmove
			OutputIterator Result = Dest + Off;
			if (Off > 0)
				::memmove_s(&*Dest, Off * sizeof (*First), &*First, Off * sizeof (*First));
			return Result;
		}

		template<class BidirectionalIterator1, class BidirectionalIterator2> inline
		BidirectionalIterator2 copy_backward_opt(BidirectionalIterator1 First, BidirectionalIterator1 Last, 
			BidirectionalIterator2 Dest, const mpl::false_& ) //nonscalar_ptr_iterator_tag)
		{	// copy [_First, _Last) backwards to [..., _Dest), arbitrary iterators
			while (First != Last)
				*--Dest = *--Last;
			return Dest;
		}

		template<class BidirectionalIterator1, class BidirectionalIterator2> inline
		BidirectionalIterator2 copy_backward_opt(BidirectionalIterator1 First, BidirectionalIterator1 Last, 
			BidirectionalIterator2 Dest, const mpl::true_& ) //scalar_ptr_iterator_tag)
		{	// copy [_First, _Last) backwards to [..., _Dest), pointers to scalars

			ptrdiff_t Off = Last - First;	// NB: non-overlapping move
			// if _OutIt is range checked, this will make sure there is enough space for the memmove
			
			BidirectionalIterator2 Result = Dest - Off;
			if (Off > 0)
				::memmove_s(&*Result, Off * sizeof (*First), &*First, Off * sizeof (*First) );
			return Result;
		}

		template<class InputIterator1, class InputIterator2, class Cat> inline
		bool Equal(InputIterator1 First1, InputIterator1 Last1, InputIterator2 First2, Cat )
		{	// compare [_First1, _Last1) to [First2, ...)
			for (; First1 != Last1; ++First1, ++First2)
				if (!(*First1 == *First2))
					return (false);
			return (true);
		}

		inline bool Equal(const char *First1,	const char *Last1, const char *First2, random_access_iterator_tag)
		{	// compare [_First1, _Last1) to [First2, ...), for chars
			return (::memcmp(First1, First2, Last1 - First1) == 0);
		}

		inline bool Equal(const signed char *First1, const signed char *Last1, const signed char *First2, random_access_iterator_tag)
		{	// compare [_First1, _Last1) to [First2, ...), for signed chars
			return (::memcmp(First1, First2, Last1 - First1) == 0);
		}

		inline bool Equal(const unsigned char *First1, const unsigned char *Last1, const unsigned char *First2,	random_access_iterator_tag )
		{	// compare [_First1, _Last1) to [First2, ...), for unsigned chars
			return (::memcmp(First1, First2, Last1 - First1) == 0);
		}



	}
	
	template<class T1, class T2> inline 
	typename detail::pointer_category_helper<T1, T2>::pointer_category pointer_category(T1&, T2&)
	{
		typename detail::pointer_category_helper<T1, T2>::pointer_category category;
		return (category);
	}
	
	template<class T> inline
	typename detail::pointer_category_helper<T, T>::pointer_category pointer_category(T&)
	{
		typename detail::pointer_category_helper<T, T>::pointer_category category;
		return (category);
	}
	

	template<class FowardIterator, class T> inline
		void fill(FowardIterator First, FowardIterator Last, const T& Val)
	{	
		detail::Fill(First, Last, Val);
	}	// copy _Val through [_First, _Last)

	template<class OutIterator, class Diff,	class T> inline
		void fill_n(OutIterator First, Diff Count, const T& Val)
	{	
		detail::Fill_n( First, Count, Val );
	}	// copy _Val _Count times through [_First, ...)

	template<class OutIt, class T> inline
	void	fill_1(OutIt First, const T& Val)
	{	
		*First = Val; 
	}
	
	// trivial traits를 사용하는 것으로 변경...
	template <class InputIterator, class OutputIterator> inline
	OutputIterator copy(InputIterator First, InputIterator Last, OutputIterator Dest)
	{	
		typedef typename iterator_traits<InputIterator>::value_type		value_type1;
		typedef typename iterator_traits<OutputIterator>::value_type	value_type2;
		enum { value =  ice_and<has_trivial_assign<value_type1>::value, has_trivial_assign<value_type2>::value>::value };
		return detail::copy_opt(First, Last, Dest,	mpl::bool_<value>());	//, pointer_category(First, Dest));
	} // copy [_First, _Last) to [_Dest, ...)

	template <class InputIterator, class OutputIterator> inline
	OutputIterator copy_1(InputIterator First, OutputIterator Dest)
	{	
		*Dest = *First;
		return ++Dest;
	} // copy [_First, _Last) to [_Dest, ...)

	template <class BidirectionalIterator1, class BidirectionalIterator2> inline
	BidirectionalIterator2 copy_backward( BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 dest)
	{
		typedef typename iterator_traits<BidirectionalIterator1>::value_type		value_type1;
		typedef typename iterator_traits<BidirectionalIterator2>::value_type	value_type2;
		enum { value =  ice_and<has_trivial_assign<value_type1>::value, has_trivial_assign<value_type2>::value>::value };
		return detail::copy_backward_opt(first, last, dest, mpl::bool_<value>()); // ,pointer_category(first, dest));
	}

	template<class InputIterator1, class InputIterator2> inline
	bool equal(InputIterator1 First1, InputIterator1 Last1, InputIterator2 First2)
	{	// compare [_First1, _Last1) to [First2, ...)
		return detail::Equal(First1, Last1, First2, Iter_random(First1, First2) );
	}

	template<class InputIterator1, class InputIterator2, class Pred> inline
	bool equal(InputIterator1 First1, InputIterator1 Last1, InputIterator2 First2, Pred pred)
	{	// compare [_First1, _Last1) to [First2, ...) using _Pred
		for (; First1 != Last1; ++First1, ++First2)
			if (!pred(*First1, *First2))
				return false;
		return true;
	}

	// TEMPLATE FUNCTION lexicographical_compare

	template<class InputIterator1, class InputIterator2> inline
	bool lexicographical_compare(InputIterator1 First1, InputIterator1 Last1,	InputIterator2 First2, InputIterator2 Last2)
	{	// order [_First1, _Last1) vs. [First2, Last2)
		for (; First1 != Last1 && First2 != Last2; ++First1, ++First2)
			if ( *First1 < *First2 )
				return (true);
			else if (*First2 < *First1)
				return (false);
		return (First1 == Last1 && First2 != Last2);
	}

	inline bool lexicographical_compare( const unsigned char *First1, const unsigned char *Last1,
		const unsigned char *First2, const unsigned char *Last2)
	{	// order [_First1, _Last1) vs. [First2, Last2), for unsigned char
		ptrdiff_t Num1 = Last1 - First1;
		ptrdiff_t Num2 = Last2 - First2;
		int Ans = ::memcmp(First1, First2, Num1 < Num2 ? Num1 : Num2);
		return (Ans < 0 || Ans == 0 && Num1 < Num2);
	}

	// TEMPLATE FUNCTION lexicographical_compare WITH PRED
	template<class InputIterator1, class InputIterator2, class Pred> inline
	bool lexicographical_compare(InputIterator1 First1, InputIterator1 Last1,	InputIterator2 First2, InputIterator2 Last2, Pred pred)
	{	// order [_First1, _Last1) vs. [First2, Last2) using _Pred
		for (; First1 != Last1 && First2 != Last2; ++First1, ++First2)
			if ( pred( *First1, *First2) )
				return (true);
			else if (pred(*First2, *First1) )
				return (false);
		return (First1 == Last1 && First2 != Last2);
	}

//#ifdef NOMINMAX
//#endif

	// TEMPLATE FUNCTION max
	template<class T> inline 
	const T&  max(const T& Left, const T& Right) {	return (Left < Right) ? Right : Left;	}

	// TEMPLATE FUNCTION max WITH PRED
	template<class T, class Pr> inline
	const T&  max(const T& Left, const T& Right, Pr pred) {	return pred(Left, Right) ? Right : Left; }

	// TEMPLATE FUNCTION min
	template<class T> inline
	const T&  min(const T& Left, const T& Right) {	return ( Right < Left) ? Right : Left;	}

	// TEMPLATE FUNCTION min WITH PRED
	template<class T, class Pr> inline
	const T&  min(const T& Left, const T& Right, Pr pred) {		return pred(Right, Left) ? Right : Left; }

	template<class T> inline
	const T& max( const T& arg1, const T& arg2, const T& arg3 ) { return i3::max( i3::max( arg1, arg2 ), arg3 );   }
	
	template<class T, class Pr> inline
	const T& max( const T& arg1, const T& arg2, const T& arg3, Pr pred) { return i3::max( i3::max(arg1, arg2, pred), arg3, pred); }

	template<class T> inline
	const T& min(const T& arg1, const T& arg2, const T& arg3 ) { return i3::min( i3::min( arg1, arg2), arg3 );		}

	template<class T, class Pr> inline
	const T& min(const T& arg1, const T& arg2, const T& arg3, Pr pred) { return i3::min( i3::min(arg1, arg2, pred), arg3, pred); }

	template<class T> inline
	const T& clamp(const T& value, const T& range_min, const T& range_max) { return (value < range_min) ? range_min : ( ( value > range_max) ? range_max : value );  }
	
	template<class T, class Pr> inline
	const T& clamp(const T& value, const T& range_min, const T& range_max, Pr pred) { return (pred(value, range_min)) ? range_min : (pred(range_max, value) ? range_max : value); }
	
}

#include "undef_minmax_end.h"
