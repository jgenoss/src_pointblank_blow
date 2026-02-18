#pragma once

#include "pp_seq_fold_left.h"		// pp_if, pp_dec, pp_seq, pp_seq_size, pp_tuple_eat Æ÷ÇÔ..

#include "detail/pp_seq_cat_impl.h"

#define pp_seq_cat(seq)			pp_seq_cat_impl(seq)
#define pp_seq_cat_s(s, seq)	pp_seq_cat_s_impl(s, seq)
