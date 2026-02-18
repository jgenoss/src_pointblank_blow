#pragma once

#include "pp_div.h"

#include "detail/pp_mod_impl.h"


#define pp_mod(x, y)						pp_mod_impl(x, y)
#define pp_mod_d(d, x, y)					pp_mod_d_impl(d, x, y)
