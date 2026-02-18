#pragma once

#include "../arithmetic/pp_inc.h"
#include "../repetition/pp_repeat.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_repeat_binary_params_1_impl.h"

#define pp_repeat_binary_params_1(count, p1, p2)	pp_repeat_binary_params_1_impl(count, p1, p2)
