#pragma once

#include "pp_dec.h"
#include "../control/pp_while.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_sub_impl.h"


#define pp_sub(x, y)						pp_sub_impl(x, y)
#define pp_sub_d(d, x, y)					pp_sub_d_impl(d, x, y)
