#pragma once

#include "../arithmetic/pp_sub.h"
#include "../logical/pp_not.h"

#include "detail/pp_less_equal_impl.h"

#define pp_less_equal(x, y)					pp_less_equal_impl(x, y)
#define pp_less_equal_d(d, x, y)			pp_less_equal_d_impl(d, x, y)

