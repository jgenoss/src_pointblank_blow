#pragma once

#include "pp_repeat.h"		// pp_catÆ÷ÇÔ..
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_enum_trailing_binary_params_impl.h"

#define pp_enum_trailing_binary_params(count, p1, p2)		pp_enum_trailing_binary_params_impl(count, p1, p2)
#define pp_enum_trailing_binary_params_z(z, count, p1, p2)	pp_enum_trailing_binary_params_z_impl(z, count, p1, p2)
