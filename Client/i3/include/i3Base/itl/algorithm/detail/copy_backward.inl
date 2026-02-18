
#include "../../xutility.h"

namespace i3
{
	namespace detail
	{
		template<class BidIt1, class BidIt2> inline
		BidIt2 copy_backward_opt(BidIt1 f, BidIt1 l, BidIt2 dest, nonscalar_ptr_iterator_tag)
		{	
			while (f != l)
				*--dest = *--l;
			return dest;
		}

		template<class BidIt1, class BidIt2> inline
		BidIt2 copy_backward_opt(BidIt1 f, BidIt1 l, BidIt2 dest, scalar_ptr_iterator_tag)
		{	
			ptrdiff_t Off = l - f;	
			BidIt2 Result = dest - Off;
			if (Off > 0)
				::memmove_s(&*Result, Off * sizeof (*f), &*f, Off * sizeof (*f) );
			return Result;
		}
	}

	template <class BidIt1, class BidIt2> inline
	BidIt2 copy_backward( BidIt1 f, BidIt1 l, BidIt2 dest)
	{
		//	typedef typename iterator_traits<BidIt1>::value_type		value_type1;
		//	typedef typename iterator_traits<BidIt2>::value_type	value_type2;
		//	enum { value =  ice_and<has_trivial_assign<value_type1>::value, has_trivial_assign<value_type2>::value>::value };
		return detail::copy_backward_opt(f, l, dest, typename i3::select_ptr_iterator_tag<BidIt1, BidIt2>::type() );
			//pointer_category(f, dest));
	}
	
	template<class BidIt1, class BidIt2> inline
	BidIt2 copy_backward_1(BidIt1 f, BidIt2 dest)
	{
		*--dest = *f;
		return dest;
	}

}