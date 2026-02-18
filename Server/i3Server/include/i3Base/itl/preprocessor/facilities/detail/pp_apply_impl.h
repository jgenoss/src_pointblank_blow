#pragma once

#define pp_apply_impl(x) pp_expr_iif(pp_is_unary(x), pp_tuple_rem_1 x)
