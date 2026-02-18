#pragma once

#include "../arithmetic/pp_inc.h"
#include "../punctuation/pp_comma_if.h"
#include "../repetition/pp_repeat.h"

#include "detail/pp_enum_trailing_params_1_impl.h"

#define pp_enum_trailing_params_1(count, param)	pp_enum_trailing_params_1_impl(count, param)
