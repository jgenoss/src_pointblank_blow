#pragma once

#include "../arithmetic/pp_inc.h"
#include "../repetition/pp_repeat.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_repeat_binary_params_1_with_a_cat_impl.h"

#define pp_repeat_binary_params_1_with_a_cat(count, p1, p2, cat)	pp_repeat_binary_params_1_with_a_cat_impl(count, p1, p2, cat)
