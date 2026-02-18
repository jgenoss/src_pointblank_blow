#pragma once

#include "../arithmetic/pp_inc.h"
#include "../repetition/pp_repeat.h"

#include "detail/pp_repeat_params_1_impl.h"

#define pp_repeat_params_1(count, param)		pp_repeat_params_1_impl(count, param)
