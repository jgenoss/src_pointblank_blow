#pragma once

#include "../control/pp_while.h"		// pp_cat, pp_error, pp_auto_rec, pp_tuple_eat Æ÷ÇÔ..

#include "../control/pp_expr_iif.h"
#include "../list/pp_list_adt.h"

#include "detail/pp_list_fold_left_impl.h"

#if 0
#define pp_list_fold_left(op, state, list)
#endif

#define pp_list_fold_left	pp_list_fold_left_impl
