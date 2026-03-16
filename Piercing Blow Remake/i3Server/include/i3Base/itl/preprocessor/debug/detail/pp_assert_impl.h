#pragma once

#define pp_assert_impl(cond) pp_iif(pp_not(cond), pp_assert_error, pp_tuple_eat_1)(...)
#define pp_assert_error(x, y, z)

#define pp_assert_msg_impl(cond, msg) pp_expr_iif(pp_not(cond), msg)
