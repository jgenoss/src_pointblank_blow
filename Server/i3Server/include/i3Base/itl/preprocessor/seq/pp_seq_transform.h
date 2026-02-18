#pragma once

#include "pp_seq_fold_left.h"		// pp_seqÆ÷ÇÔ..
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_seq_transform_impl.h"

#define	pp_seq_transform(op, data, seq)			pp_seq_transform_impl(op, data, seq)
#define pp_seq_transform_s(s, op, data, seq)	pp_seq_transform_s_impl(s, op, data, seq)
