#pragma once

#include "detail/pp_expr_iif_impl.h"

#define pp_expr_iif(bit, expr)				pp_expr_iif_impl(bit, expr)
