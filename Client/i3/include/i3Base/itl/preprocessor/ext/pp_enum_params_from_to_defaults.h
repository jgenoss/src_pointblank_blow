#pragma once

#include "pp_enum_binary_params_from_to.h"

#include "detail/pp_enum_params_from_to_with_defaults_impl.h"

#define pp_enum_params_from_to_with_defaults(first, last, param, def) pp_enum_params_from_to_with_defaults_impl(first, last, param, def)
