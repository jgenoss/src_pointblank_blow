#pragma once

#include "pp_dec.h"
#include "pp_inc.h"
#include "../control/pp_while.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_add_impl.h"

#define pp_add(x, y)						pp_add_impl(x, y)
#define pp_add_d(d, x, y)					pp_add_d_impl(d, x, y)
