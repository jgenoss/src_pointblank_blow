
#include "../../iter_swap.h"

namespace i3
{
	template<class RanIt> inline
	void random_shuffle(RanIt f, RanIt l)
	{
		if (f == l) return;

		static const int RANDOM_BITS = 15;	// minimum random bits from rand()
		static const int RANDOM_MAX = (1U << RANDOM_BITS) - 1;
		typedef typename i3::iterator_traits<RanIt>::difference_type	difference_type;

		RanIt next = f;
		for (unsigned long Index = 2; ++next != l; ++Index)
		{	
			unsigned long Rm = RANDOM_MAX;
			unsigned long Rn = ::rand() & RANDOM_MAX;
			for (; Rm < Index && Rm != ~0UL; Rm = Rm << RANDOM_BITS | RANDOM_MAX)
			{
				Rn = Rn << RANDOM_BITS | (::rand() & RANDOM_MAX);	// build random value
			}
			i3::iter_swap(next, f + difference_type(Rn % Index));	// swap a pair
		}
	}
	

	template<class RanIt, class Fn1> inline
	void random_shuffle(RanIt f, RanIt l, Fn1& fn)
	{
		if (f == l) return;

		typedef typename i3::iterator_traits<RanIt>::difference_type	difference_type;

		RanIt next = f;

		for (difference_type Index = 2; ++next != l; ++Index)
			std::iter_swap(next, f + difference_type(fn(Index) % Index) );
	}



}