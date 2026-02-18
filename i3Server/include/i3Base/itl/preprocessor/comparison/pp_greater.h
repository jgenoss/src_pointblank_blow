#pragma once

#include "pp_less.h"

#include "detail/pp_greater_impl.h"

#define pp_greater(x, y)					pp_greater_impl(x, y)
#define pp_greater_d(d, x, y)				pp_greater_d_impl(d, x, y)
