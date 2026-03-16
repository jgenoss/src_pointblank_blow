#pragma once

#include "pp_seq_first_n.h"
#include "pp_seq_rest_n.h"	// pp_incÆ÷ÇÔ

#include "detail/pp_seq_replace_impl.h"

#define pp_seq_replace(seq, i, elem)	pp_seq_replace_impl(seq, i, elem)
