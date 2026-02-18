#pragma once

#include "../arithmetic/pp_add.h"	// pp_dec, pp_inc, pp_while, pp_cat, pp_auto_rec, pp_tuple_elem Æ÷ÇÔ
#include "../punctuation/pp_comma_if.h"
#include "../repetition/pp_repeat.h"

#include "detail/pp_enum_shifted_params_1_impl.h"

#define pp_enum_shifted_params_1(count, param)	pp_enum_shifted_params_1_impl(count, param)
