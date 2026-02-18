#pragma once

#include "../pp_cat.h"
#include "../control/pp_iif.h"

#include "detail/pp_not_equal_impl.h"

#define pp_not_equal(x, y)					pp_not_equal_impl(x, y)
