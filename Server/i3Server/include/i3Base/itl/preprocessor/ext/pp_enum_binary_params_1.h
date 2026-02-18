#pragma once

#include "../arithmetic/pp_inc.h"
#include "../punctuation/pp_comma_if.h"
#include "../repetition/pp_repeat.h"		// pp_catÆ÷ÇÔ
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_enum_binary_params_1_impl.h"

#define pp_enum_binary_params_1(count, p1, p2)	pp_enum_binary_params_1_impl(count, p1, p2)
