#pragma once

#include "../control/pp_expr_iif.h"
#include "../detail/pp_is_unary.h"
#include "../tuple/pp_tuple_rem.h"

#include "detail/pp_apply_impl.h"

#define pp_apply(x)		pp_apply_impl(x)
