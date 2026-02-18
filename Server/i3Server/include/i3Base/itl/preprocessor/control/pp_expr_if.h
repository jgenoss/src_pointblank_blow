#pragma once

#include "pp_expr_iif.h"
#include "../logical/pp_bool.h"

#include "detail/pp_expr_if_impl.h"

#define pp_expr_if(cond, expr)				pp_expr_if_impl(cond, expr)
