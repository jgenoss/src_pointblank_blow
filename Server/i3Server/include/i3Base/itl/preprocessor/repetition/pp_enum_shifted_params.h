#pragma once

#include "../arithmetic/pp_dec.h"
#include "../arithmetic/pp_inc.h"
#include "../punctuation/pp_comma_if.h"
#include "pp_repeat.h"

#include "detail/pp_enum_shifted_params_impl.h"

#define pp_enum_shifted_params(count, param)	pp_enum_shifted_params_impl(count, param)
#define pp_enum_shifted_params_z(z, count, param)	pp_enum_shifted_params_z_impl(z, count, param)
