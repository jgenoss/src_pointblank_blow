#pragma once

#include "../../utility/use_default.h"
#include "../iterator_facade.h"

namespace i3
{
	template<class DerivedIt, class Base, class Val, class Cat, class Ref, class Diff>
	struct make_iterator_facade_with_default
	{
		typedef typename select_type_with_default<Val, iterator_value<Base> >::type		target_value;
		typedef typename select_type_with_default<Cat, iterator_category<Base> >::type	target_category;
		typedef typename select_type_with_default<Ref, 
			mpl::eval_if< is_same<Val, use_default>, iterator_reference<Base>, add_reference<Val> > >::type	target_reference;
		typedef typename select_type_with_default<Diff, iterator_difference<Base> >::type		target_difference;

		typedef iterator_facade<DerivedIt, target_value, target_category, target_reference, target_difference > type;
	};
}
