#pragma once

#define pp_expr_if_impl(cond, expr) pp_expr_iif(pp_bool(cond), expr)
