#pragma once

#include "pp_enum_binary_params.h"

#include "detail/pp_enum_binary_params_with_defaults_impl.h"

#define pp_enum_params_with_defaults(count, param, def)		pp_enum_params_with_defaults_impl(count, param, def)
