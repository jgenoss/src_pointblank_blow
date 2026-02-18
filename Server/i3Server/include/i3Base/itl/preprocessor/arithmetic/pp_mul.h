#pragma once

#include "pp_add.h"		//pp_inc.h, pp_dec.h, pp_while, pp_tuple_elem Æ÷ÇÔ..

#include "detail/pp_mul_impl.h"

#define pp_mul(x, y)						pp_mul_impl(x, y)
#define pp_mul_d(d, x, y)					pp_mul_d_impl(d, x, y)
