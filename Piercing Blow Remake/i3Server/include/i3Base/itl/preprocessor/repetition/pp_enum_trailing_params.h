#pragma once

#include "pp_repeat.h"

#include "detail/pp_enum_trailing_params_impl.h"

#define pp_enum_trailing_params(count, param)	pp_enum_trailing_params_impl(count, param)
#define pp_enum_trailing_params_z(z, count, param)	pp_enum_trailing_params_z_impl(z, count, param)
