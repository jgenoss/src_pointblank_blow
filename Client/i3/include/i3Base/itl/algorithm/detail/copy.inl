
#include "../../xutility.h"

namespace i3
{
	namespace detail
	{
		template<class InIt, class OutIt>	inline
		OutIt  copy_opt(InIt f, InIt l, OutIt dest, nonscalar_ptr_iterator_tag )
		{	
			for (; f != l; ++dest, ++f)
				*dest = *f;
			return dest;
		}

		template<class InIt, class OutIt>	inline
		OutIt copy_opt(InIt f, InIt l, OutIt dest, scalar_ptr_iterator_tag)
		{	
			ptrdiff_t Off = l - f;	// NB: non-overlapping move
			OutIt Result = dest + Off;
			if (Off > 0)
				::memmove_s(&*dest, Off * sizeof (*f), &*f, Off * sizeof (*f));
			return Result;
		}
	}


// trivial traits를 사용하는 것으로 변경...
	template <class InIt, class OutIt> inline
	OutIt copy(InIt f, InIt l, OutIt dest)
	{
		//	typedef typename iterator_traits<InIt>::value_type		value_type1;
		//	typedef typename iterator_traits<OutIt>::value_type	value_type2;
		//	enum { value =  ice_and<has_trivial_assign<value_type1>::value, has_trivial_assign<value_type2>::value>::value };
		return detail::copy_opt(f, l, dest, typename i3::select_ptr_iterator_tag<InIt, OutIt>::type() );
			//pointer_category(f, dest));
	}

	template <class InIt, class OutIt> inline
	OutIt copy_1(InIt f, OutIt dest)
	{
		*dest = *f;
		return ++dest;
	}

}