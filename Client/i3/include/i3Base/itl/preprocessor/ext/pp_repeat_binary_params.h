#pragma once

#include "../repetition/pp_repeat.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_repeat_binary_params_impl.h"

#define pp_repeat_binary_params(count, p1, p2)		pp_repeat_binary_params_impl(count, p1, p2)
