#pragma once

#include "../comparison/pp_less_equal.h"		// pp_iifÆ÷ÇÔ

#include "detail/pp_max_impl.h"

#define pp_max(x, y)		pp_max_impl(x, y)
#define pp_max_d(d, x, y)	pp_max_d_impl(d, x, y)
