#pragma once

#define pp_expr_iif_impl(bit, expr) pp_expr_iif_i(bit, expr)
#define pp_expr_iif_i(bit, expr)	pp_expr_iif_ ## bit(expr)
#define pp_expr_iif_0(expr)
#define pp_expr_iif_1(expr)		expr
