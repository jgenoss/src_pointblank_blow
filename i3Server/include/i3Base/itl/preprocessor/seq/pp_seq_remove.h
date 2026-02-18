#pragma once

#include "pp_seq_first_n.h"
#include "pp_seq_rest_n.h"	// pp_incÆ÷ÇÔ

#include "detail/pp_seq_remove_impl.h"

#define pp_seq_remove(seq, i)		pp_seq_remove_impl(seq, i)
