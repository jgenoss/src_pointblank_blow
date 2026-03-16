#pragma once

#include "pp_enum_binary_params_1.h"

#include "detail/pp_enum_binary_params_1_with_defaults_impl.h"

#define pp_enum_params_1_with_defaults(count, param, def)	pp_enum_params_1_with_defaults_impl(count, param, def)		
// 1부터 시작!!
