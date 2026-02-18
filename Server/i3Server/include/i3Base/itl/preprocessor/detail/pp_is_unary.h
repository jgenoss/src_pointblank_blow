#pragma once

#include "pp_check.h"

#define pp_is_unary(x) pp_check(x, pp_is_unary_check)

#define pp_is_unary_check(a) 1
#define pp_check_result_pp_is_unary_check 0, pp_nil
