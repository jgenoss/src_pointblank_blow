#pragma once

#define variant_limit		40

#include "../preprocessor/ext/pp_enum_params_from_to_with_a_default.h"

namespace i3
{
	
	template<class T0, pp_enum_params_from_to_with_a_default(1, variant_limit, class T, mpl::na) >
	class variant;

	template<class T0, pp_enum_params_from_to_with_a_default(1, variant_limit, class T, mpl::na) >
	struct make_recursive_variant;

	struct recursive_variant_ {};
	
	template<class Seq> struct make_variant_over;
	template<class Seq> struct make_recursive_variant_over;

}
