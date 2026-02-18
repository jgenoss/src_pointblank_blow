#pragma once

#include "pp_list_for_each_i.h"
#include "../punctuation/pp_comma_if.h"

#include "detail/pp_list_enum_impl.h"

#define pp_list_enum(list)			pp_list_enum_impl(list)
#define pp_list_enum_r(r, list)		pp_list_enum_r_impl(r, list)
