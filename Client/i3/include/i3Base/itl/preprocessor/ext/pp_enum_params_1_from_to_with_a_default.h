#pragma once

#include "../facilities/pp_intercept.h"
#include "pp_enum_binary_params_1_from_to.h"

#include "detail/pp_enum_params_1_from_to_with_a_default_impl.h"

#define pp_enum_params_1_from_to_with_a_default(first, last, param, def) pp_enum_params_1_from_to_with_a_default_impl(first, last, param, def)
