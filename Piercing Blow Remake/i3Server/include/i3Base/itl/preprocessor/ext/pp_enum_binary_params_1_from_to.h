#pragma once

#include "../arithmetic/pp_inc.h"
#include "../punctuation/pp_comma_if.h"
#include "../comparison/pp_not_equal.h"
#include "../repetition/pp_repeat_from_to.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_enum_binary_params_1_from_to_impl.h"

// 아래는 first + 1부터 시작해서 last로 종료..
#define pp_enum_binary_params_1_from_to(first, last, p1, p2)	pp_enum_binary_params_1_from_to_impl(first, last, p1, p2)
