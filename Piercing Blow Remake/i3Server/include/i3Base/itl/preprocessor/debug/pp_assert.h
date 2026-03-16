#pragma once

#include "../control/pp_expr_iif.h"
#include "../control/pp_iif.h"
#include "../logical/pp_not.h"
#include "../tuple/pp_tuple_eat.h"

#include "detail/pp_assert_impl.h"

#if 0
#define pp_assert(cond)
#endif

#if 0
#define pp_assert_msg(cond, msg)
#endif

#define pp_assert		pp_assert_impl
#define pp_assert_msg	pp_assert_msg_impl

