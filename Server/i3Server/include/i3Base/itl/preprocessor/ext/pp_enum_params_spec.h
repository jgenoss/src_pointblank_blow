#pragma once

#include "../arithmetic/pp_sub.h"
#include "../logical/pp_and.h"
#include "../repetition/pp_enum_params.h"
#include "pp_enum_datas.h"

#include "detail/pp_enum_params_spec_impl.h"

// 후방 인자 특화의 경우..
#define pp_enum_params_spec(total_count, param_count, param, def_param) pp_enum_params_spec_impl(total_count, param_count, param, def_param)
