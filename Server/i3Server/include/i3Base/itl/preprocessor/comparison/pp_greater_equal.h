#pragma once

#include "pp_less_equal.h"

#include "detail/pp_greater_equal_impl.h"

#define pp_greater_equal(x, y)				pp_greater_equal_impl(x, y)
#define pp_greater_equal_d(d, x, y)			pp_greater_equal_d_impl(d, x, y)
