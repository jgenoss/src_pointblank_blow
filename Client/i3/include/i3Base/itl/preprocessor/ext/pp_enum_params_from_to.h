#pragma once

#include "../punctuation/pp_comma_if.h"
#include "../comparison/pp_not_equal.h"
#include "../repetition/pp_repeat_from_to.h"

#include "detail/pp_enum_params_from_to_impl.h"

#define pp_enum_params_from_to(first, last, param)	pp_enum_params_from_to_impl(first, last, param)
