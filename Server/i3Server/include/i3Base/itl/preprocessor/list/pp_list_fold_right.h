#pragma once

#include "../control/pp_while.h"	// pp_cat, pp_error, pp_auto_rec Æ÷ÇÔ..

#include "pp_list_fold_left.h"
#include "pp_list_reverse.h"

#include "detail/pp_list_fold_right_impl.h"

#if 0
#define pp_list_fold_right(op, state, list)
#endif

#define pp_list_fold_right	pp_list_fold_right_impl
