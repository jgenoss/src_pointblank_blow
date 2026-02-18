#pragma once

#include "../arithmetic/pp_inc.h"
#include "../punctuation/pp_comma_if.h"
#include "../repetition/pp_repeat.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_enum_params_1_with_a_cat_impl.h"

// 첫 파라미터에 두번째문자를 붙여서 처리하는 특별한 케이스..
#define pp_enum_params_1_with_a_cat(count, p1, p2)		pp_enum_params_1_with_a_cat_impl(count, p1, p2)
