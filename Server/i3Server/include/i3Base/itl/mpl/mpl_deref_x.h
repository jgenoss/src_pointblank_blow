#pragma once

#include "mpl_deref.h"
#include "../type_traits/has_value.h"
#include "../type_traits/has_value_type.h"

namespace i3
{
	namespace mpl
	{
		namespace aux 
		{
			template<class Iterator>
			struct deref_has_value 
			{
				typedef typename Iterator::type			type;
				typedef typename if_< has_value_type<type>, typename type::value_type, long>::type		value_type;		// value_type이 없다면 long으로 대체한다..
				static const value_type	value =	type::value;
			};
			
			template<class Iterator>
			struct deref_has_not_value
			{
				typedef typename Iterator::type			type;
			};
		}

		template<class Iterator = na> struct deref_x : eval_if< has_integral_value<typename Iterator::type>, 
															aux::deref_has_value<Iterator>,
															aux::deref_has_not_value<Iterator> > {};
	}
}

#define na_spec (1, deref_x)
#include "aux_/mpl_na_spec.h"
