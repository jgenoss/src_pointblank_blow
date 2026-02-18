#pragma once

#include "../comparison/pp_less_equal.h"		// pp_iifÆ÷ÇÔ

#include "detail/pp_min_impl.h"

#define pp_min(x, y)		pp_min_impl(x, y)
#define pp_min_d(d, x, y)	pp_min_d_impl(d, x, y)
