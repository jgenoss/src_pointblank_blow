#pragma once

#include "pp_seq_fold_left.h"		// pp_emptyÆ÷ÇÔ..

#include "detail/pp_seq_reverse_impl.h"

#define pp_seq_reverse(seq)			pp_seq_reverse_impl(seq)
#define pp_seq_reverse_s(s, seq)	pp_seq_reverse_s_impl(s, seq)
