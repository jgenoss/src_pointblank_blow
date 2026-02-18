#pragma once

#include "../facilities/pp_intercept.h"
#include "pp_enum_binary_params.h"

#include "detail/pp_enum_params_with_a_default_impl.h"

#define pp_enum_params_with_a_default(count, param, def)	pp_enum_params_with_a_default_impl(count, param, def)
