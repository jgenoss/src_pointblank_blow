#pragma once

#include "pp_check.h"

#define pp_is_binary(x)					pp_check(x, pp_is_binary_check)
#define pp_is_binary_check(a, b)		1
#define pp_check_result_pp_is_binary_check 0, pp_nil
