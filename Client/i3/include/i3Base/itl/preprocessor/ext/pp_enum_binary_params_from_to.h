#pragma once

#include "../punctuation/pp_comma_if.h"
#include "../comparison/pp_not_equal.h"
#include "../repetition/pp_repeat_from_to.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_enum_binary_params_from_to_impl.h"

#define pp_enum_binary_params_from_to(first, last, p1, p2)	pp_enum_binary_params_from_to_impl(first, last, p1, p2)
