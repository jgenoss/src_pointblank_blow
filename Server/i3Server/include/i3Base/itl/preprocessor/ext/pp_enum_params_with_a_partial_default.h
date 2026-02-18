#pragma once

#include "../arithmetic/pp_sub.h"
#include "../logical/pp_and.h"
#include "../repetition/pp_enum_params.h"
#include "pp_enum_params_from_to_with_a_default.h"

#include "detail/pp_enum_params_with_a_partial_default_impl.h"

// 부분 기본인자 선언의 경우..
#define pp_enum_params_with_a_partial_default(total_count, no_def_count, param, def) pp_enum_params_with_a_partial_default_impl(total_count, no_def_count, param, def)
