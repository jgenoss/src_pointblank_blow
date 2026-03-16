#pragma once

#include "../repetition/pp_repeat.h"

#include "detail/pp_repeat_params_impl.h"

#define pp_repeat_params(count, param)			pp_repeat_params_impl(count, param)
