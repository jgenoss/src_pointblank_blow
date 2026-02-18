#pragma once

#include "pp_less_equal.h"
#include "pp_not_equal.h"		// pp_iifÆ÷ÇÔ
#include "../logical/pp_bitand.h"
#include "../tuple/pp_tuple_eat.h"

#include "detail/pp_less_impl.h"

#define pp_less(x, y)						pp_less_impl(x, y)
#define pp_less_d(d, x, y)					pp_less_d_impl(d, x, y)
