#pragma once

#include "pp_seq_first_n.h"
#include "pp_seq_rest_n.h"	// pp_incÆ÷ÇÔ

#include "detail/pp_seq_subseq_impl.h"

#define pp_seq_subseq(seq, i, len)		pp_seq_subseq_impl(seq, i, len)
