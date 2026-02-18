#pragma once

#include "../control/pp_expr_if.h"
#include "pp_seq_fold_left.h"		// pp_empty, pp_seqÆ÷ÇÔ
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_seq_filter_impl.h"

#define pp_seq_filter(pred, data, seq)			pp_seq_filter_impl(pred, data, seq)
#define pp_seq_filter_s(s, pred, data, seq)		pp_seq_filter_s_impl(s, pred, data, seq)
