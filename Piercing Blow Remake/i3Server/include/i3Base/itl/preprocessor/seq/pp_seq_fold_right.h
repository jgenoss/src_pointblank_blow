#pragma once

#include "pp_seq_reverse.h"		// pp_cat, pp_autorec, pp_seq_fold_left, pp_seq Æ÷ÇÔ..

#include "detail/pp_seq_fold_right_impl.h"

#if 0
#define pp_seq_fold_right(op, state, seq) ...
#endif

#define pp_seq_fold_right	pp_seq_fold_right_impl
