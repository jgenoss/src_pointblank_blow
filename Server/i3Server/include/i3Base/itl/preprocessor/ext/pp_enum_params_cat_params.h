#pragma once

#include "../punctuation/pp_comma_if.h"
#include "../repetition/pp_repeat.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_enum_params_cat_params_impl.h"

// 첫 파라미터 와 두번째 파라미터 사이에 중간인자가 붙는케이스..
#define pp_enum_params_cat_params(count, p1, cat, p2)		pp_enum_params_cat_params_impl(count, p1, cat, p2)
