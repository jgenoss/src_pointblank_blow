#pragma once

#include "pp_inc.h"
#include "../comparison/pp_less_equal.h"		// pp_sub°¡ Æ÷ÇÔµÊ..pp_sub¿¡ pp_tuple_elemÀÌ Æ÷ÇÔµÊ..
#include "../control/pp_while.h"

#include "detail/pp_div_impl.h"

#define pp_div(x, y)						pp_div_impl(x, y)
#define pp_div_d(d, x, y)					pp_div_d_impl(d, x, y)
